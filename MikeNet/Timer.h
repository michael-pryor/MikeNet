#pragma once
#include <time.h>

/**
 * @brief	%Timer object used to repeat an action every x number of milliseconds.
 * @remarks	Michael Pryor, 6/28/2010. 
 *
 * This class is used to keep track of time so that an action can be performed repeatedly 
 * with a gap between each action. \n\n
 *
 * The gap is set by SetFreq() or in the constructor. GetState() will return true after
 * this gap has expired, and then false again until the gap expires again.\n\n
 *
 * This class is thread safe.
 */
class Timer: protected CriticalSection
{
	/** @brief clock() updated last time GetState() returned true. */
	mutable clock_t timer;

	/** @brief Frequency that GetState() should return true in milliseconds. */
	clock_t freq;

public:
	Timer(clock_t freq);
	bool GetState() const;

	clock_t GetTimer() const;
	void SetTimer();
	clock_t GetFreq() const;
	void SetFreq(clock_t newFreq);

	static bool TestClass();
};