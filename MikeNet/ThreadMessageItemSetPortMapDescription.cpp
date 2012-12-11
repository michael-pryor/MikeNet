#include "FullInclude.h"

/**
 * @brief	Constructor. 
 *
 * @param	[in]	takeActionOnMe	Object to take action on.
 * @param	description				The description. 
 */
ThreadMessageItemSetPortMapDescription::ThreadMessageItemSetPortMapDescription(UpnpNatPortMapAction * takeActionOnMe, const Packet & description)
{
	_ErrorException((takeActionOnMe == NULL),"creating a message, parameter is NULL",0,__LINE__,__FILE__);

	this->takeActionOnMe = takeActionOnMe;
	this->description = description;
}

/**
 * @brief	Destructor. 
 */
ThreadMessageItemSetPortMapDescription::~ThreadMessageItemSetPortMapDescription(void)
{
}

/**
 * @brief	Changes the description of a port map. 
 *
 * @return NULL.
 */
void * ThreadMessageItemSetPortMapDescription::TakeAction()
{
	takeActionOnMe->SetDescription(description);
	return NULL;
}
