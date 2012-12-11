#include "FullInclude.h"

/**
 * @brief	Constructor. 
 *
 * @param	[in]	takeActionOnMe	Object to take action on.
 */
ThreadMessageItemUpdateNat::ThreadMessageItemUpdateNat(UpnpNatAction * takeActionOnMe)
{
	_ErrorException((takeActionOnMe == NULL),"creating a message, parameter is NULL",0,__LINE__,__FILE__);

	this->takeActionOnMe = takeActionOnMe;
}

/**
 * @brief	Destructor. 
 */
ThreadMessageItemUpdateNat::~ThreadMessageItemUpdateNat(void)
{

}

/**
 * @brief	Updates NAT, reloading all information. 
 *
 * @return	NULL. 
 */
void * ThreadMessageItemUpdateNat::TakeAction()
{
	takeActionOnMe->UpdateInfo();
	return NULL;
}
