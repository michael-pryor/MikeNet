#pragma once
#include "threadmessageitem.h"

/**
 * @brief	Message for use with ThreadSingleMessage.
 * @remarks	Michael Pryor, 9/17/2010. 
 */
class ThreadMessageItemDeletePortMap :
	public ThreadMessageItem
{
	/**
	 * @brief Controls to use when taking action.
	 */
	UpnpNatAction * takeActionOnMe;

	/**
	 * @brief ID of port map to delete.
	 */
	size_t portMapID;

	/**
	 * @brief Protocol of port map to delete.
	 *
	 * Stored as a packet because it is important that the string remains valid until
	 * the thread has finished using it.
	 */
	Packet protocol;
	
	/**
	 * @brief External port of port map to delete.
	 */
	long externalPort;

	/**
	 * @brief If true then only ThreadMessageItemDeletePortMap::portMapID (not ThreadMessageItemDeletePortMap::protocol
	 * or ThreadMessageItemDeletePortMap::externalPort)will be used when finding the port map to delete.
	 */
	bool byIdOnly;
public:
	ThreadMessageItemDeletePortMap(UpnpNatAction * natControls, size_t portMapID);
	ThreadMessageItemDeletePortMap(UpnpNatAction * natControls, const char * protocol, long externalPort);
	~ThreadMessageItemDeletePortMap(void);

	void * TakeAction();
};

