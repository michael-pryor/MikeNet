#include "FullInclude.h"

/**
 * @brief Constructor.
 */
CriticalSection::CriticalSection()
{
	BOOL bResult = InitializeCriticalSectionAndSpinCount(&CT,SPIN_COUNT);
	_ErrorException((bResult == FALSE),"initializing a critical section",WSAGetLastError(),__LINE__,__FILE__);

#ifdef _DEBUG
	controlCount = 0;
#endif
}

/**
 * @brief Destructor.
 */
CriticalSection::~CriticalSection()
{
	const char * cCommand = "an internal function (~CriticalSection)";

#ifdef _DEBUG
	_ErrorException((controlCount > 0),"cleaning up a critical section object, critical section is still being used",0,__LINE__,__FILE__);
#endif

	try
	{
		//cout << "Deleting critical section..\n";
		DeleteCriticalSection(&CT);
	}
	MSG_CATCH
}

/**
 * @brief Take control of critical section.
 *
 * Other threads attempting to take control
 * must wait for any controlling thread
 * to release control.
 */
void CriticalSection::Enter() const
{
	EnterCriticalSection(&CT);

#ifdef _DEBUG
	controlCount++;
#endif
}

/**
 * @brief Release control of critical section.
 *
 * Other threads attempting to take control
 * must wait for any controlling thread
 * to release control.
 */
void CriticalSection::Leave() const
{
#ifdef _DEBUG
	_ErrorException((controlCount < 1),"releasing control of a critical section, the calling thread does not have control",0,__LINE__,__FILE__);
	controlCount--;
#endif

	LeaveCriticalSection(&CT);
}

/**
 * @brief Test function for performance comparisons.
 *
 * @param lpParameter Pointer to ThreadSingle object, which contains pointer to CriticalSection to use.
 * @return number of Enter and Leave operations within hard coded time period.
 */
DWORD WINAPI CriticalSectionTestFunction(LPVOID lpParameter)
{
	ThreadSingle * thread = (ThreadSingle*)lpParameter;

	// Retrieve critical section to use
	CriticalSection * cs = static_cast<CriticalSection*>(thread->GetParameter());

	// Count number of iterations
	DWORD count = 0;
	
	clock_t clockAtStart = clock();

	while(clock() - clockAtStart < 1000)
	{
		cs->Enter();
		cs->Leave();
		count++;
	}

	Utility::output.Enter();
	cout << "Thread " << thread->GetThreadID() << " terminated\n";
	Utility::output.Leave();
	return (count);
}
/**
 * @brief Tests class.
 *
 * Primarily intended for performance comparisons.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool CriticalSection::TestClass()
{
	cout << "Testing CriticalSection class...\n";

	cout << "Running threads\n";
	CriticalSection cs;

	// Create group of threads
	const size_t numThreads = 5;

	ThreadSingleGroup threads;

	for(size_t n = 0;n<numThreads;n++)
	{
		ThreadSingle * thread = new (nothrow) ThreadSingle(&CriticalSectionTestFunction,&cs,false);
		Utility::DynamicAllocCheck(thread,__LINE__,__FILE__);
		thread->Resume();
		threads.Add(thread);
	}

	// Wait for threads to exit
	threads.WaitForThreadsToExit();

	// Total up return value
	size_t total = 0;
	for(size_t n = 0;n<numThreads;n++)
	{
		total += threads[n].GetExitCode();
		cout << "Count for thread " << n << " is " << threads[n].GetExitCode() << '\n';
	}

	cout << "Total: " << total << '\n';

	cout << "\n\n";
	return true;
}
