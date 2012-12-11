#include "FullInclude.h"

/**
 * @brief	Constructor. 
 *
 * @param	[in]	takeActionOnMe	Object to take action on.
 * @param	enabled					True if the port map should be enabled, false if not.
 */
ThreadMessageItemSetPortMapEnabled::ThreadMessageItemSetPortMapEnabled(UpnpNatPortMapAction * takeActionOnMe, bool enabled)
{
	_ErrorException((takeActionOnMe == NULL),"creating a message, parameter is NULL",0,__LINE__,__FILE__);

	this->takeActionOnMe = takeActionOnMe;
	this->enabled = enabled;
}

/**
 * @brief	Destructor. 
 */
ThreadMessageItemSetPortMapEnabled::~ThreadMessageItemSetPortMapEnabled(void)
{
}

/**
 * @brief	Enables or disables a port map.
 *
 * @return NULL.
 */
void * ThreadMessageItemSetPortMapEnabled::TakeAction()
{
	takeActionOnMe->SetEnabled(enabled);
	return NULL;
}
