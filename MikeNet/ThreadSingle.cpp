#include "FullInclude.h"

ConcurrentObject<size_t> ThreadSingle::threadLocalStorageAllocated((size_t)0);
DWORD ThreadSingle::threadLocalStorageIndex(0);

/**
 * @brief Constructor, starts a suspended thread.
 *
 * @param function Entry point for thread. A pointer to this object will be passed as a parameter.
 * @param parameter Parameter to be passed to thread. 
 * @param suspended If true the thread will not begin execution until Resume() is used.
 * @param manualThreadID Custom thread ID that can be retrieved by thread (optional, default = 0).
 */
ThreadSingle::ThreadSingle(LPTHREAD_START_ROUTINE function, void * parameter, size_t manualThreadID)
{
	// Threads are always started as suspended so that the thread does
	// not attempt to access the ThreadSingle object while it is still
	// being constructed.
	DWORD creationFlags = CREATE_SUSPENDED;
	this->suspended = true;

	this->parameter = parameter;
	this->running = true;
	this->manualThreadID = manualThreadID;
	this->threadError = NULL;

	AllocateThreadLocalStorage();

	// Pointer to this object is passed to thread so that it can check whether it should be running.
	// Parameter is stored, and it can access parameter as necassary.
	handle = CreateThread(0,0,function,this,creationFlags,&threadID);
	_ErrorException((handle==NULL),"creating a thread",WSAGetLastError(),__LINE__,__FILE__);
}

/**
 * @brief Destructor.
 *
 * Uses TerminateFriendly(true) and resumes the thread if necassary.
 */
ThreadSingle::~ThreadSingle(void)
{
	const char * cCommand = "an internal function (~ThreadSingle)";
	try
	{
		if(this->IsRunning() == true)
		{
			if(this->suspended == true)
			{
				Resume();
			}

			TerminateFriendly(true);
		}

		BOOL result = CloseHandle(handle);
		_ErrorException((result == FALSE),"closing a thread's handle",WSAGetLastError(),__LINE__,__FILE__);

		ClearError();

		DeallocateThreadLocalStorage();
	}
	MSG_CATCH
}

/**
 * @brief Retrieves a parameter intended for the thread which was set during construction.
 *
 * @return parameter.
 */
void * ThreadSingle::GetParameter() const
{
	return parameter;
}

/**
 * @brief Retrieves thread ID set by windows for thread.
 * 
 * @return thread ID.
 */
DWORD ThreadSingle::GetThreadID() const
{
	return threadID;
}

/**
 * @brief Forces the thread to exit.
 *
 * Use of this should be avoided where possible, favouring TerminateFriendly() instead.
 * This is because the freeing of resources make not properly occur.
 *
 * @param exitCode Exit code retrieved using GetExitCode() that thread should have.
 */
void ThreadSingle::TerminateForce(DWORD exitCode)
{
	BOOL result = TerminateThread(handle,exitCode);
	_ErrorException((result == FALSE),"terminating a thread",WSAGetLastError(),__LINE__,__FILE__);

	WaitForThreadToExit();
}

/**
 * @brief Requests that the thread exits.
 *
 * The thread is not guaranteed to exit as it may not be checking GetTerminateRequest().
 * If it is performing an operation it may also not exit straight away.
 * Use TerminateForce to guarantee quick thread exit.
 *
 * @param block If true this method will not return until the thread has exited.
 */
void ThreadSingle::TerminateFriendly(bool block)
{
	running = false;

	if(this->IsSuspended())
	{
		this->Resume();
	}

	if(block == true)
	{
		WaitForThreadToExit();
	}
}

/**
 * @brief Terminates the thread normally, first by attempting to use TerminateFriendly(), and then
 * if that fails using TerminateForce().
 *
 * @param timeout Length of time in milliseconds to wait after using TerminateFriendly() for the thread to terminate,
 * before using TerminateForce(). If TerminateForce() is used exit code will be 0.
 */
void ThreadSingle::TerminateNormal( clock_t timeout )
{
	TerminateFriendly(false);

	// Wait for thread to exit or timeout expire.
	clock_t clockAtStart = clock();
	while(IsRunning() == true && clock() - clockAtStart < timeout)
	{
		Sleep(1);
	}

	if(IsRunning() == true)
	{
		TerminateForce(0);
	}
}


/**
 * @brief	Determines whether the thread should exit.
 *
 * The thread should check this regularly and exit when this method returns true.
 *
 * @return	true if the thread should exit, false if not.
 */
bool ThreadSingle::GetTerminateRequest() const
{
	return !running.Get();
}

/**
 * @brief Retrieves the exit code of the thread.
 *
 * @return when a thread exits it returns a value, this method retrieves that value.
 */
DWORD ThreadSingle::GetExitCode() const
{
	DWORD returnMe;

	BOOL result = GetExitCodeThread(handle,&returnMe);
	_ErrorException((result == FALSE),"retrieving the exit code of a thread",WSAGetLastError(),__LINE__,__FILE__);

	return returnMe;
}

/**
 * @brief Determines whether the thread is running.
 *
 * @return true if the thread is running, false if not.
 */
bool ThreadSingle::IsRunning() const
{
	bool returnMe = false;

	// Query handle to see if thread is running.
	DWORD result = WaitForSingleObject(handle,0);
	_ErrorException((result == WAIT_FAILED),"determining if a thread is running",WSAGetLastError(),__LINE__,__FILE__);

	returnMe = (result == WAIT_TIMEOUT);

	return returnMe;
}

/**
 * @brief Does not return until the thread has exited.
 */
void ThreadSingle::WaitForThreadToExit()
{
	DWORD result = WaitForSingleObject(handle,INFINITE);
	_ErrorException((result != WAIT_OBJECT_0),"waiting for a thread to exit",WSAGetLastError(),__LINE__,__FILE__);
}

/**
 * @brief Determines whether the thread is suspended.
 *
 * @return true if the thread is suspended.
 */
bool ThreadSingle::IsSuspended() const
{
	return suspended;
}

/**
 * @brief Resumes execution of the thread after being suspended.
 *
 * Execution can be suspended using Suspend() and the thread may initially
 * be suspended when constructed.
 */
void ThreadSingle::Resume()
{
	if(IsSuspended())
	{
		DWORD result = ResumeThread(handle);
		_ErrorException((result == (DWORD)-1),"resuming a thread",WSAGetLastError(),__LINE__,__FILE__);

		suspended = false;
	}
}

/**
 * @brief Suspends execution of the thread.
 *
 * Execution can be resumed using Resume().
 *
 * @warning This method should be avoided
 * as it can cause deadlock in other threads since this thread may have
 * control of a critical section or mutex.
 */
void ThreadSingle::Suspend()
{
	if(!IsSuspended())
	{
		DWORD result = SuspendThread(handle);
		_ErrorException((result == (DWORD)-1),"suspending a thread",WSAGetLastError(),__LINE__,__FILE__);

		suspended = true;
	}
}

/**
 * @brief Retrieves a thread ID that is set manually.
 *
 * @return thread ID.
 */
size_t ThreadSingle::GetManualThreadID() const
{
	return manualThreadID;
}

/**
 * @brief	Allocates thread local storage.
 */
void ThreadSingle::AllocateThreadLocalStorage()
{
	threadLocalStorageAllocated.Enter();
	if(IsThreadLocalStorageAllocated() == false)
	{
		threadLocalStorageIndex = TlsAlloc();
	}
	// Indicate that another thread is using the local storage
	threadLocalStorageAllocated.Increase(1);

	threadLocalStorageAllocated.Leave();

	_ErrorException((threadLocalStorageIndex == TLS_OUT_OF_INDEXES),"allocating thread local storage, out of indexes",WSAGetLastError(),__LINE__,__FILE__);
}

/**
 * @brief	Determine if thread local storage has been allocated. 
 *
 * @return	true if thread local storage allocated, false if not. 
 */
bool ThreadSingle::IsThreadLocalStorageAllocated()
{
	return (threadLocalStorageAllocated.Get() > 0);
}

/**
 * @brief	Deallocates thread local storage. 
 */
void ThreadSingle::DeallocateThreadLocalStorage()
{
	BOOL result = TRUE;
	threadLocalStorageAllocated.Enter();

	// If we are the last thread that is using the local storage,
	// then deallocate it.
	if(threadLocalStorageAllocated == 1)
	{
		result = TlsFree(threadLocalStorageIndex);
	}

	// Indicate that a thread has stopped using the local storage.
	if(threadLocalStorageAllocated.Get() > 0)
	{
		threadLocalStorageAllocated.Decrease(1);
	}

	threadLocalStorageAllocated.Leave();
	_ErrorException((result == FALSE),"deallocating thread local storage",WSAGetLastError(),__LINE__,__FILE__);
}

/**
 * @brief	Gets the index used to identify thread local storage.
 *
 * @return	the thread local storage index. 
 */
DWORD ThreadSingle::GetThreadLocalStorageIndex()
{
	return threadLocalStorageIndex;
}


/**
 * @brief	Retrieves the value stored in thread local storage by the calling thread. 
 *
 * @return	the stored value for the calling thread.
 */
void * ThreadSingle::ThreadGetLocalStorageValue()
{
	_ErrorException((IsThreadLocalStorageAllocated() == false),"retrieving a thread local storage value, local storage is not allocated",0,__LINE__,__FILE__);

	void * value = TlsGetValue(threadLocalStorageIndex);
	_ErrorException((value == NULL && WSAGetLastError() != ERROR_SUCCESS),"retrieving a thread local storage value",WSAGetLastError(),__LINE__,__FILE__);
	return value;
}

/**
 * @brief	Sets the value to be stored in thread local storage by the calling thread.
 *
 * @param [in]	value	Value to be stored in thread local storage for calling thread. 
 */
void ThreadSingle::ThreadSetLocalStorageValue( void * value )
{
	_ErrorException((IsThreadLocalStorageAllocated() == false),"storing a thread local storage value, local storage is not allocated",0,__LINE__,__FILE__);

	BOOL result = TlsSetValue(threadLocalStorageIndex,value);
	_ErrorException((result == FALSE),"storing a thread local storage value",WSAGetLastError(),__LINE__,__FILE__);
}

/**
 * @brief	Calling thread updates its thread local storage with a pointer to its ThreadSingle
 * object. 
 *
 * @param [in]	object	ThreadSingle object that manages the calling thread.
 */
void ThreadSingle::ThreadSetCallingThread(ThreadSingle * object)
{
	ThreadSetLocalStorageValue(object);
}

/**
 * @brief	Retrieves the thread that is calling this method.
 *
 * @return	a pointer to a ThreadSingle object that manages the calling thread. If the main process
 * is the calling thread then NULL will be returned.
 */
ThreadSingle * ThreadSingle::GetCallingThread()
{
	return static_cast<ThreadSingle*>(ThreadGetLocalStorageValue());
}


/**
 * @brief Test function used by threads.
 *
 * @param lpParameter Pointer to ThreadSingle object.
 */
DWORD WINAPI ThreadSingleTestFunction(LPVOID lpParameter)
{
	ThreadSingle * thread = (ThreadSingle*)lpParameter;
	ThreadSingle::ThreadSetCallingThread(thread);

	Sleep(4000);

	Utility::output.Enter();
	cout << "ThreadSingle address: " << thread << ", " << "TLS address: " << ThreadSingle::GetCallingThread() << '\n';
	Utility::output.Leave();

	// Parameter
	Utility::output.Enter();
	cout << "Thread started with parameter of " << (DWORD)thread->GetParameter() << " and ID of " << thread->GetManualThreadID() << '\n';
	Utility::output.Leave();

	// Friendly termination
	while(thread->GetTerminateRequest() == false)
	{
		cout << "I am a thread, and I am running!" << '\n';
		Sleep(10);
	}

	Utility::output.Enter();
	cout << "I have terminated..\n";
	Utility::output.Leave();

	return (1234);
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool ThreadSingle::TestClass()
{
	DWORD speed = 0;

	cout << "Testing ThreadSingle class...\n";

	{
		// Construction not suspended
		Utility::output.Enter();
		cout << "Starting thread with parameter of 5000\n";
		Utility::output.Leave();
		ThreadSingle thread(&ThreadSingleTestFunction,(void*)5000,9999);
		thread.Resume();

		// Thread ID
		Utility::output.Enter();
		cout << "Thread ID is:" << thread.GetThreadID() << '\n';

		// Note: if thread is in the middle of using cout, deadlock may occur when using cout.
		// This is prevented with the Sleep() at the start of thread.
		thread.Suspend();
		cout << "Suspended thread.\n";

		cout << " Thread suspend status should be 1 and is: " << thread.IsSuspended() << '\n';
		Utility::output.Leave(); // Must cover Suspend so that thread is not suspended while using critical section.
		Sleep(speed);

		// Resume
		Utility::output.Enter();
		cout << "Resumed thread.\n";
		Utility::output.Leave();
		thread.Resume();

		Utility::output.Enter();
		cout << " Thread suspend status should be 0 and is: " << thread.IsSuspended() << '\n';
		Utility::output.Leave();
		Sleep(speed);

		// Friendly termination
		Utility::output.Enter();
		cout << "Terminating thread in a friendly way.\n";
		Utility::output.Leave();
		thread.TerminateFriendly(true);

		// Determine if thread is running
		Utility::output.Enter();
		cout << " Thread isRunning status should be 0 and is " << thread.IsRunning() << '\n';

		// Exit code
		cout << " Exit code should be 1234 and is: " << thread.GetExitCode() << '\n';
		Utility::output.Leave();
		Sleep(speed);
	}

	{	
		// Construction suspended
		Utility::output.Enter();
		cout << "Starting suspended thread with parameter of 4000\n";
		Utility::output.Leave();
		ThreadSingle thread(&ThreadSingleTestFunction,(void*)4000,true);
		Sleep(speed);

		// Resuming an initially suspended thread
		Utility::output.Enter();
		cout << "Resumed thread.\n";
		Utility::output.Leave();
		thread.Resume();

		// Determine if thread is running
		Utility::output.Enter();
		cout << " Thread isRunning status should be 1 and is " << thread.IsRunning() << '\n';
		Utility::output.Leave();
		Sleep(speed);

		// Forceful termination
		// Note: if thread is in the middle of using cout, deadlock may occur when using cout.
		// This is prevented with the Sleep() at the start of thread.
		Utility::output.Enter();
		cout << "Terminating thread forcefully with exit code of 1337.\n";
		thread.TerminateForce(1337);
		Utility::output.Leave(); // Must cover TerminateForce so that thread is not suspended while using critical section.

		// Exit code of forceful terminate
		Utility::output.Enter();
		cout << " Exit code should be 1337 and is: " << thread.GetExitCode() << '\n';
		Utility::output.Leave();
		Sleep(speed);
	}

	{
		// Terminate normally
		Utility::output.Enter();
		cout << "Starting thread with parameter of 12\n";
		Utility::output.Leave();
		ThreadSingle thread(&ThreadSingleTestFunction,(void*)12,false);
		
		Utility::output.Enter();
		cout << "Terminating normally with not enough time to do it friendly (check for deadlock)\n";
		// Note: if thread is in the middle of using cout, deadlock may occur when using cout.
		// This is prevented with the Sleep() at the start of thread.
		thread.TerminateNormal(500);
		Utility::output.Leave(); // Must cover TerminateNormal so that thread is not suspended while using critical section.

		Sleep(speed);
	}

	{
		// Terminate normally
		Utility::output.Enter();
		cout << "Starting thread with parameter of 12\n";
		Utility::output.Leave();
		ThreadSingle thread(&ThreadSingleTestFunction,(void*)12,false);

		
		Utility::output.Enter();
		cout << "Terminating normally with enough time to do it friendly (check for deadlock)\n";
		Utility::output.Leave();
		thread.TerminateNormal(8000);

		Sleep(speed);
	}

	cout << "\n";
	cout << "Logical cores: " << GetNumLogicalCores() << '\n';

	cout << "\n\n";
	return true;
}

/**
 * @brief	Used by the thread to store an error that has occurred.
 *
 * @param [in]	report	Error report describing error. Must be dynamically allocated and is now owned by this object.
 */
void ThreadSingle::SetError( ErrorReport * report )
{
	delete threadError.Get();
	this->threadError.Set(report);
}

/**
 * @brief	Retrieves a stored error report.
 *
 * @return	NULL if no report is stored.
 * @return  an error report describing the error.
 */
ErrorReport * ThreadSingle::GetError() const
{
	return threadError.Get();
}

/**
 * @brief	Determines whether an error report is stored.
 *
 * @return	true if a report is stored, false if not.
 */
bool ThreadSingle::IsErrorStored() const
{
	return threadError.Get() != NULL;
}

/**
 * @brief	Erases any currently stored error report.
 */
void ThreadSingle::ClearError()
{
	threadError.Enter();
		delete threadError.Get();
		threadError = NULL;
	threadError.Leave();
}

/**
 * @brief	Sets the terminate request. 
 */
void ThreadSingle::RequestTerminate()
{
	running = false;
}

/**
 * @brief	Gets the number logical cores on the system. 
 *
 * @return	the number of logical cores. 
 */
size_t ThreadSingle::GetNumLogicalCores()
{
	SYSTEM_INFO systemInfo;
	GetNativeSystemInfo(&systemInfo);

	return systemInfo.dwNumberOfProcessors;
}
