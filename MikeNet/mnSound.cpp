#include "FullInclude.h"
#include "mnDBPWrapper.h"
#include "PointerConverter.h"

/**
 * @brief	Creates a sound device object for the specified device ID.
 *
 * @param	deviceID	Identifier for the device.
 * @param	output		True if it is an output device, false if it is an input device.
 *
 * @return	a sound device object that should be deallocated using delete.
 */
SoundDevice * _GetSoundDevice( unsigned int deviceID, bool output )
{
	SoundDevice * device = NULL;
	if(output == true)
	{
		device = new (nothrow) SoundDeviceOutput(deviceID);
	}
	else
	{
		device = new (nothrow) SoundDeviceInput(deviceID);
	}
	Utility::DynamicAllocCheck(device,__LINE__,__FILE__);

	return device;
}


/**
 * @brief	Retrieves the number of input devices available.
 *
 * @return	the number of input devices available.
 */
DBP_CPP_DLL size_t mnSound::GetNumInputDevices()
{
	const char * cCommand = "mnSound::GetNumInputDevices";
	size_t returnMe = 0;

	try
	{
		returnMe = SoundDeviceInput::GetNumDevices();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the number of output devices available.
 *
 * @return	the number of output devices available.
 */
DBP_CPP_DLL size_t mnSound::GetNumOutputDevices()
{
	const char * cCommand = "mnSound::GetNumOutputDevices";
	size_t returnMe = 0;

	try
	{
		returnMe = SoundDeviceOutput::GetNumDevices();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the manufacturer ID of the device.
 *
 * @param	deviceID	Identifier for the device.
 * @param	output		True if it is an output device, false if it is an input device.
 *
 * @return	the manufacturer identifier. 
 */
DBP_CPP_DLL unsigned short mnSound::GetDeviceManuID(unsigned int deviceID, bool output)
{
	const char * cCommand = "mnSound::GetDeviceManuID";
	unsigned short returnMe = 0;

	try
	{
		SoundDevice * device = _GetSoundDevice(deviceID,output);
		returnMe = device->GetManufacturerID();
		delete device;
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the product ID of the device.
 *
 * @param	deviceID	Identifier for the device.
 * @param	output		True if it is an output device, false if it is an input device.
 *
 * @return	the product identifier. 
 */
DBP_CPP_DLL unsigned short mnSound::GetDeviceProductID(unsigned int deviceID, bool output)
{
	const char * cCommand = "mnSound::GetDeviceProductID";
	unsigned short returnMe = 0;

	try
	{
		SoundDevice * device = _GetSoundDevice(deviceID,output);
		returnMe = device->GetProductID();
		delete device;
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Gets the minor version of the device's driver e.g. if the version if 4.5 then 5 will be returned.
 *
 * @param	deviceID	Identifier for the device.
 * @param	output		True if it is an output device, false if it is an input device.
 *
 * @return	the driver minor version. 
 */

DBP_CPP_DLL unsigned char mnSound::GetDeviceDriverMinorVersion(unsigned int deviceID, bool output)
{
	const char * cCommand = "mnSound::GetDeviceDriverMinorVersion";
	unsigned char cReturn = 0;

	try
	{
		SoundDevice * device = _GetSoundDevice(deviceID,output);
		cReturn = device->GetDriverMinorVersion();
		delete device;
	}
	STD_CATCH

	return(cReturn);
}

/**
 * @brief	Retrieves the major version of the device's driver e.g. if the version is 4.5 then 4 will be returned.
 *
 * @param	deviceID	Identifier for the device.
 * @param	output		True if it is an output device, false if it is an input device.
 *
 * @return	the driver major version. 
 */
DBP_CPP_DLL unsigned char mnSound::GetDeviceDriverMajorVersion(unsigned int deviceID, bool output)
{
	const char * cCommand = "mnSound::GetDeviceDriverMajorVersion";
	unsigned char cReturn = 0;

	try
	{
		SoundDevice * device = _GetSoundDevice(deviceID,output);
		cReturn = device->GetDriverMajorVersion();
		delete device;
	}
	STD_CATCH

	return(cReturn);
}

/**
 * @brief	Retrieves the name of the device.
 *
 * @param	deviceID	Identifier for the device.
 * @param	output		True if it is an output device, false if it is an input device.
 *
 * @return	the name of the device. This should be deallocated using delete[].
 * @return	NULL if an error occurred.
 */
CPP_DLL char * mnSound::GetDeviceName(unsigned int deviceID, bool output)
{
	const char * cCommand = "mnSound::GetDeviceName";
	char * strReturn = NULL;

	try
	{
		SoundDevice * device = _GetSoundDevice(deviceID,output);
		const char * aux = device->GetName();
		size_t bytes = strlen(aux)+1;
		strReturn = new (nothrow) char[bytes];
		strcpy_s(strReturn,bytes,aux);
		delete device;
	}
	STD_CATCH

	return(strReturn);
}


/**
 * @brief	Determine if a format with the specified settings is supported by the device.
 *
 * @param	deviceID			Identifier for the device.
 * @param	output				True if it is an output device, false if it is an input device.
 * @param	khz					Samples per second: 11025 (11.025 kHz), 22050 (22.05 kHz), 44100 (44.1 kHz), 48000 (48 kHz) or 96000 (96kHz).
 * @param	bits				Bits (8 or 16).
 * @param	channels			Number of channels channels (1 or 2). 
 *
 * @return	1 if format supported, 0 if not.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::GetDeviceFormatSupport(unsigned int deviceID, bool output, DWORD khz, WORD bits, WORD channels)
{
	const char * cCommand = "mnSound::GetDeviceFormatSupport";
	int returnMe = -1;

	try
	{
		SoundDevice * device = _GetSoundDevice(deviceID,output);
		returnMe = device->IsFormatSupported(khz,bits,channels);
		delete device;
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the number of channels supported by the device.
 *
 * @param	deviceID	Identifier for the device.
 * @param	output		True if it is an output device, false if it is an input device.
 *
 * @return	the number of channels supported.
 */
DBP_CPP_DLL unsigned short mnSound::GetDeviceMaxChannels(unsigned int deviceID, bool output)
{
	const char * cCommand = "mnSound::GetDeviceMaxChannels";
	unsigned short returnMe = 0;

	try
	{
		SoundDevice * device = _GetSoundDevice(deviceID,output);
		returnMe = device->GetNumChannelsSupported();
		delete device;
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Determine whether left and right volume levels can be changed independently of each other.
 *
 * @param	outputDeviceID	Identifier for the device.
 *
 * @return	1 if independent volume change supported, 0 if not. 
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::GetIndependentVolumeControlSupported(unsigned int outputDeviceID)
{
	const char * cCommand = "mnSound::IsIndependentVolumeControlSupported";
	int returnMe = -1;

	try
	{
		SoundDeviceOutput device(outputDeviceID);
		returnMe = device.IsIndependentVolumeControlSupported();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Determine whether pitch control is supported.
 *
 * @param	outputDeviceID	Identifier for the device.
 *
 * @return	1 if pitch control supported, 0 if not. 
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::GetPitchControlSupported(unsigned int outputDeviceID)
{
	const char * cCommand = "mnSound::IsPitchControlSupported";
	int returnMe = -1;

	try
	{
		SoundDeviceOutput device(outputDeviceID);
		returnMe = device.IsPitchControlSupported();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Determine if playback rate control is supported.
 *
 * @param	outputDeviceID	Identifier for the device.
 *
 * @return	1 if playback rate control supported, 0 if not. 
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::GetPlaybackRateControlSupported(unsigned int outputDeviceID)
{
	const char * cCommand = "mnSound::IsPlaybackRateControlSupported";
	int returnMe = -1;

	try
	{
		SoundDeviceOutput device(outputDeviceID);
		returnMe = device.IsPlaybackRateControlSupported();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Determine whether sample accurate position information can be retrieved.
 *
 * @param	outputDeviceID	Identifier for the device.
 *
 * @return	1 if sample accurate, 0 if not. 
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::GetSampleAccurate(unsigned int outputDeviceID)
{
	const char * cCommand = "mnSound::IsSampleAccurate";
	int returnMe = -1;

	try
	{
		SoundDeviceOutput device(outputDeviceID);
		returnMe = device.IsSampleAccurate();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Determine whether volume change is supported by this device.
 *
 * @param	outputDeviceID	Identifier for the device.
 *
 * @return	1 if volume change supported, 0 if not. 
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::GetVolumeControlSupported(unsigned int outputDeviceID)
{
	const char * cCommand = "mnSound::IsVolumeControlSupported";
	int returnMe = -1;

	try
	{
		SoundDeviceOutput device(outputDeviceID);
		returnMe = device.IsVolumeControlSupported();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Starts the input/output module.
 *
 * If the module is already started, it will be restarted.
 *
 * @param	numInputInstances	Number of input instances. 
 * @param	numOutputInstances	Number of output instances. 
 *
 * @return 0 if no error.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::StartSound( size_t numInputInstances, size_t numOutputInstances )
{
	const char * cCommand = "mnSound::StartSound";
	int returnMe = 0;

	try
	{
		SoundUtility::Start(numInputInstances,numOutputInstances);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Cleans up the sound input/output module.
 *
 * @return	0 if no error.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::FinishSound()
{
	const char * cCommand = "mnSound::FinishSound";
	int returnMe = 0;

	try
	{
		SoundUtility::Finish();
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Retrieves the number of input instances created using mnSound::StartSound.
 *
 * @return	the number input instances. 
 */
DBP_CPP_DLL size_t mnSound::GetNumInputInstances()
{
	const char * cCommand = "mnSound::GetNumInputInstances";
	size_t returnMe = 0;

	try
	{
		returnMe = SoundUtility::GetNumInputInstances();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the number of output instances created using mnSound::StartSound.
 *
 * @return	the number output instances. 
 */
DBP_CPP_DLL size_t mnSound::GetNumOutputInstances()
{
	const char * cCommand = "mnSound::GetNumOutputInstances";
	size_t returnMe = 0;

	try
	{
		returnMe = SoundUtility::GetNumOutputInstances();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Creates a sound profile which describes the quality of sound input and output.
 *
 * mnSound::DeleteSoundProfile should be used to clean this up.
 *
 * @return a pointer (cast to an INT_PTR) to a SoundFormat object. 
 */
DBP_CPP_DLL INT_PTR mnSound::CreateSoundProfile()
{
	const char * cCommand = "mnSound::CreateSoundProfile";
	INT_PTR returnMe = NULL;

	try
	{
		SoundFormat * profile = new (nothrow) SoundFormat();
		Utility::DynamicAllocCheck(profile,__LINE__,__FILE__);
		returnMe = reinterpret_cast<INT_PTR>(profile);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Deletes the specified sound profile which was created using mnSound::CreateSoundProfile.
 *
 * @param	profile	The profile to delete.
 *
 * @return  0 if no error occurred.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::DeleteSoundProfile(INT_PTR profile)
{
	const char * cCommand = "mnSound::DeleteSoundProfile";
	int returnMe = 0;

	try
	{
		SoundFormat * ptr = PointerConverter::GetPtrFromInt<SoundFormat>(profile);
		delete ptr;
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Changes the settings stored by the sound profile.
 *
 * @param	profile			The profile to change.
 * @param	khz				Samples per second: 11025 (11.025 kHz), 22050 (22.05 kHz), 44100 (44.1 kHz), 48000 (48 kHz) or 96000 (96 kHz). If 0 then no change is made.
 * @param	bitsPerSample			Bits per sample: 8 or 16. If 0 then no change is made.
 * @param	channels		Number of channels. If 0 then no change is made.
 *
 * @return  0 if no error occurred.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::SetSoundProfile(INT_PTR profile, DWORD khz, WORD bitsPerSample, WORD channels)
{
	const char * cCommand = "mnSound::SetSoundProfile";
	int returnMe = 0;

	try
	{
		SoundFormat * ptr = PointerConverter::GetPtrFromInt<SoundFormat>(profile);
		ptr->Set(khz,bitsPerSample,channels);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Sets the format to the default high quality.
 *
 * The format will be set with the following values:
 * SoundFormat::HIGH_SAMPLES_PER_SEC, SoundFormat::HIGH_BITS_PER_SAMPLE, SoundFormat::HIGH_CHANNELS.
 *
 * @param	profile	The profile to use. 
 *
 * @return  0 if no error occurred.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::SetSoundProfileHighQuality(INT_PTR profile)
{
	const char * cCommand = "mnSound::SetSoundProfileHighQuality";
	int returnMe = 0;

	try
	{
		SoundFormat * ptr = PointerConverter::GetPtrFromInt<SoundFormat>(profile);
		ptr->SetHighQuality();
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Sets the format to the default medium quality.
 *
 * The format will be set with the following values:
 * SoundFormat::MED_SAMPLES_PER_SEC, SoundFormat::MED_BITS_PER_SAMPLE, SoundFormat::MED_CHANNELS.
 *
 * @param	profile	The profile to use. 
 *
 * @return  0 if no error occurred.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::SetSoundProfileMediumQuality(INT_PTR profile)
{
	const char * cCommand = "mnSound::SetSoundProfileMediumQuality";
	int returnMe = 0;

	try
	{
		SoundFormat * ptr = PointerConverter::GetPtrFromInt<SoundFormat>(profile);
		ptr->SetMediumQuality();
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Sets the format to the default low quality.
 *
 * The format will be set with the following values:
 * SoundFormat::LOW_SAMPLES_PER_SEC, SoundFormat::LOW_BITS_PER_SAMPLE, SoundFormat::LOW_CHANNELS.
 *
 * @param	profile	The profile to use. 
 *
 * @return  0 if no error occurred.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::SetSoundProfileLowQuality(INT_PTR profile)
{
	const char * cCommand = "mnSound::SetSoundProfileLowQuality";
	int returnMe = 0;

	try
	{
		SoundFormat * ptr = PointerConverter::GetPtrFromInt<SoundFormat>(profile);
		ptr->SetLowQuality();
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Retrieves the number of channels option.
 *
 * @param	profile	The profile to use. 
 *
 * @return number of channels. See mnSound::SetSoundProfile for more information.
 */
DBP_CPP_DLL	WORD mnSound::GetSoundProfileChannels(INT_PTR profile)
{
	const char * cCommand = "mnSound::GetSoundProfileChannels";
	int returnMe = -1;

	try
	{
		SoundFormat * ptr = PointerConverter::GetPtrFromInt<SoundFormat>(profile);
		returnMe = ptr->GetNumChannels();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the samples per second option.
 *
 * @param	profile	The profile to use. 
 *
 * @return samples per second. See mnSound::SetSoundProfile for more information.
 */
DBP_CPP_DLL DWORD mnSound::GetSoundProfileSamplesPerSecond(INT_PTR profile)
{
	const char * cCommand = "mnSound::GetSoundProfileSamplesPerSecond";
	int returnMe = -1;

	try
	{
		SoundFormat * ptr = PointerConverter::GetPtrFromInt<SoundFormat>(profile);
		returnMe = ptr->GetSamplesPerSecond();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the bits per sample option.
 *
 * @param	profile	The profile to use. 
 *
 * @return	bits per sample. See mnSound::SetSoundProfile for more information.
 */
DBP_CPP_DLL WORD mnSound::GetSoundProfileBitsPerSample(INT_PTR profile)
{
	const char * cCommand = "mnSound::GetSoundProfileBitsPerSample";
	int returnMe = -1;

	try
	{
		SoundFormat * ptr = PointerConverter::GetPtrFromInt<SoundFormat>(profile);
		returnMe = ptr->GetBitsPerSample();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Starts an input instance.
 *
 * @param	instanceID		Identifier for the instance slot to use. 
 * @param	inputDeviceID	ID of device that instance should use to retrieve input data. Set to -1 and the device will be chosen automatically. 
 * @param	numBuffers		Number of buffers. 
 * @param	buffTimeLength	Length of a buffer in milliseconds.
 * @param	soundFormat		The sound format that device should use. 
 *
 * @return  0 if no error occurred.
 * @return	-1 if an error occurred.
 */
int mnSound::StartInput(size_t instanceID, unsigned int inputDeviceID, size_t numBuffers, size_t buffTimeLength, const SoundFormat & soundFormat)
{
	const char * cCommand = "mnSound::StartInput";
	int returnMe = 0;

	try
	{
		SoundInstanceInput * instance = new (nothrow) SoundInstanceInput(numBuffers,buffTimeLength,soundFormat,inputDeviceID);
		Utility::DynamicAllocCheck(instance,__LINE__,__FILE__);
		SoundUtility::AddInputInstance(instanceID,instance);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Starts an input instance.
 *
 * @param	instanceID		Identifier for the instance slot to use. 
 * @param	inputDeviceID	ID of device that instance should use to retrieve input data. Set to -1 and the device will be chosen automatically. 
 * @param	numBuffers		Number of buffers. 
 * @param	buffTimeLength	Length of a buffer in milliseconds.
 * @param	soundFormat		The sound format that device should use. 
 *
 * @return  0 if no error occurred.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::StartInput(size_t instanceID, unsigned int inputDeviceID, size_t numBuffers, size_t buffTimeLength, INT_PTR soundFormat)
{
	const char * cCommand = "mnSound::StartInput";
	int returnMe = 0;

	try
	{
		SoundFormat & ref = PointerConverter::GetRefFromInt<SoundFormat>(soundFormat);
		StartInput(instanceID,inputDeviceID,numBuffers,buffTimeLength,ref);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Cleans up the specified input instance.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return  0 if no error occurred.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::FinishInput(size_t instanceID)
{
	const char * cCommand = "mnSound::FinishInput";
	int returnMe = 0;

	try
	{
		SoundUtility::CleanupInputInstance(instanceID);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Pauses input. 
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return  0 if no error occurred.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::PauseInput(size_t instanceID)
{
	const char * cCommand = "mnSound::PauseInput";
	int returnMe = 0;

	try
	{
		SoundUtility::GetInputInstance(instanceID)->PauseInput();
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Unpauses input. 
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return  0 if no error occurred.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::UnpauseInput(size_t instanceID)
{
	const char * cCommand = "mnSound::UnpauseInput";
	int returnMe = 0;

	try
	{
		SoundUtility::GetInputInstance(instanceID)->UnpauseInput();
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Determines whether input is unpaused.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return  1 if input is unpaused.
 * @return  0 if input is paused.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::GetInputUnpaused(size_t instanceID)
{
	const char * cCommand = "mnSound::GetInputUnpaused";
	int returnMe = -1;

	try
	{
		returnMe = !SoundUtility::GetInputInstance(instanceID)->IsPaused();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Determines whether input is paused.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return  1 if input is paused.
 * @return  0 if input is unpaused.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::GetInputPaused(size_t instanceID)
{
	const char * cCommand = "mnSound::GetInputPaused";
	int returnMe = -1;
	
	try
	{
		returnMe = SoundUtility::GetInputInstance(instanceID)->IsPaused();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves input data that has been received.
 *
 * @param	instanceID			Identifier for the instance. 
 * @param [out]	destination		Destination packet to copy data to.
 *
 * @return	the number of packets in the retrieved input data queue before this call. 
 */
size_t mnSound::GetInputData( size_t instanceID, Packet & destination )
{
	const char * cCommand = "mnSound::GetInputData";
	size_t returnMe = -1;

	try
	{
		returnMe = SoundUtility::GetInputInstance(instanceID)->GetRetrievedPacket(&destination);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves input data that has been received.
 *
 * @param	instanceID			Identifier for the instance. 
 * @param [out]	destination		Destination packet to copy data to.
 *
 * @return	the input data. 
 */
DBP_CPP_DLL size_t mnSound::GetInputData( size_t instanceID, INT_PTR destination )
{
	const char * cCommand = "mnSound::GetInputData";
	size_t returnMe = 0;

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(destination);
		returnMe = GetInputData(instanceID,ref);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the number of input channels in use by the instance.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return	the number of input channels in use. 
 */
DBP_CPP_DLL WORD mnSound::GetInputChannels(size_t instanceID)
{
	const char * cCommand = "mnSound::GetInputChannels";
	WORD returnMe = 0;

	try
	{
		returnMe = SoundUtility::GetInputInstance(instanceID)->GetFormat().GetNumChannels();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the samples rate in use by the instance.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return	the sample rate.
 */
DBP_CPP_DLL DWORD mnSound::GetInputSamplesPerSec(size_t instanceID)
{
	const char * cCommand = "mnSound::GetInputSamplesPerSec";
	DWORD returnMe = 0;

	try
	{
		returnMe = SoundUtility::GetInputInstance(instanceID)->GetFormat().GetSamplesPerSecond();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the bits per sample of the instance.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return	bits per sample.
 */
DBP_CPP_DLL WORD mnSound::GetInputBitsPerSample(size_t instanceID)
{
	const char * cCommand = "mnSound::GetInputBitsPerSample";
	unsigned short returnMe = 0;

	try
	{
		returnMe = SoundUtility::GetInputInstance(instanceID)->GetFormat().GetBitsPerSample();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the number of buffers in use by the instance.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return	the number of buffers in use.
 */
DBP_CPP_DLL size_t mnSound::GetInputNoBuffers(size_t instanceID)
{
	const char * cCommand = "mnSound::GetInputNoBuffers";
	size_t returnMe = 0;

	try
	{
		returnMe = SoundUtility::GetInputInstance(instanceID)->GetNumBuffers();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the number of milliseconds worth of data that each buffer can store.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return	milliseconds per buffer.
 */
DBP_CPP_DLL size_t mnSound::GetInputTimePerBuffer(size_t instanceID)
{
	const char * cCommand = "mnSound::GetInputTimePerBuffer";
	size_t returnMe = 0;

	try
	{
		returnMe = SoundUtility::GetInputInstance(instanceID)->GetBufferTimeLength();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Starts an output instance.
 *
 * @param	instanceID			Identifier for the instance. 
 * @param	outputDeviceID		ID of device that instance should use to post output data.
 * @param	msStoredDataCap		The length of time for output not to be played without new output being added
 *								to the queue, before ending the wait and resuming playback regardless of the amount of data stored.
 * @param	timeGapCap			The amount of data that needs to be stored for playback to commence.
 * @param	soundFormat			Sound format that device should use.
 *
 * @return  0 if no error occurred.
 * @return	-1 if an error occurred.
 */
int mnSound::StartOutput(size_t instanceID, unsigned int outputDeviceID, size_t msStoredDataCap, size_t timeGapCap, const SoundFormat & soundFormat)
{
	const char * cCommand = "mnSound::StartOutput";
	int returnMe = 0;

	try
	{
		SoundInstanceOutput * instance = new (nothrow) SoundInstanceOutput(msStoredDataCap,static_cast<clock_t>(timeGapCap),soundFormat,outputDeviceID);
		Utility::DynamicAllocCheck(instance,__LINE__,__FILE__);
		SoundUtility::AddOutputInstance(instanceID,instance);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Starts an output instance.
 *
 * @param	instanceID			Identifier for the instance. 
 * @param	outputDeviceID		ID of device that instance should use to post output data.
 * @param	msStoredDataCap		The length of time for output not to be played without new output being added
 *								to the queue, before ending the wait and resuming playback regardless of the amount of data stored.
 * @param	timeGapCap			The amount of data that needs to be stored for playback to commence.
 * @param	soundFormat			Sound format that device should use.
 *
 * @return  0 if no error occurred.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::StartOutput(size_t instanceID, unsigned int outputDeviceID, size_t msStoredDataCap, size_t timeGapCap, INT_PTR soundFormat)
{
	const char * cCommand = "mnSound::StartOutput";
	int returnMe = 0;

	try
	{
		SoundFormat & ref = PointerConverter::GetRefFromInt<SoundFormat>(soundFormat);
		StartOutput(instanceID,outputDeviceID,msStoredDataCap,timeGapCap,ref);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Cleans up an output instance.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return  0 if no error occurred.
 * @return	-1 if an error occurred. 
 */
DBP_CPP_DLL int mnSound::FinishOutput(size_t instanceID)
{
	const char * cCommand = "mnSound::FinishOutput";
	int returnMe = 0;

	try
	{
		SoundUtility::CleanupOutputInstance(instanceID);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Outputs the specified packet.
 *
 * @param	instanceID	Identifier for the instance. 
 * @param	packet		The packet to output.
 *
 * @return  0 if no error occurred.
 * @return	-1 if an error occurred.
 */
int mnSound::PlayData(size_t instanceID, const Packet & packet)
{
	const char * cCommand = "mnSound::PlayData";
	int returnMe = 0;

	try
	{
		Packet * queueMe = new (nothrow) Packet(packet);
		Utility::DynamicAllocCheck(queueMe,__LINE__,__FILE__);
		SoundUtility::GetOutputInstance(instanceID)->QueuePacket(queueMe);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Outputs the specified packet.
 *
 * @param	instanceID	Identifier for the instance. 
 * @param	packet		The packet to output.
 *
 * @return  0 if no error occurred.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::PlayData(size_t instanceID, INT_PTR packet)
{
	const char * cCommand = "mnSound::PlayData";
	int returnMe = 0;

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		PlayData(instanceID,ref);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Changes values which may effect how smooth playback is.
 *
 * @param	instanceID	Identifier for the instance. 
 * @param	msStoredDataCap	The amount of data (in milliseconds) that needs to be stored for playback to commence.
 * @param	timeGapCap	The length of time for output not to be played without new output being added
 * to the queue, before ending the wait and resuming playback regardless of the amount of data stored.
 */
DBP_CPP_DLL int mnSound::SetOutputSmoothValues(size_t instanceID, size_t msStoredDataCap, size_t timeGapCap)
{
	const char * cCommand = "mnSound::SetOutputSmoothValues";
	int returnMe = 0;

	try
	{
		SoundUtility::GetOutputInstance(instanceID)->SetSmoothValues(msStoredDataCap,static_cast<clock_t>(timeGapCap));
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Retrieves the number of output channels in use by the instance.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return	the number of output channels in use. 
 */
DBP_CPP_DLL WORD mnSound::GetOutputChannels(size_t instanceID)
{
	const char * cCommand = "mnSound::GetOutputChannels";
	WORD returnMe = 0;

	try
	{
		returnMe = SoundUtility::GetOutputInstance(instanceID)->GetFormat().GetNumChannels();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the samples rate in use by the instance.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return	the sample rate.
 */
DBP_CPP_DLL DWORD mnSound::GetOutputSamplesPerSec(size_t instanceID)
{
	const char * cCommand = "mnSound::GetOutputSamplesPerSec";
	DWORD returnMe = 0;

	try
	{
		returnMe = SoundUtility::GetOutputInstance(instanceID)->GetFormat().GetSamplesPerSecond();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the bits per sample of the instance.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return	bits per sample.
 */
DBP_CPP_DLL WORD mnSound::GetOutputBitsPerSample(size_t instanceID)
{
	const char * cCommand = "mnSound::GetOutputBitsPerSample";
	WORD returnMe = 0;

	try
	{
		returnMe = SoundUtility::GetOutputInstance(instanceID)->GetFormat().GetBitsPerSample();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the amount of data (in milliseconds) that needs to be stored for playback to commence.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return	the stored data cap. 
 */
DBP_CPP_DLL size_t mnSound::GetOutputStoredDataCap(size_t instanceID)
{
	const char * cCommand = "mnSound::GetOutputStoredDataCap";
	size_t returnMe = 0;

	try
	{
		returnMe = SoundUtility::GetOutputInstance(instanceID)->GetStoredDataCapMs();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the length of time for output not to be played without new output being added
 * to the queue, before ending the wait and resuming playback regardless of the amount of data stored.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return	the length of time for output not to be played without new output being added
 * to the queue, before ending the wait and resuming playback regardless of the amount of data stored.
 */
DBP_CPP_DLL size_t mnSound::GetOutputLastAddedCap(size_t instanceID)
{
	const char * cCommand = "mnSound::GetOutputLastAddedCap";
	size_t returnMe = 0;

	try
	{
		returnMe = SoundUtility::GetOutputInstance(instanceID)->GetTimeGapCap();

	}
	STD_CATCH

	return(returnMe);
}






/**
 * @brief	Changes the volume of sound data stored in a packet.
 *
 * @param	bitsPerSample		The bits per sample of the stored data in the packet.
 * @param [in,out]	dataPacket		The packet to manipulate.
 * @param	percentageChange	The percentage to change the volume e.g. 200 doubles the volume. 
 */
int mnSound::SetDataVolume(WORD bitsPerSample, Packet * dataPacket, double percentageChange)
{
	const char * cCommand = "mnSound::SetDataVolume";
	int returnMe = 0;
	
	try
	{
		SoundInstanceInput::SetDataVolume(bitsPerSample,dataPacket,percentageChange);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Calculates the volume of sound data stored in a packet.
 *
 * @param	bitsPerSample	The bits per sample of the sound data stored in the specified packet.
 * @param	dataPacket		The packet to use. 
 *
 * @return	the volume of the data. If 16 bit then the range is 0 to 65535. If 8 bit then the range is 0 to 255.
 */
int mnSound::GetDataVolume(WORD bitsPerSample, const Packet * dataPacket)
{
	const char * cCommand = "mnSound::GetDataVolume";
	int returnMe = 0;
	
	try
	{
		returnMe = SoundInstanceInput::GetDataVolume(bitsPerSample,dataPacket);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Changes the volume of sound data stored in a packet.
 *
 * @param	bitsPerSample		The bits per sample of the stored data in the packet.
 * @param [in,out]	dataPacket		The packet to manipulate.
 * @param	percentageChange	The percentage to change the volume e.g. 200 doubles the volume.
 * 
 * @return 0 if no error occurred.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mnSound::SetDataVolume(WORD bitsPerSample, INT_PTR dataPacket, double percentageChange)
{
	const char * cCommand = "mnSound::SetDataVolume";
	int returnMe = 0;
	
	try
	{
		Packet * ptr = PointerConverter::GetPtrFromInt<Packet>(dataPacket);
		SetDataVolume(bitsPerSample,ptr,percentageChange);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Calculates the volume of sound data stored in a packet.
 *
 * @param	bitsPerSample	The bits per sample of the sound data stored in the specified packet.
 * @param	dataPacket		The packet to use. 
 *
 * @return	the volume of the data. If 16 bit then the range is 0 to 65535. If 8 bit then the range is 0 to 255.
 */
DBP_CPP_DLL int mnSound::GetDataVolume(WORD bitsPerSample, INT_PTR dataPacket)
{
	const char * cCommand = "mnSound::GetDataVolume";
	int returnMe = 0;
	
	try
	{
		Packet * ptr = PointerConverter::GetPtrFromInt<Packet>(dataPacket);
		return GetDataVolume(bitsPerSample,ptr);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Retrieves the playback rate of sound output by accessing the device driver.
 *
 * This operation may not be supported by the device driver. You can check this using mnSound::GetPlaybackRateControlSupported.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return playback rate. This is as a percentage where 100 is no rate change, 200 doubles the rate, 50 halves the rate.
 */
DBP_CPP_DLL double mnSound::GetHardwarePlaybackRate(size_t instanceID)
{
	const char * cCommand = "mnSound::GetHardwarePlaybackRate";
	double dReturn = 0.0;

	try
	{
		dReturn = SoundUtility::GetOutputInstance(instanceID)->GetHardwarePlaybackRate();
	}
	STD_CATCH

	return(dReturn);
}

/**
 * @brief	Retrieves the pitch change in action by accessing the device driver.
 *
 * This operation may not be supported by the device driver. You can check this using mnSound::GetPitchControlSupported.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * @return	pitch change. This is as a percentage where 100 is no pitch change, 200 doubles the pitch, 50 halves the pitch.
 */
DBP_CPP_DLL double mnSound::GetHardwarePitch(size_t instanceID)
{
	const char * cCommand = "mnSound::GetHardwarePitch";
	double dReturn = 0.0;

	try
	{
		dReturn = SoundUtility::GetOutputInstance(instanceID)->GetHardwarePitch();
	}
	STD_CATCH

	return(dReturn);
}

/**
 * @brief	Retrieves the volume of the left channel.
 *
 * This operation may not be supported by the device driver. You can check this using mnSound::GetVolumeControlSupported.
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * If the driver does not support independent left/right volume change, this will
 * return the volume of both channels.
 *
 * @return	the volume of the left channel.
 */
DBP_CPP_DLL unsigned short mnSound::GetHardwareVolumeLeft(size_t instanceID)
{
	const char * cCommand = "mnSound::GetHardwareVolumeLeft";
	unsigned short returnMe = 0;

	try
	{
		returnMe = SoundUtility::GetOutputInstance(instanceID)->GetHardwareVolumeLeft();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Retrieves the volume of the right channel.
 *
 * If only one channel exists then this will return the same as SoundInstanceOutput::GetHardwareVolumeLeft().
 *
 * @param	instanceID	Identifier for the instance. 
 *
 * This operation may not be supported by the device driver. You can check this using mnSound::GetVolumeControlSupported.
 *
 * @return	the volume of the right channel.
 */
DBP_CPP_DLL unsigned short mnSound::GetHardwareVolumeRight(size_t instanceID)
{
	const char * cCommand = "mnSound::GetHardwareVolumeRight";
	unsigned short returnMe = 0;

	try
	{
		returnMe = SoundUtility::GetOutputInstance(instanceID)->GetHardwareVolumeRight();
	}
	STD_CATCH

	return(returnMe);
}


/**
 * @brief	Changes the playback rate of sound output by accessing the device driver.
 *
 * This operation may not be supported by the device driver. You can check this using mnSound::GetPlaybackRateControlSupported.
 *
 * @param	instanceID	Identifier for the instance.
 * @param	rate	New playback rate. This is as a percentage where 100 is no rate change, 200 doubles the rate, 50 halves the rate.
 */
DBP_CPP_DLL int mnSound::SetHardwarePlaybackRate(size_t instanceID, double rate)
{
	const char * cCommand = "mnSound::SetHardwarePlaybackRate";
	int returnMe = 0;

	try
	{
		SoundUtility::GetOutputInstance(instanceID)->SetHardwarePlaybackRate(rate);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Changes the pitch of sound output by accessing the device driver.
 *
 * This operation may not be supported by the device driver. You can check this using mnSound::GetPitchControlSupported.
 *
 * @param	instanceID	Identifier for the instance.
 * @param	pitch	Pitch change. This is as a percentage where 100 is no pitch change, 200 doubles the pitch, 50 halves the pitch.
 */
DBP_CPP_DLL int mnSound::SetHardwarePitch(size_t instanceID, double pitch)
{
	const char * cCommand = "mnSound::SetHardwarePitch";
	int returnMe = 0;

	try
	{
		SoundUtility::GetOutputInstance(instanceID)->SetHardwarePitch(pitch);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Changes the volume sound output by accessing the device driver.
 *
 * This operation may not be supported by the device driver. You can check this using mnSound::GetVolumeControlSupported.
 *
 * If the driver does not support independent left/right volume change, only the left value
 * will be taken into account and this will be the value used by both channels.
 *
 * @param	instanceID	Identifier for the instance.
 * @param	left	New volume for the left channel.
 * @param	right	New volume for the right channel.
 */
DBP_CPP_DLL int mnSound::SetHardwareVolume(size_t instanceID, WORD left, WORD right)
{
	const char * cCommand = "mnSound::SetHardwareVolume";
	int returnMe = 0;

	try
	{
		SoundUtility::GetOutputInstance(instanceID)->SetHardwareVolume(left,right);
	}
	STD_CATCH_RM

	return(returnMe);
}

#ifdef DBP
/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mnSound::DBP_GetDeviceName(DWORD pOldString, size_t DeviceID, bool Output)
{
	const char * cReturn = GetDeviceName(DeviceID, Output);

	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

	return(dReturn);
}
#endif