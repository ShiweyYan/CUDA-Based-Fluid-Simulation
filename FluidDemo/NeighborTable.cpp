#include "NeighborTable.h"
#include <assert.h>

//-----------------------------------------------------------------------------------------------------------------
NeighborTable::NeighborTable() :currentIndex(0)
{
}

//-----------------------------------------------------------------------------------------------------------------
NeighborTable::~NeighborTable()
{
}

//-----------------------------------------------------------------------------------------------------------------
void NeighborTable::reset(int pointCounts)
{
	for (unsigned int i = 0; i < neighborTable.size();++i)
	{
		delete neighborTable[i].p;
	}
	neighborTable.clear();
	neighborTable.reserve(pointCounts);
	neighborTable.resize(pointCounts);
}

void NeighborTable::SetCurrentIndex(int index)
{
	currentIndex = index;
	tempData.reserve(MAX_NEIGHTBOR_COUNTS);
	tempData.resize(0);
}

bool NeighborTable::addNeighbor( int neighborIndex,float distance)
{
	int tempSize = tempData.size();
	if (tempSize == MAX_NEIGHTBOR_COUNTS)
	{
		return false;
	}
	tempData.resize(tempSize + 1);
	tempData[tempSize].distance = distance;
	tempData[tempSize].neighborIndex = neighborIndex;
	return true;
}

void NeighborTable::Commit()
{
	neighborTable[currentIndex].count = tempData.size();
	neighborTable[currentIndex].p = new PointExtraData[tempData.size()];
	for (unsigned int i = 0; i < tempData.size(); ++i)
	{
		neighborTable[currentIndex].p[i] = tempData[i];
	}
}

//-----------------------------------------------------------------------------------------------------------------
void NeighborTable::getNeighborInfo(int pTindex, int index, unsigned int& neighborIndex, float& neighborDistance)
{
	if ((unsigned int)pTindex<neighborTable.size() && (unsigned short)index<neighborTable[pTindex].count)
	{
		neighborIndex = neighborTable[pTindex].p[index].neighborIndex;
		neighborDistance = neighborTable[pTindex].p[index].distance;
	}
	else
	{
		assert(false);
	}
}
