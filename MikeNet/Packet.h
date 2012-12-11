#pragma once
class Packet;
#include "ComString.h"
class EncryptKey;
#include "CriticalSection.h"
#include "ThreadSingleMessageKeepLastUser.h"
#include "StoreQueue.h"
#include "Utility.h"

/**
 * @brief	Efficiently and safely stores and manipulates data.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * This class is at the core of the networking module and is one of
 * the main reasons for its efficiency.\n\n
 * 
 * Data added to the class moves a cursor along. This cursor acts in 
 * the same way as one used in a word processing application (e.g. Notepad).
 * If I add an integer (4 bytes in size) the cursor will move from 0 to 4,
 * and the data will be inserted in elements 0, 1, 2 and 3.
 * Then I add a string 5 bytes in size and the cursor moves from 4 to 9, and
 * the data is inserted starting at 4, 5, 6, 7, 8 and 9. The cursor is also used with
 * retrieving data and acts in the same way.\n\n
 *
 * There are two sizes associated with each packet. Firstly there is the memory size.
 * This is the amount of memory (in bytes) that is allocated to the packet. If more
 * memory is needed it is automatically allocated but for maximum efficiency you should
 * try to allocate all memory at the start using ChangeMemorySize() or SetMemorySize(). \n\n
 *
 * Secondly there is used size. This is the amount of memory allocated to the packet
 * that is actually in use. The used size can never be more than the
 * memory size.
 *
 * The way the used size and memory size interact with each other is a key contributor
 * to efficiency. e.g. You may want to reuse the same packet for multiple purposes, some that
 * require small amounts of memory and some that require large amounts of memory. Instead
 * of decreasing and increasing the memory size between usage we simply change the used size. \n\n
 *
 * How about a second example? If I want to empty the packet I only need to change the used size
 * to 0. I do not need to change the contents of the packet, or do any additional assignment!\n\n
 *
 * The last reason that this class is efficient lies in some funky (but actually fairly simple) 
 * memory manipulation that goes on, particularly with the Insert() and Erase() methods which are
 * very quick. \n\n
 *
 * This class is thread safe.
 */
class Packet: public CriticalSection, public ThreadSingleMessageKeepLastUser, public MemoryUsage
{
	/**
	 * @brief When true data will not be cleaned up on destruction. 
	 *
	 * This is only set to true by SetDataPtr() and can be reversed by UnsetDataPtr().
	 * It is useful when packet commands are required on an unassociated data block.
	 */
	bool dataPtrChanged;

	/**
	 * @brief Memory block of size which contains packet data.
	 *
	 * Packet::memSize is the amount of memory allocated.\n
	 * Packet::usedSize is the amount of memory in use.
	 */
	char * data;

	/**
	 * @brief ID of client that packet was received from.
	 *
	 * If this value is not set value will be 0.
	 */
	size_t clientFrom;

	/**
	 * @brief Operation ID associated with packet.
	 *
	 * If value is not set value will be 0.
	 */
	size_t operation;

	/**
	 * @brief ID of instance that packet was received on.
	 *
	 * If value is not set value will be 0.
	 */
	size_t instance;

	/**
	 * @brief Value indicating packet age.
	 *
	 * Usually the higher the value the younger the packet.
	 */
 	clock_t age;

	/**
	 * @brief Position within packet that Add() and Get() should begin reading or writing data.
	 *
     * Packet::cursorPos <= Packet::usedSize.\n
	 * Packet::cursorPos <= Packet::memSize.
	 */
	mutable size_t cursorPos;

	/**
	 * @brief Amount of memory allocated to Packet::data.
	 */
	size_t memSize;

	/**
	 * @brief Amount of Packet::data that is in use.
	 */
	size_t usedSize;


public:
	/**
	 * @brief Number of bytes including any padding that prefix added using AddSizeT() or AddClockT() use.
	 *
	 * This is not always the same as sizeof(size_t) or sizeof(clock_t).
	 */
	static const unsigned int prefixSizeBytes = Utility::LargestSupportedBytesInt;
private:
	void _UpdateMemoryAndCursor(size_t addSize);


	void Copy(const Packet & copyMe);
	void Copy(const char * copyMe);
	void Copy(const ComString & comString);

	static void _AddPacket(Packet & destination, const Packet & source);
	static void _AddWSABUF(Packet & destination, const WSABUF & source, size_t used);

	void DefaultVariables(bool full);

public:
	static void DecryptWSABUF(WSABUF decryptMe, size_t used, size_t offset, const EncryptKey * key);

	Packet operator+ (const Packet &);
	void operator+= (const Packet &);
	bool operator== (const Packet &) const;
	bool operator!= (const Packet &) const;

	bool operator== (const char *) const;
	bool operator!= (const char *) const;

	Packet addWSABUF (const WSABUF &, size_t used);
	void addEqualWSABUF (const WSABUF &, size_t used);
	bool compareWSABUF (const WSABUF &, size_t used);
	Packet & equalWSABUF (const WSABUF &, size_t used, size_t offset);

	void Clear();
	
	Packet();
	Packet(const WSABUF & data, size_t used, size_t offset, size_t clientFrom, size_t operation, size_t instance, clock_t clock);
	void LoadFull(const WSABUF & paraData, size_t paraUsed, size_t paraOffset, size_t paraClientFrom, size_t paraOperation, size_t paraInstance, clock_t paraClock);

	~Packet();

	Packet(const Packet &);
	Packet(const char *);
	Packet(const ComString &);

	Packet & operator= (const Packet &);
	Packet & operator= (const char *);
	Packet & operator= (const ComString &);

	void Erase(size_t startPos, size_t amount);
	void Insert(size_t amount);

	bool Find(size_t startPos, size_t endPos, const Packet & findMe, size_t * result) const;

	void AddStringC(const char*, size_t length, bool prefix);
	void AddSizeT(size_t);
	void AddClockT(clock_t);

	size_t GetPacketRemainder() const;
	char * GetStringC(size_t length, bool nullTerminated) const;
	void GetStringC(char * destination, size_t length, bool nullTerminated) const;
	size_t GetStringSize() const;
	size_t GetSizeT() const;
	clock_t GetClockT() const;

	size_t GetMemorySize() const;
	void SetMemorySize(size_t size);
	void ChangeMemorySize(size_t size);
	void SetUsedSize(size_t size);
	size_t GetUsedSize() const;
	size_t GetCursor() const;
	void SetCursor(size_t position) const;
	void IncCursor(size_t amount) const;
	clock_t GetAge() const;
	void SetAge(clock_t clock);
	size_t GetClientFrom() const;
	void SetClientFrom(size_t clientFrom);
	size_t GetOperation() const;
	void SetOperation(size_t operation);
	size_t GetInstance() const;
	void SetInstance(size_t instance);

	char * GetDataPtr();
	const char * GetDataPtr() const;
	char * GetNullTerminated();
	const char * GetNullTerminated() const;
	bool IsNullTerminated() const;
	char * GetDataPtrCopy() const;
	void PtrIntoWSABUF(WSABUF & buffer) const;
	void CopyIntoWSABUF(WSABUF & buffer) const;

	void SetDataPtr(char * newPtr, size_t paraMemSize, size_t paraUsedSize);
	void UnsetDataPtr();
private:
	void DoEncryptionOperation(bool encryption, const EncryptKey & key, bool block);
public:
	void Encrypt(const EncryptKey & key, bool block = true);
	void Decrypt(const EncryptKey & key, bool block = true);

	void WaitUntilLastEncryptionOperationFinished();
	bool IsLastEncryptionOperationFinished();

	void AddHex(const char * hex);
	char * GetHex(size_t startPos, size_t numBytes) const;

	void OutputContents();

	static bool TestClass();

	/**
	 * @brief Retrieves data of any type from the packet.
	 *
	 * Data is retrieved from the packet's data buffer starting at Packet::cursorPos.
	 * Packet::cursorPos is moved along by the size of the data retrieved.\n\n
	 *
	 * C strings should not be retrieved using this method, use GetStringC() instead.
	 *
	 * @exception ErrorReport If retrieving data would mean exceeding the used size.
	 * @return retrieved data.
	 */
	template<typename T>
	T Get() const
	{
		T returnMe = 0;
		const char * cCommand = "Packet::Get";

		Enter();
		try
		{
			// Get the new end cursor position after getting
			size_t endPos = GetCursor() + sizeof(returnMe);

			// Prevent going out of bounds
			_ErrorException((endPos > GetUsedSize()),"getting data from a packet. The end of the packet was reached before all data could be extracted from the packet",0,__LINE__,__FILE__);

			// Copy data into Return
			returnMe = *(T*)&data[GetCursor()];

			// Move cursor
			SetCursor(endPos);
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & Error){Leave();	throw(Error);}
		catch(...){Leave();	throw(-1);}

		Leave();

		return(returnMe);
	}

	/**
	 * @brief Retrieves data of any type from the specified position without moving the cursor.
	 *
	 * C strings should not be retrieved using this method, use GetStringC() instead.
	 *
	 * @exception ErrorReport If retrieving data would mean exceeding the used size.
	 *
	 * @param position Position to retrieve data from.
	 *
	 * @return retrieved data.
	 */
	template<typename T>
	T GetPrefix(size_t position) const
	{
		T returnMe = 0;
		const char * cCommand = "Packet::GetPrefix";

		Enter();
		try
		{
			size_t originalCursor = GetCursor();

			SetCursor(position);
			returnMe = Get<T>();
		
			SetCursor(originalCursor);
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & Error){Leave();	throw(Error);}
		catch(...){Leave();	throw(-1);}

		Leave();

		return(returnMe);
	}

	size_t GetPrefixSizeT(size_t position = 0) const;

	/**
	 * @brief Adds data of any type to the packet.
	 *
	 * Data is added to the packet's data buffer starting at Packet::cursorPos.
	 * Packet::cursorPos is moved along by the size of the data retrieved.\n\n
	 *
	 * C strings should not be added using this method, use AddStringC() instead.
	 *
	 * @exception ErrorReport If retrieving data would mean exceeding the used size.
	 */
	template<typename T>
	void Add(T add)
	{
		const char * cCommand = "Packet::Add";

		Enter();

		try
		{
			// Get the new end position after adding
			size_t endPos = GetCursor() + sizeof(add);

			// Increase memory size as necessary
			if(endPos > GetMemorySize())
			{
				ChangeMemorySize(endPos);
			}

			// Increase used size as necessary
			if(endPos > GetUsedSize())
			{
				SetUsedSize(endPos);
			}

			// add each byte of 'add' to the buffer.
			*(T*)&data[GetCursor()] = add;	

			// Move cursor
			SetCursor(endPos);
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & Error){Leave();	throw(Error);}
		catch(...){Leave();	throw(-1);}

		Leave();
	}
};