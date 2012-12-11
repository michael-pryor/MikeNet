#pragma once
#include "ThreadSingleMessageKeepLast.h"

/**
 * @brief	Used by objects where all instances of that object share a group of threads.
 * @remarks	Michael Pryor, 9/21/2010. 
 */
class ThreadSingleMessageKeepLastUser
{
	/**
	 * @brief Number of objects using the threads.
	 *
	 * Usage begins when Encrypt or Decrypt is first used, and ends when the packet
	 * is destroyed. \n\n
	 *
	 * Elements are referenced by class ID.
	 * 
	 * Access is controlled using ThreadSingleMessageKeepLastUser::threads critical section.
	 */
	static vector<size_t> threadsUsageCount;

	/**
	 * @brief True if the object is using any threads.
	 *
	 * A thread may inherit this but not actually start any threads,
	 * it is optional.
	 */
	ConcurrentObject<bool> usingThreads;

	/**
	 * @brief Local store of the last message that was sent to the threads.
	 *
	 * We use a local store, instead of a global store stored by ThreadSingleMessageKeepLast so that
	 * each individual object can poll on its own last message, rather than polling on the
	 * last encryption message sent by any thread. \n\n
	 *
	 * A vector is needed because there must be one message per thread.
	 */
	StoreVector<ThreadMessageItem> lastMessage;

	/**
	 * @brief ID of class that extends this object.
	 *
	 * Is needed to ensure each class has its own set of threads
	 * which do not conflict.
	 */
	size_t classIndex;

	/**
	 * @brief Threads used by class.
	 *
	 * First dimension is the class ID.
	 * Second dimension is the thread ID.
	 */
	static StoreVector<StoreVector<ThreadSingleMessageKeepLast>> threads;
public:

	/**
	 * @brief Class index value for Packet.
	 */
	static size_t CLASS_INDEX_PACKET;

	/**
	 * @brief Class index value for SoundInstance.
	 */
	static size_t CLASS_INDEX_SOUND;

	ThreadSingleMessageKeepLastUser();
	ThreadSingleMessageKeepLastUser(size_t classIndex, size_t numThreads, LPTHREAD_START_ROUTINE function, void * parameter);
	virtual ~ThreadSingleMessageKeepLastUser();
protected:
	static void SetupThreads(size_t classIndex, size_t numThreads, LPTHREAD_START_ROUTINE function, void * parameter);
	void SetupThreadsLocal(size_t classIndex, size_t numThreads, LPTHREAD_START_ROUTINE function, void * parameter);
	static void CleanupThreads(size_t classIndex);
	
public:
	static void CleanupThreadsForAllClasses();
protected:
	void CleanupThreadsLocal();

	void PostMessageItem(size_t threadID, ThreadMessageItem * message);
	size_t GetNumThreads();
	void WaitUntilLastThreadOperationFinished();
	bool IsLastThreadOperationFinished();
};

