#include "GameTime.h"

#include <Windows.h>

GameTime::GameTime():
	mSecondsPerCount(0.0),
	mDeltaTime(-1.0),
	mBaseCount(0),
	mPauseCount(0),
	mStopCount(0),
	mLastTickCount(0),
	mCurrentCount(0),
	mIsStopped(false) {
	// get CPU counts per second
	__int64 countsPerSecThisDevice;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecThisDevice);
	mSecondsPerCount = 1.0 / (double)countsPerSecThisDevice;
}

void GameTime::tick() {
	if (mIsStopped) {
		mDeltaTime = 0.0;
		return;
	}

	__int64 curCount;
	QueryPerformanceCounter((LARGE_INTEGER*)&curCount);
	mCurrentCount = curCount;
	mDeltaTime = (curCount - mLastTickCount) * mSecondsPerCount;
	mLastTickCount = mCurrentCount;
	mDeltaTime = max(0.0, mDeltaTime);
}

void GameTime::reset() {
	__int64 curCount;
	QueryPerformanceCounter((LARGE_INTEGER*)&curCount);

	mBaseCount = curCount;
	mLastTickCount = curCount;
	mPauseCount = 0;
	mStopCount = 0;
	mIsStopped = false;
}

void GameTime::start() {
	if (!mIsStopped) return;

	__int64 curTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&curTime);

	// a timer can be stopped multiple times, so pause time will always been accumulated
	mPauseCount += curTime - mStopCount;
	mLastTickCount = curTime;
	mStopCount = 0;
	mIsStopped = false;
}

void GameTime::stop() {
	if (mIsStopped) return;

	__int64 curTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&curTime);

	mStopCount = curTime;
	mIsStopped = true;
}

float GameTime::deltaTime() const {
	return (float)mDeltaTime;
}

float GameTime::totalTime() const {
	if (mIsStopped) {
		return (float)((mStopCount - mPauseCount - mBaseCount) * mSecondsPerCount);
	}
	else {
		return (float)((mCurrentCount - mPauseCount - mBaseCount) * mSecondsPerCount);
	}
}

bool GameTime::isStopped() const {
	return mIsStopped;
}

