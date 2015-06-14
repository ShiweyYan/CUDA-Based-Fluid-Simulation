#include "PartitionGrid.h"

void PartitionGrid::Init(const Box& gridBox, float cellSize, float scale)
{
	minCorner = gridBox.min;
	maxCorner = gridBox.max;
	this->gridSize = maxCorner - minCorner;

	this->cellSize = cellSize / scale;

	this->gridResolution.x = Ceil(gridSize.x / this->cellSize);
	this->gridResolution.y = Ceil(gridSize.y / this->cellSize);
	this->gridResolution.z = Ceil(gridSize.z / this->cellSize);

	this->gridSize = gridResolution*this->cellSize;//重新计算grid大小

	this->gridDelta = gridResolution / gridSize;

	this->gridData.resize((int)(gridSize.x*gridSize.y*gridSize.z));
};

void PartitionGrid::InsertParticle(PointBuffer* pBuffer)
{
	for (int& i:gridData)
	{
		i = -1;
	}
	for (int i = 0; i < pBuffer->Size();++i)
	{
		Point* temp = pBuffer->GetPoint(i);
		int index = GetGridCellIndex(temp->pos);
		if (index>=0&&index<pBuffer->Size())
		{
			temp->next = gridData[index];
			gridData[index] = i;
		}
		else
		{
			temp->next = -1;
		}
	}
}

int PartitionGrid::GetGridCellIndex(float3& pos)
{
	int gx = (int)((pos.x - minCorner.x) * gridDelta.x);
	int gy = (int)((pos.y - minCorner.y) * gridDelta.y);
	int gz = (int)((pos.z - minCorner.z) * gridDelta.z);
	return (gz*gridResolution.y + gy)*gridResolution.x + gx;
}

void PartitionGrid::FindCells(const float3& p, float radius, int* gridCell)
{
	for (int i = 0; i < 8; i++) gridCell[i] = -1;

	// 计算球的范围(grid)
	int sph_min_x = (int)((-radius + p.x - minCorner.x) * gridDelta.x);
	int sph_min_y = (int)((-radius + p.y - minCorner.y) * gridDelta.y);
	int sph_min_z = (int)((-radius + p.z - minCorner.z) * gridDelta.z);
	if (sph_min_x < 0) sph_min_x = 0;
	if (sph_min_y < 0) sph_min_y = 0;
	if (sph_min_z < 0) sph_min_z = 0;

	gridCell[0] = (sph_min_z*gridResolution.y + sph_min_y)*gridResolution.x + sph_min_x;
	gridCell[1] = gridCell[0] + 1;
	gridCell[2] = (int)(gridCell[0] + gridResolution.x);
	gridCell[3] = gridCell[2] + 1;

	if (sph_min_z + 1 < gridResolution.z) 
	{
		gridCell[4] = (int)(gridCell[0] + gridResolution.y*gridResolution.x);
		gridCell[5] = gridCell[4] + 1;
		gridCell[6] = (int)(gridCell[4] + gridResolution.x);
		gridCell[7] = gridCell[6] + 1;
	}
	if (sph_min_x + 1 >= gridResolution.x) 
	{
		gridCell[1] = -1;		gridCell[3] = -1;
		gridCell[5] = -1;		gridCell[7] = -1;
	}
	if (sph_min_y + 1 >= gridResolution.y) 
	{
		gridCell[2] = -1;		gridCell[3] = -1;
		gridCell[6] = -1;		gridCell[7] = -1;
	}
}

PartitionGrid::PartitionGrid()
{
}

PartitionGrid::~PartitionGrid()
{
}