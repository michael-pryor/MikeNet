#include "FullInclude.h"

vector<size_t> ThreadSingleMessageKeepLastUser::threadsUsageCount;
StoreVector<StoreVector<ThreadSingleMessageKeepLast>> ThreadSingleMessageKeepLastUser::threads;

size_t ThreadSingleMessageKeepLastUser::CLASS_INDEX_PACKET = 0;
size_t ThreadSingleMessageKeepLastUser::CLASS_INDEX_SOUND = 1;

/**
 * @brief	Default constructor. 
 */
ThreadSingleMessageKeepLastUser::ThreadSingleMessageKeepLastUser()
	: usingThreads(false)
{
	classIndex = NULL;
}

/**
 * @brief	Constructor. 
 *
 * @param classIndex	Each class that extends this class must have a unique index
 * which is used to ensure that one class does not attempt to use another classes threads.
 * @param numThreads	Number of threads to setup.
 * @param function Entry point for thread. A pointer to this object will be passed as a parameter.
 * @param parameter Parameter to be passed to thread. 
 */
ThreadSingleMessageKeepLastUser::ThreadSingleMessageKeepLastUser(size_t classIndex, size_t numThreads, LPTHREAD_START_ROUTINE function, void * parameter)
	: usingThreads(false)
{
	SetupThreadsLocal(classIndex, numThreads, function, parameter);
}

/**
 * @brief	Destructor. 
 */
ThreadSingleMessageKeepLastUser::~ThreadSingleMessageKeepLastUser()
{
	const char * cCommand = "An internal function (~ThreadSingleMessageKeepLastUser)";
	
	try
	{
		CleanupThreadsLocal();
		lastMessage.Clear();
	}
	MSG_CATCH
}

/**
 * @brief Sets up threads for all objects, loading them into ThreadSingleMessageKeepLastUser::threads.
 *
 * If threads are already setup then does nothing except increase ThreadSingleMessageKeepLastUser::threadsUsageCount.
 *
 * @param classIndex	Each class that extends this class must have a unique index
 * which is used to ensure that one class does not attempt to use another classes threads.
 * @param numThreads Number of threads to setup.
 * @param function Entry point for thread. A pointer to this object will be passed as a parameter.
 * @param parameter Parameter to be passed to thread. 
 */
void ThreadSingleMessageKeepLastUser::SetupThreads(size_t classIndex, size_t numThreads, LPTHREAD_START_ROUTINE function, void * parameter)
{
	threads.Enter();

	try
	{
		if(threads.Size() <= classIndex)
		{
			threads.ResizeAllocate(classIndex+1);
			threadsUsageCount.resize(classIndex+1,0);
		}

		if(threadsUsageCount[classIndex] == 0)
		{
			threads[classIndex].Clear();
			for(size_t n = 0;n<numThreads;n++)
			{
				ThreadSingleMessageKeepLast * newThread = new (nothrow) ThreadSingleMessageKeepLast(function,parameter,n);
				Utility::DynamicAllocCheck(newThread,__LINE__,__FILE__);
				newThread->Resume();

				threads[classIndex].Add(newThread);
			}
			threadsUsageCount[classIndex] = 1;
		}
		else
		{
			threadsUsageCount[classIndex]++;
		}
	}
	// Release control of all objects before throwing exception
	catch (ErrorReport & error){threads.Leave(); throw(error);}
	catch (...){threads.Leave(); throw(-1);}
	threads.Leave();
}

/**
 * @brief	Sets up the threads for a single object (this object).
 *
 * Calls ThreadSingleMessageKeepLastUser::SetupThreads.
 *
 * @param classIndex	Each class that extends this class must have a unique index
 * which is used to ensure that one class does not attempt to use another classes threads.
 * @param numThreads Number of threads to setup.
 * @param function Entry point for thread. A pointer to this object will be passed as a parameter.
 * @param parameter Parameter to be passed to thread. 
 */
void ThreadSingleMessageKeepLastUser::SetupThreadsLocal(size_t classIndex, size_t numThreads, LPTHREAD_START_ROUTINE function, void * parameter)
{
	this->usingThreads.Enter();
	if(this->usingThreads == false)
	{
		this->classIndex = classIndex;

		SetupThreads(classIndex, numThreads,function,parameter);
		this->usingThreads = true;

		// One message per thread.
		lastMessage.Resize(numThreads);
	}
	this->usingThreads.Leave();
}

/**
 * @brief	Cleans up threads for all objects.
 *
 * @param classIndex	Each class that extends this class must have a unique index
 * which is used to ensure that one class does not attempt to use another classes threads.
 *
 * Does nothing if threads are not already setup.
 */
void ThreadSingleMessageKeepLastUser::CleanupThreads(size_t classIndex)
{
	threads[classIndex].Enter();
	try
	{
		if(threadsUsageCount[classIndex] > 0)
		{
			threadsUsageCount[classIndex]--;
			if(threadsUsageCount[classIndex] == 0)
			{
				threads[classIndex].Clear();
			}
		}
	}
	// Release control of all objects before throwing exception
	catch (ErrorReport & error){threads[classIndex].Leave(); throw(error);}
	catch (...){threads[classIndex].Leave(); throw(-1);}

	threads[classIndex].Leave();
}

/**
 * Cleans up the threads of all classes using this system.
 */
void ThreadSingleMessageKeepLastUser::CleanupThreadsForAllClasses()
{
	threads.Clear();
}


/**
 * @brief	Cleans up threads for a single object (this object).
 *
 * Calls ThreadSingleMessageKeepLastUser::CleanupThreads if this is the last object using the
 * threads that has not been destroyed.\n\n
 *
 * Does nothing if object never used threads.
 */
void ThreadSingleMessageKeepLastUser::CleanupThreadsLocal()
{
	usingThreads.Enter();
	try
	{
		if(usingThreads == true)
		{
			CleanupThreads(classIndex);
		
			for(size_t n = 0;n<lastMessage.Size();n++)
			{
				if(lastMessage.IsAllocated(n) == true)
				{
					bool shouldCleanup = lastMessage[n].ShouldSenderCleanup();

					if(shouldCleanup == true)
					{
						lastMessage.Deallocate(n);
					}
				}
			}
			usingThreads = false;
		}
	}
	catch(...) {usingThreads.Leave(); throw;}
	usingThreads.Leave();
}

/**
 * @brief	Does not return until the last operation has finished.
 */
void ThreadSingleMessageKeepLastUser::WaitUntilLastThreadOperationFinished()
{
	for(size_t n = 0;n<lastMessage.Size();n++)
	{
		if(lastMessage.IsAllocated(n) == true)
		{
			lastMessage[n].WaitUntilNotInUseByThread();
		}
	}
}

/**
 * @brief	Determines whether the last operation has finished.
 *
 * @return	true if the last operation has finished, false if not.
 */
bool ThreadSingleMessageKeepLastUser::IsLastThreadOperationFinished()
{
	bool isInUse = false;

	for(size_t n = 0;n<lastMessage.Size();n++)
	{
		if(lastMessage.IsAllocated(n) == true)
		{
			if(lastMessage[n].IsMessageInUseByThread() == true)
			{
				isInUse = true;
				break;
			}
		}
	}

	return !isInUse;
}

/**
 * @brief	Posts a message to the thread to be received using ThreadSingleMessage::GetMessageItem().
 *
 * @param	threadID ID of thread to post message to.
 * @param	message	The message to post.
 */
void ThreadSingleMessageKeepLastUser::PostMessageItem(size_t threadID, ThreadMessageItem * message)
{
	threads[classIndex][threadID].PostMessageItem(message,lastMessage.GetPtr(threadID));
}

/**
 * @brief	Retrieves the number of threads that are operational. 
 *
 * @return	the number of threads.
 */
size_t ThreadSingleMessageKeepLastUser::GetNumThreads()
{
	return threads[classIndex].Size();
}