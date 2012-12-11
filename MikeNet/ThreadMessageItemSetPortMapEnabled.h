#pragma once
#include "threadmessageitem.h"

/**
 * @brief	Message for use with ThreadSingleMessage.
 * @remarks	Michael Pryor, 9/17/2010. 
 */
class ThreadMessageItemSetPortMapEnabled :
	public ThreadMessageItem
{
	/**
	 * @brief Object to take action on.
	 */
	UpnpNatPortMapAction * takeActionOnMe;

	/**
	 * @brief If true the port map should be created enabled.
	 */
	bool enabled;

public:
	ThreadMessageItemSetPortMapEnabled(UpnpNatPortMapAction * takeActionOnMe, bool enabled);
	~ThreadMessageItemSetPortMapEnabled(void);

	void * TakeAction();
};

