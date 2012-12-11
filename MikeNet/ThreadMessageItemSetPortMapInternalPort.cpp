#include "FullInclude.h"

/**
 * @brief	Constructor. 
 *
 * @param	[in]	takeActionOnMe	Object to take action on.
 * @param	internalPort			The internal port. 
 */
ThreadMessageItemSetPortMapInternalPort::ThreadMessageItemSetPortMapInternalPort(UpnpNatPortMapAction * takeActionOnMe, long internalPort)
{
	_ErrorException((takeActionOnMe == NULL),"creating a message, parameter is NULL",0,__LINE__,__FILE__);

	this->takeActionOnMe = takeActionOnMe;
	this->internalPort = internalPort;
}

/**
 * @brief	Destructor. 
 */
ThreadMessageItemSetPortMapInternalPort::~ThreadMessageItemSetPortMapInternalPort(void)
{
}

/**
 * @brief	Changes the internal port of a port map.
 *
 * @return NULL.
 */
void * ThreadMessageItemSetPortMapInternalPort::TakeAction()
{
	takeActionOnMe->SetInternalPort(internalPort);
	return NULL;
}
