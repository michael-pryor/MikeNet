#pragma once

/**
 * @brief	Input instance which accesses an input device and retrieves input from it.
 * @remarks	Michael Pryor, 9/20/2010. 
 */
class SoundInstanceInput : public SoundInstance
{
	/**
	 * @brief Length of time that each buffer records for in milliseconds.
	 */
	size_t buffTimeLength;

	/**
	 * @brief Number of samples that can be stored in a single full buffer.
	 */
	size_t buffSizeSamples;

	/**
	 * @brief Number of bytes allocated to a single buffer.
	 */
	size_t buffSizeBytes;
public:
	/**
	 * @brief Default value for SoundInstanceInput::buffer.Size().
	 */
	static const size_t DEFAULT_BUFF_COUNT = 2;

	/**
	 * @brief Default value for SoundInstanceInput::buffTimeLength.
	 */
	static const size_t DEFAULT_BUFF_TIME_LENGTH = 250;
private:
	/**
	 * @brief Controls device.
	 */
	HWAVEIN deviceHandle;
	
	/**
	 * @brief Stores input data, is used by SoundInstanceInput::buffer.
	 */
	StoreVector<Packet> bufferData;

	/**
	 * @brief Stores input data, uses SoundInstanceInput::bufferData.
	 */
	StoreVector<WAVEHDR> buffer;

	/**
	 * @brief Records the number of buffers in use.
	 *
	 * Buffers must not be deallocated until this is 0.
	 */
	ConcurrentObject<size_t> bufferInUseCount;

	/**
	 * @brief Sound format in use.
	 */
	SoundFormat format;

	/**
	 * @brief Signaled when input is unpaused.
	 */
	ConcurrentObject<bool> unpaused;

	/**
	 * @brief Stores retrieved input data.
	 */
	StoreQueue<Packet> retrievedInput;
public:
	SoundInstanceInput(size_t numberOfBuffers, size_t buffTimeLength, const SoundFormat & format, unsigned int deviceID);
	~SoundInstanceInput();

	void UnprepareBuffer(LPWAVEHDR wavehdr);
	void QueueBuffer(LPWAVEHDR wavehdr);

	void DealWithData(WAVEHDR * buffer);

	void UnpauseInput();
	void PauseInput();
	bool IsPaused();

	const SoundFormat & GetFormat() const;
	size_t GetBufferTimeLength() const;
    size_t GetBufferSizeSamples() const;
	size_t GetBufferSizeBytes() const;
	size_t GetNumBuffers() const;

	void AddRetrievedPacket(Packet * packet);
	size_t GetRetrievedPacket(Packet * destination);
	
	static void SetDataVolume(WORD BitsPerSample, Packet * dataPacket, double percentageChange);
	static int GetDataVolume(WORD bitsPerSample, const Packet * dataPacket);
};