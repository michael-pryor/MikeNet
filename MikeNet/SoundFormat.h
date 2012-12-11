#pragma once

/**
 * @brief	Format of sound, describing samples per second, bits per sample and number of channels.
 * @remarks	Michael Pryor, 9/20/2010. 
 */
class SoundFormat
{
	/**
	 * @brief Structure that stores information about sound format.
	 */
	WAVEFORMATEX format;
public:
	/**
	 * @brief Samples per second for low quality.
	 */
	static const DWORD LOW_SAMPLES_PER_SEC = 8000;

	/**
	 * @brief Samples per second for medium quality.
	 */
	static const DWORD MED_SAMPLES_PER_SEC = 44100;

	/**
	 * @brief Samples per second for high quality.
	 */
	static const DWORD HIGH_SAMPLES_PER_SEC = 96000;

	/**
	 * @brief Bits per sample for low quality.
	 */
	static const WORD LOW_BITS_PER_SAMPLE = 8;

	/**
	 * @brief Bits per sample for medium quality.
	 */
	static const WORD MED_BITS_PER_SAMPLE = 8;

	/**
	 * @brief Bits per sample for high quality.
	 */
	static const WORD HIGH_BITS_PER_SAMPLE = 16;

	/**
	 * @brief Number of channels for low quality.
	 */
	static const WORD LOW_CHANNELS = 1;

	/**
	 * @brief Number of channels for medium quality.
	 */
	static const WORD MED_CHANNELS = 1;

	/**
	 * @brief Number of channels for high quality.
	 */
	static const WORD HIGH_CHANNELS = 2;

private:
	void DefaultVariables();
public:
	SoundFormat();
	SoundFormat(const SoundFormat & copyMe);
	SoundFormat & operator=(const SoundFormat & copyMe);

	DWORD GetSamplesPerSecond() const;
	WORD GetBitsPerSample() const;
	WORD GetNumChannels() const;
	WORD GetBytesPerSample() const;

	WAVEFORMATEX * GetFormat();

	void Set(DWORD samplesPerSecond, WORD bits, WORD channels);
	void SetLowQuality();
	void SetMediumQuality();
	void SetHighQuality();

};