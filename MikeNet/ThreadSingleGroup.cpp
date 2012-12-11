#include "FullInclude.h"

/**
 * @brief Constructor.
 */
ThreadSingleGroup::ThreadSingleGroup()
{

}

/**
 * @brief Destructor.
 */
ThreadSingleGroup::~ThreadSingleGroup(void)
{
}

/**
 * @brief Forces all threads in this group to exit.
 *
 * Use of this should be avoided where possible, favouring TerminateFriendly() instead.
 * This is because the freeing of resources make not properly occur.
 *
 * @param exitCode Exit code retrieved using ThreadSingle::GetExitCode() that thread should have.
 */
void ThreadSingleGroup::TerminateForce( DWORD exitCode )
{
	for(size_t n = 0;n<this->Size();n++)
	{
		(*this)[n].TerminateForce(exitCode);
	}
}

/**
 * @brief Requests that all threads in this group exit.
 *
 * Threads are not guaranteed to exit as they may not be checking GetTerminateRequest().
 * If they are performing an operation they may also not exit straight away.
 * Use TerminateForce to guarantee quick thread exit.
 *
 * @param block If true this method will not return until the thread has exited.
 */
void ThreadSingleGroup::TerminateFriendly( bool block )
{
	for(size_t n = 0;n<this->Size();n++)
	{
		(*this)[n].TerminateFriendly(block);
	}
}

/**
 * @brief Terminates the thread normally, first by attempting to use TerminateFriendly(), and then
 * if that fails using TerminateForce().
 *
 * @param timeout Length of time in milliseconds to wait after using TerminateFriendly() for the threads to terminate,
 * before using TerminateForce(). If TerminateForce() is used exit code will be 0.
 */
void ThreadSingleGroup::TerminateNormal( clock_t timeout )
{
	clock_t startClock = clock();

	for(size_t n = 0;n<this->Size();n++)
	{
		// Calculate time left before timeout expires
		// If timeout has expired then stop terminating
		clock_t timeTaken = clock() - startClock;
		clock_t timeLeft = timeout - timeTaken;

		if(timeout > timeTaken)
		{
			TerminateForce(0);
		}

		(*this)[n].TerminateNormal(timeLeft);
	}
}

/**
 * @brief Determines whether all threads in the group are running.
 *
 * @return true if all threads in the group are running, false if not.
 */
bool ThreadSingleGroup::IsRunning() const
{
	bool running = true;
	for(size_t n = 0;n<this->Size();n++)
	{
		if((*this)[n].IsRunning() == false)
		{
			running = false;
			break;
		}
	}

	return running;
}

/**
 * @brief Determines whether all threads in the group are suspended.
 *
 * @return true if all threads in the group are suspended.
 */
bool ThreadSingleGroup::IsSuspended() const
{
	bool suspended = true;
	for(size_t n = 0;n<this->Size();n++)
	{
		if((*this)[n].IsSuspended() == false)
		{
			suspended = false;
			break;
		}
	}

	return suspended;
}

/**
 * @brief Resumes execution of all threads in the group after being suspended.
 *
 * Execution can be suspended using Suspend() and a thread may initially
 * be suspended when constructed.
 */
void ThreadSingleGroup::Resume()
{
	for(size_t n = 0;n<this->Size();n++)
	{
		(*this)[n].Resume();
	}
}

/**
 * @brief Suspends execution of all threads in the group.
 *
 * Execution can be resumed using Resume().
 *
 * @warning This method should be avoided
 * as it can cause deadlock in other threads since a thread may have
 * control of a critical section or mutex.
 */
void ThreadSingleGroup::Suspend()
{
	for(size_t n = 0;n<this->Size();n++)
	{
		(*this)[n].Suspend();
	}
}

/**
 * @brief Does not return until all threads have exited.
 */
void ThreadSingleGroup::WaitForThreadsToExit()
{
	for(size_t n = 0;n<this->Size();n++)
	{
		(*this)[n].WaitForThreadToExit();
	}
}

/**
 * @brief Test function used by threads.
 *
 * @param lpParameter Pointer to ThreadSingle object.
 */
DWORD WINAPI ThreadSingleGroupTestFunction(LPVOID lpParameter)
{
	ThreadSingle * thread = (ThreadSingle*)lpParameter;
	thread->ThreadSetCallingThread(thread);

	Sleep(4000);

	// Parameter
	Utility::output.Enter();
	cout << "ThreadSingle address: " << thread << ", " << "TLS address: " << ThreadSingle::GetCallingThread() << '\n';
	cout << "Thread started with parameter of " << (DWORD)thread->GetParameter() << '\n';
	Utility::output.Leave();

	// Friendly termination
	while(thread->GetTerminateRequest() == false)
	{
		cout << "I am a thread, and I am running!" << '\n';
		Sleep(100);
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
bool ThreadSingleGroup::TestClass()
{
	cout << "Testing ThreadSingleGroup class...\n";

	DWORD speed = 0;
	{
		ThreadSingleGroup group;

		// Construction not suspended
		Utility::output.Enter();
		cout << "Starting threads and adding them to group.\n";
		Utility::output.Leave();

		ThreadSingle * thread1 = new (nothrow) ThreadSingle(&ThreadSingleGroupTestFunction,(void*)5000);
		ThreadSingle * thread2 = new (nothrow) ThreadSingle(&ThreadSingleGroupTestFunction,(void*)3000);
		ThreadSingle * thread3 = new (nothrow) ThreadSingle(&ThreadSingleGroupTestFunction,(void*)4000);
		thread1->Resume();
		thread2->Resume();
		thread3->Resume();
		group.Add(thread1);
		group.Add(thread2);
		group.Add(thread3);
		
		// Suspend
		Utility::output.Enter();

		// Note: if thread is in the middle of using cout, deadlock may occur when using cout.
		// This is prevented with the Sleep() at the start of thread.
		group.Suspend(); 

		cout << "Suspended group.\n";
		cout << " Group suspend status should be 1 and is: " << group.IsSuspended() << '\n';
		Utility::output.Leave(); // Must cover Suspend so that thread is not suspended while using critical section.

		Sleep(speed);

		// Resume
		Utility::output.Enter();
		cout << "Resumed group.\n";
		Utility::output.Leave();
		group.Resume();

		Utility::output.Enter();
		cout << " Group suspend status should be 0 and is: " << group.IsSuspended() << '\n';
		Utility::output.Leave();

		Sleep(speed);

		// Friendly termination
		Utility::output.Enter();
		cout << "Terminating group in a friendly way.\n";
		Utility::output.Leave();

		group.TerminateFriendly(true);

		// Determine if thread is running
		Utility::output.Enter();
		cout << " Group isRunning status should be 0 and is " << group.IsRunning() << '\n';
		Utility::output.Leave();
	}

	{
		ThreadSingleGroup group;

		// Terminate normally
		Utility::output.Enter();
		cout << "Starting threads and adding them to group.\n";
		Utility::output.Leave();

		ThreadSingle * thread1 = new (nothrow) ThreadSingle(&ThreadSingleGroupTestFunction,(void*)5000);
		ThreadSingle * thread2 = new (nothrow) ThreadSingle(&ThreadSingleGroupTestFunction,(void*)3000);
		ThreadSingle * thread3 = new (nothrow) ThreadSingle(&ThreadSingleGroupTestFunction,(void*)4000);
		thread1->Resume();
		thread2->Resume();
		thread3->Resume();
		group.Add(thread1);
		group.Add(thread2);
		group.Add(thread3);
		
		Utility::output.Enter();
		cout << "Terminating group normally with not enough time to do it friendly.\n";

		group.TerminateNormal(1000);
		Utility::output.Leave(); // Must cover TerminateNormal so that thread releases critical section.


		Sleep(speed);
	}

	cout << "\n\n";
	return true;
}