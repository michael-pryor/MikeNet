#pragma once
#include "threadmessageitem.h"

/**
 * @brief	Message for use with ThreadSingleMessage.
 * @remarks	Michael Pryor, 9/17/2010. 
 */
class ThreadMessageItemSetPortMapInternalIP :
	public ThreadMessageItem
{
	/**
	 * @brief Object to take action on.
	 */
	UpnpNatPortMapAction * takeActionOnMe;

	/**
	 * @brief Internal IP of new port map.
	 *
	 * Stored as a packet because it is important that the string remains valid until
	 * the thread has finished using it.
	 */
	Packet internalIP;

public:
	ThreadMessageItemSetPortMapInternalIP(UpnpNatPortMapAction * takeActionOnMe, const Packet & internalIP);
	~ThreadMessageItemSetPortMapInternalIP(void);

	void * TakeAction();
};

