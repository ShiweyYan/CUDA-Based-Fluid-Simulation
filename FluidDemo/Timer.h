#pragma once
#include <Windows.h>

class Timer
{
public:
	Timer();

	//返回当前时间到上次reset之间的游戏时间
	double TotalTime();
	//返回两帧之间的时间
	double DeltaTime()const{ return mDeltaTime; }

	//重置计时器
	void Reset();
	//用于恢复暂停
	void Start();
	//暂停
	void Pause();
	//滴答，每帧调用
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