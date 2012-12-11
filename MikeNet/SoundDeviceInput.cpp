#include "FullInclude.h"

/**
 * @brief	Constructor. 
 *
 * @param	deviceID	Identifier for the device. 
 */
SoundDeviceInput::SoundDeviceInput(unsigned int deviceID)
{
	MMRESULT result = waveInGetDevCaps(deviceID,&info,sizeof(WAVEINCAPS));
	_ErrorException((result != MMSYSERR_NOERROR),"retrieving information about a sound input device",result,__LINE__,__FILE__);
}

/**
 * @brief	Destructor. 
 */
SoundDeviceInput::~SoundDeviceInput(void)
{
}

/**
 * @brief	Retrieves the manufacturer ID of the device.
 *
 * @return	the manufacturer identifier. 
 */
WORD SoundDeviceInput::GetManufacturerID() const
{
	return info.wMid;
}

/**
 * @brief	Retrieves the product ID of the device.
 *
 * @return	the product identifier. 
 */
WORD SoundDeviceInput::GetProductID() const
{
	return info.wPid;
}

/**
 * @brief	Retrieves the name of the device.
 *
 * @return	the name of the device. 
 */
const char * SoundDeviceInput::GetName() const
{
	return info.szPname;
}

/**
 * @brief	Retrieves information about what formats are supported by the device.
 *
 * @return	information about what formats are supported by the device (stored in the bits of a DWORD).
 */
DWORD SoundDeviceInput::GetFormatSupport() const
{
	return info.dwFormats;
}

/**
 * @brief	Retrieves the number of channels supported by the device.
 *
 * @return	the number of channels supported.
 */
WORD SoundDeviceInput::GetNumChannelsSupported() const
{
	return info.wChannels;
}

/**
 * @brief	Retrieves the number of input devices available.
 *
 * @return	the number of input devices available.
 */
unsigned int SoundDeviceInput::GetNumDevices()
{
	return waveInGetNumDevs();
}

/**
 * @brief	Retrieves the major version of the device's driver e.g. if the version is 4.5 then 4 will be returned.
 *
 * @return	the driver major version. 
 */
unsigned char SoundDeviceInput::GetDriverMajorVersion() const
{
	return LOBYTE(info.vDriverVersion);
}

/**
 * @brief	Gets the minor version of the device's driver e.g. if the version if 4.5 then 5 will be returned.
 *
 * @return	the driver minor version. 
 */
unsigned char SoundDeviceInput::GetDriverMinorVersion() const
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
bool SoundDeviceInput::TestClass()
{
	cout << "Testing SoundDeviceInput class...\n";
	bool problem = false;

	for(unsigned int n = 0;n<SoundDeviceInput::GetNumDevices();n++)
	{
		SoundDeviceInput device(n);

		cout << "Device " << n << '\n';
		cout << " Name: " << device.GetName() << '\n';
		cout << " channels supported: " << device.GetNumChannelsSupported() << '\n';
		cout << " Manufacturer ID: " << device.GetManufacturerID() << '\n';
		cout << " Product ID: " << device.GetProductID() << '\n';
		cout << " Version: " << device.GetDriverMajorVersion() << "." << device.GetDriverMinorVersion() << '\n';
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
