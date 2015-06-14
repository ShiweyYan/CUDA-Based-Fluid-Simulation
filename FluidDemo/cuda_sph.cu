
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "device_functions.h"
#include <cuda_D3D11_interop.h>

#include <stdio.h>
#include <iostream>
#include "MyMath.h"
#include "MyVector3.h"

#include <D3D11.h>

struct cudaGraphicsResource* positionsVB_CUDA;

//-------------GPU�õ�����--------------------
__constant__ static const  float  unitScale = 0.012f;			// �ߴ絥λ
__constant__ static const float unitScale2 = 0.000144f;		//�߶ȵ�λƽ��
__constant__ static const float viscosity = 1.0f;				// ճ��
__constant__ static const float restDensity = 1000.f;			// �ܶ�
__constant__ static const float pointMass = 0.0004f;			// ��������
//__constant__ static const float gasConstantK = 1.0f;				// �������巽�̳���
__constant__ static const float  gpu_smoothRadius = 0.01f;			// �⻬�˰뾶

__constant__ static const float boundartStiffness = 10000.f;
__constant__ static const float boundaryDampening = 256.f;
__constant__ static const float speedLimiting = 200.f;

__constant__ static const   float edgeWidth = 2.f;//�߽���

__constant__ static const  float gravityY = -9.8f;

__constant__ static const  float deltaTime = 0.003f;

//__constant__ static const  int gpu_particleCount = 10000;

__constant__ static const  int gpu_maxParticlePerGrid = 100;

__constant__ static const  int gpu_maxNeighborCount = 60;

//-------------CPU�õ�����-----------------
const int blockCount = 20;

const int particleCount = 10000;

const int maxParticlePerGrid = 100;

const int maxNeighborCount = 60;

const int	W = 15;

const int H = 15;

const int GRIDH = 30;

const float scale = 0.012f;
const float smoothRadius = 0.01f;			// �⻬�˰뾶
float cellSize = 2 * smoothRadius;

const float cpuDensity = 1000.f;			// �ܶ�
const float cpuMass = 0.0004f;			// ��������

//Poly6 Kernel
float kernelPoly6 = 315.0f / (64.0f * 3.141592f * pow(smoothRadius, 9));
//Spiky Kernel
float kernelSpiky = -45.0f / (3.141592f * pow(smoothRadius, 6));
//Viscosity Kernel
float kernelViscosity = 45.0f / (3.141592f * pow(smoothRadius, 6));


struct Box
{
	Box() = default;
	Box(Box& box) { mMin = box.mMin; mDim = box.mDim; }
	void operator=(Box& box){ mMin = box.mMin; mDim = box.mDim; }
	MyMath::float3 mMin{ 0.f, 0.f, 0.f };
	MyMath::int3 mDim;// { 0, 0, 0 };//��������Max���������ֵ��������2h��
};

struct WaterBox
{
	float minX;
	float minZ;
	float minY;
	float maxX;
	float maxZ;
};

struct Particle
{
	MyMath::float3 mPos{ 0, 0, 0 };
	MyMath::float3 mVec{ 0, 0, 0 };
	MyMath::float3	 mAccel{ 0, 0, 0 };	//���ٶ�

	MyMath::float3		mVecEval{ 0, 0, 0 };

	float			mDensity{ 0 };	//�ܶ�
	float			mPressure{ 0 };	//ѹ��
};

struct hashUnit
{
	int mGridIndex{ 0 };
	int mParticleIndex{ 0 };
};

struct indexUnit
{
	int mBegin{ 0 };
	int mEnd{ 0 };
};

__device__ void swap(hashUnit*const hashTable, int i, int j)
{
	hashUnit temp = hashTable[i];
	hashTable[i] = hashTable[j];
	hashTable[j] = temp;
}

__global__ void zeroGridCount(int*const orderedGridContainer)
{
	const unsigned int tid = (blockIdx.x*blockDim.x) + threadIdx.x + ((blockIdx.y*blockDim.y) + threadIdx.y)*blockDim.x*gridDim.x;

	orderedGridContainer[tid * gpu_maxParticlePerGrid] = 0;
}

__global__ void buidGridContent(Particle*const container, Box*const gridBox, int*const orderedGridContainer)
{
	const unsigned int tid = (blockIdx.x*blockDim.x) + threadIdx.x + ((blockIdx.y*blockDim.y) + threadIdx.y)*blockDim.x*gridDim.x;

	const  float cellSize = gpu_smoothRadius * 2 / unitScale;
	MyMath::float3 pos = container[tid].mPos;
	MyMath::float3 min = gridBox->mMin;
	MyMath::int3 dim = gridBox->mDim;

	//����ÿ��cell�����ӵ�������
	int index = ((int)((pos.y - min.y) / cellSize)*dim.x*dim.z + (int)((pos.z - min.z) /
		cellSize)*dim.x + (int)((pos.x - min.x) / cellSize)) * gpu_maxParticlePerGrid;
	int offset = atomicAdd(&orderedGridContainer[index], 1);
	offset += 1;
	orderedGridContainer[index + offset] = tid;
}

__device__ void findCells(int*cells, Particle& particle, Box* box)
{
	for (int i = 0; i < 8; i++)
	{
		cells[i] = -1;
	}

	float r = gpu_smoothRadius / unitScale;
	// ������ķ�Χ(grid)
	int sph_min_x = (int)((-r + particle.mPos.x - box->mMin.x) / 2 / r);
	int sph_min_y = (int)((-r + particle.mPos.y - box->mMin.y) / 2 / r);
	int sph_min_z = (int)((-r + particle.mPos.z - box->mMin.z) / 2 / r);
	if (sph_min_x < 0) sph_min_x = 0;
	if (sph_min_y < 0) sph_min_y = 0;
	if (sph_min_z < 0) sph_min_z = 0;

	cells[0] = (sph_min_y*box->mDim.z + sph_min_z)*box->mDim.x + sph_min_x;
	cells[1] = cells[0] + 1;
	cells[2] = (int)(cells[0] + box->mDim.x);
	cells[3] = cells[2] + 1;

	if (sph_min_y + 1 < box->mDim.y)
	{
		cells[4] = (int)(cells[0] + box->mDim.x*box->mDim.z);
		cells[5] = cells[4] + 1;
		cells[6] = (int)(cells[4] + box->mDim.x);
		cells[7] = cells[6] + 1;
	}
	if (sph_min_x + 1 >= box->mDim.x)
	{
		cells[1] = -1;		cells[3] = -1;
		cells[5] = -1;		cells[7] = -1;
	}
	if (sph_min_z + 1 >= box->mDim.z)
	{
		cells[2] = -1;		cells[3] = -1;
		cells[6] = -1;		cells[7] = -1;
	}
}

__device__ float square(MyMath::float3& p0, MyMath::float3& p1)
{
	return pow(p0.x - p1.x, 2) + pow(p0.y - p1.y, 2) + pow(p0.z - p1.z, 2);
}

__device__ float length(MyMath::float3& p0, MyMath::float3& p1)
{
	return sqrt(pow(p0.x - p1.x, 2) + pow(p0.y - p1.y, 2) + pow(p0.z - p1.z, 2));
}

__device__ float lengthSqr(MyMath::float3& p)
{
	return p.x*p.x + p.y*p.y + p.z*p.z;
}

__device__ float evolution(float input)
{
	return sqrt(input);
}

__global__ void computeDensityAndPressure(int*const orderedGridContainer, Particle*const container, Box*const box, float*const kernelPoly6, int*const neighborTable)
{
	const unsigned int tid = (blockIdx.x*blockDim.x) + threadIdx.x + ((blockIdx.y*blockDim.y) + threadIdx.y)*blockDim.x*gridDim.x;

	float sum = 0;
	float h2 = gpu_smoothRadius*gpu_smoothRadius;
	//int maxNeighborCount = gpu_maxNeighborCount
	//�ҵ��̶߳�Ӧ������
	Particle selfParticle = container[tid];
	//8�������ڹ⻬���Ӱ뾶�ڵ�cells
	int cells[8];
	findCells(cells, selfParticle, box);

	int neighborCount = 0;

	//ͨ������indexTable��ÿ��cell�е����ӽ��б���
	for (int i = 0; i < 8; i++)
	{
		if (~cells[i])
		{
			int index = cells[i] * gpu_maxParticlePerGrid;
			//int num = orderedGridContainer[index];

			for (int j = 1; j <= orderedGridContainer[index]; ++j)
			{
				int neighborIndex = orderedGridContainer[index + j];
				if (tid == neighborIndex)
				{
					sum +=h2 * h2 * h2;
				}
				else
				{
					MyMath::float3 neighborPos = container[neighborIndex].mPos;
					float disX = selfParticle.mPos.x - neighborPos.x;
					float disY = selfParticle.mPos.y - neighborPos.y;
					float disZ = selfParticle.mPos.z - neighborPos.z;
					float dis2 = (disX*disX + disY*disY + disZ*disZ)*unitScale2;//*unitScale;

					//����������С�ڹ⻬���Ӱ뾶
					if (dis2 < h2)
					{
						++neighborCount;
						//���м���
						sum += (h2 - dis2) * (h2 - dis2) * (h2 - dis2);
						int neiborIndex = tid * gpu_maxNeighborCount + neighborCount;
						neighborTable[neiborIndex] = neighborIndex;
						//dis2Table[neiborIndex] = dis2;
						if (neighborCount == gpu_maxNeighborCount - 1)
						{
							i = 8;
							break;
						}
					}
				}
			}
		}
	}

	neighborTable[tid * gpu_maxNeighborCount] = neighborCount;

	//�����ܶȺ�ѹ��
	container[tid].mDensity = (*kernelPoly6)*pointMass*sum;
	float temp = container[tid].mDensity / restDensity;
	container[tid].mPressure = 100.f*(temp*temp*temp*temp*temp*temp - 1);//gasConstantK*(container[tid].mDensity - restDensity);
}

__global__ void computeForce(Particle*const container, float*const kernelSpiky, float*const kernelViscosity, int*const neighborTable)
{
	const unsigned int tid = (blockIdx.x*blockDim.x) + threadIdx.x + ((blockIdx.y*blockDim.y) + threadIdx.y)*blockDim.x*gridDim.x;

	MyMath::float3 sum0;
	MyMath::float3 sum1;

	int neiborBeginPos = tid * gpu_maxNeighborCount;
	//�ҵ��̶߳�Ӧ������
	Particle tempParticle = container[tid];
	int temp = neighborTable[neiborBeginPos];
	//Particle neighborParticle = container[neighborTable[neiborBeginPos + j]];
	//ͨ������neighborTable��ÿ���ڽ����ӽ��б���
	for (int j = 1; j <= temp; ++j)
	{
		Particle neighborParticle = container[neighborTable[neiborBeginPos + j]];
		//float dis = length(neighborParticle.mPos*unitScale, tempParticle.mPos*unitScale);//�����Ӿ���
		//float dis = sqrt(dis2Table[neiborBeginPos + j]);
		float disX = tempParticle.mPos.x - neighborParticle.mPos.x;
		float disY = tempParticle.mPos.y - neighborParticle.mPos.y;
		float disZ = tempParticle.mPos.z - neighborParticle.mPos.z;
		float dis = sqrt(disX*disX + disY*disY + disZ*disZ)*unitScale;

		float h_r = gpu_smoothRadius - dis;
		//���м���
		sum0 += (tempParticle.mPos - neighborParticle.mPos) *((tempParticle.mPressure + neighborParticle.mPressure) / neighborParticle.mDensity*h_r*h_r / dis);

		sum1 += (neighborParticle.mVecEval - tempParticle.mVecEval) *(h_r / neighborParticle.mDensity);
	}

	//������������õļ��ٶ�
	container[tid].mAccel = sum0*((-*kernelSpiky)*unitScale / (2 * tempParticle.mDensity)*pointMass) + sum1*(viscosity**kernelViscosity / tempParticle.mDensity*pointMass);
}

__device__ float dot(MyMath::float3& para0, MyMath::float3& para1)
{
	return para0.x*para1.x + para0.y*para1.y + para0.z*para1.z;
}

__global__ void advance(Particle*const container, WaterBox*const box)
{
	const unsigned int tid = (blockIdx.x*blockDim.x) + threadIdx.x + ((blockIdx.y*blockDim.y) + threadIdx.y)*blockDim.x*gridDim.x;

	Particle particle = container[tid];
	MyMath::float3 acc = particle.mAccel;

	//����
	MyMath::float3 g(0.f, gravityY, 0.f);
	acc += g;

	//�ٶ�����
	float sl2 = speedLimiting*speedLimiting;
	float acc2 = acc.x*acc.x + acc.y*acc.y + acc.z*acc.z;//lengthSqr(acc);
	if (acc2>sl2)
	{
		acc = acc*speedLimiting / sqrt(acc2);
	}

	// �߽�����
	//Z����
	float diff = (edgeWidth - (particle.mPos.z - box->minZ))*unitScale;
	if (diff>0)
	{
		MyMath::float3 normal(0.f, 0.f, 1.f);
		float adj = boundartStiffness * diff - boundaryDampening *(normal.x*particle.mVecEval.x + normal.y*particle.mVecEval.y + normal.z*particle.mVecEval.z);//dot(normal, particle.mVecEval);
		acc.x += adj * normal.x;
		acc.y += adj * normal.y;
		acc.z += adj * normal.z;
	}

	diff = (edgeWidth - (box->maxZ - particle.mPos.z))*unitScale;
	if (diff>0)
	{
		MyMath::float3 normal(0.f, 0.f, -1.f);
		float adj = boundartStiffness * diff - boundaryDampening * (normal.x*particle.mVecEval.x + normal.y*particle.mVecEval.y + normal.z*particle.mVecEval.z);
		acc.x += adj * normal.x;
		acc.y += adj * normal.y;
		acc.z += adj * normal.z;
	}

	//X����
	diff = (edgeWidth - (particle.mPos.x - box->minX))*unitScale;
	if (diff>0)
	{
		MyMath::float3 normal(1.f, 0.f, 0.f);
		float adj = boundartStiffness * diff - boundaryDampening * (normal.x*particle.mVecEval.x + normal.y*particle.mVecEval.y + normal.z*particle.mVecEval.z);
		acc.x += adj * normal.x;
		acc.y += adj * normal.y;
		acc.z += adj * normal.z;
	}

	diff = (edgeWidth - (box->maxX - particle.mPos.x))*unitScale;
	if (diff>0)
	{
		MyMath::float3 normal(-1.f, 0.f, 0.f);
		float adj = boundartStiffness * diff - boundaryDampening * (normal.x*particle.mVecEval.x + normal.y*particle.mVecEval.y + normal.z*particle.mVecEval.z);
		acc.x += adj * normal.x;
		acc.y += adj * normal.y;
		acc.z += adj * normal.z;
	}

	//Y����
	diff = (edgeWidth - (particle.mPos.y - box->minY))*unitScale;
	if (diff>0)
	{
		MyMath::float3 normal(0.f, 1.f, 0.f);
		float adj = boundartStiffness * diff - boundaryDampening * (normal.x*particle.mVecEval.x + normal.y*particle.mVecEval.y + normal.z*particle.mVecEval.z);
		acc.x += adj * normal.x;
		acc.y += adj * normal.y;
		acc.z += adj * normal.z;
	}




	//�����ٶȺ�λ��
	MyMath::float3 vNext = particle.mVec + acc*deltaTime;
	container[tid].mVecEval = (particle.mVec + vNext)*0.5f;
	container[tid].mVec = vNext;
	container[tid].mPos = particle.mPos + vNext*deltaTime/unitScale;

}

WaterBox waterBox;
WaterBox* gpu_waterBox;

Box* gpu_gridBox;
Box cpu_gridBox;

//hashUnit cpu_hashTable[particleCount];
//hashUnit* gpu_hashTable;

Particle cpu_container[particleCount];
Particle* gpu_container;

int* gpu_orderedGridContainer;

int cpu_gridCount;
int* gpu_gridCount;

float* gpu_poly6;
float* gpu_kernelSpiky;
float* gpu_kernelViscosity;

int* gpu_neighborTable;
//float* gpu_dis2Table;

void resetWaterBox()
{
	waterBox.minX = 0.f;
	waterBox.minZ = 0.f;
	waterBox.minY = 0.f;
	waterBox.maxX = W * (cellSize / scale);
	waterBox.maxZ = H * (cellSize / scale);
}

void initeWaterPos()
{
	float pointDistance = pow(cpuMass / cpuDensity, 1.f / 3.f) / scale; //���Ӽ��
	MyMath::int3 fuidDim;
	fuidDim.x = (int)(cpu_gridBox.mDim.x / 2 * cellSize / scale / pointDistance);
	fuidDim.z = (int)(cpu_gridBox.mDim.z / 2 * cellSize / scale / pointDistance);
	fuidDim.y = 0;
	//��ʼ��container
	const int baseHeight = 5;
	int id = 0;
	bool FULL = false;
	while (true)
	{
		for (int z = 0; z < fuidDim.z; ++z)
		{
			for (int x = 0; x < fuidDim.x; ++x)
			{
				cpu_container[id].mPos.x = (waterBox.maxX - waterBox.minX) / 4 + x*pointDistance;
				cpu_container[id].mPos.z = (waterBox.maxZ - waterBox.minZ) / 4 + z*pointDistance;
				cpu_container[id].mPos.y = baseHeight + fuidDim.y*pointDistance;
				++id;
				if (id == particleCount)
				{
					FULL = true;
					break;
				}
			}
			if (FULL)
			{
				break;
			}
		}
		if (FULL)
		{
			break;
		}
		++fuidDim.y;
	}
}

//��ʼ������
extern "C" void initData()
{
	//����ˮ��
	resetWaterBox();

	int dimX = (int)((waterBox.maxX - waterBox.minX) / (cellSize / scale));
	if (((waterBox.maxX - waterBox.minX) / (cellSize / scale))>dimX)
	{
		dimX += 1;
	}
	int dimZ = (int)((waterBox.maxZ - waterBox.minZ) / (cellSize / scale));
	if (((waterBox.maxZ - waterBox.minZ) / (cellSize / scale))>dimZ)
	{
		dimZ += 1;
	}
	cudaMalloc((void**)&gpu_waterBox, sizeof(WaterBox));
	cudaMemcpy(gpu_waterBox, &waterBox, sizeof(WaterBox), cudaMemcpyHostToDevice);

	//��ʼ��gridBox
	cpu_gridBox.mMin = { waterBox.minX, waterBox.minY, waterBox.minZ };
	cpu_gridBox.mDim = { dimX, 30, dimZ };
	cudaMalloc((void**)&gpu_gridBox, sizeof(Box));
	cudaMemcpy(gpu_gridBox, &cpu_gridBox, sizeof(Box), cudaMemcpyHostToDevice);

	//��GPU�ϸ�hashTable�����ڴ�
	//cudaMalloc((void**)&gpu_hashTable, sizeof(hashUnit)*particleCount);

	//��ʼ��ˮ��λ��
	initeWaterPos();

	cudaMalloc((void**)&gpu_container, sizeof(Particle)*particleCount);
	cudaMemcpy(gpu_container, cpu_container, sizeof(Particle)*particleCount, cudaMemcpyHostToDevice);

	cudaMalloc((void**)&gpu_orderedGridContainer, sizeof(int)* maxParticlePerGrid * particleCount);

	//��GPU����������
	cpu_gridCount = cpu_gridBox.mDim.x*cpu_gridBox.mDim.y*cpu_gridBox.mDim.z;
	cudaMalloc((void**)&gpu_gridCount, sizeof(int));
	cudaMemcpy(gpu_gridCount, &cpu_gridCount, sizeof(int), cudaMemcpyHostToDevice);

	//��GPU��N-S kernelֵ
	cudaMalloc((void**)&gpu_poly6, sizeof(float));
	cudaMemcpy(gpu_poly6, &kernelPoly6, sizeof(float), cudaMemcpyHostToDevice);

	cudaMalloc((void**)&gpu_kernelSpiky, sizeof(float));
	cudaMemcpy(gpu_kernelSpiky, &kernelSpiky, sizeof(float), cudaMemcpyHostToDevice);

	cudaMalloc((void**)&gpu_kernelViscosity, sizeof(float));
	cudaMemcpy(gpu_kernelViscosity, &kernelViscosity, sizeof(float), cudaMemcpyHostToDevice);

	cudaMalloc((void**)&gpu_neighborTable, sizeof(int)*particleCount * maxNeighborCount);
	
	//cudaMalloc((void**)&gpu_dis2Table, sizeof(int)*particleCount * maxNeighborCount);
}

//�ͷ���Դ
extern "C" void shutDown()
{
	cudaFree(gpu_gridBox);
	cudaFree(gpu_container);
	//cudaFree(gpu_hashTable);
	cudaFree(gpu_gridCount);
	cudaFree(gpu_poly6);
	cudaFree(gpu_kernelSpiky);
	cudaFree(gpu_kernelViscosity);
	cudaFree(gpu_neighborTable);
	//cudaFree(gpu_dis2Table);
	cudaFree(gpu_orderedGridContainer);
	cudaFree(gpu_waterBox);

}

bool boxIsChange = false;
bool reset = false;
void updateBox()
{
	if (reset)
	{
		resetWaterBox();
	}

	int dimX = (int)((waterBox.maxX - waterBox.minX) / (cellSize / scale));
	if (((waterBox.maxX - waterBox.minX) / (cellSize / scale)) > dimX)
	{
		dimX += 1;
	}
	int dimZ = (int)((waterBox.maxZ - waterBox.minZ) / (cellSize / scale));
	if (((waterBox.maxZ - waterBox.minZ) / (cellSize / scale)) > dimZ)
	{
		dimZ += 1;
	}
	cpu_gridBox.mMin = { waterBox.minX, waterBox.minY, waterBox.minZ };
	cpu_gridBox.mDim = { dimX, GRIDH, dimZ };

	cudaMemcpy(gpu_waterBox, &waterBox, sizeof(WaterBox), cudaMemcpyHostToDevice);
	cudaMemcpy(gpu_gridBox, &cpu_gridBox, sizeof(Box), cudaMemcpyHostToDevice);

	boxIsChange = false;

	if (reset)
	{
		initeWaterPos();
		cudaMemcpy(gpu_container, cpu_container, sizeof(Particle)*particleCount, cudaMemcpyHostToDevice);
		reset = false;
	}
}

//ÿ֡����
extern "C" void runOnCuda()
{
	zeroGridCount << <cpu_gridBox.mDim.y, cpu_gridCount / cpu_gridBox.mDim.y >> >(gpu_orderedGridContainer);

	buidGridContent << <blockCount, particleCount / blockCount >> >(gpu_container, gpu_gridBox, gpu_orderedGridContainer);

	//�����ܶȺ�ѹ��
	computeDensityAndPressure << <blockCount, particleCount / blockCount >> >(gpu_orderedGridContainer, gpu_container, gpu_gridBox, gpu_poly6, gpu_neighborTable);

	//������
	computeForce << <blockCount, particleCount / blockCount >> >(gpu_container, gpu_kernelSpiky, gpu_kernelViscosity, gpu_neighborTable);

	//�ƶ�
	advance << <blockCount, particleCount / blockCount >> >(gpu_container, gpu_waterBox);

	if (boxIsChange)
	{
		updateBox();
	}

	//cudaMemcpy(cpu_container, gpu_container, sizeof(Particle)*particleCount, cudaMemcpyDeviceToHost);

}

extern "C" MyVector3::vector3 getParticlePos(int i)
{
	MyVector3::vector3 pos;
	void* tempP = &cpu_container[i].mPos;
	memcpy(&pos, tempP, sizeof(pos));
	return pos;
}

extern "C" int getParticleCount()
{
	return particleCount;
}

extern "C" void cudaSetD3DDevice()
{
	cudaSetDevice(0);
}

extern "C" void registerResource(ID3D11Buffer* positionsVB)
{
	cudaGraphicsD3D11RegisterResource(&positionsVB_CUDA, positionsVB, cudaGraphicsRegisterFlagsNone);
	cudaGraphicsResourceSetMapFlags(positionsVB_CUDA,cudaGraphicsMapFlagsWriteDiscard);
}

extern "C" void releaseCudaVB()
{
	cudaGraphicsUnregisterResource(positionsVB_CUDA);
}

__global__ void writeInstanceData(MyVector3::vector3* positions, Particle*const container)
{
	const unsigned int tid = (blockIdx.x*blockDim.x) + threadIdx.x + ((blockIdx.y*blockDim.y) + threadIdx.y)*blockDim.x*gridDim.x;

	positions[tid].x = container[tid].mPos.x;
	positions[tid].y = container[tid].mPos.y;
	positions[tid].z = container[tid].mPos.z;
}

extern "C" void writeData()
{
	MyVector3::vector3* positions; 
	// mapʵ������
	cudaGraphicsMapResources(1, &positionsVB_CUDA, 0); 
	size_t num_bytes; 
	cudaGraphicsResourceGetMappedPointer((void**)&positions, &num_bytes, positionsVB_CUDA); 
	// ִ��kernel
	writeInstanceData << <blockCount, particleCount / blockCount >> >(positions, gpu_container);
	// Unmap ʵ������
	cudaGraphicsUnmapResources(1, &positionsVB_CUDA, 0);
}

float step = 0.1f;
extern "C" void changeWaterbox(int i)
{
	boxIsChange = true;
	switch (i)
	{
	case 0:
		waterBox.minX -= step; break;
	case 1:
		waterBox.minX += step; break;
	case 2:
		waterBox.maxX -= step; break;
	case 3:
		waterBox.maxX += step; break;
	case 4:
		reset = true; break;
	default:
		break;
	}
}