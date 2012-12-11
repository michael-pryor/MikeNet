#pragma once
#include "threadsingle.h"
#include "ThreadMessageItem.h"

/**
 * @brief	Extends ThreadSingle with additional functionality for threads to receive and be sent ThreadMessageItem messages.
 * @remarks	Michael Pryor, 9/16/2010. 
 */
class ThreadSingleMessage :
	public ThreadSingle
{
	/**
	 * @brief Signaled when the thread is ready to receive messages.
	 *
	 * Until the thread has used GetMessageItem, PostMessageItem will fail. This is because.
	 * GetMessageItem initializes the message queue.
	 */
	ConcurrencyEvent threadReady;
public:
	/**
	 * @brief Value to pass to windows functions to ensure that messages dealt with
	 * are only ones that are intended specifically for this thread.
	 */
	static const HWND THIS_QUEUE_ONLY;

	ThreadSingleMessage(LPTHREAD_START_ROUTINE function, void * parameter, size_t manualThreadID=0);
	virtual ~ThreadSingleMessage(void);

	virtual void PostMessageItem(ThreadMessageItem * message);
	ThreadMessageItem * GetMessageItem();
	bool IsQueueEmpty() const;

	bool GetTerminateRequest() const;

	virtual void TerminateFriendly(bool block);

	void WaitForThreadToBeReady();

	static bool TestClass();
};

