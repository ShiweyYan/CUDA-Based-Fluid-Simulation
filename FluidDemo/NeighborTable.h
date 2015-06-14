#pragma  once
#include "SphMath.h"
#include <vector>
#include <list>

class NeighborTable
{
public:
	void reset(int pointCounts);
	void SetCurrentIndex(int index);//��������Ҫ�����ĸ����ӵ�����
	bool addNeighbor( int neighborIndex, float distance);
	void Commit();//����ʱ�����ύ��neighborTable��
	short getNeighborCounts(unsigned int ptIndex) { return neighborTable[ptIndex].count; }
	void getNeighborInfo( int pTindex,int index, unsigned int& neighborIndex, float& neighborDistance);//ÿ�ε��û�ȡ��һ����Ϣ������ֵ�ǵڼ����ھӣ������һ�����ص���һ��neighbor��

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

	//����ÿ�������ʱ����
	int currentIndex;
	std::vector<PointExtraData> tempData;

public:
	NeighborTable();
	~NeighborTable();
};