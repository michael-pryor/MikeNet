#include "FullInclude.h"
#include <sstream>

/**
 * @brief	Constructor for 256 bit strength encryption.
 *
 * @param key1	Key required upon decryption.
 * @param key2	Key required upon decryption.
 * @param key3	Key required upon decryption.
 * @param key4	Key required upon decryption.
 */
EncryptKey::EncryptKey(__int64 key1, __int64 key2, __int64 key3, __int64 key4)
{
	Packet keyStore;
	SetBitStrength(256,keyStore);

	keyStore.Add(key1);
	keyStore.Add(key2);
	keyStore.Add(key3);
	keyStore.Add(key4);

	KeyExpansion(keyStore);
}

/**
 * @brief				Constructor for 256 bit strength encryption.
 *
 * @param key1			Key required upon decryption.
 * @param key2			Key required upon decryption.
 * @param key3			Key required upon decryption.
 * @param key4			Key required upon decryption.
 * @param key5			Key required upon decryption.
 * @param key6			Key required upon decryption.
 * @param key7			Key required upon decryption.
 * @param key8			Key required upon decryption.
 * @param differentType Ignored, needed to differentiate from __int64 versions.
 */
EncryptKey::EncryptKey(int key1, int key2, int key3, int key4, int key5, int key6, int key7, int key8, bool differentType)
{
	Packet keyStore;
	SetBitStrength(256,keyStore);

	keyStore.Add(key1);
	keyStore.Add(key2);
	keyStore.Add(key3);
	keyStore.Add(key4);
	keyStore.Add(key5);
	keyStore.Add(key6);
	keyStore.Add(key7);
	keyStore.Add(key8);

	KeyExpansion(keyStore);
}

/**
 * @brief	Constructor for 192 bit strength encryption.
 *
 * @param key1	Key required upon decryption.
 * @param key2	Key required upon decryption.
 * @param key3	Key required upon decryption.
 */
EncryptKey::EncryptKey(__int64 key1, __int64 key2, __int64 key3)
{
	Packet keyStore;
	SetBitStrength(192,keyStore);

	keyStore.Add(key1);
	keyStore.Add(key2);
	keyStore.Add(key3);

	KeyExpansion(keyStore);
}

/**
 * @brief				Constructor for 192 bit strength encryption.
 *
 * @param key1			Key required upon decryption.
 * @param key2			Key required upon decryption.
 * @param key3			Key required upon decryption.
 * @param key4			Key required upon decryption.
 * @param key5			Key required upon decryption.
 * @param key6			Key required upon decryption.
 * @param differentType Ignored, needed to differentiate from __int64 versions.
 */
EncryptKey::EncryptKey(int key1, int key2, int key3, int key4, int key5, int key6, bool differentType)
{
	Packet keyStore;
	SetBitStrength(192,keyStore);

	keyStore.Add(key1);
	keyStore.Add(key2);
	keyStore.Add(key3);
	keyStore.Add(key4);
	keyStore.Add(key5);
	keyStore.Add(key6);

	KeyExpansion(keyStore);
}

/**
 * @brief	Constructor for 128 bit strength encryption.
 *
 * @param key1	Key required upon decryption.
 * @param key2	Key required upon decryption.
 */
EncryptKey::EncryptKey(__int64 key1, __int64 key2)
{
	Packet keyStore;
	SetBitStrength(128,keyStore);

	keyStore.Add(key1);
	keyStore.Add(key2);

	KeyExpansion(keyStore);
}

/**
 * @brief				Constructor for 128 bit strength encryption.
 *
 * @param key1			Key required upon decryption.
 * @param key2			Key required upon decryption.
 * @param key3			Key required upon decryption.
 * @param key4			Key required upon decryption.
 * @param differentType Ignored, needed to differentiate from __int64 versions.
 */
EncryptKey::EncryptKey(int key1, int key2, int key3, int key4, bool differentType)
{
	Packet keyStore;
	SetBitStrength(128,keyStore);

	keyStore.Add(key1);
	keyStore.Add(key2);
	keyStore.Add(key3);
	keyStore.Add(key4);

	KeyExpansion(keyStore);
}
/**
 * @brief	Deep copy constructor / assignment operator helper method.
 *
 * @param	copyMe	Object to copy.
 */
void EncryptKey::Copy( const EncryptKey & copyMe )
{
	this->numIntegers = copyMe.numIntegers;
	this->numRounds = copyMe.numRounds;
	this->roundKey = copyMe.roundKey;
	this->ucharRoundKey = reinterpret_cast<unsigned char*>(this->roundKey.GetDataPtr());
	this->bitStrength = copyMe.bitStrength;
}


/**
 * @brief	Deep copy constructor.
 *
 * @param	copyMe	Object to copy.
 */
EncryptKey::EncryptKey( const EncryptKey & copyMe )
{
	Copy(copyMe);
}

/**
 * @brief	Deep assignment operator.
 *
 * @param	copyMe	Object to copy.
 *
 * @return	a reference to this object. 
 */
EncryptKey & EncryptKey::operator=( const EncryptKey & copyMe )
{
	Copy(copyMe);
	return *this;
}


/**
 * @brief	Substitutes the bytes of a word with their equivalent in NetUtility::EncryptionBox.
 *
 * @param [in,out]	word	Word to substitute, must point to EncryptKey::WORD_SIZE bytes of data.
 *
 * @return	@a word.
 */
unsigned char * EncryptKey::SubWord(unsigned char * word)
{
	for(size_t n = 0;n<WORD_SIZE;n++)
	{
		word[n] = NetUtility::EncryptionBox[word[n]];
	}

	return word;
}

/**
 * @brief	Rotates a word one byte to the left.
 *
 * @param [in,out]	word	Word to substitute, must point to EncryptKey::WORD_SIZE bytes of data.
 *
 * @return	@a word.
 */
unsigned char * EncryptKey::RotateWordLeft(unsigned char * word)
{
	unsigned char b0 = word[0];

	for(size_t n = 0;n<WORD_SIZE;n++)
	{
		word[n] = word[n+1];
	}

	word[WORD_SIZE-1] = b0;

	return(word);
}

/**
 * @brief	Expand key, creating 'round keys' which are later combined with
 * the input based on the Advanced Encryption Standard specification.
 *
 * @param key Key to generate round keys from.
 */
void EncryptKey::KeyExpansion(const Packet & key)
{
	unsigned char temp[4];
	
	// The first round key is the key itself.
	for(size_t i=0;i<numIntegers;i++)
	{
		for(size_t n = 0;n<WORD_SIZE;n++)
		{
			roundKey.GetDataPtr()[i*4+n]=key.GetDataPtr()[i*4+n];
		}
	}

	// All other round keys are found from the previous round keys.
	for(size_t i = numIntegers;i < WORD_SIZE * (numRounds+1);i++)
	{
		for(size_t j=0;j<WORD_SIZE;j++)
		{
			temp[j]=roundKey.GetDataPtr()[(i-1) * WORD_SIZE + j];
		}
		if (i % numIntegers == 0)
		{
			// Rotate char array to the right one byte.
			RotateWordLeft(temp);

			// Apply EncryptionBox to each byte of char array.
			SubWord(temp);

			// EncryptionRoundConstant
			temp[0] =  temp[0] ^ NetUtility::EncryptionRoundConstant[i/numIntegers];
		}
		else
		{
			if (numIntegers > 6 && i % numIntegers == WORD_SIZE)
			{
				SubWord(temp);
			}
		}

		// Combine temp with roundKey.
		for(size_t n = 0;n<WORD_SIZE;n++)
		{
			roundKey.GetDataPtr()[i*WORD_SIZE+n] = roundKey.GetDataPtr()[(i-numIntegers)*WORD_SIZE+n] ^ temp[n];
		}
	}
}



/**
 * @brief	Destructor.
 */
EncryptKey::~EncryptKey()
{
	const char * cCommand = "an internal function (~EncryptKey)";
	try
	{

	}
	MSG_CATCH
}

/**
 * @brief	Allocates memory based on bit strength of encryption (128,192 or 256 bit). 
 *
 * @param	type	Bit strength.
 * @param [out] key Packets memory will be adjusted so that key data can be added without
 * memory reallocation.
 */
// 
void EncryptKey::SetBitStrength( size_t type, Packet & key )
{
	// Allocate new memory
	switch(type)
	{
		case 256:
			roundKey.SetMemorySize(240);
		break;

		case 192:
			roundKey.SetMemorySize(208);
		break;

		case 128:
			roundKey.SetMemorySize(176);
		break;

		default:
			roundKey.SetMemorySize(0);
			_ErrorException(true,"setting the encrypt key type. Must be 256, 192 or 128 bit",1,__LINE__,__FILE__);
		break;
	}

	// 8 bits in byte
	key.SetMemorySize(type / 8);
	numIntegers = static_cast<unsigned char>((type / (8*sizeof(int))));

	numRounds = numIntegers+6;

	this->bitStrength = type;

	// Must be done after memory allocation takes place.
	this->ucharRoundKey = reinterpret_cast<unsigned char*>(roundKey.GetDataPtr());
}

/**
 * @brief	Gets the number of rounds of encryption or decryption that should take place.
 *
 * @return	the number rounds. 
 */
unsigned char EncryptKey::GetNumRounds() const
{
	return numRounds;
}

/**
 * @brief	Returns pointer to round key.
 *
 * @return	pointer to round key.
 */
unsigned char * EncryptKey::GetRoundKeys() const
{
	return ucharRoundKey;
}

/**
 * @brief	Retrieves a __int64 from a hex string.
 *
 * @param	hex	The hexadecimal to convert, must store 8 hexadecimal.
 *
 * @return	__int64 represenation of hex string.
 */
__int64 RetrieveIntFromHex(const char * hex)
{
	Packet converter;
	converter.AddHex(hex);
	converter.SetCursor(0);
	return converter.Get<__int64>();
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool EncryptKey::TestClass()
{
	cout << "Testing EncryptKey class...\n";
	bool problem = false;
	
	__int64 key1 = 5000;
	__int64 key2 = 51255125;
	__int64 key3 = 123213123;
	__int64 key4 = 909058353388;

	{
		// Testing encryption of a large packet, to check that all parts are decrypted.
		Packet encryptMe("Wikipedia's goal is to be a free content encyclopedia, with free content defined as content that does not bear copyright restrictions on the right to redistribute, study, modify and improve, or otherwise use works for any purpose in any medium, even commercially. But because free as in cost and free as in freedom are two entirely different concepts, images freely available on the Internet may still be inappropriate for Wikipedia. Any content not satisfying criteria, such as \"non-commercial use only\" images, images with permission for use on Wikipedia only, or images fully copyrighted are therefore classified as non-free. \
						  The licensing policy of the Wikimedia Foundation requires all content hosted on Wikipedia to be free content. However, there are exceptions. The policy allows projects to adopt an exemption doctrine policy allowing the use of non-free content within strictly defined limitations. There are situations where acquiring a freely licensed image for a particular subject may not be possible; non-free content can be used on Wikipedia in these cases, but only within the doctrine of fair use. The use of non-free images on Wikipedia must fall within purposely stricter standards than defined by copyright law as defined by our non-free content criteria as described below.");
		encryptMe.SetUsedSize(1024);
		
		EncryptKey key128(key1,key2);
		cout << "Original contents: ";
		encryptMe.OutputContents();
		cout << '\n';

		size_t originalClock = clock();
		encryptMe.Encrypt(key128);
		cout << "Time taken to encrypt: " << clock() - originalClock << '\n';

		originalClock = clock();
		encryptMe.Decrypt(key128);
		cout << "Time taken to decrypt: " << clock() - originalClock << '\n';

		cout << "Decrypted contents: ";
		encryptMe.OutputContents();

		// Test encryption generates correct data
		cout << "\n\n";
		encryptMe.Clear();
		Packet original;
		Packet desiredCipher;

		// 128 bit
		key1 = RetrieveIntFromHex("2b7e151628aed2a6");
		key2 = RetrieveIntFromHex("abf7158809cf4f3c");
		EncryptKey encryptKey128(key1,key2);

		encryptMe.AddHex("3243f6a8885a308d313198a2e0370734");
		desiredCipher.AddHex("3925841d02dc09fbdc118597196a0b32");

		original = encryptMe;

		encryptMe.Encrypt(encryptKey128);

		if(encryptMe == desiredCipher)
		{
			cout << "Encryption 128 is good\n";
		}
		else
		{
			problem = true;
			cout << "Encryption 128 is bad\n";
		}

		encryptMe.Decrypt(encryptKey128);

		if(encryptMe == original)
		{
			cout << "Decryption 128 is good\n";
		}
		else
		{
			problem = true;
			cout << "Decryption 128 is bad\n";
		}

		// 192 bit
		encryptMe = original;
		encryptMe.SetCursor(0);
		desiredCipher.Clear();
		key1 = RetrieveIntFromHex("2b7e151628aed2a6");
		key2 = RetrieveIntFromHex("abf7158809cf4f3c");
		key3 = RetrieveIntFromHex("762e7160f38b4da5");
		EncryptKey encryptKey192(key1,key2,key3);

		encryptMe.AddHex("3243f6a8885a308d313198a2e0370734");
		desiredCipher.AddHex("f9fb29aefc384a250340d833b87ebc00"); 

		original = encryptMe;

		encryptMe.Encrypt(encryptKey192);
		if(encryptMe == desiredCipher)
		{
			cout << "Encryption 192 is good\n";
		}
		else
		{
			problem = true;
			cout << "Encryption 192 is bad\n";
		}

		encryptMe.Decrypt(encryptKey192);

		if(encryptMe == original)
		{
			cout << "Decryption 192 is good\n";
		}
		else
		{
			problem = true;
			cout << "Decryption 192 is bad\n";
		}

		// 256 bit
		encryptMe = original;
		encryptMe.SetCursor(0);
		desiredCipher.Clear();
		key1 = RetrieveIntFromHex("2b7e151628aed2a6");
		key2 = RetrieveIntFromHex("abf7158809cf4f3c");
		key3 = RetrieveIntFromHex("762e7160f38b4da5");
		key4 = RetrieveIntFromHex("6a784d9045190cfe");
		EncryptKey encryptKey256(key1,key2,key3,key4);

		encryptMe.AddHex("3243f6a8885a308d313198a2e0370734");
		desiredCipher.AddHex("1a6e6c2c662e7da6501ffb62bc9e93f3");

		original = encryptMe;

		encryptMe.Encrypt(encryptKey256);
		if(encryptMe == desiredCipher)
		{
			cout << "Encryption 256 is good\n";
		}
		else
		{
			problem = true;
			cout << "Encryption 256 is bad\n";
		}

		encryptMe.Decrypt(encryptKey256);

		if(encryptMe == original)
		{
			cout << "Decryption 256 is good\n";
		}
		else
		{
			problem = true;
			cout << "Decryption 256 is bad\n";
		}
	}
	cout << "\n\n";
	return !problem;
}