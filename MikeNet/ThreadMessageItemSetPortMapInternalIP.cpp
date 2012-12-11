#include "FullInclude.h"

/**
 * @brief	Constructor. 
 *
 * @param	[in]	takeActionOnMe	Object to take action on.
 * @param	internalIP				The internal ip. 
 */
ThreadMessageItemSetPortMapInternalIP::ThreadMessageItemSetPortMapInternalIP(UpnpNatPortMapAction * takeActionOnMe, const Packet & internalIP)
{
	_ErrorException((takeActionOnMe == NULL),"creating a message, parameter is NULL",0,__LINE__,__FILE__);

	this->takeActionOnMe = takeActionOnMe;
	this->internalIP = internalIP;
}

/**
 * @brief	Destructor. 
 */
ThreadMessageItemSetPortMapInternalIP::~ThreadMessageItemSetPortMapInternalIP(void)
{
}

/**
 * @brief	Changes the internal IP of a port map.
 *
 * @return NULL.
 */
void * ThreadMessageItemSetPortMapInternalIP::TakeAction()
{
	takeActionOnMe->SetInternalIP(internalIP);
	return NULL;
}
