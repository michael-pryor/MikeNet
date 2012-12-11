#pragma once

/**
 * @brief	Stores information about a sound input or output device.
 * @remarks	Michael Pryor, 9/20/2010. 
 */
class SoundDevice
{
public:
	SoundDevice();
	virtual ~SoundDevice();

	virtual WORD GetManufacturerID() const=0;
	virtual WORD GetProductID() const=0;
	virtual const char * GetName() const=0;

private:
	virtual DWORD GetFormatSupport() const=0;
public:
	bool IsFormatSupported(DWORD samplesPerSecond, WORD bits, WORD channels) const;

	virtual unsigned char GetDriverMajorVersion() const=0;
	virtual unsigned char GetDriverMinorVersion() const=0;

	virtual WORD GetNumChannelsSupported() const=0;
};

