#include "FullInclude.h"

/**
 * @brief	Default constructor. 
 */
SoundDevice::SoundDevice()
{
}

/**
 * @brief	Destructor. 
 */
SoundDevice::~SoundDevice()
{
}

/**
 * @brief	Determine if a format with the specified settings is supported by the device.
 *
 * @param	samplesPerSecond	Samples per second: 11025 (11.025 kHz), 22050 (22.05 kHz), 44100 (44.1 kHz), 48000 (48 kHz) or 96000 (96kHz).
 * @param	bits				Bits (8 or 16).
 * @param	channels			Number of channels channels (1 or 2). 
 *
 * @return	true if format supported, false if not. 
 */
bool SoundDevice::IsFormatSupported( DWORD samplesPerSecond, WORD bits, WORD channels ) const
{
	// Select flag
	size_t flag = NULL;

	switch(samplesPerSecond)
	{
		// 11.025 KHZ
		case(11025):
			switch(channels)
			{
				// 1 CHANNEL
				case(1):
					switch(bits)
					{
						// 8 bits
						case(8):
							flag = WAVE_FORMAT_1M08;
						break;

						// 16 bits
						case(16):
							flag = WAVE_FORMAT_1M16;
						break;

						// ERROR
						default:
							_ErrorException(true,"selecting the appropriate flag, invalid bits parameter; must be 8 or 16",0,__LINE__,__FILE__);
						break;
					}
				break;

				// 2 CHANNELS
				case(2):
					switch(bits)
					{
						// 8 bits
						case(8):
							flag = WAVE_FORMAT_1S08;
						break;

						// 16 bits
						case(16):
							flag = WAVE_FORMAT_1S16;
						break;

						// ERROR
						default:
							_ErrorException(true,"selecting the appropriate flag, invalid bits parameter; must be 8 or 16",0,__LINE__,__FILE__);
						break;
					}
				break;

				// ERROR
				default:
					_ErrorException(true,"selecting the appropriate flag, invalid channels parameter; must be 1 or 2",0,__LINE__,__FILE__);
				break;
			}			
		break;

		// 22.05 KHZ
		case(22050):
			switch(channels)
			{
				// 1 CHANNEL
				case(1):
					switch(bits)
					{
						// 8 bits
						case(8):
							flag = WAVE_FORMAT_2M08;
						break;

						// 16 bits
						case(16):
							flag = WAVE_FORMAT_2M16;
						break;

						// ERROR
						default:
							_ErrorException(true,"selecting the appropriate flag, invalid bits parameter; must be 8 or 16",0,__LINE__,__FILE__);
						break;
					}
				break;

				// 2 CHANNELS
				case(2):
					switch(bits)
					{
						// 8 bits
						case(8):
							flag = WAVE_FORMAT_2S08;
						break;

						// 16 bits
						case(16):
							flag = WAVE_FORMAT_2S16;
						break;

						// ERROR
						default:
							_ErrorException(true,"selecting the appropriate flag, invalid bits parameter; must be 8 or 16",0,__LINE__,__FILE__);
						break;
					}
				break;

				// ERROR
				default:
					_ErrorException(true,"selecting the appropriate flag, invalid channels parameter; must be 1 or 2",0,__LINE__,__FILE__);
				break;
			}		
		break;

		// 44.1 KHZ
		case(44100):
			switch(channels)
			{
				// 1 CHANNEL
				case(1):
					switch(bits)
					{
						// 8 bits
						case(8):
							flag = WAVE_FORMAT_4M08;
						break;
							
						// 16 bits
						case(16):
							flag = WAVE_FORMAT_4M16;
						break;
							
						// ERROR
						default:
							_ErrorException(true,"selecting the appropriate flag, invalid bits parameter; must be 8 or 16",0,__LINE__,__FILE__);
						break;
					}
				break;

				// 2 CHANNELS
				case(2):
					switch(bits)
					{
						// 8 bits
						case(8):
							flag = WAVE_FORMAT_4S08;
						break;
							
						// 16 bits
						case(16):
							flag = WAVE_FORMAT_4S16;
						break;
							
						// ERROR
						default:
							_ErrorException(true,"selecting the appropriate flag, invalid bits parameter; must be 8 or 16",0,__LINE__,__FILE__);
						break;
					}
				break;

				// ERROR
				default:
					_ErrorException(true,"selecting the appropriate flag, invalid channels parameter; must be 1 or 2",0,__LINE__,__FILE__);
				break;
			}	
		break;

		// 48 KHZ
		case(48000):
			switch(channels)
			{
				// 1 CHANNEL
				case(1):
					switch(bits)
					{
						// 8 bits
						case(8):
							flag = WAVE_FORMAT_48M08;
						break;

						// 16 bits
						case(16):
							flag = WAVE_FORMAT_48M16;
						break;

						// ERROR
						default:
							_ErrorException(true,"selecting the appropriate flag, invalid bits parameter; must be 8 or 16",0,__LINE__,__FILE__);
						break;
					}
				break;

				// 2 CHANNELS
				case(2):
					switch(bits)
					{
						// 8 bits
						case(8):
							flag = WAVE_FORMAT_48S08;
						break;

						// 16 bits
						case(16):
							flag = WAVE_FORMAT_48S16;
						break;

						default:
							_ErrorException(true,"selecting the appropriate flag, invalid bits parameter; must be 8 or 16",0,__LINE__,__FILE__);
						break;
					}
				break;

				// ERROR
				default:
					_ErrorException(true,"selecting the appropriate flag, invalid channels parameter; must be 1 or 2",0,__LINE__,__FILE__);
				break;
			}	
		break;

		// 96 KHZ
		case(96000):
			switch(channels)
			{
				// 1 CHANNEL
				case(1):
					switch(bits)
					{
						// 8 bits
						case(8):
							flag = WAVE_FORMAT_96M08;
						break;

						// 16 bits
						case(16):
							flag = WAVE_FORMAT_96M16;
						break;

						// ERROR
						default:
							_ErrorException(true,"selecting the appropriate flag, invalid bits parameter; must be 8 or 16",0,__LINE__,__FILE__);
						break;
					}
				break;

				// 2 CHANNELS
				case(2):
					switch(bits)
					{
						// 8 bits
						case(8):
							flag = WAVE_FORMAT_96S08;
						break;

						// 16 bits
						case(16):
							flag = WAVE_FORMAT_96S16;
						break;

						// ERROR
						default:
							_ErrorException(true,"selecting the appropriate flag, invalid bits parameter; must be 8 or 16",0,__LINE__,__FILE__);
						break;
					}
				break;

				// ERROR
				default:
					_ErrorException(true,"selecting the appropriate flag, invalid channels parameter; must be 1 or 2",0,__LINE__,__FILE__);
				break;
			}	
		break;

		// ERROR
		default:
			_ErrorException(true,"selecting the appropriate flag, invalid KHZ parameter; must be 11, 22, 44, 48 or 96",0,__LINE__,__FILE__);
		break;
	}


	return BITMASK_GET(GetFormatSupport(),flag);
}
