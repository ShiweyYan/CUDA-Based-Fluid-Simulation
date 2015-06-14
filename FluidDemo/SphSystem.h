#pragma  once

#include "Point.h"
#include "PartitionGrid.h"
#include "NeighborTable.h"

class SphSystem
{
public:
	 void init(unsigned short maxPointCounts,
		 float3& wallBox_min,  float3& wallBox_max,
		 float3& initFluidBox_min,  float3& initFluidBox_max,
		 float3& gravity)
	{
		_init(maxPointCounts,
			Box(float3(wallBox_min), float3(wallBox_max)),
			Box(float3(initFluidBox_min), float3(initFluidBox_max)),
			float3(gravity));
	}

	  int getPointCounts(void) const { return m_pointBuffer.Size(); }
	  std::vector<Point>& getPointBuf()  { return m_pointBuffer.GetPointBuffer(); }
	/** 逻辑帧 */
	 void tick(void);

private:
	/** 初始化系统
	*/
	void _init(unsigned short maxPointCounts, const Box& wallBox, const Box& initFluidBox, const float3& gravity);
	/** 计算密度、压力、以及相邻关系 */
	void computePressure(void);
	/** 计算加速度 */
	void computeForce(void);
	/** 移动粒子*/
	void advance(void);
	/** 创建初始液体块*/
	void addFluidVolume(const Box& fluidBox, float spacing);

private:
	PointBuffer m_pointBuffer;
	PartitionGrid m_gridContainer;
	NeighborTable m_neighborTable;

	// SPH Kernel
	float m_kernelPoly6;
	float m_kernelSpiky;
	float m_kernelViscosity;

	//其他参数
	float m_unitScale;
	float m_viscosity;
	float m_restDensity;
	float m_pointMass;
	float m_smoothRadius;
	float m_gasConstantK;
	float m_boundartStiffness;
	float m_boundaryDampening;
	float m_speedLimiting;
	float3 m_gravityDir;

	Box m_sphWallBox;
public:
	SphSystem();
	~SphSystem();
};