#include "FullInclude.h"

/**
 * @brief	Default constructor. 
 */
SoundFormat::SoundFormat()
{
	DefaultVariables();
}

/**
 * @brief	Deep copy constructor.
 *
 * @param	copyMe	Object to copy. 
 */
SoundFormat::SoundFormat( const SoundFormat & copyMe )
{
	this->format = copyMe.format;
}

/**
 * @brief	Deep assignment operator.
 *
 * @param	copyMe	Object to copy. 
 *
 * @return	a reference to this object.
 */
SoundFormat & SoundFormat::operator=(const SoundFormat & copyMe)
{
	this->format = copyMe.format;
	return *this;
}

/**
 * @brief	Sets variables to default.
 */
void SoundFormat::DefaultVariables()
{
	format.wFormatTag = WAVE_FORMAT_PCM;
	SetMediumQuality();
}

/**
 * @brief	Sets the format to the default low quality.
 *
 * The format will be set with the following values:
 * SoundFormat::LOW_SAMPLES_PER_SEC, SoundFormat::LOW_BITS_PER_SAMPLE, SoundFormat::LOW_CHANNELS.
 */
void SoundFormat::SetLowQuality()
{
	Set(LOW_SAMPLES_PER_SEC,LOW_BITS_PER_SAMPLE,LOW_CHANNELS);
}

/**
 * @brief	Sets the format to the default medium quality.
 *
 * The format will be set with the following values:
 * SoundFormat::MED_SAMPLES_PER_SEC, SoundFormat::MED_BITS_PER_SAMPLE, SoundFormat::MED_CHANNELS.
 */
void SoundFormat::SetMediumQuality()
{
	Set(MED_SAMPLES_PER_SEC,MED_BITS_PER_SAMPLE,MED_CHANNELS);
}

/**
 * @brief	Sets the format to the default high quality.
 *
 * The format will be set with the following values:
 * SoundFormat::HIGH_SAMPLES_PER_SEC, SoundFormat::HIGH_BITS_PER_SAMPLE, SoundFormat::HIGH_CHANNELS.
 */
void SoundFormat::SetHighQuality()
{
	Set(HIGH_SAMPLES_PER_SEC,HIGH_BITS_PER_SAMPLE,HIGH_CHANNELS);
}

/**
 * @brief	Sets sound format settings.
 *
 * @param	samplesPerSecond	Samples per second: 11025 (11.025 kHz), 22050 (22.05 kHz), 44100 (44.1 kHz) or 48000 (48 kHz). If 0 then no change is made.
 * @param	bits				Bits per sample: 8 or 16. If 0 then no change is made.
 * @param	channels			Number of channels. If 0 then no change is made.
 */
void SoundFormat::Set(DWORD samplesPerSecond, WORD bits, WORD channels)
{
	if(channels > 0)
	{
		format.nChannels = channels;
	}

	if(samplesPerSecond > 0)
	{
		format.nSamplesPerSec = samplesPerSecond;
	}

	if(samplesPerSecond > 0)
	{
		format.wBitsPerSample = bits;
	}

	// Below settings are for WAVE_FORMAT_PCM
	format.nBlockAlign = format.nChannels*format.wBitsPerSample/8;
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
	format.cbSize = 0;
};

/**
 * @brief	Retrieves the number of samples per second. 
 *
 * @return	number of samples per second.
 */
DWORD SoundFormat::GetSamplesPerSecond() const
{
	return format.nSamplesPerSec;
}

/**
 * @brief	Retrieves the number of bits per sample on a single channel.
 *
 * @return	the number of bits per sample.
 */
WORD SoundFormat::GetBitsPerSample() const
{
	return format.wBitsPerSample;
}

/**
 * @brief	Retrieves the number of bytes per sample across multiple channels. 
 *
 * @return	the number of bytes per sample.
 */
WORD SoundFormat::GetBytesPerSample() const
{
	return format.nBlockAlign;
}

/**
 * @brief	Retrieves the number of channels.
 *
 * @return	the number of channels. 
 */
WORD SoundFormat::GetNumChannels() const
{
	return format.nChannels;
}

/**
 * @brief	Retrieves the WAVEFORMATEX structure managed by this object.
 *
 * @return	structure.
 */
WAVEFORMATEX * SoundFormat::GetFormat()
{
	return &format;
}
