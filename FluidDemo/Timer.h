#pragma once
#include <Windows.h>

class Timer
{
public:
	Timer();

	//���ص�ǰʱ�䵽�ϴ�reset֮�����Ϸʱ��
	double TotalTime();
	//������֮֡���ʱ��
	double DeltaTime()const{ return mDeltaTime; }

	//���ü�ʱ��
	void Reset();
	//���ڻָ���ͣ
	void Start();
	//��ͣ
	void Pause();
	//�δ�ÿ֡����
	void Tick();

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 frequency;
	__int64 mPauseCount;
	__int64 mBaseCount;
	__int64 mPrevCount;
	__int64 mCurrentCount;
	bool mStopped;
};