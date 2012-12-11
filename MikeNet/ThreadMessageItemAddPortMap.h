#pragma once
#include "threadmessageitem.h"
#include "Packet.h"

/**
 * @brief	Message for use with ThreadSingleMessage.
 * @remarks	Michael Pryor, 9/17/2010. 
 */
class ThreadMessageItemAddPortMap :
	public ThreadMessageItem
{
	/**
	 * @brief External port of new port map.
	 */
	long externalPort;

	/**
	 * @brief Protocol of new port map.
	 *
	 * Stored as a packet because it is important that the string remains valid until
	 * the thread has finished using it.
	 */
	Packet protocol;

	/**
	 * @brief Internal port of new port map.
	 */
	long internalPort;

	/**
	 * @brief Internal IP of new port map.
	 *
	 * Stored as a packet because it is important that the string remains valid until
	 * the thread has finished using it.
	 */
	Packet internalIP;
	
	/**
	 * @brief If true the port map should be created enabled.
	 */
	bool enabled;

	/**
	 * @brief Description of the port map.
	 *
	 * Stored as a packet because it is important that the string remains valid until
	 * the thread has finished using it.
	 */
	Packet description;

	/**
	 * @brief Object to take action on.
	 */
	UpnpNatAction * takeActionOnMe;
public:
	ThreadMessageItemAddPortMap(UpnpNatAction * takeActionOnMe, long externalPort, const Packet & protocol, long internalPort, const Packet & internalIP, bool enabled, const Packet & description);
	~ThreadMessageItemAddPortMap(void);

	void * TakeAction();
};

