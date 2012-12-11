#pragma once
#include "Packet.h"

/**
 * @brief	Encryption key which is used in conjunction with the Packet class to encrypt and decrypt packets.
 * @remarks	Michael Pryor, 9/20/2010. 
 */
class EncryptKey
{
public:	
	/**
	 * @brief Size of a word in bytes.
	 */
	const static size_t WORD_SIZE = 4;
private:
	/**
	 * @brief Number of integers making up EncryptKey::key.
	 */
	unsigned char numIntegers;

	/**
	 * @brief Numbers of rounds of encryption.
	 */
	unsigned char numRounds;

	/**
	 * @brief Round key, generated from EncryptKey::key which is used
	 * when encrypting and decrypting.
	 */
	Packet roundKey;

	/**
	 * @brief Pointer to data of EncryptKey::roundKey.
	 */
	unsigned char * ucharRoundKey;

	/**
	 * @brief Strength of encryption, either 256, 192 or 128.
	 */
	size_t bitStrength;

	unsigned char * RotateWordLeft(unsigned char * RotateMe);
	unsigned char * SubWord(unsigned char * SubMe);

	void SetBitStrength(size_t Type, Packet & key);
	void KeyExpansion(const Packet & key);

	void Copy(const EncryptKey & copyMe);
public:
	EncryptKey(const EncryptKey & copyMe);
	EncryptKey & operator=(const EncryptKey & copyMe);
	EncryptKey(__int64 key1, __int64 key2, __int64 key3, __int64 key4);	
	EncryptKey(int key1, int key2, int key3, int key4, int key5, int key6, int key7, int key8, bool differentType);
	EncryptKey(__int64 key1, __int64 key2, __int64 key3);
	EncryptKey(int key1, int key2, int key3, int key4, int key5, int key6, bool differentType);
	EncryptKey(__int64 key1, __int64 key2);
	EncryptKey(int key1, int key2, int key3, int key4, bool differentType);
	~EncryptKey();

	unsigned char GetNumRounds() const;
	unsigned char * GetRoundKeys() const;

	static bool TestClass();
};