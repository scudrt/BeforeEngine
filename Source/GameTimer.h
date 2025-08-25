#pragma once

#include <Windows.h>

class GameTimer {
public:
	GameTimer();

	/*
	* Returns the total time of game in seconds
	*/
	float totalTime() const;

	/*
	* Returns the frame time in seonds
	*/
	float deltaTime() const;

	/*
	* Returns true if the timer has called stop()
	*/
	bool isStopped() const;

	/*
	* Update the timer per frame
	*/
	void tick();

	/*
	* Timer managing functions
	*/
	void reset();
	void start();
	void stop();
private:
	// Useful variable for calculating time
	double mSecondsPerCount;
	// Frame time in seconds
	double mDeltaTime;

	// The Cycle count when the timer begins
	__int64 mBaseCounter;
	// Total time of pausing
	__int64 mTotalPauseCounter;
	// Cycle count of the last frame
	__int64 mLastFrameCounter;
	// Cycle count of the current frame
	__int64 mCurrentCounter;

	// The timestamp when the stop() is called
	__int64 mStopCounter;
	// True if the timer is stopped
	bool mIsStopped;
};
