#include "Timer.h"

Timer::Timer() :mDeltaTime(0),  frequency(0), mPauseCount(0), mBaseCount(0), mPrevCount(0), mCurrentCount(0), mStopped(false)
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	mSecondsPerCount = 1.0 / (double)frequency;
}

void Timer::Reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&mBaseCount);
	mCurrentCount = mPrevCount = mBaseCount;
	mStopped = false;
}

double Timer::TotalTime()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&mCurrentCount);
	return (mCurrentCount - mPauseCount - mBaseCount)*mSecondsPerCount;
}

void Timer::Start()
{
	mPrevCount = mCurrentCount;
	QueryPerformanceCounter((LARGE_INTEGER*)&mCurrentCount);
	mPauseCount += mCurrentCount - mPrevCount;
	mStopped = false;
}

void Timer::Pause()
{
	mStopped = true;
	QueryPerformanceCounter((LARGE_INTEGER*)&mCurrentCount);
}

void Timer::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0.0;
		return;
	}
	QueryPerformanceCounter((LARGE_INTEGER*)&mCurrentCount);
	mDeltaTime = (mCurrentCount - mPrevCount)*mSecondsPerCount;
	mPrevCount = mCurrentCount;
}