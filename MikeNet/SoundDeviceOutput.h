#pragma once
#include "sounddevice.h"

/**
 * @brief	Stores information about an output device.
 * @remarks	Michael Pryor, 9/20/2010. 
 */
class SoundDeviceOutput :
	public SoundDevice
{
	/**
	 * @brief Stores information about the output device.
	 */
	WAVEOUTCAPS info;
public:
	SoundDeviceOutput(unsigned int deviceID);
	virtual ~SoundDeviceOutput(void);

	WORD GetManufacturerID() const;
	WORD GetProductID() const;
	const char * GetName() const;
	unsigned char GetDriverMajorVersion() const;
	unsigned char GetDriverMinorVersion() const;
private:
	DWORD GetFormatSupport() const;
public:
	WORD GetNumChannelsSupported() const;

	bool IsVolumeControlSupported() const;
	bool IsIndependentVolumeControlSupported() const;
	bool IsPitchControlSupported() const;
	bool IsPlaybackRateControlSupported() const;
	bool IsSampleAccurate() const;

	static unsigned int GetNumDevices();

	static bool TestClass();
};

