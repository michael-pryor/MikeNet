#pragma once

/**
 * @brief	Output instance which accesses an output device and sends output to it.
 * @remarks	Michael Pryor, 9/21/2010. 
 */
class SoundInstanceOutput : public SoundInstance
{
	// Load/unload waveform buffer
	void UnprepareBuffer(LPWAVEHDR wavehdr);
	void QueueBuffer(LPWAVEHDR wavehdr);

	/**
	 * @brief Controls device.
	 */
	HWAVEOUT device;

	/**
	 * @brief Stores data that will be output.
	 */
	StoreQueue<Packet> outputData;

	/**
	 * @brief Monitors when data was last added to SoundInstanceOutput::outputData.
	 *
	 * Access is should be controlled using SoundInstanceOutput::outputData's critical section.
	 */
	Timer outputDataLastAdded;

	/**
	 * @brief Total number of bytes in SoundInstanceOutput::outputData queue.
	 *
	 * Access is should be controlled using SoundInstanceOutput::outputData's critical section.
	 */
	size_t outputDataTotalBytes;
	
	/**
	 * @brief If SoundInstanceOutput::outputDataTotalBytes >= this then data will be played.
	 *
	 * Access is should be controlled using SoundInstanceOutput::outputData's critical section.
	 */
	size_t outputDataTotalBytesCap;
	
	/**
	 * @brief Amount of data in milliseconds that must be stored before data is played.
	 *
	 * Access is should be controlled using SoundInstanceOutput::outputData's critical section.
	 */
	size_t outputDataTotalTimeCap;

public:
	/**
	 * @brief Default value for SoundInstanceOutput::outputDataLastAddedCap in milliseconds.
	 */
	static const size_t DEFAULT_TIME_SINCE_LAST_ADDED_CAP = 200;

	/**
	 * @brief Default value for SoundInstanceOutput::outputDataTotalTimeCap in milliseconds.
	 */
	static const size_t DEFAULT_TIME_STORED_CAP = 500;
private:
	/**
	 * @brief Sound format in use.
	 */
	SoundFormat format;

	/**
	 * @brief Number of buffers currently in use by waveform or the thread.
	 */
	ConcurrentObject<size_t> bufferInUseCount;
	
	/**
	 * @brief Stores a pointer to all instances of SoundInstanceOutput.
	 *
	 * This is necessary so that the sound output thread can deal with all instances in one thread.
	 */
	static StoreVector<SoundInstanceOutput> instanceOutputList;

	/**
	 * @brief Thread which manages output, deciding when buffers should be output.
	 *
	 * Access should be controlled using SoundInstanceOutput::instanceOutputList's critical section.
	 */
	static ThreadSingle * instanceOutputThread;

public:
	static SoundInstanceOutput * GetInstanceOutput(size_t instanceID);
	static size_t GetNumInstanceOutput();
	static void EnterInstanceOutputList();
	static void LeaveInstanceOutputList();


	SoundInstanceOutput(size_t msStoredDataCap, clock_t timeGapCap, const SoundFormat & format, unsigned int deviceID);
	~SoundInstanceOutput();

	void QueuePacket(Packet * packet);
	size_t DequeuePacket();
	void DecideWhetherToOutput();

	void SetSmoothValues(size_t msStoredDataCap, clock_t timeGapCap);
	size_t GetStoredDataCapMs() const;
	size_t GetTimeGapCap() const;
	const SoundFormat & GetFormat() const;

	void SetHardwarePlaybackRate(double Rate);
	void SetHardwarePitch(double Pitch);
	void SetHardwareVolume(unsigned short Left, unsigned short Right);
	WORD GetHardwareVolumeLeft();
	WORD GetHardwareVolumeRight();
	double GetHardwarePitch();
	double GetHardwarePlaybackRate();

	void DealWithData(WAVEHDR * buffer);
	
	static bool TestClass();

};