#include "FullInclude.h"

/**
 * @brief	Default constructor. 
 *
 * @param [in]	natControls		Object to take action on.
 * @param		portMapID		Identifier for the port map. 
 */
ThreadMessageItemDeletePortMap::ThreadMessageItemDeletePortMap(UpnpNatAction * natControls, size_t portMapID)
{
	this->takeActionOnMe = natControls;
	this->portMapID = portMapID;
	byIdOnly = true;
}

/**
 * @brief	Constructor. 
 *
 * @param [in]	takeActionOnMe		Object to take action on.
 * @param		protocol		The protocol. 
 * @param		externalPort	The external port. 
 */
ThreadMessageItemDeletePortMap::ThreadMessageItemDeletePortMap( UpnpNatAction * takeActionOnMe, const char * protocol, long externalPort )
{
	_ErrorException((takeActionOnMe == NULL),"creating a message, parameter is NULL",0,__LINE__,__FILE__);

	this->takeActionOnMe = takeActionOnMe;
	this->protocol = protocol;
	this->externalPort = externalPort;
}

/**
 * @brief	Destructor. 
 */
ThreadMessageItemDeletePortMap::~ThreadMessageItemDeletePortMap(void)
{
}

/**
 * @brief	Deletes a port map.
 *
 * @return NULL.
 */
void * ThreadMessageItemDeletePortMap::TakeAction()
{
	if(byIdOnly == true)
	{
		takeActionOnMe->DeletePortMap(portMapID);
	}
	else
	{
		takeActionOnMe->DeletePortMap(protocol.GetNullTerminated(),externalPort);
	}
	return NULL;
}
