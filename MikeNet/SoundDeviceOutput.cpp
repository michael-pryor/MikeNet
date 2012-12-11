#include "FullInclude.h"

/**
 * @brief	Constructor. 
 *
 * @param	deviceID	Identifier for the device. Set to -1 and the device will be chosen automatically.
 */
SoundDeviceOutput::SoundDeviceOutput(unsigned int deviceID)
{
	MMRESULT result = waveOutGetDevCaps(deviceID,&info,sizeof(WAVEOUTCAPS));
	_ErrorException((result != MMSYSERR_NOERROR),"retrieving information about a sound output device",result,__LINE__,__FILE__);
}

/**
 * @brief	Destructor. 
 */
SoundDeviceOutput::~SoundDeviceOutput(void)
{
}

/**
 * @brief	Retrieves the manufacturer ID of the device.
 *
 * @return	the manufacturer identifier. 
 */
WORD SoundDeviceOutput::GetManufacturerID() const
{
	return info.wMid;
}

/**
 * @brief	Retrieves the product ID of the device.
 *
 * @return	the product identifier. 
 */
WORD SoundDeviceOutput::GetProductID() const
{
	return info.wPid;
}

/**
 * @brief	Retrieves the name of the device.
 *
 * @return	the name of the device. 
 */
const char * SoundDeviceOutput::GetName() const
{
	return info.szPname;
}

/**
 * @brief	Retrieves information about what formats are supported by the device.
 *
 * @return	information about what formats are supported by the device (stored in the bits of a DWORD).
 */
DWORD SoundDeviceOutput::GetFormatSupport() const
{
	return info.dwFormats;
}

/**
 * @brief	Retrieves the number of channels supported by the device.
 *
 * @return	the number of channels supported.
 */
WORD SoundDeviceOutput::GetNumChannelsSupported() const
{
	return info.wChannels;
}

/**
 * @brief	Determine whether volume change is supported by this device.
 *
 * @return	true if volume change supported, false if not. 
 */
bool SoundDeviceOutput::IsVolumeControlSupported() const
{
	return BITMASK_GET(info.dwSupport,WAVECAPS_VOLUME);
}

/**
 * @brief	Determine whether left and right volume levels can be changed independently of each other.
 *
 * @return	true if independent volume change supported, false if not. 
 */
bool SoundDeviceOutput::IsIndependentVolumeControlSupported() const
{
	return BITMASK_GET(info.dwSupport,WAVECAPS_LRVOLUME);
}

/**
 * @brief	Determine whether pitch control is supported.
 *
 * @return	true if pitch control supported, false if not. 
 */
bool SoundDeviceOutput::IsPitchControlSupported() const
{
	return BITMASK_GET(info.dwSupport,WAVECAPS_PITCH);
}

/**
 * @brief	Determine if playback rate control is supported.
 *
 * @return	true if playback rate control supported, false if not. 
 */
bool SoundDeviceOutput::IsPlaybackRateControlSupported() const
{
	return BITMASK_GET(info.dwSupport,WAVECAPS_PLAYBACKRATE);
}

/**
 * @brief	Determine whether sample accurate position information can be retrieved.
 *
 * @return	true if sample accurate, false if not. 
 */
bool SoundDeviceOutput::IsSampleAccurate() const
{
	return BITMASK_GET(info.dwSupport,WAVECAPS_SAMPLEACCURATE);
}

/**
 * @brief	Retrieves the number of output devices available.
 *
 * @return	the number of output devices available.
 */
unsigned int SoundDeviceOutput::GetNumDevices()
{
	return waveOutGetNumDevs();
}

/**
 * @brief	Retrieves the major version of the device's driver e.g. if the version is 4.5 then 4 will be returned.
 *
 * @return	the driver major version. 
 */
unsigned char SoundDeviceOutput::GetDriverMajorVersion() const
{
	return LOBYTE(info.vDriverVersion);
}

/**
 * @brief	Gets the minor version of the device's driver e.g. if the version if 4.5 then 5 will be returned.
 *
 * @return	the driver minor version. 
 */
unsigned char SoundDeviceOutput::GetDriverMinorVersion() const
{
	return HIBYTE(info.vDriverVersion);	
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool SoundDeviceOutput::TestClass()
{
	cout << "Testing SoundDeviceOutput class...\n";
	bool problem = false;

	for(unsigned int n = 0;n<SoundDeviceOutput::GetNumDevices();n++)
	{
		SoundDeviceOutput device(n);

		cout << "Device " << n << '\n';
		cout << " Name: " << device.GetName() << '\n';
		cout << " channels supported: " << device.GetNumChannelsSupported() << '\n';
		cout << " Manufacturer ID: " << device.GetManufacturerID() << '\n';
		cout << " Product ID: " << device.GetProductID() << '\n';
		cout << " Version: " << device.GetDriverMajorVersion() << "." << device.GetDriverMinorVersion() << '\n';
		cout << " Independent volume control supported: " << device.IsIndependentVolumeControlSupported() << '\n';
		cout << " Pitch control supported: " << device.IsPitchControlSupported() << '\n';
		cout << " Playback rate control supported: " << device.IsPlaybackRateControlSupported() << '\n';
		cout << " Sample accurate: " << device.IsSampleAccurate() << '\n';
		cout << " Volume control supported: " << device.IsVolumeControlSupported() << '\n';
		cout << " Supports 11.025, mono, 8 bit: " << device.IsFormatSupported(11025,8,1) << '\n';
		cout << " Supports 11.025, mono, 16 bit: " << device.IsFormatSupported(11025,16,1) << '\n';
		cout << " Supports 11.025, stereo, 8 bit: " << device.IsFormatSupported(11025,8,2) << '\n';
		cout << " Supports 11.025, stereo, 16 bit: " << device.IsFormatSupported(11025,16,2) << '\n';
		cout << " Supports 22.05, mono, 8 bit: " << device.IsFormatSupported(22050,8,1) << '\n';
		cout << " Supports 22.05, mono, 16 bit: " << device.IsFormatSupported(22050,16,1) << '\n';
		cout << " Supports 22.05, stereo, 8 bit: " << device.IsFormatSupported(22050,8,2) << '\n';
		cout << " Supports 22.05, stereo, 16 bit: " << device.IsFormatSupported(22050,16,2) << '\n';
		cout << " Supports 44.1, mono, 8 bit: " << device.IsFormatSupported(44100,8,1) << '\n';
		cout << " Supports 44.1, mono, 16 bit: " << device.IsFormatSupported(44100,16,1) << '\n';
		cout << " Supports 44.1, stereo, 8 bit: " << device.IsFormatSupported(44100,8,2) << '\n';
		cout << " Supports 44.1, stereo, 16 bit: " << device.IsFormatSupported(44100,16,2) << '\n';
		cout << " Supports 48, mono, 8 bit: " << device.IsFormatSupported(48000,8,1) << '\n';
		cout << " Supports 48, mono, 16 bit: " << device.IsFormatSupported(48000,16,1) << '\n';
		cout << " Supports 48, stereo, 8 bit: " << device.IsFormatSupported(48000,8,2) << '\n';
		cout << " Supports 48, stereo, 16 bit: " << device.IsFormatSupported(48000,16,2) << '\n';
		cout << " Supports 96, mono, 8 bit: " << device.IsFormatSupported(96000,8,1) << '\n';
		cout << " Supports 96, mono, 16 bit: " << device.IsFormatSupported(96000,16,1) << '\n';
		cout << " Supports 96, stereo, 8 bit: " << device.IsFormatSupported(96000,8,2) << '\n';
		cout << " Supports 96, stereo, 16 bit: " << device.IsFormatSupported(96000,16,2) << '\n';
	}
	
	cout << "\n\n";
	return !problem;
}
