#pragma once
#include "threadmessageitem.h"

/**
 * @brief	Message used to tell thread to terminate.
 * @remarks	Michael Pryor, 9/16/2010. 
 */
class ThreadMessageItemShutdown :
	public ThreadMessageItem
{
	ThreadSingleMessage * shutMeDown;

public:
	ThreadMessageItemShutdown(ThreadSingleMessage * shutMeDown);
	~ThreadMessageItemShutdown();

	void * TakeAction();
};

