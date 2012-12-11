#pragma once
#include "threadmessageitem.h"
class SoundInstance;


/**
 * @brief	Message which deals with a completed sound operation, sent to a ThreadSingleMessage thread.
 * @remarks	Michael Pryor, 9/20/2010. 
 */
class ThreadMessageItemSoundCallback :
	public ThreadMessageItem
{
	/**
	 * @brief Sound instance to pass @a buffer to when calling SoundInstance::DealWithData.
	 */
	SoundInstance * instance;

	/**
	 * @brief Completed buffer to deal with.
	 */
	WAVEHDR * buffer;

public:
	ThreadMessageItemSoundCallback(SoundInstance * instance, WAVEHDR * buffer);
	virtual ~ThreadMessageItemSoundCallback();

	void * TakeAction();
};

