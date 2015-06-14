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
	float3 minCorner;//��С�������
	float3 maxCorner;//���������
	int3 gridResolution;//����ֱ���
	float3 gridSize;//�����С
	float3 gridDelta;
	float cellSize;//��λ���ӵĴ�С

public:
	PartitionGrid();
	~PartitionGrid();
};