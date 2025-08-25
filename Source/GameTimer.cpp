#include "GameTimer.h"

#define GET_COUNTER(x) QueryPerformanceCounter((LARGE_INTEGER*)&x);

GameTimer::GameTimer()
: mIsStopped(false)
, mStopCounter(0) {
	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	mSecondsPerCount = 1.0 / double(countsPerSecond);
}

/*
* Update the timer per frame
*/
void GameTimer::tick() {
	if (mIsStopped) {
		mDeltaTime = 0;
		return;
	}

	GET_COUNTER(mCurrentCounter);
	mDeltaTime = max(0.0, mSecondsPerCount * (mCurrentCounter - mLastFrameCounter));
	mLastFrameCounter = mCurrentCounter;
}

void GameTimer::reset() {
	GET_COUNTER(mBaseCounter);
	mLastFrameCounter = mBaseCounter;
	mStopCounter = 0;
	mIsStopped = false;
}

void GameTimer::start() {
	if (!mIsStopped) {
		return;
	}

	GET_COUNTER(mLastFrameCounter);
	mTotalPauseCounter += mLastFrameCounter - mStopCounter;
	mStopCounter = 0;
	mIsStopped = false;
}

void GameTimer::stop() {
	if (mIsStopped) {
		return;
	}

	mIsStopped = true;
	GET_COUNTER(mStopCounter);

}

float GameTimer::deltaTime() const {
	return (float)mDeltaTime;
}

float GameTimer::totalTime() const {
	if (mIsStopped) {
		return (float)(mStopCounter - mBaseCounter - mTotalPauseCounter) * mSecondsPerCount;
	}
	else {
		return (float)(mCurrentCounter - mBaseCounter - mTotalPauseCounter) * mSecondsPerCount;
	}
}

bool GameTimer::isStopped() const {
	return mIsStopped;
}
