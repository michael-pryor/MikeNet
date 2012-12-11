#include "FullInclude.h"
class SoundInstanceInput;

/**
 * @brief	Callback function which is called when an input buffer has been filled.
 *
 * @param	hwi			The input device that filled the buffer.
 * @param	uMsg		The event that happened. We only respond to WIM_DATA which indicates that a buffer has been filled.
 * @param	dwInstance	A pointer to a SoundInstanceInput object that manages the device.
 * @param	dwBuffer	Buffer containing data.
 * @param	dwParam2	Ignored.
 */
void CALLBACK waveInProc(
  HWAVEIN hwi,
  UINT uMsg,
  DWORD dwInstance,
  DWORD dwBuffer,
  DWORD dwParam2)
{
	const char * cCommand = "an internal function (waveInProc)";
	try
	{
		// If an input buffer has been filled.
		if(uMsg == WIM_DATA)
		{
			_ErrorException((dwInstance == NULL),"dealing with a completed sound input operation, dwInstance is NULL",0,__LINE__,__FILE__);
			_ErrorException((dwBuffer == NULL),"dealing with a completed sound input operation, dwBuffer is NULL",0,__LINE__,__FILE__);

			// Pass completion notification to thread.
			SoundInstance * inputInstance = reinterpret_cast<SoundInstance*>(dwInstance);
			WAVEHDR * buffer = reinterpret_cast<WAVEHDR*>(dwBuffer);

			inputInstance->PostBuffer(buffer);
		}
	}
	MSG_CATCH
}

/**
 * @brief	Unprepares buffer. 
 *
 * @param	buffer	The buffer to use. 
 */
void SoundInstanceInput::UnprepareBuffer(LPWAVEHDR buffer)
{
	MMRESULT result = waveInUnprepareHeader(deviceHandle,buffer,sizeof(WAVEHDR));
	_ErrorException((result != MMSYSERR_NOERROR),"unpreparing an input sound header",result,__LINE__,__FILE__);
}

/**
 * @brief	Queues a buffer ready to receive input data.
 *
 * @param	buffer	The buffer to queue.
 */
void SoundInstanceInput::QueueBuffer(LPWAVEHDR buffer)
{
	MMRESULT result = 0;
	
	// Indicate that another buffer is now in use
	bufferInUseCount.Increase(1);

	// Reset buffer so that more data can be collected
	buffer->dwBytesRecorded = 0;
	buffer->dwFlags = 0;
	buffer->dwLoops = 0;
	buffer->dwUser = 0;
	buffer->lpNext = 0;
	buffer->reserved = 0;
	
	// Prepare buffer
	result = waveInPrepareHeader(deviceHandle,buffer,sizeof(WAVEHDR));
	_ErrorException((result != MMSYSERR_NOERROR),"preparing a sound header for input",result,__LINE__,__FILE__);

	// Add buffer, so that it can receive more data
	result = waveInAddBuffer(deviceHandle,buffer,sizeof(WAVEHDR));
	_ErrorException((result != MMSYSERR_NOERROR),"adding a sound buffer to the input queue",result,__LINE__,__FILE__);
}

/**
 * @brief	Constructor. 
 *
 * @param	numberOfBuffers	Number of buffers. 
 * @param	buffTimeLength	Length of a buffer in milliseconds.
 * @param	format			Sound format that device should use.
 * @param	deviceID		ID of device that instance should use to retrieve input data. Set to -1 and the device will be chosen automatically.
 */
SoundInstanceInput::SoundInstanceInput(size_t numberOfBuffers, size_t buffTimeLength, const SoundFormat & format, unsigned int deviceID)
	: SoundInstance(), unpaused(false), bufferInUseCount((size_t)0)
{
	this->format = format;
	this->buffTimeLength = buffTimeLength;
	this->buffSizeSamples = (format.GetSamplesPerSecond() * buffTimeLength)/1000;
	this->buffSizeBytes = format.GetBytesPerSample() * this->buffSizeSamples;

	WAVEHDR defaultHdr;

	bufferData.ResizeAllocate(numberOfBuffers);
	buffer.ResizeAllocate(numberOfBuffers,defaultHdr);

	for(size_t n = 0;n<bufferData.Size();n++)
	{
		bufferData[n].SetMemorySize(buffSizeBytes);
		buffer[n].dwBufferLength = static_cast<DWORD>(bufferData[n].GetMemorySize());
		buffer[n].lpData = bufferData[n].GetDataPtr();
		buffer[n].dwBytesRecorded = 0;
		buffer[n].dwFlags = 0;
		buffer[n].dwLoops = 0;
		buffer[n].dwUser = 0;
		buffer[n].lpNext = 0;
		buffer[n].reserved = 0;
	}

	MMRESULT result = waveInOpen(&deviceHandle,deviceID,this->format.GetFormat(),(DWORD_PTR)&waveInProc,(DWORD_PTR)this,WAVE_FORMAT_DIRECT | CALLBACK_FUNCTION);
	_ErrorException((result != MMSYSERR_NOERROR),"gaining access to an input device",result,__LINE__,__FILE__);
}

/**
 * @brief	Destructor. 
 */
SoundInstanceInput::~SoundInstanceInput()
{
	const char * cCommand = "an internal function (~SoundInstanceInput)";
	try
	{
		// We pause input so that buffers currently receiving input stop receiving input.
		PauseInput();

		MMRESULT result = waveInClose(deviceHandle);
		_ErrorException((result != MMSYSERR_NOERROR),"releasing an input device",result,__LINE__,__FILE__);
	}
	MSG_CATCH
}

/**
 * @brief	Unpauses input.
 */
void SoundInstanceInput::UnpauseInput()
{
	unpaused.Set(true);

	// Prepare input buffers
	for(size_t n = 0;n<buffer.Size();n++)
	{
		QueueBuffer(&buffer[n]);
	}

	// Start receiving input
	MMRESULT result = waveInStart(deviceHandle);
	_ErrorException((result != MMSYSERR_NOERROR),"starting to receive input data",result,__LINE__,__FILE__);
}

/**
 * @brief	Pauses input.
 *
 * Any buffers receiving data stop receiving data immediately.
 */
void SoundInstanceInput::PauseInput()
{
	// Take control of unpaused critical section in order to prevent thread
	// from loading a buffer during the shutdown process.
	unpaused.Enter();
	unpaused.Set(false);

	// Release buffers
	MMRESULT result = 0;

	result = waveInReset(deviceHandle);
	_ErrorException((result != MMSYSERR_NOERROR),"releasing input sound buffers that are not in progress",result,__LINE__,__FILE__);

	result = waveInStop(deviceHandle);
	_ErrorException((result != MMSYSERR_NOERROR),"releasing input sound buffers that are in progress",result,__LINE__,__FILE__);
	
	// Must release here otherwise deadlock may occur while thread waits for release
	// and we wait for bufferInUseCount to be 0.
	unpaused.Leave();

	// Wait for all buffers to be returned to application.
	while(bufferInUseCount.Get() > 0)
	{
		Sleep(1);
	}
}

/**
 * @brief	Determine whether input is paused. 
 *
 * @return	true if paused, false if not. 
 */
bool SoundInstanceInput::IsPaused()
{
	return !unpaused.Get();
}

/**
 * @brief	Adds a packet to the retrieved packet queue.
 *
 * @param [in] packet	The packet to add. This packet is now owned by this object and should not
 * be referenced elsewhere.
 */
void SoundInstanceInput::AddRetrievedPacket( Packet * packet )
{
	retrievedInput.Add(packet);
}

/**
 * @brief Retrieves a packet from retrieved packet queue.
 *
 * @param [out] destination	Destination to copy packet to.
 *
 * @return number of packets in packet queue.
 */
size_t SoundInstanceInput::GetRetrievedPacket( Packet * destination )
{
	return retrievedInput.Get(destination);
}

/**
 * @brief	Called indirectly by callback thread to deal with a completed buffer.
 *
 * The callback thread must not directly call this method, but instead posts a message
 * to a thread initiated by this API. This restriction is mentioned in the waveform documentation on MSDN.\n\n
 *
 * If unpaused the buffer will be re-queued to receive more data.
 *
 * @param [in,out] buffer	Pointer to the completed buffer.
 */
void SoundInstanceInput::DealWithData(WAVEHDR * buffer)
{
	_ErrorException((buffer == NULL),"dealing with a completed input buffer, buffer must not be NULL",0,__LINE__,__FILE__);

	// Unload buffer.
	// Must do this before accessing the contents of the buffer.
	UnprepareBuffer(buffer);					

	// Copy buffer data into packet so that it can be put into queue.
	// Note that even if paused, we should still add data to the queue.
	if(buffer->dwBytesRecorded > 0)
	{
		Packet * packet = new (nothrow) Packet;
		Utility::DynamicAllocCheck(packet,__LINE__,__FILE__);

		packet->SetUsedSize(buffer->dwBytesRecorded);
		memcpy(packet->GetDataPtr(), buffer->lpData, buffer->dwBytesRecorded);

		AddRetrievedPacket(packet);
	}

	// Load buffer so that it can receive more input data.
	unpaused.Enter();
	if(unpaused.Get() == true)
	{
		QueueBuffer(buffer);
	}
	unpaused.Leave();

	// Indicate that we have finished with input buffer
	// that we unprepared earlier thread is not finished
	// with buffer until all input related functions are
	// completed, this prevents the instance being cleaned
	// up and the thread trying to access it.
	bufferInUseCount.Decrease(1);
}

/**
 * @brief	Retrieves the sound format in use.
 *
 * @return	the sound format in use. 
 */
const SoundFormat & SoundInstanceInput::GetFormat() const
{
	return format;
}

/**
 * @brief Retrieves the length of time that each buffer records for in milliseconds.
 *
 * @return length of time that each buffer records for in milliseconds.
 */
size_t SoundInstanceInput::GetBufferTimeLength() const
{
	return buffTimeLength;
}

/**
 * @brief Retrieves the number of samples that can be stored in a single full buffer.
 *
 * @return number of samples that can be stored in a single full buffer.
 */
size_t SoundInstanceInput::GetBufferSizeSamples() const
{
	return buffSizeSamples;
}

/**
 * @brief Retrieves the number of bytes allocated to a single buffer.
 *
 * @return number of bytes allocated to a single buffer.
 */
size_t SoundInstanceInput::GetBufferSizeBytes() const
{
	return buffSizeBytes;
}

/**
 * @brief	Retrieves the number of buffers allocated.
 *
 * @return	the number of buffers.
 */
size_t SoundInstanceInput::GetNumBuffers() const
{
	return buffer.Size();
}

/**
 * @brief	Changes the volume of sound data stored in a packet.
 *
 * @param	BitsPerSample		The bits per sample of the stored data in the packet.
 * @param [out]	dataPacket		The packet to manipulate.
 * @param	percentageChange	The percentage to change the volume e.g. 200 doubles the volume. 
 */
void SoundInstanceInput::SetDataVolume( WORD BitsPerSample, Packet * dataPacket, double percentageChange )
{
	double changeValue = 100.0;
		
	if(percentageChange != 0)
	{
		changeValue = percentageChange / changeValue;
	}
	else
	{
		changeValue = 0.0;
	}

	switch(BitsPerSample)
	{
		// 16 bits (integer)
		case(sizeof(unsigned short) * 8):
			// Avoid going out of bounds
			_ErrorException((dataPacket->GetUsedSize() % 2 > 0),"changing a sound buffer's volume, invalid number of bytes, must be a multiple of 2 in 16 bit mode",0,__LINE__,__FILE__);

			// Loop through each byte of the packet
			for(size_t n = 0; n<dataPacket->GetUsedSize(); n+=sizeof(short int))
			{
				// Get base integer, which represents a single sound sample
				// Large type due to type of changeInt
				__int64 baseInt = *(short int*)&dataPacket->GetDataPtr()[n];

				// Stores the change that should be made to the integer
				// Large type so that numbers out of bouds can be dealt with properly
				__int64 changeInt;

				// Perform change based on percentage change
				changeInt = static_cast<long long int>(baseInt * changeValue);

				// Ensure that change is not out of bounds
				if( (baseInt > 0) && (baseInt+changeInt > SHRT_MAX) )
				{
					changeInt = SHRT_MAX-baseInt;
				}
				if ( (baseInt < 0) && (baseInt+changeInt < SHRT_MIN) ) 
				{
					changeInt = SHRT_MIN-baseInt;
				}

				// Adjust byte
				baseInt += changeInt;
				*(short int*)&dataPacket->GetDataPtr()[n] = static_cast<short int>(baseInt);
			}			

		break;

		// 8 bits (unsigned char)
		case(sizeof(unsigned char) * 8):
		{
			// Loop through each byte of the packet
			for(size_t n = 0; n<dataPacket->GetUsedSize(); n++)
			{
				// Get base byte in unsigned char form
				// Large type due to type of changeByte
				__int64 baseByte = *(unsigned char*)&dataPacket->GetDataPtr()[n];

				// Stores the change that should be made to the byte
				// Large type so that numbers out of bounds can be dealt with properly
				__int64 changeByte;

				// Perform change based on percentage change
				// Note 128 is no sound output, wave moves around this
				changeByte = static_cast<long long int>((baseByte-128) * changeValue);

				// Ensure that change is not out of bounds
				if( (baseByte > 128) && (baseByte+changeByte > UCHAR_MAX) )
				{
					changeByte = UCHAR_MAX-baseByte;
				}
				if ( (baseByte < 128) && (baseByte+changeByte < 0) ) 
				{
					changeByte = -baseByte;
				}

				// Adjust byte
				baseByte += changeByte;
				dataPacket->GetDataPtr()[n] = *(signed char*)&baseByte;
			}
		}
		break;

		// Error
		default:
			_ErrorException(true,"changing a sound buffer's volume, invalid number of bits; must be 8 or 16", 0, __LINE__, __FILE__);
		break;
	}
}

/**
 * @brief	Calculates the volume of sound data stored in a packet.
 *
 * @param	bitsPerSample	The bits per sample of the sound data stored in the specified packet.
 * @param	dataPacket		The packet to use. 
 *
 * @return	the volume of the data. If 16 bit then the range is 0 to 65535. If 8 bit then the range is 0 to 255.
 */
int SoundInstanceInput::GetDataVolume(WORD bitsPerSample, const Packet * dataPacket)
{
	unsigned __int64 totalVolume = 0, numberEntries = 0;

	switch(bitsPerSample)
	{
		// 16 bits (integer)
		case(sizeof(unsigned short) * 8):
		{
			// Loop through each byte of the packet
			for(size_t n = 0; n<dataPacket->GetUsedSize(); n+=sizeof(short int))
			{
				// Avoid going out of bounds
				_ErrorException((n >= dataPacket->GetUsedSize()),"determining a sound buffer's volume, invalid number of bytes, must be a multiple of 2 in 16 bit mode",0,__LINE__,__FILE__);

				// Get base integer, which represents a single sound sample
				short int baseInt = *(short int*)&dataPacket->GetDataPtr()[n];
					
				// Ensure that value is positive, distance from 0 indicates volume
				baseInt = abs(baseInt);
				if(baseInt == SHRT_MIN)
				{
					baseInt = SHRT_MAX;
				}

				// Add to total
				totalVolume+=baseInt;
				numberEntries++;
			}
		}
		break;

		// 8 bits (unsigned char)
		case(sizeof(unsigned char) * 8):
		{
			// Loop through each byte of the packet
			for(size_t n = 0; n<dataPacket->GetUsedSize(); n++)
			{
				// Get base byte in unsigned char form
				unsigned char baseByte = *(unsigned char*)&dataPacket->GetDataPtr()[n];

				// Calculate volume, which is the distance from 128
				unsigned char distance;
				if(baseByte < 128)
				{
					distance = 128-baseByte;
				}
				else
				{
					distance = baseByte-128;
				}

				// Add to total
				totalVolume+=distance;
				numberEntries++;
			}
		}
		break;

		// Error
		default:
			_ErrorException(true,"determining a sound buffer's volume, invalid number of bits; must be 8 or 16", 0, __LINE__, __FILE__);
		break;
	}

	// Calculate average volume
	// Casting is okay because short int and unsigned char are used during process
	// so they cannot be out of range of int
	return static_cast<int>(totalVolume / numberEntries);
}
