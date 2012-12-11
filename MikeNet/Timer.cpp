#include "FullInclude.h"

/**
 * @brief Constructor.
 *
 * @param freq Frequency that clock should tick over in milliseconds.
 */
Timer::Timer(clock_t freq)
{
	this->freq = freq;
	this->timer = clock();
}

/**
 * @brief Returns true when GetFreq() gap has expired, and then
 * false on subsequent calls until the gap expires again.
 *
 * @return true if clock has ticked over.
 * @return false if clock has not ticked over yet.
 */
bool Timer::GetState() const
{
	bool returnMe;
	Enter();
	if(clock() - timer > freq)
	{
		timer = clock();
		returnMe = true;
	}
	else
	{
		returnMe = false;
	}
	Leave();
	return returnMe;
}

/**
 * @brief Retrieves clock_t value updated the last time GetState() returned true.
 *
 * @return clock_t value updated the last time GetState() returned true.
 */
clock_t Timer::GetTimer() const
{
	Enter();
	clock_t returnMe = timer;
	Leave();
	return returnMe;
}

/**
 * @brief Updates Timer::timer setting its value to clock().
 */
void Timer::SetTimer()
{
	Enter();
	timer = clock();
	Leave();
}


/**
 * @brief Retrieves the length of time between each occurrence of GetState() returning true.
 *
 * @return frequency that clock should tick over in milliseconds.
 */
clock_t Timer::GetFreq() const
{
	Enter();
	clock_t returnMe = freq;
	Leave();
	return returnMe;
}

/**
 * @brief Changes the length of time between each occurrence of GetState() returning true.
 *
 * @param newFreq New frequency that clock should tick over in milliseconds.
 */
void Timer::SetFreq(clock_t newFreq)
{
	Enter();
	freq = newFreq;
	Leave();
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool Timer::TestClass()
{
	cout << "Testing Timer class...\n";
	Timer timer(1000);

	size_t startClock = clock();

	cout << "Timer frequency: " << timer.GetFreq() << '\n';
	cout << "Timer clock: " << timer.GetTimer() << '\n';

	while(clock() - startClock < 10000)
	{
		bool state = timer.GetState();

		if(state == true)
		{
			cout << "Timer state: " << state << '\n';
		}

		// Change frequency towards the end.
		if(clock() - startClock > 6000)
		{
			timer.SetFreq(200);
		}
	}

	cout << "\n\n";
	return true;
}