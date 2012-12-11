#include "FullInclude.h"

/**
 * @brief Resets all variables to default.
 */
void ConcurrencyControl::DefaultVariables()
{
	threads = 0;
	constructed = false;
	access = NULL;
	accessRead = NULL;
	accessWrite = NULL;
}

/**
 * @brief Checks that thread ID is in bounds, and throws an exception if it is not.
 *
 * @param threadID ID of thread to check.
 */
void ConcurrencyControl::ValidateThreadID(size_t threadID) const
{
#ifdef _DEBUG
	_ErrorException((threadID >= threads),"using a ConcurrencyControl object, invalid thread ID specified",0,__LINE__,__FILE__);
#endif
}

/**
 * @brief Constructor, sets variables to default and prepares object ready for use by one thread.
 */
ConcurrencyControl::ConcurrencyControl()
{
	DefaultVariables();
	Construct(1);
}

/**
 * @brief Constructor, sets variables to default and prepares object ready for use by multiple threads.
 *
 * @param paraThreads Number of threads that will be using object.
 */
ConcurrencyControl::ConcurrencyControl(size_t paraThreads)
{
	DefaultVariables();
	Construct(paraThreads);
}

/**
 * @brief Sets up object ready for use.
 * 
 * @param paraThreads Number of threads that will be using object.
 */
void ConcurrencyControl::Construct(size_t paraThreads)
{
	constructed = true;
	threads = paraThreads;

	access = new (nothrow) CriticalSection[threads];
	Utility::DynamicAllocCheck(access,__LINE__,__FILE__);

	accessRead = new (nothrow) size_t[threads];
	Utility::DynamicAllocCheck(accessRead,__LINE__,__FILE__);
	
	accessWrite = new (nothrow) size_t[threads];
	Utility::DynamicAllocCheck(accessWrite,__LINE__,__FILE__);

	for(size_t n = 0;n<threads;n++)
	{
		accessRead[n] = 0;
		accessWrite[n] = 0;
	}
}

/**
 * @brief Cleans up object and resets variables to default.
 *
 * Object can be reused if @c Construct is used again.
 */
void ConcurrencyControl::Cleanup()
{
	if(constructed == true)
	{
#ifdef _DEBUG
		for(size_t n = 0;n<threads;n++)
		{
			_ErrorException((accessRead[n] > 0),"cleaning up a ConcurrencyControl object, object is still in use (read)",0,__LINE__,__FILE__);
			_ErrorException((accessWrite[n] > 0),"cleaning up a ConcurrencyControl object, object is still in use (write)",0,__LINE__,__FILE__);
		}
#endif

		// Deallocate memory
		delete[] access;
		delete[] accessRead;
		delete[] accessWrite;

		// Reset variables
		DefaultVariables();
	}
}

/**
 * @brief Destructor, cleans up object.
 */
ConcurrencyControl::~ConcurrencyControl()
{
	const char * cCommand = "an internal function (~ConcurrencyControl)";
	try
	{
		Cleanup();
	}
	MSG_CATCH
}

/**
 * @brief	Retrieves the calling thread's manual thread ID.
 *
 * @return	the calling thread's manual thread ID.
 */
size_t ConcurrencyControl::GetThreadID() const
{
	ThreadSingle * thread = ThreadSingle::GetCallingThread();
	
	// NULL indicates main process.
	if(thread == NULL)
	{
		return NetUtility::GetMainProcessThreadID();
	}

	return thread->GetManualThreadID();
}


/**
 * @brief Take read control of object.
 */
void ConcurrencyControl::EnterRead() const
{
	size_t threadID = GetThreadID();

	ValidateThreadID(threadID);
	access[threadID].Enter();
	accessRead[threadID]++;
}
/**
 * @brief Release read control of object.
 */
void ConcurrencyControl::LeaveRead() const
{
	size_t threadID = GetThreadID();

	ValidateThreadID(threadID);

#ifdef _DEBUG
	_ErrorException((accessRead[threadID] < 1),"executing ConcurrencyControl::LeaveRead, thread does not have read control",0,__LINE__,__FILE__);
#endif

	accessRead[threadID]--;
	access[threadID].Leave();
}
/**
 * @brief Safely take write control of object.
 *
 * Read control will be released before taking write control,
 * and then retaken (to the same depth) after taking write control.
 * This is done in order to prevent deadlock.
 */
void ConcurrencyControl::EnterWrite()
{
	size_t threadID = GetThreadID();

	ValidateThreadID(threadID);

	// Ensure that thread doesn't have read control
	// in order to prevent deadlock regarding multiple levels
	// of access.
	size_t OLD_ACCESS_Read = accessRead[threadID];
	while(accessRead[threadID] > 0)
	{
		LeaveRead();
	}

	// Take write control	
	for(size_t n = 0;n<threads;n++)
	{
		access[n].Enter();
	}

	accessWrite[threadID]++;
	
	// Retake original read control
	for(size_t n = 0;n<OLD_ACCESS_Read;n++)
	{
		EnterRead();
	}
}

/**
 * @brief Release write control of object.
 */
void ConcurrencyControl::LeaveWrite()
{
	size_t threadID = GetThreadID();

	ValidateThreadID(threadID);
#ifdef _DEBUG
	_ErrorException((accessWrite[threadID] < 1),"executing ConcurrencyControl::LeaveWrite, thread does not have write control",0,__LINE__,__FILE__);
#endif

	accessWrite[threadID]--;

	for(size_t n = 0;n<threads;n++)
	{
		access[n].Leave();
	}
}

/**
 * @brief Retrieves the number of threads setup to use this object. 
 *
 * @return the number of threads setup to use this object.
 *
 * @note Thread ID's for this object range from inclusive 0 to exclusive number of threads.
 */
size_t ConcurrencyControl::GetNumThreads() const
{
	return threads;
}


size_t * globalInteger;
/**
 * @brief Test function for performance comparisons.
 *
 * @param lpParameter Pointer to ThreadSingle object, which contains pointer to CriticalSection to use.
 * @return number of Enter and Leave operations within hard coded time period.
 */
DWORD WINAPI ConcurrencyControlTestFunction(LPVOID lpParameter)
{
	ThreadSingle * thread = (ThreadSingle*)lpParameter;
	size_t threadID = thread->GetManualThreadID();
	ThreadSingle::ThreadSetCallingThread(thread);

	// Retrieve critical section to use
	ConcurrencyControl * control = static_cast<ConcurrencyControl*>(thread->GetParameter());

	// Count iterations
	DWORD count = 0;
	
	clock_t clockAtStart = clock();
	size_t * oldGlobalInteger = globalInteger;

	while(clock() - clockAtStart < 1000)
	{
		// Multiple levels of read control.
		control->EnterRead();
			size_t temp = *globalInteger;
		control->LeaveRead();

		// Multiple levels of read control
		control->EnterRead();
		control->EnterRead();
			temp = *globalInteger;
		control->LeaveRead();
		control->LeaveRead();

		// Single level of write control.
		control->EnterWrite();
			globalInteger = NULL;
			globalInteger = oldGlobalInteger;
			*globalInteger = threadID;
		control->LeaveWrite();

		// Single level of read and write control at the same time.
		control->EnterRead();
			control->EnterWrite();
				globalInteger = NULL;
				globalInteger = oldGlobalInteger;
				*globalInteger = threadID;
			control->LeaveWrite();
		control->LeaveRead();

		// Multiple levels of read and write control at the same time.
		control->EnterRead();
		control->EnterRead();
			control->EnterWrite();
			control->EnterWrite();
				globalInteger = NULL;
				globalInteger = oldGlobalInteger;
				*globalInteger = threadID;
			control->LeaveWrite();
			control->LeaveWrite();
		control->LeaveRead();
		control->LeaveRead();

		count++;
	}

	Utility::output.Enter();
	cout << "Thread " << threadID << " terminated\n";
	Utility::output.Leave();
	return (count);
}



/**
 * @brief Tests class.
 *
 * It is difficult to test for race condition but this is done by changing
 * a pointer; if a thread does not wait for the pointer to be changed back, to point
 * to something meaningful, the debugger will detect writing to a bad memory location.
 * The debugger does not seem to detect two threads modifying the same variable at the same time. \n\n
 *
 * When running this test you should check for:
 * - Debugger alerts.
 * - Deadlock.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool ConcurrencyControl::TestClass()
{
	cout << "Testing ConcurrencyControl class...\n";

	globalInteger = new size_t(0);

	// Multiple thread usage.
	{
		const size_t numThreads = 4;
		ConcurrencyControl control(numThreads);

		cout << "Running threads...\n";

		// Create group of threads
		ThreadSingleGroup threads;

		for(size_t n = 0;n<numThreads;n++)
		{
			ThreadSingle * thread = new ThreadSingle(&ConcurrencyControlTestFunction,&control,n);
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
	}

	delete globalInteger;
	return true;
}