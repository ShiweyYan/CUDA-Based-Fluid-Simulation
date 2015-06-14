#pragma once
//#include <Windows.h>
//#include <math.h>
//#include <assert.h>

namespace MyMath
{
	 //__device__ const float EPSILON = 0.0000001f;

	struct float3
	{
		float x;
		float y;
		float z;

		__host__ __device__ float3() { x = 0.f; y = 0.f; z = 0.f; }
		__host__ __device__ float3(float x, float y, float z) :x(x), y(y), z(z){}
		__host__ __device__ float3(float3& f) : x(f.x), y(f.y), z(f.z){}

		//float length()
		//{
		//	float temp = x*x + y*y + z*z;
		//	return pow(temp, 0.5f);
		//}

		__host__ __device__ float lengthSqr()
		{
			return x*x + y*y + z*z;
		}

		__host__ __device__ float3& operator=(const float3& f)
		{
			this->x = f.x;
			this->y = f.y;
			this->z = f.z;

			return *this;
		}

		__host__ __device__ float3 operator+(float3& f)
		{
			float3 temp(this->x + f.x, this->y + f.y, this->z + f.z);
			return temp;
		}

		__host__ __device__ float3 operator-(float3& f)
		{
			float3 temp(this->x - f.x, this->y - f.y, this->z - f.z);
			return temp;
		}

		__host__ __device__ float3 operator*(float f)
		{
			float3 temp(this->x*f, this->y*f, this->z*f);
			return temp;
		}

		__host__ __device__ float3 operator/(float f)
		{
			if (f<0.0000001f && f>-0.0000001f)
			{
				//为了可以在GPU上跑,暂时注释掉
				//MessageBox(NULL, "float3的除数不能为0", NULL, 0);
				//assert(false);
			}
			float3 temp(this->x / f, this->y / f, this->z / f);
			return temp;
		}

		__host__ __device__ float3 operator*(int f)
		{
			float3 temp(this->x*f, this->y*f, this->z*f);
			return temp;
		}

		__host__ __device__ float3 operator/(int f)
		{
			if (f == 0)
			{
				//为了可以在GPU上跑,暂时注释掉
				//MessageBox(NULL, "float3的除数不能为0", NULL, 0);
				//assert(false);
			}
			float3 temp(this->x / f, this->y / f, this->z / f);
			return temp;
		}

		__host__ __device__ float3 operator/(float3& f)
		{
			float3 temp(this->x / f.x, this->y / f.y, this->z / f.z);
			return temp;
		}

		__host__ __device__ float3 operator*(float3& f)
		{
			float3 temp(this->x*f.x, this->y*f.y, this->z*f.z);
			return temp;
		}

		__host__ __device__ void operator+=(float3& f)
		{
			this->x = this->x + f.x;
			this->y = this->y + f.y;
			this->z = this->z + f.z;
		}

		__host__ __device__ void operator+=(const float3& f)
		{
			this->x = this->x + f.x;
			this->y = this->y + f.y;
			this->z = this->z + f.z;
		}

		__host__ __device__ float reduceLength2(const float3& f)
		{
			return (this->x - f.x)*(this->x - f.x) + (this->y - f.y)*(this->y - f.y) + (this->z - f.z)*(this->z - f.z);
		}

		__host__ __device__ float reduceLength(const float3& f)
		{
			return sqrt((this->x - f.x)*(this->x - f.x) + (this->y - f.y)*(this->y - f.y) + (this->z - f.z)*(this->z - f.z));
		}
	};

	struct int3
	{
		int x;
		int y;
		int z;

		__host__ __device__ int3() :x(0), y(0), z(0){}
		__host__ __device__ int3(int x, int y, int z) : x(x), y(y), z(z){}
		__host__ __device__ int3(int3& i) : x(i.x), y(i.y), z(i.z){}
	};
}
