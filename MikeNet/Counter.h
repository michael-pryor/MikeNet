#pragma once
#include <time.h>

/**
 * @brief	%Counter object, used to detect a large frequency of an event occurring simultaneously.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * If no increment has happened for GetTimeout() number of milliseconds, the counter will be reset to 0.
 * If the counter reaches GetCounterLimit() it is reset to 0 and Increment() returns true.\n\n
 *
 * This class is thread safe.
 */
class Counter: protected CriticalSection
{
	/** @brief Stores clock() value of last unreset increment. */
	clock_t timer;

	/** @brief After no increment for this number of milliseconds object is reset. */
	clock_t timeout;

	/** @brief %Counter incremented by Increment method. */
	size_t counter;

	/** @brief Maximum that counter can be, when counter reaches this limit it is reset. */
	size_t counterLimit;

public:
	/** @brief Default value for Counter::counterLimit. */
	const static size_t DEFAULT_LIMIT = 1000;

	/** @brief Default value for Counter::timeout. */
	const static size_t DEFAULT_TIMEOUT = 500;

	Counter(clock_t timeout, size_t counterLimit);
	bool Increment();

	void Reset();

	clock_t GetTimer();
	clock_t GetTimeout();
	void SetTimeout(clock_t newTimeout);

	size_t GetCounter();
	size_t GetCounterLimit();
	void SetCounterLimit(size_t newCounterLimit);

	static bool TestClass();
};