#include "FullInclude.h"
/**
 * @brief	Constructor. 
 *
 * @param	encrypt			If true ThreadMessageItemEncrypt::TakeAction() will perform encryption, if false if will perform decryption.
 * @param [in,out]	packet	Packet to encrypt.
 * @param	p_key				Key used to encrypt or decrypt data. Must have its round keys already generated.
 * @param	threadID		Identifier for the thread that this message will be sent to. Thread IDs must start at 0.
 * @param	numThreads		Number of threads participating in the encryption of this packet. 
 */
ThreadMessageItemEncrypt::ThreadMessageItemEncrypt(bool encrypt, Packet * packet, const EncryptKey & p_key, size_t threadID, size_t numThreads) :
	key(p_key)
{
	this->encrypt = encrypt;
	this->packet = reinterpret_cast<unsigned char*>(packet->GetDataPtr());
	this->packetSize = packet->GetUsedSize();
	this->threadID = threadID;
	this->numThreads = numThreads;
}

/**
 * @brief	Destructor. 
 */
ThreadMessageItemEncrypt::~ThreadMessageItemEncrypt()
{
}

/**
 * @brief	Encrypts or decrypts parts of a packet.
 *
 * This method is designed for parallel encryption and decryption of packets. Where
 * a packet is split up into chunks and the thread acts upon.
 *
 * @return some actions may require further activity by the thread which
 * cannot take place within the method call. This return value can be used
 * by the thread to determine what to do.
 */
void * ThreadMessageItemEncrypt::TakeAction()
{
	for(size_t n = (ENCRYPTION_CHUNK_SIZE*threadID); n<packetSize; n+=(ENCRYPTION_CHUNK_SIZE*numThreads))
	{
		if(encrypt == true)
		{
			Cipher(packet+n,packet+n,key.GetNumRounds(),key.GetRoundKeys());
		}
		else
		{
			InverseCipher(packet+n,packet+n,key.GetNumRounds(),key.GetRoundKeys());
		}
	}
	return NULL;
}


/**
 * @brief Decrypts 16 bytes of data.
 *
 * Based on Advanced Encryption Standard specification.
 * 
 * @param dataToEncrypt Data to encrypt, must be at least 16 bytes in size.
 * @param result Destination to store encrypted data, must be at least 16 bytes in size.
 * @param numRounds Number of rounds.
 * @param roundKey Round key.
 */
void ThreadMessageItemEncrypt::InverseCipher(unsigned char * dataToEncrypt,unsigned char * result, unsigned char numRounds, unsigned char * roundKey)
{
	// Copy data into state for manipulation
	for(size_t r = 0;r<EncryptKey::WORD_SIZE;r++)
	{
		for(size_t c = 0;c<EncryptKey::WORD_SIZE;c++)
		{
			state[r][c] = result[r+(c*EncryptKey::WORD_SIZE)];
		}
	}

	// Perform decryption operations
	XorRoundKey(numRounds,roundKey);

	for(size_t r = numRounds-1; r>=1; r--)
	{
		ShiftRowsRight();
		InverseSubBytes();
		XorRoundKey(r,roundKey);
		InverseMixColumns();
	}

	ShiftRowsRight();
	InverseSubBytes();
	XorRoundKey(0,roundKey);
	
	// Copy state into data now that we have finished
	for(int r = 0;r<EncryptKey::WORD_SIZE;r++)
	{
		for(int c = 0;c<EncryptKey::WORD_SIZE;c++)
		{
			result[r+(c*EncryptKey::WORD_SIZE)] = state[r][c];
		}
	}
}

/**
 * @brief Encrypts 16 bytes of data.
 *
 * Based on Advanced Encryption Standard specification.
 * 
 * @param dataToEncrypt Data to encrypt, must be at least 16 bytes in size.
 * @param result Destination to store encrypted data, must be at least 16 bytes in size.
 * @param numRounds Number of rounds.
 * @param roundKey Round key.
 */
void ThreadMessageItemEncrypt::Cipher(unsigned char * dataToEncrypt,unsigned char * result, unsigned char numRounds, unsigned char * roundKey)
{
	// Copy data into state for manipulation
	for(size_t r = 0;r<EncryptKey::WORD_SIZE;r++)
	{
		for(size_t c = 0;c<EncryptKey::WORD_SIZE;c++)
		{
			state[r][c] = dataToEncrypt[r+(c*EncryptKey::WORD_SIZE)];
		}
	}

	// Perform encryption operations
	XorRoundKey(0,roundKey);

	for(size_t r = 1; r<numRounds; r++)
	{
		SubBytes();
		ShiftRowsLeft();
		MixColumns();
		XorRoundKey(r,roundKey);
	}

	SubBytes();
	ShiftRowsLeft();
	XorRoundKey(numRounds,roundKey);
	
	// Copy state into data now that we have finished
	for(int r = 0;r<EncryptKey::WORD_SIZE;r++)
	{
		for(int c = 0;c<EncryptKey::WORD_SIZE;c++)
		{
			result[r+(c*EncryptKey::WORD_SIZE)] = state[r][c];
		}
	}
}

/**
 * @brief Applies XOR.
 *
 * Based on Advanced Encryption Standard specification.
 * 
 * @param round Current round.
 * @param roundKey Round key.
 */
void ThreadMessageItemEncrypt::XorRoundKey(size_t round, unsigned char * roundKey)   
 { 
     for(size_t i=0;i<EncryptKey::WORD_SIZE;i++)  
	 {
         for(size_t j=0;j<EncryptKey::WORD_SIZE;j++) 
		 {
             state[j][i] ^= roundKey[round * EncryptKey::WORD_SIZE * EncryptKey::WORD_SIZE + i * EncryptKey::WORD_SIZE + j];
		 }
	 }
}

/**
 * @brief Substitutes each byte of state with one from the EncryptionBox table.
 *
 * Based on Advanced Encryption Standard specification.
 */
void ThreadMessageItemEncrypt::SubBytes()
{
	for(size_t r = 0;r<EncryptKey::WORD_SIZE;r++)
	{
		for(size_t c = 0;c<EncryptKey::WORD_SIZE;c++)
		{
			state[r][c] = NetUtility::EncryptionBox[state[r][c]];
		}
	}
}
/**
 * @brief Substitutes each byte of state with one from the InverseEncryptionBox table.
 *
 * Based on Advanced Encryption Standard specification.
 */
void ThreadMessageItemEncrypt::InverseSubBytes()
{
	for(size_t r = 0;r<EncryptKey::WORD_SIZE;r++)
	{
		for(size_t c = 0;c<EncryptKey::WORD_SIZE;c++)
		{
			state[r][c] = NetUtility::InverseEncryptionBox[state[r][c]];
		}
	}
}

/**
 * @brief Shifts each row columns left.
 *
 * The number of columns = row number. Row 0 is not shifted because it has 0 offset.\n\n
 *
 * Based on Advanced Encryption Standard specification.	
 */
void ThreadMessageItemEncrypt::ShiftRowsLeft()
{
	unsigned char aux[EncryptKey::WORD_SIZE];

	for(size_t r = 1;r<EncryptKey::WORD_SIZE;r++)
	{
		for(size_t c = 0;c<EncryptKey::WORD_SIZE;c++)
		{
			aux[c] = state[r][(c+r) % EncryptKey::WORD_SIZE];
		}

		for(size_t c = 0;c<EncryptKey::WORD_SIZE;c++)
		{
			state[r][c] = aux[c];
		}
	}
}
/**
 * @brief Shifts each row columns right.
 *
 * The number of columns = row number. Row 0 is not shifted because it has 0 offset.\n\n
 *
 * Based on Advanced Encryption Standard specification.	
 */
void ThreadMessageItemEncrypt::ShiftRowsRight()
{
	unsigned char aux[4];

	for(size_t r = 1;r<EncryptKey::WORD_SIZE;r++)
	{
		for(size_t c = 0;c<EncryptKey::WORD_SIZE;c++)
		{
			aux[(c+r) % EncryptKey::WORD_SIZE] = state[r][c];
		}

		for(size_t c = 0;c<EncryptKey::WORD_SIZE;c++)
		{
			state[r][c] = aux[c];
		}
	}
}

/**
 * @brief Mixes columns up.
 *
 * Based on Advanced Encryption Standard specification.
 */
void ThreadMessageItemEncrypt::MixColumns()
{
	unsigned char aux[EncryptKey::WORD_SIZE]; // Temporary store of column's elements

	for(size_t c = 0;c<EncryptKey::WORD_SIZE;c++)
	{
		for(size_t r = 0;r<EncryptKey::WORD_SIZE;r++)
		{
			aux[r] = state[r][c];
		}

		for(size_t r = 0;r<EncryptKey::WORD_SIZE;r++)
		{
			state[r][c] = _gMul(0x02,aux[r]) ^ _gMul(0x03,aux[(r+1) % EncryptKey::WORD_SIZE]) ^ aux[(r+2) % EncryptKey::WORD_SIZE] ^ aux[(r+3) % EncryptKey::WORD_SIZE];
		}
	}
}

/**
 * @brief Unmixes columns.
 *
 * Based on Advanced Encryption Standard specification.
 */
void ThreadMessageItemEncrypt::InverseMixColumns()
{
	unsigned char aux[4];

	for(size_t c = 0;c<4;c++)
	{
		for(size_t r = 0;r<4;r++)
		{
			aux[r] = state[r][c];
		}

		for(size_t r = 0;r<4;r++)
		{
			state[r][c] = _gMul(0x0e,aux[r]) ^ _gMul(0x0b,aux[(r+1) % EncryptKey::WORD_SIZE]) ^ _gMul(0x0d,aux[(r+2) % EncryptKey::WORD_SIZE]) ^ _gMul(0x09,aux[(r+3) % EncryptKey::WORD_SIZE]);
		}	
	}
}



/**
 * @brief Performs galois field multiplication (@a a * @a b).
 *
 * Rijndael (what AES based on) uses what is known as a galois field which requires
 * a special set of non standard operators to perform its maths.\n\n
 *
 * Based on Advanced Encryption Standard specification.
 * 
 * @param a A number to be multiplied with @a b.
 * @param b A number to be multiplied with @a a.
 * @return @a a multiplied with @a b (galois field).
 */
unsigned char ThreadMessageItemEncrypt::_gMul(unsigned char a, unsigned char b)
{
	unsigned char p = 0;
	unsigned char counter;
	unsigned char hi_bit_set;

	for(counter = 0; counter < 8; counter++)
	{
		if((b & 1) == 1)
		{
			p ^= a;
		}

		hi_bit_set = (a & 0x80);
		a <<= 1;

		if(hi_bit_set == 0x80)
		{
			a ^= 0x1b;
		}

		b >>= 1;
	}
	return p;
}
