#pragma once
#include "SphMath.h"
#include "Point.h"

class PartitionGrid
{
public:

	void Init(const Box& girdBox,float cellSize,float scale);
	void InsertParticle(PointBuffer* pBuffer);
	void FindCells(const float3& p, float radius, int* gridCell);
	int GetGridCellIndex(float3& pos);
	int GetGridData(int index)const{ return gridData[index]; }

private:
	std::vector<int> gridData;
	float3 minCorner;//最小点的坐标
	float3 maxCorner;//最大点的坐标
	int3 gridResolution;//网格分辨率
	float3 gridSize;//网格大小
	float3 gridDelta;
	float cellSize;//单位格子的大小

public:
	PartitionGrid();
	~PartitionGrid();
};