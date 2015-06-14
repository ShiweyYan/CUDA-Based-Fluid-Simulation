#pragma  once
#include "SphMath.h"
#include <vector>
#include <list>

class NeighborTable
{
public:
	void reset(int pointCounts);
	void SetCurrentIndex(int index);//设置现在要计算哪个粒子的邻域
	bool addNeighbor( int neighborIndex, float distance);
	void Commit();//将临时数据提交到neighborTable中
	short getNeighborCounts(unsigned int ptIndex) { return neighborTable[ptIndex].count; }
	void getNeighborInfo( int pTindex,int index, unsigned int& neighborIndex, float& neighborDistance);//每次调用获取下一个信息，返回值是第几个邻居，到最后一个后会回到第一个neighbor；

private:
	enum { MAX_NEIGHTBOR_COUNTS = 100, };

	struct PointExtraData
	{
		unsigned int neighborIndex;
		float distance;
	};

	struct TableUnit
	{
		PointExtraData* p;
		short count;
	};

	std::vector<TableUnit> neighborTable;

	//对于每个点的临时变量
	int currentIndex;
	std::vector<PointExtraData> tempData;

public:
	NeighborTable();
	~NeighborTable();
};