#include "FullInclude.h"

/**
 * @brief Constructor.
 */
ConcurrencyControlSimple::ConcurrencyControlSimple()
{
	numReading = 0;
}

/**
 * @brief Destructor.
 */
ConcurrencyControlSimple::~ConcurrencyControlSimple()
{

}

/**
 * @brief Take read control of object.
 *
 * Write control will not be possible by other threads until LeaveRead() is used.
 * EnterRead can be used multiple times by the same thread before LeaveRead() but is is essential that the same number of LeaveRead()'s
 * as EnterRead()'s are used to prevent deadlock, since threads that want to take write control will wait for all read control to
 * be released.\n\n
 *
 * In contrast to ConcurrencyControl, an additional parameter @a readCount is needed. If this is undesirable try
 * using ConcurrencyControl.
 *
 * @param [out] readCount Increased by 1. This is used by a thread to keep track
 * of how many calls to EnterRead() it has made, so that EnterWrite() can correctly
 * take read control if it is the only thread that has read control.\n\n
 *
 * This parameter does not always need to be 'accurate'. This means
 * that if a method takes and releases read control before it returns it is
 * okay to pass readCount as 0. ReadCount will increase by 1 and then
 * decrease by 1 and it doesn't matter if the thread already has
 * read control before the method call.
 */
void ConcurrencyControlSimple::EnterRead(size_t & readCount) const
{
	// Take write control temporarily, to ensure that no thread
	// is writing whilst we take read control.
	//
	// Since we are not actually going to do anything that involves writing
	// we do not use EnterWrite() method as this would require readCount
	// to be fully accurate (see method description for exactly what I mean
	// by 'accurate'). We just need to know that no other thread is writing
	// or start writing while we take read control.
	writing.Enter();

	// Take read control
	mtControl.Enter();
		numReading++;
	mtControl.Leave();

	// Release write control now that we have fully taken read control
	writing.Leave();

	readCount++;
}

/**
 * @brief Release read control of object.
 *
 * Read control is taken using EnterRead().
 *
 * @warning This method MUST NOT be used unless the calling thread already has read control.
 *
 * @param [out] readCount Decreased by 1. This is used by a thread to keep track
 * of how many calls to EnterRead() it has made, so that EnterWrite() can correctly
 * take read control if it is the only thread that has read control.\n\n
 *
 * This parameter does not always need to be 'accurate'. This means
 * that if a method takes and releases read control before it returns it is
 * okay to pass readCount as 0. ReadCount will increase by 1 and then
 * decrease by 1 and it doesn't matter if the thread already has
 * read control before the method call.
 */
void ConcurrencyControlSimple::LeaveRead(size_t & readCount) const
{
	mtControl.Enter();
		numReading--;
	mtControl.Leave();

	readCount--;
}

/**
 * @brief Take write control of object.
 *
 * Write control cannot be taken unless no other thread has read or write control.
 * The method will block until write control can be taken. \n\n
 *
 * It is essential that a call to EnterWrite() is counteracted by a call to LeaveWrite().
 *
 * @param readCount This is used by a thread to keep track
 * of how many calls to EnterRead() it has made, so that EnterWrite() can correctly
 * take read control if it is the only thread that has read control.
 *
 * This parameter does not always need to be calculated. This means
 * that if it is certain that readCount is 0, a method can simple pass 0
 * directly to this method without calculating the value traditionally
 * by passing a reference to EnterRead() and LeaveRead(). It is essential
 * however that the readCount is correct in order to prevent deadlock.
 */
void ConcurrencyControlSimple::EnterWrite(size_t readCount)
{

	size_t originalReadCount = readCount;

	// Release read control to prevent deadlock if two threads have read control
	// and attempt to take write control at the same time.
	while(readCount > 0)
	{
		LeaveRead(readCount);
	}

EnterWriteTryAgain:
	// Before proceeding, ensure that all other threads have released
	// read control.
	while(IsAnyoneElseReading(readCount) == true)
	{
		Sleep(1);
	}

	// Take write control, no other threads will be able to take
	// read or write control at this point.
	writing.Enter();
	
	// Check that between waiting for read control to be released by other threads
	// and taking write control, no other thread has taken read control.
	// We cannot take write control before waiting for read control to be released
	// because this can cause deadlock in the event that a thread trys to take
	// another layer of read control (i.e. multiple EnterRead() calls).
	if(IsAnyoneElseReading(readCount) == true)
	{
		writing.Leave();
		goto EnterWriteTryAgain;
	}

	// Retake read control
	while(readCount < originalReadCount)
	{
		EnterRead(readCount);
	}
}

/**
 * @brief Release write control of object.
 *
 * Write control MUST NOT be released unless the calling thread has write control.
 *
 * This parameter does not always need to be calculated. This means
 * that if it is certain that readCount is 0, a method can simple pass 0
 * directly to this method without calculating the value traditionally
 * by passing a reference to EnterRead() and LeaveRead(). It is essential
 * however that the readCount is correct in order to prevent deadlock.
 */
void ConcurrencyControlSimple::LeaveWrite()
{
	writing.Leave();
}

/**
 * @brief Returns false if no other thread has read control.
 *
 * @return false if no other thread has read control.
 */
bool ConcurrencyControlSimple::IsAnyoneElseReading(size_t readCount) const
{
	bool returnMe = false;

	// Before proceeding, ensure that all other threads have released
	// read control.
	mtControl.Enter();

	try
	{
		_ErrorException((numReading < readCount),"executing ConcurrencyControlSimple::IsAnyoneElseReading, readCount parameter is less than numReading",0,__LINE__,__FILE__);
		returnMe = !(numReading == readCount);
	}
	catch (ErrorReport & error)
	{
		mtControl.Leave();
		throw(error);
	}

	mtControl.Leave();
	return returnMe;
}






size_t * globalInteger2;
/**
 * @brief Test function for performance comparisons.
 *
 * @param lpParameter Pointer to ThreadSingle object, which contains pointer to CriticalSection to use.
 * @return number of Enter and Leave operations within hard coded time period.
 */
DWORD WINAPI ConcurrencyControlSimpleTestFunction(LPVOID lpParameter)
{
	ThreadSingle * thread = (ThreadSingle*)lpParameter;

	// Retrieve critical section to use
	ConcurrencyControlSimple * control = static_cast<ConcurrencyControlSimple*>(thread->GetParameter());

	// Count iterations
	DWORD count = 0;
	
	clock_t clockAtStart = clock();
	size_t * oldGlobalInteger = globalInteger2;

	size_t readCount = 0;

	while(clock() - clockAtStart < 1000)
	{
		// Multiple levels of read control.
		control->EnterRead(readCount);
			size_t temp = *globalInteger2;
		control->LeaveRead(readCount);

		// Multiple levels of read control
		control->EnterRead(readCount);
		control->EnterRead(readCount);
			temp = *globalInteger2;
		control->LeaveRead(readCount);
		control->LeaveRead(readCount);

		// Single level of write control.
		control->EnterWrite(readCount);
			globalInteger2 = NULL;
			globalInteger2 = oldGlobalInteger;
			*globalInteger2 = readCount;
		control->LeaveWrite();

		// Single level of read and write control at the same time.
		control->EnterRead(readCount);
			control->EnterWrite(readCount);
				globalInteger2 = NULL;
				globalInteger2 = oldGlobalInteger;
				*globalInteger2 = readCount;
			control->LeaveWrite();
		control->LeaveRead(readCount);

		// Multiple levels of read and write control at the same time.
		control->EnterRead(readCount);
		control->EnterRead(readCount);
			control->EnterWrite(readCount);
			control->EnterWrite(readCount);
				globalInteger2 = NULL;
				globalInteger2 = oldGlobalInteger;
				*globalInteger2 = readCount;
			control->LeaveWrite();
			control->LeaveWrite();
		control->LeaveRead(readCount);
		control->LeaveRead(readCount);

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
bool ConcurrencyControlSimple::TestClass()
{
	cout << "Testing ConcurrencyControlSimple class...\n";

	globalInteger2 = new size_t(0);

	// Multiple thread usage.
	{
		const size_t numThreads = 20;
		ConcurrencyControlSimple control;

		cout << "Running threads...\n";

		// Create group of threads
		ThreadSingleGroup threads;

		for(size_t n = 0;n<numThreads;n++)
		{
			ThreadSingle * thread = new (nothrow) ThreadSingle(&ConcurrencyControlSimpleTestFunction,&control);
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

	delete globalInteger2;
	return true;
}