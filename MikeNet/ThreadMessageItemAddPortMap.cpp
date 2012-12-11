#include "FullInclude.h"

/**
 * @brief	Constructor. 
 *
 * @param	[in]	takeActionOnMe	Object to take action on.
 * @param	externalPort			The external port. 
 * @param	protocol				The protocol. 
 * @param	internalPort			The internal port. 
 * @param	internalIP				The internal ip. 
 * @param	enabled					True if the entry should be enabled, false if not. 
 * @param	description				A description of the port map. 
 */
ThreadMessageItemAddPortMap::ThreadMessageItemAddPortMap(UpnpNatAction * takeActionOnMe, long externalPort, const Packet & protocol, long internalPort, const Packet & internalIP, bool enabled, const Packet & description )
{
	_ErrorException((takeActionOnMe == NULL),"creating a message, parameter is NULL",0,__LINE__,__FILE__);
	this->takeActionOnMe = takeActionOnMe;
	this->externalPort = externalPort;
	this->protocol = protocol;
	this->internalPort = internalPort;
	this->internalIP = internalIP;
	this->enabled = enabled;
	this->description = description;
}

/**
 * @brief	Destructor. 
 */
ThreadMessageItemAddPortMap::~ThreadMessageItemAddPortMap()
{
}

/**
 * @brief	Adds the port map to the UPnP NAT list.
 *
 * @return	NULL.
 */
void * ThreadMessageItemAddPortMap::TakeAction()
{
	takeActionOnMe->AddPortMap(externalPort,protocol.GetNullTerminated(),internalPort,internalIP.GetNullTerminated(),enabled,description.GetNullTerminated());
	return NULL;
}
