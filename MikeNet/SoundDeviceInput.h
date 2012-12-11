#pragma once
#include "sounddevice.h"

/**
 * @brief	Stores information about an input device.
 * @remarks	Michael Pryor, 9/20/2010. 
 */
class SoundDeviceInput :
	public SoundDevice
{
	/**
	 * @brief Stores information about the input device.
	 */
	WAVEINCAPS info;
public:
	SoundDeviceInput(unsigned int deviceID);
	virtual ~SoundDeviceInput(void);

	WORD GetManufacturerID() const;
	WORD GetProductID() const;
	const char * GetName() const;
	unsigned char GetDriverMajorVersion() const;
	unsigned char GetDriverMinorVersion() const;

private:
	DWORD GetFormatSupport() const;
public:
	WORD GetNumChannelsSupported() const;

	static unsigned int GetNumDevices();

	static bool TestClass();
};

