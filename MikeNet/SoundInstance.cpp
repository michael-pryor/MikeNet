#include "FullInclude.h"

/**
 * @brief	Default constructor. 
 *
 * Sets up one single callback thread.
 */
SoundInstance::SoundInstance(void)
	: ThreadSingleMessageKeepLastUser(ThreadSingleMessageKeepLastUser::CLASS_INDEX_SOUND,1,SoundCallbackThread,NULL)
{
}

/**
 * @brief	Destructor. 
 */
SoundInstance::~SoundInstance(void)
{
}

/**
 * @brief	Posts a completed sound buffer to the thread so that it can be dealt with.
 *
 * @param [in]	buffer	The buffer to post.
 */
void SoundInstance::PostBuffer(WAVEHDR * buffer)
{
	_ErrorException((buffer == NULL),"posting a completed sound buffer to the thread, buffer is NULL",0,__LINE__,__FILE__);
	ThreadMessageItem * message = new ThreadMessageItemSoundCallback(this,buffer);
	this->PostMessageItem(0,message);
}