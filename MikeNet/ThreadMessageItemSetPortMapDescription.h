#pragma once
#include "threadmessageitem.h"

/**
 * @brief	Message for use with ThreadSingleMessage.
 * @remarks	Michael Pryor, 9/17/2010. 
 */
class ThreadMessageItemSetPortMapDescription :
	public ThreadMessageItem
{
	/**
	 * @brief Object to take action on.
	 */
	UpnpNatPortMapAction * takeActionOnMe;

	/**
	 * @brief Description of the port map.
	 *
	 * Stored as a packet because it is important that the string remains valid until
	 * the thread has finished using it.
	 */
	Packet description;

public:
	ThreadMessageItemSetPortMapDescription(UpnpNatPortMapAction * takeActionOnMe, const Packet & description);
	~ThreadMessageItemSetPortMapDescription(void);

	void * TakeAction();
};

