#include "FullInclude.h"

/**
 * @brief	Default constructor. 
 */
ThreadMessageItemShutdown::ThreadMessageItemShutdown(ThreadSingleMessage * shutMeDown)
{
	this->shutMeDown = shutMeDown;
}

/**
 * @brief	Destructor.
 */
ThreadMessageItemShutdown::~ThreadMessageItemShutdown()
{

}

/**
 * @brief	Requests that the thread terminates.
 *
 * @return	NULL. 
 */
void * ThreadMessageItemShutdown::TakeAction()
{
	shutMeDown->RequestTerminate();
	return NULL;
}
