#pragma once
#include <Windows.h>
#include <math.h>
#include <assert.h>

const float EPSILON = 0.0000001f;

struct float3
{
	float x;
	float y;
	float z;

	float3() { x = 0.f; y = 0.f; z = 0.f; }
	float3(float x, float y, float z) :x(x), y(y), z(z){}
	float3(float3& f) :x(f.x), y(f.y), z(f.z){}

	float length()
	{
		float temp = x*x + y*y + z*z;
		return pow(temp, 0.5f);
	}

	float lengthSqr()
	{
		return x*x + y*y + z*z;
	}

	float dot(float3& f)
	{
		return x*f.x + y*f.y + z*f.z;
	}

	void operator=(const float3& f)
	{
		this->x = f.x;
		this->y = f.y;
		this->z = f.z;
	}

	float3 operator+(float3& f)
	{
		return float3(this->x + f.x, this->y + f.y, this->z + f.z);
	}

	float3 operator-(float3& f)
	{
		return float3(this->x - f.x, this->y - f.y, this->z - f.z);
	}

	float3 operator*(float f)
	{
		return float3(this->x*f, this->y*f, this->z*f);
	}

	float3 operator/(float f)
	{
		if (f<EPSILON && f>-EPSILON)
		{
			MessageBox(NULL, L"float3的除数不能为0", NULL, 0);
			assert(false);
		}
		return float3(this->x / f, this->y / f, this->z / f);
	}

	float3 operator*(int f)
	{
		return float3(this->x*f, this->y*f, this->z*f);
	}

	float3 operator/(int f)
	{
		if (f==0)
		{
			MessageBox(NULL, L"float3的除数不能为0", NULL, 0);
			assert(false);
		}
		return float3(this->x / f, this->y / f, this->z / f);
	}

	float3 operator/(float3& f)
	{
		return float3(this->x / f.x, this->y / f.y, this->z / f.z);
	}

	float3 operator*(float3& f)
	{
		return float3(this->x*f.x, this->y*f.y, this->z*f.z);
	}
};

struct int3
{
	int x;
	int y;
	int z;

	int3() = default;
	int3(int x, int y, int z) :x(x), y(y), z(z){}
	int3(int3& i) :x(i.x), y(i.y), z(i.z){}

	void operator=(const int3& i)
	{
		this->x = i.x;
		this->y = i.y;
		this->z = i.z;
	}

	int3 operator+(int3& i)
	{
		return int3(this->x + i.x, this->y + i.y, this->z + i.z);
	}

	int3 operator-(int3& i)
	{
		return int3(this->x - i.x, this->y - i.y, this->z - i.z);
	}

	float3 operator*(float i)
	{
		return float3(this->x*i, this->y*i, this->z*i);
	}

	float3 operator/(float i)
	{
		if (i<EPSILON && i>-EPSILON)
		{
			MessageBox(NULL, L"float3的除数不能为0", NULL, 0);
			assert(false);
		}
		return float3(this->x / i, this->y / i, this->z / i);
	}

	int3 operator*(int i)
	{
		return int3(this->x*i, this->y*i, this->z*i);
	}

	float3 operator/(int i)
	{
		if (i == 0)
		{
			MessageBox(NULL, L"float3的除数不能为0", NULL, 0);
			assert(false);
		}
		return float3(this->x / (float)i, this->y / (float)i, this->z / (float)i);
	}

	float3 operator/(int3& i)
	{
		return float3(this->x / (float)i.x, this->y / (float)i.y, this->z / (float)i.z);
	}

	int3 operator*(int3& i)
	{
		return int3(this->x*i.x, this->y*i.y, this->z*i.z);
	}

	float3 operator/(float3& i)
	{
		return float3(this->x / i.x, this->y / i.y, this->z / i.z);
	}

	float3 operator*(float3& i)
	{
		return float3((float)this->x*i.x, (float)this->y*i.y, (float)this->z*i.z);
	}
};

struct Box
{
	Box() = default;
	Box(const float3& min, const float3& max){ this->min = min; this->max = max; }
	Box(const Box& box){ min = box.max; max = box.max; }

	float3 min;
	float3 max;
};

inline int Ceil(float in)
{
	if (in > int(in))
	{
		return int(in) + 1;
	}
	else
		return int(in);
}