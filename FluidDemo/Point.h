#pragma once
#include "SphMath.h"
#include <vector>

struct Point
{
	float3		pos{ 0, 0, 0 };	//位置
	float3		accel{ 0, 0, 0 };		//加速度

	float3		velocity{ 0, 0, 0 };
	float3		velocity_eval{ 0, 0, 0 };

	float			density{ 0 };	//密度
	float			pressure{ 0 };	//压力

	int				next{ 0 };
};

class PointBuffer
{
public:
	PointBuffer();
	~PointBuffer();

	void Reset(int count);
	Point* AddPoint();
	Point* GetPoint(int index){ return &buffer[index]; }
	std::vector<Point>& GetPointBuffer(){ return buffer; }
	int Size()const{ return count; }
private:
	std::vector<Point> buffer;
	int count;
};