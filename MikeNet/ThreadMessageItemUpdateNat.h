#pragma once
#include "threadmessageitem.h"

/**
 * @brief	Message for use with ThreadSingleMessage.
 * @remarks	Michael Pryor, 9/17/2010. 
 */
class ThreadMessageItemUpdateNat :
	public ThreadMessageItem
{
	/**
	 * @brief Object to take action on.
	 */
	UpnpNatAction * takeActionOnMe;
public:
	ThreadMessageItemUpdateNat(UpnpNatAction * takeActionOnMe);
	~ThreadMessageItemUpdateNat(void);

	void * TakeAction();
};

