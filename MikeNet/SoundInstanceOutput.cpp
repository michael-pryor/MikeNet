#include "FullInclude.h"
#include <intsafe.h>

ThreadSingle * SoundInstanceOutput::instanceOutputThread = NULL;
StoreVector<SoundInstanceOutput> SoundInstanceOutput::instanceOutputList(false);

/**
 * @brief	Callback function which is called when an output buffer has been completely used.
 *
 * @param	hwi			The output device that filled the buffer.
 * @param	uMsg		The event that happened. We only respond to WOM_DONE which indicates that a buffer has been completely used.
 * @param	dwInstance	A pointer to a SoundInstanceOutput object that manages the device.
 * @param	dwBuffer	Buffer containing data.
 * @param	dwParam2	Ignored.
 */
void CALLBACK waveOutProc(
  HWAVEOUT hwo,      
  UINT uMsg,         
  DWORD_PTR dwInstance,  
  DWORD_PTR dwBuffer,    
  DWORD_PTR dwParam2)
{
	const char * cCommand = "an internal function (waveOutProc)";
	try
	{
		// If an output buffer has been filled.
		if(uMsg == WOM_DONE)
		{
			_ErrorException((dwInstance == NULL),"dealing with a completed sound output operation, dwInstance is NULL",0,__LINE__,__FILE__);
			_ErrorException((dwBuffer == NULL),"dealing with a completed sound output operation, dwBuffer is NULL",0,__LINE__,__FILE__);

			// Pass completion notification to thread.
			SoundInstance * outputInstance = reinterpret_cast<SoundInstance*>(dwInstance);
			WAVEHDR * buffer = reinterpret_cast<WAVEHDR*>(dwBuffer);

			outputInstance->PostBuffer(buffer);
		}
	}
	MSG_CATCH
}

/**
 * @brief	Constructor. 
 *
 * @param	msStoredDataCap	The length of time for output not to be played without new output being added
 * to the queue, before ending the wait and resuming playback regardless of the amount of data stored.
 * @param	timeGapCap	The amount of data that needs to be stored for playback to commence.
 * @param	format			Sound format that device should use.
 * @param	deviceID		ID of device that instance should use to post output data.
 */
SoundInstanceOutput::SoundInstanceOutput(size_t msStoredDataCap, clock_t timeGapCap, const SoundFormat & format, unsigned int deviceID)
	: bufferInUseCount((size_t)0), outputDataLastAdded(0)
{
	// Must default to 0 because SetOutputSmoothValues may not update these variables.
	// It does not update if the parameters are 0.
	this->outputDataLastAdded = 0;
	this->outputDataTotalBytes = 0;
	this->outputDataTotalBytesCap = 0;
	this->outputDataTotalTimeCap = 0;

	this->format = format;

	SetSmoothValues(msStoredDataCap,timeGapCap);
	
	MMRESULT result = waveOutOpen(&device,deviceID,this->format.GetFormat(),(DWORD_PTR)&waveOutProc,(DWORD_PTR)this,WAVE_FORMAT_DIRECT | CALLBACK_FUNCTION);
	_ErrorException((result != MMSYSERR_NOERROR),"gaining access to an output device",result,__LINE__,__FILE__);

	// Add instance to output list.
	instanceOutputList.Enter();
		if(instanceOutputList.Size() == 0)
		{
			instanceOutputThread = new (nothrow) ThreadSingle(&_ManageSoundOutput,NULL);
			Utility::DynamicAllocCheck(instanceOutputThread,__LINE__,__FILE__);
			instanceOutputThread->Resume();
		}

		instanceOutputList.Add(this);
	instanceOutputList.Leave();
}

/**
 * @brief	Destructor. 
 */
SoundInstanceOutput::~SoundInstanceOutput()
{
	const char * cCommand = "an internal function (~SoundInstanceOutput)";
	try
	{
		// Cleanup store.
		// Must be done before waveOutReset to prevent attempts to play data after waveOutReset.
		outputData.Clear();

		// Release buffers.
		MMRESULT result = waveOutReset(device);
		_ErrorException((result != MMSYSERR_NOERROR),"releasing output sound buffers",result,__LINE__,__FILE__);

		// Wait for buffers to be released and unloaded.
		while(bufferInUseCount.Get() > 0)
		{
			Sleep(1);
		}

		// Remove instance from output list.
		// We take control throughout the process in case two instances
		// are cleaned up at the same time by different threads.
		// Only one thread can have shouldCleanupThread set to true.
		// We must release control before cleaning up the thread to prevent deadlock.
		instanceOutputList.Enter();
			for(size_t n = 0;n<instanceOutputList.Size();n++)
			{
				if(*instanceOutputList.GetPtr(n) == this)
				{
					instanceOutputList.Erase(n);
					break;
				}
			}
			bool shouldCleanupThread = (instanceOutputList.Size() == 0);
		instanceOutputList.Leave();

		// Must not have critical section control at this point (see earlier comments).
		if(shouldCleanupThread == true)
		{
			delete instanceOutputThread;
			instanceOutputThread = NULL;
		}
	}
	MSG_CATCH
}

/**
 * @brief	Unprepares buffer. 
 *
 * @param	buffer	The buffer to use. 
 */
void SoundInstanceOutput::UnprepareBuffer(LPWAVEHDR buffer)
{
	// Unprepare buffer
	MMRESULT result = waveOutUnprepareHeader(device,buffer,sizeof(WAVEHDR));
	_ErrorException((result != MMSYSERR_NOERROR),"unpreparing an output sound header",result,__LINE__,__FILE__);
}

/**
 * @brief	Queues a buffer ready to receive input data.
 *
 * @param	buffer	The buffer to queue.
 */
void SoundInstanceOutput::QueueBuffer(LPWAVEHDR buffer)
{
	MMRESULT result = 0;

	buffer->dwBytesRecorded = 0;
	buffer->dwFlags = 0;
	buffer->dwLoops = 0;
	buffer->dwUser = 0;
	buffer->lpNext = 0;
	buffer->reserved = 0;

	// Prepare buffer
	result = waveOutPrepareHeader(device,buffer,sizeof(WAVEHDR));
	_ErrorException((result != MMSYSERR_NOERROR),"preparing a sound buffer for output",result,__LINE__,__FILE__);

	// Add buffer, so that it can receive more data
	result = waveOutWrite(device,buffer,sizeof(WAVEHDR));
	_ErrorException((result != MMSYSERR_NOERROR),"adding a sound buffer to the output queue",result,__LINE__,__FILE__);
}

/**
 * @brief	Adds a packet to the output queue.
 *
 * @param [in,out]	packet	Packet to add. This packet is now owned by this object
 * and should not be referenced elsewhere.
 */
void SoundInstanceOutput::QueuePacket(Packet * packet)
{
	outputData.Enter();
		outputData.Add(packet);
		outputDataLastAdded.SetTimer();
		outputDataTotalBytes += packet->GetUsedSize();
	outputData.Leave();
}

/**
 * @brief	Retrieves a packet from the output queue.
 *
 * @return	the number of packets in the queue before this call.
 */
size_t SoundInstanceOutput::DequeuePacket()
{
	size_t returnMe = 0;

	// Retrieve packet to be played from output queue.
	Packet packet;
	outputData.Enter();
		returnMe = outputData.Get(&packet);

		if(returnMe > 0)
		{
			outputDataTotalBytes -= packet.GetUsedSize();
		}
	outputData.Leave();

	// Indicate a buffer is about to be loaded.
	bufferInUseCount.Increase(1);
		
	// Create buffer, this is destroyed by
	// the callback thread after usage.
	WAVEHDR * buffer = new (nothrow) WAVEHDR;
	Utility::DynamicAllocCheck(buffer,__LINE__,__FILE__);

	buffer->lpData = new (nothrow) char[packet.GetUsedSize()];
	Utility::DynamicAllocCheck(buffer->lpData,__LINE__,__FILE__);

	buffer->dwBufferLength = static_cast<DWORD>(packet.GetUsedSize());
	memcpy(buffer->lpData,packet.GetDataPtr(),packet.GetUsedSize());

	// Queue buffer ready to be output.
	QueueBuffer(buffer);

	return(returnMe);
}

/**
 * @brief	Changes the playback rate of sound output by accessing the device driver.
 *
 * This operation may not be supported by the device driver. You can check this using the.
 * SoundDeviceOutput class.
 *
 * @param	rate	New playback rate. This is as a percentage where 100 is no rate change, 200 doubles the rate, 50 halves the rate.
 */
void SoundInstanceOutput::SetHardwarePlaybackRate(double rate)
{
	// Convert rate into waveform format
	WORD integer = 0;
	WORD fraction = 0;
	Utility::SplitDouble(rate,integer,fraction);
	DWORD convertedRate = MAKELONG(fraction,integer);

	// Change playback rate
	MMRESULT result = waveOutSetPlaybackRate(device,convertedRate);
	_ErrorException((result != MMSYSERR_NOERROR),"setting the hardware playback rate of an output device",result,__LINE__,__FILE__);
}

/**
 * @brief	Retrieves the playback rate of sound output by accessing the device driver.
 *
 * This operation may not be supported by the device driver. You can check this using the.
 * SoundDeviceOutput class.
 *
 * @return playback rate. This is as a percentage where 100 is no rate change, 200 doubles the rate, 50 halves the rate.
 */
double SoundInstanceOutput::GetHardwarePlaybackRate()
{
	// Retrieve playback rate
	DWORD rate = 0;
	MMRESULT result = waveOutGetPlaybackRate(device,&rate);
	_ErrorException((result != MMSYSERR_NOERROR),"retrieving the hardware playback rate of an output device",result,__LINE__,__FILE__);

	// Convert from waveform format into friendly format
	WORD integer = HIWORD(rate);
	WORD fraction = LOWORD(rate);

	return Utility::MakeDouble(integer,fraction);
}

/**
 * @brief	Changes the pitch of sound output by accessing the device driver.
 *
 * This operation may not be supported by the device driver. You can check this using the.
 * SoundDeviceOutput class.
 *
 * @param	pitch	Pitch change. This is as a percentage where 100 is no pitch change, 200 doubles the pitch, 50 halves the pitch.
 */
void SoundInstanceOutput::SetHardwarePitch(double pitch)
{
	// Convert rate into waveform format
	WORD integer = 0;
	WORD fraction = 0;
	Utility::SplitDouble(pitch,integer,fraction);
	DWORD convertedPitch = MAKELONG(fraction,integer);

	// Change playback rate
	MMRESULT result = waveOutSetPitch(device,convertedPitch);
	_ErrorException((result != MMSYSERR_NOERROR),"setting the hardware pitch of an output device",result,__LINE__,__FILE__);
}

/**
 * @brief	Retrieves the pitch change in action by accessing the device driver.
 *
 * This operation may not be supported by the device driver. You can check this using the.
 * SoundDeviceOutput class.
 *
 * @return	pitch change. This is as a percentage where 100 is no pitch change, 200 doubles the pitch, 50 halves the pitch.
 */
double SoundInstanceOutput::GetHardwarePitch()
{
	// Retrieve pitch
	DWORD pitch = 0;
	MMRESULT result = waveOutGetPitch(device,&pitch);
	_ErrorException((result != MMSYSERR_NOERROR),"retrieving the hardware pitch of an output device",result,__LINE__,__FILE__);

	// Convert from waveform format into friendly format
	WORD integer = HIWORD(pitch);
	WORD fraction = LOWORD(pitch);

	return Utility::MakeDouble(integer,fraction);
}


/**
 * @brief	Changes the volume sound output by accessing the device driver.
 *
 * This operation may not be supported by the device driver. You can check this using the.
 * SoundDeviceOutput class.\n\n
 *
 * If the driver does not support independent left/right volume change, only the left value
 * will be taken into account and this will be the value used by both channels.
 *
 * @param	left	New volume for the left channel.
 * @param	right	New volume for the right channel.
 */
void SoundInstanceOutput::SetHardwareVolume(unsigned short left, unsigned short right)
{
	// Convert rate into waveform format
	DWORD convertedVolume = MAKELONG(left,right);
	
	// Change playback rate
	MMRESULT result = waveOutSetVolume(device,convertedVolume);
	_ErrorException((result != MMSYSERR_NOERROR),"setting the hardware volume of an output device",result,__LINE__,__FILE__);
}

/**
 * @brief	Retrieves the volume of the left channel.
 *
 * This operation may not be supported by the device driver. You can check this using the.
 * SoundDeviceOutput class.
 *
 * If the driver does not support independent left/right volume change, this will
 * return the volume of both channels.
 *
 * @return	the volume of the left channel.
 */
WORD SoundInstanceOutput::GetHardwareVolumeLeft()
{
	// Retrieve playback rate.
	DWORD volume = 0;
	MMRESULT result = waveOutGetVolume(device,&volume);
	_ErrorException((result != MMSYSERR_NOERROR),"retrieving the hardware volume of the left channel of an output device",result,__LINE__,__FILE__);

	return LOWORD(volume);
}

/**
 * @brief	Retrieves the volume of the right channel.
 *
 * If only one channel exists then this will return the same as SoundInstanceOutput::GetHardwareVolumeLeft().
 *
 * This operation may not be supported by the device driver. You can check this using the.
 * SoundDeviceOutput class.
 *
 * @return	the volume of the right channel.
 */
WORD SoundInstanceOutput::GetHardwareVolumeRight()
{
	// Retrieve playback rate.
	DWORD volume = 0;
	MMRESULT result = waveOutGetVolume(device,&volume);
	_ErrorException((result != MMSYSERR_NOERROR),"retrieving the hardware volume of the right channel of an output device",result,__LINE__,__FILE__);

	if(format.GetNumChannels() >= 2)
	{
		return HIWORD(volume);
	}
	else
	{
		return LOWORD(volume);
	}
}

/**
 * @brief	Changes values which may effect how smooth playback is.
 *
 * @param	msStoredDataCap	The amount of data (in milliseconds) that needs to be stored for playback to commence.
 * @param	timeGapCap	The length of time for output not to be played without new output being added
 * to the queue, before ending the wait and resuming playback regardless of the amount of data stored.
 */
void SoundInstanceOutput::SetSmoothValues( size_t msStoredDataCap, clock_t timeGapCap )
{
	outputData.Enter();
	if(msStoredDataCap > 0)
	{
		this->outputDataTotalTimeCap = msStoredDataCap;
	}
	if(timeGapCap > 0)
	{
		this->outputDataLastAdded.SetFreq(timeGapCap);
	}

	// Determine the number of samples (blocks) for the specified amount of time
	// Note: divide by 1000 last to avoid imprecision due to decimal.
	size_t numSamples = (format.GetSamplesPerSecond() * outputDataTotalTimeCap) / 1000;

	// Determine the amount of memory required for this number of samples.
	outputDataTotalBytesCap = numSamples * format.GetBytesPerSample();
	outputData.Leave();
}

/**
 * @brief	Retrieves the amount of data (in milliseconds) that needs to be stored for playback to commence.
 *
 * @return	stored data cap (in milliseconds).
 */
size_t SoundInstanceOutput::GetStoredDataCapMs() const
{
	outputData.Enter();
	size_t returnMe = outputDataTotalTimeCap;
	outputData.Leave();
	return returnMe;
}

/**
 * @brief	Retrieves the length of time for output not to be played without new output being added
 * to the queue, before ending the wait and resuming playback regardless of the amount of data stored.
 *
 * @return	length of time in milliseconds.
 */
size_t SoundInstanceOutput::GetTimeGapCap() const
{
	outputData.Enter();
	size_t returnMe = outputDataLastAdded.GetFreq();
	outputData.Leave();
	return returnMe;
}

/**
 * @brief	Retrieves the sound format in use.
 *
 * @return	the sound format in use.
 */
const SoundFormat & SoundInstanceOutput::GetFormat() const
{
	return format;
}


/**
 * @brief	Called indirectly by callback thread to deal with a completed buffer after output process has finished.
 *
 * The callback thread must not directly call this method, but instead posts a message
 * to a thread initiated by this API. This restriction is mentioned in the waveform documentation on MSDN.\n\n
 *
 * @param [in,out] buffer	Pointer to the completed buffer.
 */
void SoundInstanceOutput::DealWithData(WAVEHDR * buffer)
{
	_ErrorException((buffer == NULL),"dealing with a completed output buffer, buffer must not be NULL",0,__LINE__,__FILE__);

	// Unload buffer so that we can access it.
	UnprepareBuffer(buffer);

	// Deallocate memory (removing buffer).
	delete[] buffer->lpData;
	delete buffer;

	// Indicate that we have finished with output buffer.
	bufferInUseCount.Decrease(1);
}

/**
 * @brief	Retrieves an output instance.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return	an output instance.
 */
SoundInstanceOutput * SoundInstanceOutput::GetInstanceOutput( size_t instanceID )
{
	_ErrorException((instanceID >= instanceOutputList.Size()),"retrieving an output instance, invalid instance ID",0,__LINE__,__FILE__);
	return &instanceOutputList[instanceID];
}

/**
 * @brief	Retrieves the number of output instance initialized.
 *
 * @return	the number of output instances initialized.
 */
size_t SoundInstanceOutput::GetNumInstanceOutput()
{
	return instanceOutputList.Size();
}

/**
 * @brief	Method may choose to output a packet, extracting it from the packet queue.
 */
void SoundInstanceOutput::DecideWhetherToOutput()
{
	outputData.Enter();

	// If there is data waiting to be output.
	if(outputData.Size() > 0)
	{
		// If the total number of sound bytes in waiting is more
		// than the limit then we should output.
		if(outputDataTotalBytes > outputDataTotalBytesCap)
		{
			// Output enough data for us to be under the limit again
			while(outputDataTotalBytes > outputDataTotalBytesCap)
			{
				DequeuePacket();
			}
		}
		// If we are under the limit then we should check that
		// the remaining data has not been left waiting for too long.
		else
		{
			// Output all data when it has been in waiting for too long.
			if(outputDataLastAdded.GetState() == true)
			{
				while(outputData.Size() > 0)
				{
					DequeuePacket();
				}
			}
		}
	}

	outputData.Leave();
}

/**
 * @brief	Take control of SoundInstanceOutput::instanceOutputList.
 */
void SoundInstanceOutput::EnterInstanceOutputList()
{
	instanceOutputList.Enter();
}

/**
 * @brief	Release control of SoundInstanceOutput::instanceOutputList.
 */
void SoundInstanceOutput::LeaveInstanceOutputList()
{
	instanceOutputList.Leave();
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool SoundInstanceOutput::TestClass()
{
	cout << "Testing SoundInstanceOutput class...\n";
	bool problem = false;

	SoundFormat format;
	format.SetHighQuality();
	SoundInstanceInput inputInstance(SoundInstanceInput::DEFAULT_BUFF_COUNT,SoundInstanceInput::DEFAULT_BUFF_TIME_LENGTH,format,0);
	SoundInstanceInput inputInstance2(SoundInstanceInput::DEFAULT_BUFF_COUNT,SoundInstanceInput::DEFAULT_BUFF_TIME_LENGTH,format,0);
	SoundInstanceOutput outputInstance(SoundInstanceOutput::DEFAULT_TIME_STORED_CAP,SoundInstanceOutput::DEFAULT_TIME_SINCE_LAST_ADDED_CAP,format,0);
	SoundInstanceOutput outputInstance2(SoundInstanceOutput::DEFAULT_TIME_STORED_CAP,SoundInstanceOutput::DEFAULT_TIME_SINCE_LAST_ADDED_CAP,format,0);
	inputInstance.UnpauseInput();

	Timer runLength(10000);
	Timer pauseInput(20000);
	Timer decreaseVolumeLeft(10000000);
	Timer changePitch(100000000);
	Timer changePlaybackRate(1000000);
	while(runLength.GetState() != true)
	{
		Sleep(1);

		Packet destination;
		if(inputInstance.GetRetrievedPacket(&destination) > 0)
		{
			cout << "(1) Retrieved data of size " << destination.GetUsedSize() << "\n";
			outputInstance.QueuePacket(new Packet(destination));
		}

		if(inputInstance2.GetRetrievedPacket(&destination) > 0)
		{
			cout << "(2) Retrieved data of size " << destination.GetUsedSize() << "\n";
			outputInstance2.QueuePacket(new Packet(destination));
		}

		if(pauseInput.GetState() == true)
		{
			if(inputInstance.IsPaused())
			{
				cout << "(1) Unpausing input\n";
				inputInstance.UnpauseInput();
			}
			else
			{
				cout << "(1) Pausing input\n";
				inputInstance.PauseInput();
			}

			if(inputInstance2.IsPaused())
			{
				cout << "(2) Unpausing input\n";
				inputInstance2.UnpauseInput();
			}
			else
			{
				cout << "(2) Pausing input\n";
				inputInstance2.PauseInput();
			}
		}

		if(decreaseVolumeLeft.GetState() == true)
		{
			unsigned short left = outputInstance.GetHardwareVolumeLeft();
			unsigned short right = outputInstance.GetHardwareVolumeRight();
			cout << "(1) Volume: L" << left << ", R" << right << '\n';

			cout << "(1) Decreasing volume\n";
			outputInstance.SetHardwareVolume(left-500,right);
		}

		if(changePitch.GetState() == true)
		{
			double pitch = outputInstance.GetHardwarePitch();
			cout << "(1) Pitch: " << pitch << '\n';

			cout << "(1) Decreasing pitch\n";
			outputInstance.SetHardwarePitch(pitch-1.0);
		}

		if(changePlaybackRate.GetState() == true)
		{
			double rate = outputInstance.GetHardwarePlaybackRate();
			cout << "(1) Playback rate: " << rate << '\n';

			cout << "(1) Decreasing playback rate\n";
			outputInstance.SetHardwarePitch(rate-1.0);
		}
	}

	cout << "\n\n";
	return !problem;
}



