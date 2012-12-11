#pragma once
#include "threadmessageitem.h"
#include "EncryptKey.h"
class Packet;

/**
 * @brief	Message which encrypts or decrypts part of a packet, sent to a ThreadSingleMessage thread.
 * @remarks	Michael Pryor, 9/20/2010. 
 */
class ThreadMessageItemEncrypt :
	public ThreadMessageItem
{
    /**
     * @brief If true ThreadMessageItemEncrypt::TakeAction() will perform encryption, if false if will perform decryption.
     */
	bool encrypt;

	/**
	 * @brief Packet data to encrypt.
	 */
	unsigned char * packet;

	/**
	 * @brief Number of bytes of data in ThreadMessageItemEncrypt::packet.
	 */
	size_t packetSize;

	/**
	 * @brief Key used to encrypt or decrypt data.
	 */
	const EncryptKey & key;

	/**
	 * @brief ID of thread that this message will be sent to.
	 */
	size_t threadID;

	/**
	 * @brief Number of threads participating in the encryption of this packet.
	 */
	size_t numThreads;

	/** @brief Stores intermediate encryption result. */
	unsigned char state[4][4];

	void InverseCipher(unsigned char * dataToEncrypt,unsigned char * result, unsigned char numRounds, unsigned char * roundKey);
	void Cipher(unsigned char * dataToEncrypt, unsigned char * result, unsigned char numRounds, unsigned char * roundKey);

	void XorRoundKey(size_t round, unsigned char * roundKey);
	void InverseMixColumns();
	void MixColumns();
	void ShiftRowsRight();
	void ShiftRowsLeft();
	void InverseSubBytes();
	void SubBytes();
	unsigned char _gMul(unsigned char a, unsigned char b);

public:
	/**
	 * @brief Amount of data that is encrypted in one ThreadMessageItemEncrypt::Cipher or decrypted in one ThreadMessageItemEncrypt::InverseCipher operation.
	 */
	static const size_t ENCRYPTION_CHUNK_SIZE = 16;
	
	ThreadMessageItemEncrypt(bool encrypt, Packet * packet, const EncryptKey & encryptKey, size_t threadID, size_t numThreads);
	virtual ~ThreadMessageItemEncrypt();

	void * TakeAction();
};

