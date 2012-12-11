#include "FullInclude.h"

/**
 * @brief Test function for performance comparisons.
 *
 * @param lpParameter Pointer to ThreadSingle object, which contains pointer to ConcurrentObject to use.
 * @return number of Enter and Leave operations within hard coded time period.
 */
DWORD WINAPI ConcurrentObjectTestFunction(LPVOID lpParameter)
{
	ThreadSingle * thread = (ThreadSingle*)lpParameter;

	// Retrieve critical section to use
	ConcurrentObject<int*> * co = static_cast<ConcurrentObject<int*> *>(thread->GetParameter());

	// Count number of iterations
	DWORD count = 0;
	
	clock_t clockAtStart = clock();

	while(clock() - clockAtStart < 1000)
	{
		// Test Get()
		int * temp = co->Get();
		if(temp == NULL)
		{
			Utility::output.Enter();
			cout << "Variable = NULL, something is wrong.\n";
			Utility::output.Leave();
		}

		// Test Set()
		co->Set((int*)1000);

		co->Enter();
		co->Set(NULL);
		co->Set(temp);
		co->Leave();

		count++;
	}

	Utility::output.Enter();
	cout << "Thread " << thread->GetThreadID() << " terminated\n";
	Utility::output.Leave();
	return (count);
}