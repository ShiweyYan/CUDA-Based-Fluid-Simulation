#include "Point.h"

PointBuffer::PointBuffer() :count(0)
{
}

PointBuffer::~PointBuffer()
{
}

void PointBuffer::Reset(int count)
{
	this->count = 0;
	buffer.reserve(count);
	buffer.resize(0);
}

Point* PointBuffer::AddPoint()
{
	buffer.resize(++count);
	return &buffer[count-1];
}