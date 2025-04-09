#pragma once

class GameTime {
public:
	GameTime();

	float totalTime() const;
	float deltaTime() const;
	bool isStopped() const;

	void reset();
	void start();
	void stop();
	void tick();
private:
	double mSecondsPerCount;
	double mDeltaTime;

	/* Counters for time recording, and the relationship between time and the count is:
	* time = count * mSecondsPerCount
	*/
	__int64 mBaseCount; // counter for the timer beginning recording
	__int64 mPauseCount;
	__int64 mStopCount;
	__int64 mLastTickCount;
	__int64 mCurrentCount;

	bool mIsStopped;
};
