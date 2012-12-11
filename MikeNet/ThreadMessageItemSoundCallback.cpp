#include "FullInclude.h"

/**
 * @brief	Constructor. 
 */
ThreadMessageItemSoundCallback::ThreadMessageItemSoundCallback(SoundInstance * instance, WAVEHDR * buffer)
{
	_ErrorException((instance == NULL),"creating a callback sound message, instance is NULL",0,__LINE__,__FILE__);
	_ErrorException((buffer == NULL),"creating a callback sound message, buffer is NULL",0,__LINE__,__FILE__);

	this->instance = instance;
	this->buffer = buffer;
}

/**
 * @brief	Destructor. 
 */
ThreadMessageItemSoundCallback::~ThreadMessageItemSoundCallback()
{
}

/**
 * @brief Passes buffer to SoundInstance::DealWithData.
 */
void * ThreadMessageItemSoundCallback::TakeAction()
{
	instance->DealWithData(buffer);

	return NULL;
}
