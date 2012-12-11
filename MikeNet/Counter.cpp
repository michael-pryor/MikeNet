#include "FullInclude.h"

/**
 * @brief Constructor.
 *
 * @param timeout If the counter has not been incremented for this number of milliseconds
 * then the object will be reset, NULL if not used.
 * @param counterLimit When counterLimit is reached Increment will return true and
 * the object will be reset, NULL if not used.
 */
Counter::Counter(clock_t timeout, size_t counterLimit)
{
	this->timeout = timeout;
	this->timer = clock();
	this->counter = 0;
	this->counterLimit = counterLimit;
}

/**
 * @brief Reset object, timer is updated and counter set to 0.
 */
void Counter::Reset()
{
	Enter();
	timer = clock();
	counter = 0;
	Leave();
}

/**
 * @brief Increments counter by 1.
 *
 * @return true if counter limit has been reached, false if not.
 */
bool Counter::Increment()
{
	bool returnMe;
	Enter();
	counter++;
	if(counterLimit != NULL && counter >= counterLimit)
	{
		Reset();
		returnMe = true;
	}
	else
	{
		if(timeout != NULL)
		{
			if(clock() - timer > timeout)
			{
				Reset();
			}
			else
			{
				timer = clock();
			}
		}
		returnMe = false;
	}
	Leave();
	return returnMe;
	
}

/**
 * @brief Returned stored timer value.
 *
 * @return Counter::timer.
 */
clock_t Counter::GetTimer()
{
	Enter();
	clock_t returnMe = timer;
	Leave();
	return returnMe;
}

/**
 * @brief Retrieves timeout value, if the counter has not been incremented for this number of milliseconds then it is reset.
 *
 * @return Counter::timeout().
 */
clock_t Counter::GetTimeout()
{
	Enter();
	clock_t returnMe = timeout;
	Leave();
	return returnMe;
}

/**
 * @brief Change timeout value, if the counter has not been incremented for this number of milliseconds then it is reset.
 *
 * @param newTimeout New timeout value.
 */
void Counter::SetTimeout(clock_t newTimeout)
{
	Enter();
	timeout = newTimeout;
	Leave();
}

/**
 * @brief Retrieves counter value.
 *
 * @return Counter::counter.
 */
size_t Counter::GetCounter()
{
	Enter();
	size_t returnMe = counter;
	Leave();
	return returnMe;
}

/**
 * @brief Retrieves the maximum that Counter::counter will be allowed to reach.
 *
 * @return Counter::counterLimit.
 */
size_t Counter::GetCounterLimit()
{
	Enter();
	size_t returnMe = counterLimit;
	Leave();
	return returnMe;
}

/**
 * @brief Retrieves the maximum that Counter::counter will be allowed to reach.
 *
 * @param newCounterLimit New counter limit.
 */
void Counter::SetCounterLimit(size_t newCounterLimit)
{
	Enter();
	counterLimit = newCounterLimit;
	Leave();
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool Counter::TestClass()
{
	cout << "Testing Counter class...\n";
	Counter counter(300,20);

	size_t startClock = clock();

	cout << "Counter limit: " << counter.GetCounterLimit() << '\n';
	cout << "Counter timeout: " << counter.GetTimeout() << '\n';

	while(clock() - startClock < 10000)
	{
		bool reset = counter.Increment();

		cout << "Counter " << counter.GetCounter() << ", reset: " << reset << '\n';

		// Pause that should not cause timeout.
		Sleep(150);

		// Ensure counter times out towards end of test.
		if(clock() - startClock > 6000)
		{
			Sleep(200);
		}
	}

	cout << "\n\n";
	return true;
}