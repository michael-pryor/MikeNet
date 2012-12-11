#pragma once

/**
 * @brief	Base class for any type of sound instance.
 * @remarks	Michael Pryor, 9/25/2010. 
 */
class SoundInstance : public ThreadSingleMessageKeepLastUser
{
public:
	SoundInstance(void);
	virtual ~SoundInstance(void);

	/**
	 * @brief	Called indirectly by callback thread to deal with a completed buffer after process has finished.
	 *
	 * The callback thread must not directly call this method, but instead posts a message
	 * to a thread initiated by this API. This restriction is mentioned in the waveform documentation on MSDN.\n\n
	 *
	 * @param [in,out] buffer	Pointer to the completed buffer.
	 */
	virtual void DealWithData(WAVEHDR * buffer) = 0;

	void PostBuffer(WAVEHDR * buffer);
};

