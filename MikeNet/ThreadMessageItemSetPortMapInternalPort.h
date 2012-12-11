#pragma once
#include "threadmessageitem.h"

/**
 * @brief	Message for use with ThreadSingleMessage.
 * @remarks	Michael Pryor, 9/17/2010. 
 */
class ThreadMessageItemSetPortMapInternalPort :
	public ThreadMessageItem
{
	/**
	 * @brief Object to take action on.
	 */
	UpnpNatPortMapAction * takeActionOnMe;

	/**
	 * @brief Internal port of new port map.
	 */
	long internalPort;
public:
	ThreadMessageItemSetPortMapInternalPort(UpnpNatPortMapAction * takeActionOnMe, long internalPort);
	~ThreadMessageItemSetPortMapInternalPort(void);

	void * TakeAction();
};

