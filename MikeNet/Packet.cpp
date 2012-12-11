#include "FullInclude.h"

/**
 * @brief Changes Packet::data to point to an alternative point in memory that is not managed by the object.
 *
 * The object will no longer attempt to deallocate or reallocate this memory and assumes no responsibility over it.\n\n
 *
 * SetMemorySize() and ChangeMemorySize() will fail.
 *
 * @exception ErrorReport If @a paraUsedSize > @a paraMemSize. 
 *
 * @param newPtr New pointer to set Packet::data to.
 * @param paraMemSize Memory size of pointer.
 * @param paraUsedSize Amount of memory pointed to by @a newPtr that should be considered in use.
 */
void Packet::SetDataPtr(char * newPtr, size_t paraMemSize, size_t paraUsedSize)
{
	Enter();

	try
	{
		// Check parameters
		_ErrorException((paraUsedSize > paraMemSize),"changing a packet's data pointer, used size cannot be more than memory size",0,__LINE__,__FILE__);

		// Cleanup old memory
		delete[] data;

		// Set data and related variables
		dataPtrChanged = true; // data will not be cleaned up
		memSize = paraMemSize;
		usedSize = paraUsedSize;
		cursorPos = 0;

		data = newPtr;
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief Undoes SetDataPtr returning Packet object to normal usage.
 *
 * @exception ErrorReport If dataPtrChanged = false.
 */
void Packet::UnsetDataPtr()
{
	Enter();
	try
	{
		_ErrorException((dataPtrChanged == false),"attempting UnsetDataPtr, dataPtrChanged must be true",0,__LINE__,__FILE__);
		dataPtrChanged = false;
		data = NULL;
		memSize = 0;
		usedSize = 0;
		cursorPos = 0;
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}
	Leave();
}

/**
 * @brief Decrypts WSABUF.
 *
 * @param decryptMe WSABUF to decrypt.
 * @param used Number of bytes of the WSABUF buffer that are in use; not including @a offset bytes (note that WSABUF.len indicates the size of the buffer, not the amount that has been filled).
 * @param offset Number of bytes from the start of the buffer that should be ignored (i.e. Not copied). Remember that @a used does not include @a offset bytes.
 * @param key Decryption key to use while decrypting.
 */
void Packet::DecryptWSABUF(WSABUF decryptMe, size_t used, size_t offset, const EncryptKey * key)
{
	// Create packet that uses decryptMe's memory
	Packet packet;
	packet.SetDataPtr(decryptMe.buf+offset,used,used);

	// Decrypt packet
	packet.Decrypt(*key);
}

/**
 * @brief Sets internal variables to default.
 *
 * @param paraFull If true then data and memSize are set to 0, this is only done
 * in the constructor because in all other places memory should be cleaned up.
 */
void Packet::DefaultVariables(bool paraFull)
{
	Enter();

	if(paraFull == true)
	{
		dataPtrChanged = false;
		data = NULL;
		memSize = 0;
	}

	clientFrom = 0;
	operation = 0;
	instance = 0;
	age = 0;
	cursorPos = 0;
	usedSize = 0;
	
	Leave();
}

/**
 * @brief Constructor.
 */
Packet::Packet()
{
	DefaultVariables(true);
}

/**
 * @brief Destructor.
 */
Packet::~Packet()
{
	const char * cCommand = "an internal function (~Packet)";
	try
	{
		CleanupThreadsLocal();

		if(dataPtrChanged == false)
		{
			delete[] data;
		}

		DefaultVariables(true);
	}
	MSG_CATCH
}

/**
 * @brief Copy constructor / assignment operator helper function.
 *
 * @warning Packet::memSize and packet::data must be valid (for both objects) before using this method
 * e.g. If memSize is 0 then data must be NULL.
 *
 * @warning Never let this method copy dataPtrChanged, because it is a deep copy!
 * 
 * @param copyMe Packet to be copied into this object.
 */
void Packet::Copy(const Packet & copyMe)
{
	copyMe.Enter();
	this->Enter();
	
	try
	{
		if(copyMe.usedSize > 0)
		{
			// Allocate memory if more is needed
			if(memSize < copyMe.usedSize)
			{
				SetMemorySize(copyMe.usedSize);
			}

			// Copy data
			memcpy(data,copyMe.data,copyMe.usedSize);
		}

		clientFrom = copyMe.clientFrom;
		operation = copyMe.operation;
		instance = copyMe.instance;
		age = copyMe.age;
		cursorPos = copyMe.cursorPos;
		usedSize = copyMe.usedSize;
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){this->Leave(); copyMe.Leave(); throw(Error);}
	catch(...){this->Leave(); copyMe.Leave(); throw(-1);}

	this->Leave();
	copyMe.Leave();
}

/**
 * @brief Deep copy constructor.
 *
 * @param	copyMe	Object to copy.
 */
Packet::Packet(const Packet & copyMe)
{
	// Set to default, so that Copy works properly
	DefaultVariables(true);

	// Copy data
	Copy(copyMe);
}

/**
 * @brief Deep assignment operator.
 *
 * @param	copyMe	Object to copy. 
 *
 * @return	reference to this object.
 */
Packet & Packet::operator= (const Packet & copyMe)
{
	// Copy data
	Copy(copyMe);

	return(*this);
}

/**
 * @brief Copy constructor / assignment operator helper function.
 *
 * @warning Packet::memSize and packet::data must be valid (for both objects) before using this method
 * e.g. If memSize is 0 then data must be NULL.
 * 
 * @param copyMe String to copy.
 * @param includeNullTerminator If true null terminator will be copied into packet, if false null terminator
 * is dropped.
 */
void Packet::Copy(const char * copyMe)
{
	Enter();

	// Determine string length
	size_t strLength;

	if(copyMe == NULL)
	{
		strLength = 0;
	}
	else
	{
		strLength = strlen(copyMe);
	}

	// Allocate memory if more is needed
	if(memSize < strLength)
	{
		SetMemorySize(strLength);
	}

	if(strLength > 0)
	{
		// Copy data
		memcpy(data, copyMe, strLength);
	}
	
	usedSize = strLength;
	cursorPos = strLength;

	Leave();
}

/**
 * @brief Deep copy constructor.
 *
 * @param	copyMe	String to copy.
 */
Packet::Packet(const char * copyMe)
{
	// Set to default, so that Copy works properly
	DefaultVariables(true);

	// Copy data
	Copy(copyMe);
}

/**
 * @brief Deep assignment operator.
 *
 * @param	copyMe	String to copy.
 */
Packet & Packet::operator=(const char * copyMe)
{
	// Set to default, so that Copy works properly
	DefaultVariables(true);

	// Copy data
	Copy(copyMe);

	return *this;
}

/**
 * @brief Copy constructor / assignment operator helper function.
 *
 * @warning Packet::memSize and packet::data must be valid (for both objects) before using this method
 * e.g. If memSize is 0 then data must be NULL.
 * 
 * @param copyMe String to copy.
 */
void Packet::Copy(const ComString & copyMe)
{
	Enter();

	copyMe.CopyIntoPacket(*this);

	Leave();
}

/**
 * @brief Deep copy constructor.
 *
 * @param	copyMe	COM string to copy.
 */
Packet::Packet(const ComString & copyMe)
{
	DefaultVariables(true);
	Copy(copyMe);
}

/**
 * @brief Deep assignment operator.
 *
 * @param	copyMe	COM string to copy.
 *
 * @return	reference to this object.
 */
Packet & Packet::operator= (const ComString & copyMe)
{
	Copy(copyMe);
	return *this;
}

/**
 * @brief WSABUF assignment operator.
 *
 * @note This method does not actually implement the = operator because 3 parameters (not just one) are required.
 * @exception ErrorReport If copyMe.len < @a used + @a offset as this would cause a buffer overrun. 
 *
 * @param copyMe WSABUF to copy.
 * @param used Number of bytes of the WSABUF buffer that are in use; not including @a offset bytes (note that WSABUF.len indicates the size of the buffer, not the amount that has been filled).
 * @param offset Number of bytes from the start of the buffer that should be ignored (i.e. Not copied). Remember that @a used does not include @a offset bytes.
 *
 * @return reference to this object.
 */
Packet & Packet::equalWSABUF (const WSABUF & copyMe, size_t used, size_t offset)
{
	Enter();

	try
	{
		_ErrorException((copyMe.len < used + offset),"using equalWSABUF packet method, precondition (copyMe.len < used + offset) failed, buffer overrun would be caused",0,__LINE__,__FILE__);

		// Memory size
		// Resize buffer if it is too small
		if(memSize < used)
		{
			SetMemorySize(used);
		}

		// Copy data
		usedSize = used;
		memcpy(data, copyMe.buf + offset, used);
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();

	return(*this);
}


/**
 * @brief Equality operator.
 *
 * Packet::usedSize, Packet::cursorPos and Packet::memSize are ignored in the comparison.
 * 
 * @param param Packet to compare.
 *
 * @return true if @a param and this object are the same, false if they are different.
 */
bool Packet::operator==(const Packet & param) const
{
	bool returnMe = false;

	param.Enter();
	this->Enter();

	try
	{
		// Check to see if general variables are the same
		if( (param.clientFrom == clientFrom) &&
			(param.age == age) &&
			(param.instance == instance) &&
			(param.operation == operation) &&
			(param.usedSize == usedSize) )
		{
			// Check to see if contents of packet is the same
			bool same = true;

			for(size_t n = 0; n<usedSize; n++)
			{
				if(param.data[n] != data[n])
				{
					same = false;
					break;
				}
			}

			returnMe = same;
		}
		else
		{
			returnMe = false;
		}
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){this->Leave(); param.Leave(); throw(Error);}
	catch(...){this->Leave(); param.Leave(); throw(-1);}

	this->Leave();
	param.Leave();

	return returnMe;
}

/**
 * @brief Not equal operator.
 *
 * Packet::usedSize, Packet::cursorPos and Packet::memSize are ignored in the comparison.
 * 
 * @param packet Packet to compare.
 *
 * @return false if @a param and this object are the same, true if they are different.
 */
bool Packet::operator!=( const Packet & packet) const
{
	return !(*this == packet);
}


/**
 * @brief C string equality operator.
 *
 * Packet::usedSize is compared with @a param's length.
 * Packet::data is compared with @a param's contents.
 * 
 * @param param Null terminated string to compare.
 *
 * @return true if @a param and this object are the same, false if they are different.
 */
bool Packet::operator==(const char * param) const
{
	bool returnMe = false;
	size_t stringSize = strlen(param);	

	Enter();
	try
	{
		// Check to see if string is same size.
		// Note that contents of packet may be NULL terminated,
		// stringSize does not include the NULL terminator.
		if(stringSize == usedSize || stringSize == usedSize-1)
		{
			// Check to see if contents of packet is the same
			bool same = true;

			for(size_t n = 0; n<usedSize; n++)
			{
				if(param[n] != data[n])
				{
					same = false;
					break;
				}
			}

			returnMe = same;
		}
		else
		{
			returnMe = false;
		}
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();

	return returnMe;
}

/**
 * @brief C string not equal operator.
 *
 * Packet::usedSize is compared with @a param's length.
 * Packet::data is compared with @a param's contents.
 * 
 * @param str Null terminated string to compare.
 *
 * @return false if @a param and this object are the same, true if they are different.
 */
bool Packet::operator!=( const char * str) const
{
	return !(*this == str);
}

/**
 * @brief WSABUF equality operator.
 *
 * Packet::usedSize is compared with @a used.
 * Packet::data is compared with @a copyMe.buf's contents.
 * 
 * @param copyMe WSABUF to compare.
 * @param used Number of bytes used in WSABUF. Note that copyMe.len indicates only the size of the buffer, not how much of the buffer is in use.
 *
 * @return true if @a copyMe and this object are the same, false if they are different.
 */
bool Packet::compareWSABUF (const WSABUF & copyMe, size_t used)
{
	bool returnMe = false;
	Enter();

	try
	{
		// Check to see if general variables are the same
		if(usedSize == used)
		{
			// Check to see if contents of packet is the same
			bool same = true;

			for(size_t n = 0; n<usedSize; n++)
			{
				if(copyMe.buf[n] != data[n])
				{
					same = false;
					break;
				}
			}

			returnMe = same;
		}
		else
		{
			returnMe = false;
		}
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();

	return returnMe;
}

/**
 * @brief Addition operator helper method.
 *
 * @a source is added to @a destination at position Packet::cursorPos and @a source is copied entirely up to @a source.usedSize.
 * @a destination's cursor is set to the new end of the packet (@a destination.cursorPos + @a source.usedSize).
 *
 * @param [out] destination Packet that @a source will be added to.
 * @param source Packet that will be added to @a destination.
 */
void Packet::_AddPacket(Packet & destination, const Packet & source)
{
	destination.Enter();
	source.Enter();

	try
	{
		size_t originalCursor = destination.GetCursor();
		destination._UpdateMemoryAndCursor(source.GetUsedSize());
		memcpy(destination.data + originalCursor, source.data, source.GetUsedSize());
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){source.Leave(); destination.Leave();	throw(Error);}
	catch(...){source.Leave(); destination.Leave(); throw(-1);}

	source.Leave();
	destination.Leave();

}

/**
 * @brief Addition operator (indirect).
 *
 * @param addMe Packet we want to add to this object. The contents of this object is not changed,
 * the addition happens in an auxiliary object is returned.
 *
 * @return a packet containing @a addMe added to this object.
 */
Packet Packet::operator+ (const Packet & addMe)
{
	Packet aux;

	// Do not use 'this' since we do not want to change this
	// we want to return a copy of this + addMe
	aux = *this;
	_AddPacket(aux, addMe);

	return(aux);
}

/**
 * @brief Addition operator (direct).
 *
 * @a addMe is added directly to this object.
 *
 * @param addMe Packet we want to add to this object.
 */
void Packet::operator+= (const Packet & addMe)
{
	// Add 'addMe' directly to 'this'
	_AddPacket(*this,addMe);
}


/**
 * @brief Addition operator helper method.
 *
 * Adds WSABUF @a source to @a destination.
 * @a source is added to @a destination at position usedSize and @a source is copied up to @a used.
 * @a destination's cursor is not changed.
 *
 * @param destination Packet that @a source will be added to.
 * @param source WSABUF that will be added to @a destination.
 * @param used Number of bytes of @a source that are in use. Note that @a source.len indicates the size of the buffer rather than the number of bytes in use.
 */
void Packet::_AddWSABUF(Packet & destination, const WSABUF & source, size_t used)
{
	_ErrorException((source.len < used),"using _AddWSABUF packet method, precondition (source.len < used) failed, buffer overrun would be caused",0,__LINE__,__FILE__);
	
	destination.Enter();

	try
	{
		// Allocate more memory if necessary
		size_t newSize = destination.usedSize + used;
		if(newSize > destination.memSize)
		{
			destination.ChangeMemorySize(newSize);
		}

		// Copy Source
		memcpy(destination.data + destination.usedSize, source.buf, used);

		// Increase used size
		destination.usedSize += used;
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){destination.Leave();	throw(Error);}
	catch(...){destination.Leave();	throw(-1);}

	destination.Leave();
}

/**
 * @brief Addition operator (indirect).
 * 
 * The contents of this object is not changed.
 *
 * @param addMe WSABUF we want to add to this object. The contents of this object is not changed,
 * the addition happens in an auxiliary object is returned.
 * @param used Number of bytes of @a addMe that are in use. Note that @a addMe.len indicates the size of the buffer rather than the number of bytes in use.
 * @return a packet containing @a addMe added to this object.
 */
Packet Packet::addWSABUF (const WSABUF & addMe, size_t used)
{
	Packet Return;

	// Do not use 'this' since we do not want to change this
	// we want to return a copy of this + addMe
	Return = *this;
	_AddWSABUF(Return, addMe, used);

	return(Return);
}

/**
 * @brief Addition operator (direct).
 *
 * @a addMe is added directly to this object.
 *
 * @param addMe WSABUF we want to add to this object.
 * @param used Number of bytes of @a addMe that are in use. Note that @a addMe.len indicates the size of the buffer rather than the number of bytes in use.
 */
void Packet::addEqualWSABUF (const WSABUF & addMe, size_t used)
{
	// Add 'addMe' directly to 'this'
	_AddWSABUF(*this, addMe, used);
}



/**
 * @brief Empties packet and resets variables.
 *
 * Packet::memSize and Packet::data are left unchanged.
 */
void Packet::Clear()
{
	DefaultVariables(false);
}

/**
 * @brief	Starts an encryption or decryption operation.
 *
 * @param	encryption	If true an encryption operation will be started, if false a decryption operation will be started.
 * @param	key			The key to use.
 * @param block If true the method will not return until the encryption operation has finished, if false
 * it will return straight away and Packet::IsLastEncryptionOperationFinished and Packet::WaitUntilLastEncryptionOperationFinished
 * can be used to determine when it is completed.
 */
void Packet::DoEncryptionOperation(bool encryption, const EncryptKey & key, bool block)
{
	// Setup threads (if necessary) and post message to each thread.
	// Each thread will encrypt a part of the packet.
	SetupThreadsLocal(ThreadSingleMessageKeepLastUser::CLASS_INDEX_PACKET,ThreadSingle::GetNumLogicalCores(),&EncryptionThread,NULL);
	for(size_t n = 0;n<GetNumThreads();n++)
	{
		ThreadMessageItem * encryptionMessage = new (nothrow) ThreadMessageItemEncrypt(encryption,this,key,n,GetNumThreads());
		Utility::DynamicAllocCheck(encryptionMessage,__LINE__,__FILE__);
		PostMessageItem(n,encryptionMessage);
	}

	if(block == true)
	{
		WaitUntilLastEncryptionOperationFinished();
	}
}

/**
 * @brief Decrypts packet.
 *
 * Note that you must not read or write data to the packet while the operation is in progress.
 * 
 * @exception ErrorReport If Packet::usedSize = 0.
 * @exception ErrorReport If Packet::usedSize < 16.
 * @exception ErrorReport If Packet::usedSize % 16 != 0.
 *
 * @param key Key to use when decrypting, must be the same as the key used for encrypting for decryption to work.
 * @param block If true the method will not return until the encryption operation has finished, if false
 * it will return straight away and Packet::IsLastEncryptionOperationFinished and Packet::WaitUntilLastEncryptionOperationFinished
 * can be used to determine when it is completed.
 */
void Packet::Decrypt( const EncryptKey & key, bool block )
{
	Enter();

	try
	{
		// Ensure that 16 byte chunks can be made
		// Note: % operator acts unusually with value 1 and 0
		_ErrorException((usedSize == 0),"decrypting a packet. There is no data to decrypt (used size is 0)",0,__LINE__,__FILE__);
		_ErrorException((usedSize < 16),"decrypting a packet. Packet's used size must be a multiple of 16",0,__LINE__,__FILE__);
		_ErrorException((usedSize % 16 != 0),"decrypting a packet. Packet's used size must be a multiple of 16",0,__LINE__,__FILE__);
		
		DoEncryptionOperation(false,key,block);
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief Encrypts packet.
 *
 * Note that you must not read or write data to the packet while the operation is in progress.
 * 
 * @exception ErrorReport If Packet::usedSize = 0.
 *
 * @param key Key to use when encrypting, must be the same as the key used for decrypting for decryption to work.
 * @param block If true the method will not return until the encryption operation has finished, if false
 * it will return straight away and Packet::IsLastEncryptionOperationFinished and Packet::WaitUntilLastEncryptionOperationFinished
 * can be used to determine when it is completed.
 */
void Packet::Encrypt( const EncryptKey & key, bool block )
{
	Enter();

	try
	{
		// Must be data
		_ErrorException((usedSize == 0),"encrypting a packet. There is no data to encrypt (used size is 0)",0,__LINE__,__FILE__);

		// Calculate new used size
		size_t newUsedSize;

		// Ensure that packet is multiple of ENCRYPTION_CHUNK_SIZE because
		// encryption is done on ENCRYPTION_CHUNK_SIZE byte chunks
		if(usedSize < ThreadMessageItemEncrypt::ENCRYPTION_CHUNK_SIZE)
		{
			newUsedSize = ThreadMessageItemEncrypt::ENCRYPTION_CHUNK_SIZE;
		}
		else
		{
			size_t mod = usedSize % ThreadMessageItemEncrypt::ENCRYPTION_CHUNK_SIZE;
			
			if(mod != 0)
			{
				newUsedSize = usedSize+(ThreadMessageItemEncrypt::ENCRYPTION_CHUNK_SIZE-mod);
			}
			else
			{
				// No change
				newUsedSize = usedSize;
			}
		}

		SetUsedSize(newUsedSize);

		DoEncryptionOperation(true,key,block);
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief Changes the memory size of the packet, erasing all packet data in the process.
 *
 * Packet options are not lost after using this command.
 * Only Packet::memSize, Packet::cursorPos and Packet::usedSize are changes.\n\n
 *
 * See Packet::memSize for more information on memory size.
 * 
 * @param newSize New memory size.
 */
void Packet::SetMemorySize(size_t newSize)
{
	int returnMe = 0;

	Enter();
	try
	{
		if(dataPtrChanged == false)
		{
			if(newSize != GetMemorySize())
			{
				// Cleanup
				delete[] data;

				// Allocate memory
				if(newSize > 0)
				{
					data = new (nothrow) char[newSize];
					Utility::DynamicAllocCheck(data,__LINE__,__FILE__);
				}
				else
				{
					data = NULL;
				}
			}
		}
			
		// Set size
		memSize = newSize;

		// Clear packet
		Clear();
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief Changes the memory size of the packet without erasing its contents.
 *
 * Packet::memSize cannot be decreased below Packet::usedSize,
 * if you try to do this the Packet::memSize will be set to = Packet::usedSize
 * as it decreases as much as possible.\n\n
 *
 * See Packet::memSize for more information on memory size.
 *
 * @param newSize New memory size.
 */
void Packet::ChangeMemorySize(size_t newSize)
{
	Enter();

	try
	{
		// Reallocate
		if(dataPtrChanged == false)
		{
			if(newSize != memSize)
			{
				if(newSize > 0)
				{
					// Store current data
					Packet Temp(*this);

					// Change memory size, current data is lost
					SetMemorySize(newSize);

					// Restore data
					*this = Temp;
				}
				else
				{
					// Deallocate completely if NewSize is 0
					delete[] data;
					data = NULL;
				}
			}
		}
		else
		{
			memSize = newSize;
		}
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief Erases part of the packet, decreasing Packet::usedSize and Packet::cursorPos.
 * 
 * @param startPos Position to begin erasing.
 * @param amount Number of bytes to erase.
 */
void Packet::Erase(size_t startPos, size_t amount)
{
	Enter();

	try
	{
		// If amount is 0 then we don't need to erase anything
		if(amount > 0)
		{
			// Ensure that there is enough data to erase, so that we don't go out of bounds
			_ErrorException((amount+startPos > usedSize),"erasing part of a packet, startPos is too high or/and amount is too high",0,__LINE__,__FILE__);
				
			// Simply need to decrease used size if there is
			// nothing to shift down, i.e. we erase up to the
			// end of the packet
			if(amount + startPos < usedSize)
			{
				// Destination begins at startPos
				// Source begins at startPos + amount, straight after data to erase
				// Source ends at the end of all data currently in use, so that no useful data is lost in the shift
				memcpy(data + startPos,(data + startPos) + amount, (usedSize - startPos) - amount);
			}

			// Change used size
			usedSize -= amount;

			// Change cursor
			if(amount >= cursorPos)
			{
				cursorPos = 0;
			}
			else
			{
				cursorPos -= amount;
			}
		}
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief Inserts empty space at cursorPos, changing Packet::usedSize and Packet::memSize as necessary.
 *
 * Packet::cursorPos is not changed; this means that data can be written over empty space straight away.
 *
 * @param amount Number of bytes to insert.
 */
void Packet::Insert(size_t amount)
{
	Enter();

	try
	{
		// If amount is 0 then we don't need to insert anything
		if(amount > 0)
		{
			// Calculate new size
			size_t oldSize = GetUsedSize();
			size_t newSize = oldSize + amount;

			// Adjust memory size and used size
			SetUsedSize(newSize);

			// Destination begins after insert area
			// Source is start of insert area
			// Source ends at end of the packet
			size_t sourceSize = oldSize - cursorPos;
			char * destination = data + cursorPos + amount;

			// Note: cannot do straight memcpy because source is part of destination
			// Store source separately
			char * tempSource = new (nothrow) char[sourceSize];
			Utility::DynamicAllocCheck(tempSource,__LINE__,__FILE__);
			memcpy(tempSource, data + cursorPos, sourceSize);

			// Copy source into destination
			memcpy(destination, tempSource, sourceSize);

			// Deallocate temporary source
			delete[] tempSource;
		}
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief Changes Packet::usedSize, increasing Packet::memSize if necessary.
 */
void Packet::SetUsedSize(size_t paraSize)
{
	Enter();

	try
	{
		if(paraSize > memSize)
		{
			ChangeMemorySize(paraSize);
		}

		usedSize = paraSize;

		if(cursorPos > usedSize)
		{
			cursorPos = usedSize;
		}
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief Retrieves Packet::usedSize.
 *
 * @return Packet::usedSize.
 */
size_t Packet::GetUsedSize() const
{
	Enter();
		size_t returnMe = usedSize;
	Leave();

	return(returnMe);
}

/**
 * @brief Retrieves Packet::memSize.
 *
 * @return Packet::memSize.
 */
size_t Packet::GetMemorySize() const
{
	Enter();
		size_t returnMe = memSize;
	Leave();

	return(returnMe);
}

/**
 * @brief Retrieves Packet::age.
 *
 * @return Packet::age.
 */
clock_t Packet::GetAge() const
{
	Enter();
		clock_t returnMe = age;
	Leave();

	return(returnMe);
}

/**
 * @brief Changes Packet::age.
 *
 * @param age New age value.
 */
void Packet::SetAge(clock_t age)
{
	Enter();
		this->age = age;
	Leave();
}

/**
 * @brief Retrieves the number of unread bytes in the packet.
 *
 * @return Packet::usedSize - Packet::cursorPos.
 */
size_t Packet::GetPacketRemainder() const
{
	Enter();
		size_t returnMe = usedSize - cursorPos;
	Leave();

	return(returnMe);
}

/**
 * @brief Changes Packet::cursorPos.
 *
 * @param paraCursor New cursor position.
 *
 * @exception ErrorReport If @a paraCursor > Packet::usedSize.
 */
void Packet::SetCursor(size_t paraCursor) const
{
	Enter();
	try
	{
		_ErrorException((paraCursor > usedSize),"changing the cursor position, new position > used size",0,__LINE__,__FILE__);
		cursorPos = paraCursor;
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief Retrieves Packet::cursorPos.
 *
 * @return Packet::cursorPos.
 */
size_t Packet::GetCursor() const
{
	Enter();
		size_t returnMe = cursorPos;
	Leave();

	return(returnMe);
}

/**
 * @brief Retrieves Packet::clientFrom.
 *
 * @return the ID of the client that this packet was received from.
 */
size_t Packet::GetClientFrom() const
{
	Enter();
		size_t returnMe = clientFrom;
	Leave();

	return(returnMe);
}

/**
 * @brief Retrieves Packet::operation.
 *
 * @return operation ID that packet was received on.
 */
size_t Packet::GetOperation() const
{
	Enter();
		size_t returnMe = operation;
	Leave();

	return(returnMe);
}

/**
 * @brief Retrieves Packet::instance.
 *
 * @return instance ID that packet was received on.
 */
size_t Packet::GetInstance() const
{
	Enter();
		size_t returnMe = instance;
	Leave();

	return(returnMe);
}

/**
 * @brief Sets Packet::clientFrom.
 *
 * @param clientFrom The ID of the client that this packet was received from.
 */
void Packet::SetClientFrom( size_t clientFrom )
{
	Enter();
		this->clientFrom = clientFrom;
	Leave();
}

/**
 * @brief Sets Packet::operation.
 *
 * @param operation Operation ID that packet was received on.
 */
void Packet::SetOperation( size_t operation )
{
	Enter();
		this->operation = operation;
	Leave();
}

/**
 * @brief Sets Packet::instance.
 *
 * @param instance Instance ID that packet was received on.
 */
void Packet::SetInstance( size_t instance )
{
	Enter();
		this->instance = instance;
	Leave();
}

/**
 * @brief Adds a variable of type size_t to the packet.
 *
 * Data is added to the packet's data buffer starting at Packet::cursorPos.
 * Packet::cursorPos is moved along by the size of the data retrieved.\n\n
 *
 * Note that the cursor will move Utility::LargestSupportedBytesInt length
 * even if sizeof(size_t) is smaller. This is to ensure that the different bit versions
 * of the module are compatible with each other.
 *
 * @param add Data to add.
 */
void Packet::AddSizeT(size_t add)
{
	Enter();
	try
	{
		Add(add);
	
		// Pad insertion so that 32 bit insertion is the same size as 64 bit insertion
		unsigned int difference = Utility::LargestSupportedBytesInt - sizeof(add);
		if(difference > 0)
		{
			size_t originalCursor = GetCursor();
			_UpdateMemoryAndCursor(difference);
			ZeroMemory(data + originalCursor, difference); // Ensure bytes are set to 0.
		}
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief Adds a variable of type clock_t to the packet. 
 *
 * Data is added to the packet's data buffer starting at Packet::cursorPos.
 * Packet::cursorPos is moved along by the size of the data retrieved.\n\n
 *
 * Note that the cursor will move Utility::LargestSupportedBytesInt length
 * even if sizeof(clock_t) is smaller. This is to ensure that the different bit versions
 * of the module are compatible with each other.
 *
 * @param add Data to add.
 */
void Packet::AddClockT(clock_t add)
{
	Enter();

	try
	{
		Add(add);

		// Pad insertion so that 32 bit insertion is the same size as 64 bit insertion
		unsigned int difference = Utility::LargestSupportedBytesInt - sizeof(add);
		if(difference > 0)
		{
			size_t originalCursor = GetCursor();
			_UpdateMemoryAndCursor(difference);
			ZeroMemory(data + originalCursor, difference); // Ensure bytes are set to 0.
		}
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief Adjusts Packet::usedSize and Packet::memSize if Packet::cursorPos increases beyond it with the addition of @a addSize.
 *
 * @param amount Size that Packet::cursorPos may increase to after this method has been called.
 */
void Packet::_UpdateMemoryAndCursor(size_t amount)
{
	Enter();
	try
	{
		size_t newSize = GetCursor() + amount;

		if(newSize > GetUsedSize())
		{
			SetUsedSize(newSize);
		}
		IncCursor(amount);
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}
	Leave();
}

/**
 * @brief Adds a C string to the packet.
 *
 * Adds @a add to this object. Data is added to the packet's data buffer starting at Packet::cursorPos.
 * Packet::cursorPos is moved along by the size of the data retrieved.\n\n
 *
 * @param add C string to add.
 * @param length Length of C string, if 0 then the string must be NULL terminated and the length will be calculated automatically..
 * @param prefix If true the string will be added with a prefix that is used by GetStringC to determine its length and how much data to get.
 */
void Packet::AddStringC(const char * add, size_t length, bool prefix)
{
	Enter();

	try
	{
		// Find string size
		if(length == 0)
		{
			length = strlen(add);
		}

		size_t originalCursor = GetCursor();

		// Update cursor, used size and memory size
		size_t sizeOfDataToAdd = length;
		if(prefix == true)
		{
			sizeOfDataToAdd += Packet::prefixSizeBytes;
		}
		_UpdateMemoryAndCursor(sizeOfDataToAdd);

		// Move cursor back to original position so that we can insert data correctly
		size_t newCursor = GetCursor();
		SetCursor(originalCursor);

		// Add prefix which is the size of the string (not including the prefix)
		if(prefix == true)
		{
			// Must adjust used size BEFORE cursor position (Add changes cursor position)
			// Must adjust used size after memory size (ChangeMemorySize uses copy constructor)
			AddSizeT(length);
		}
		
		// Copy data
		if(length > 0)
		{
			memcpy(data + GetCursor(), add, length);
		}

		// Move cursor forwards to just after the data we have added
		SetCursor(newCursor);
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief Retrieves a variable of type size_t from the packet.
 *
 * Data is retrieved from the packet's data buffer starting at Packet::cursorPos.
 * Packet::cursorPos is moved along by the size of the data retrieved.\n\n
 *
 * Note that the cursor will move Utility::LargestSupportedBytesInt length
 * even if sizeof(size_t) is smaller. This is to ensure that the different bit versions
 * of the module are compatible with each other.
 *
 * @return data from the packet.
 */
size_t Packet::GetSizeT() const
{
	size_t returnMe = 0;

	Enter();
	try
	{
		returnMe = Get<size_t>();

		// Pad retrieval so that 32 bit retrieval is the same size as 64 bit retrieval
		unsigned int difference = Utility::LargestSupportedBytesInt - sizeof(returnMe);
		if(difference > 0)
		{
			SetCursor(GetCursor() + difference);
		}
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();

	return(returnMe);
}

/**
 * @brief Retrieves a variable of type size_t from the specified position without moving the cursor.
 *
 * Note that the cursor will move Utility::LargestSupportedBytesInt length
 * even if sizeof(size_t) is smaller. This is to ensure that the different bit versions
 * of the module are compatible with each other.
 *
 * @param position Position to retrieve data from.
 *
 * @return retrieved data.
 */
size_t Packet::GetPrefixSizeT( size_t position ) const
{
	size_t returnMe = 0;
	const char * cCommand = "Packet::GetPrefixSizeT";

	Enter();
	try
	{
		size_t originalCursor = GetCursor();

		SetCursor(position);
		returnMe = GetSizeT();

		SetCursor(originalCursor);
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();

	return(returnMe);
}


/**
 * @brief Retrieves a variable of type clock_t from the packet. 
 *
 * Data is retrieved from the packet's data buffer starting at Packet::cursorPos.
 * Packet::cursorPos is moved along by the size of the data retrieved.\n\n
 *
 * Note that the cursor will move Utility::LargestSupportedBytesInt length
 * even if sizeof(clock_t) is smaller. This is to ensure that the different bit versions
 * of the module are compatible with each other.
 *
 * @return data from the packet.
 */
clock_t Packet::GetClockT() const
{
	clock_t returnMe = 0;
	Enter();
	try
	{
		returnMe = Get<clock_t>();

		// Pad retrieval so that 32 bit retrieval is the same size as 64 bit retrieval
		unsigned int difference = Utility::LargestSupportedBytesInt - sizeof(returnMe);
		if(difference > 0)
		{
			SetCursor(GetCursor() + difference);
		}
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}
	Leave();

	return(returnMe);
}

/**
 * @brief Retrieves the size of a string by reading its prefix.
 *
 * The string must have a prefix or this command will return meaningless values.\n\n
 *
 * The prefix is read at Packet::cursorPos. Packet::cursorPos is NOT changed;
 * this means that you can use this command and straight away use GetStringC().
 *
 * @return the size of the string.
 */
size_t Packet::GetStringSize() const
{
	size_t stringSize = 0;
	Enter();
	try
	{
		size_t originalCursor = GetCursor();
		stringSize = GetSizeT();
		SetCursor(originalCursor);
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}
	Leave();
	return stringSize;
}

/**
 * @brief Gets a C string from the packet, allocating memory to it and returning it.
 *
 * Data is retrieved from the packet's data buffer starting at Packet::cursorPos.
 * Packet::cursorPos is moved along by the size of the data retrieved.
 * 
 * @param length Length of string to retrieve, if 0 then the string must have a prefix which indicates its length and this is used to determine how much data to retrieve.
 * @param nullTerminated If true return string has a null terminator appended.
 *
 * @return c string from the packet.
 */
char * Packet::GetStringC(size_t length, bool nullTerminated) const
{
	char * returnMe = NULL;
	Enter();

	try
	{
		// Get size in bytes of string
		size_t strSize;
		if(length == 0)
		{
			strSize = GetSizeT();
		}
		else
		{
			strSize = length;
		}
					
		// Prevent going out of bounds
		_ErrorException((strSize > GetPacketRemainder()),"getting a C string from a packet, the string size is too large",strSize,__LINE__,__FILE__);
		
		// Allocate memory to return string
		if(nullTerminated == true)
		{
			returnMe = new (nothrow) char[strSize+1];
			Utility::DynamicAllocCheck(returnMe,__LINE__,__FILE__);

			returnMe[strSize] = '\0'; // Null terminator
		}
		else
		{
			returnMe = new (nothrow) char[strSize];
			Utility::DynamicAllocCheck(returnMe,__LINE__,__FILE__);
		}

		// Copy data into C string
		memcpy(returnMe,data+cursorPos,strSize);

		// Move cursor
		cursorPos += strSize;
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();

	return(returnMe);
}

/**
 * Gets a C string from the packet, copying it into the specified memory location.
 *
 * @warning @a destination must have enough memory to store the retrieved string, use GetStringSize() to determine
 * how much memory is required.
 * 
 * @param destination Pointer to memory where C string should be copied into.
 * @param length Length of string to retrieve, if 0 then the string must have a prefix which indicates its length and this is used to determine how much data to retrieve.
 * @param nullTerminated If true return string has a null terminator appended.
 */
void Packet::GetStringC(char * destination, size_t length, bool nullTerminated) const
{
	Enter();

	try
	{
		// Get size in bytes of string
		size_t strSize;
		if(length == 0)
		{
			strSize = GetSizeT();
		}
		else
		{
			strSize = length;
		}
			
		// Prevent going out of bounds
		_ErrorException((strSize > usedSize-cursorPos),"getting a C string from a packet, the string size is too large",0,__LINE__,__FILE__);
		
		// Put relevant bytes into return string
		memcpy(destination,data+cursorPos,strSize);

		// Add null terminator
		if(nullTerminated == true)
		{
			destination[strSize] = '\0';
		}

		// Move cursor
		cursorPos += strSize;
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief Increases Packet::cursorPos.
 *
 * @exception ErrorReport If new cursor position > Packet::usedSize.
 *
 * @param amount Amount to increase Packet::cursorPos by.
 */
void Packet::IncCursor(size_t amount) const
{
	Enter();

	try
	{
		size_t newCursor = cursorPos += amount;
		_ErrorException((newCursor > usedSize),"increasing the cursor position, new position > used size",0,__LINE__,__FILE__);
		cursorPos = newCursor;
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief Retrieves a mutable pointer to Packet::data.
 *
 * @warning Incorrect use of this method can cause severe problems, use with caution.
 * @return pointer to data.
 */
char * Packet::GetDataPtr() 
{
	Enter();
		char * returnMe = data;
	Leave();
	return returnMe;
}

/**
 * @brief Retrieves a constant pointer to Packet::data.
 *
 * @warning Incorrect use of this method can cause severe problems, use with caution.
 * @return pointer to data.
 */
const char * Packet::GetDataPtr() const
{
	Enter();
		const char * returnMe = data;
	Leave();
	return returnMe;
}

/**
 * @brief Returns a copy of Packet::data.
 *
 * @return copy of data.
 */
char * Packet::GetDataPtrCopy() const
{
	char * returnMe = NULL;
	Enter();
	try
	{
		returnMe = new (nothrow) char[GetUsedSize()];
		Utility::DynamicAllocCheck(returnMe,__LINE__,__FILE__);
		memcpy(returnMe, GetDataPtr(), GetUsedSize());
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}
	Leave();

	return returnMe;
}

/**
 * @brief Copies pointer of packet data into a WSABUF, ignoring pre-existing WSABUF contents.
 *
 * @param [out]	buffer	The buffer to copy into.
 *
 * @warning Contents of @a buffer is ignored so any memory deallocation of the buffer should
 * take place before calling this method.
 * @warning Manipulation of this packet will impact on the buffer since the two will be linked
 * to the same data pointer, use with caution.
 * @warning Do not modify the data of buffer as this would violate const correctness. \n
 * this method is constant because it is assumed that the buffer will not be modified.
 */
void Packet::PtrIntoWSABUF(WSABUF & buffer) const
{
	Enter();
	try
	{
		buffer.buf = (char*)GetDataPtr(); // Cast is explained in warnings above
		buffer.len = static_cast<ULONG>(GetUsedSize());
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}
	Leave();
}

/**
 * @brief Copies packet into a WSABUF, allocating new memory and ignoring pre-existing WSABUF contents.
 *
 * @param [out]	buffer	The buffer to copy into.
 * @warning Contents of @a buffer is ignored so any memory deallocation of the buffer should
 * take place before calling this method.
 */
void Packet::CopyIntoWSABUF(WSABUF & buffer) const
{
	Enter();
	try
	{
		buffer.buf = GetDataPtrCopy();
		buffer.len = static_cast<ULONG>(GetUsedSize());
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}
	Leave();
}

/**
 * @brief Constructor to be used to create a new packet after data has been received via winsock.
 *
 * @param paraData Data to copy into the packet.
 * @param paraUsed Number of bytes of the WSABUF buffer (@a paraData) that are in use; not including @a paraOffset bytes (note that WSABUF.len indicates the size of the buffer, not the amount that has been filled).
 * @param paraOffset Number of bytes from the start of the buffer (@a paraData) that should be ignored (i.e. Not copied). Remember that @a paraUsed does not include @a paraOffset bytes.
 * @param paraClientFrom The client that the packet was received from. ClientFrom is set to this.
 * @param paraOperation The operation of the packet. ParaOperation is set to this.
 * @param paraInstance The instance that the packet was received on. Operation is set to this.
 * @param paraClock The age value of the packet. Clock is set to this.
 */
Packet::Packet(const WSABUF & paraData, size_t paraUsed, size_t paraOffset, size_t paraClientFrom, size_t paraOperation, size_t paraInstance, clock_t paraClock)
{
	DefaultVariables(true);
	LoadFull(paraData,paraUsed,paraOffset,paraClientFrom,paraOperation,paraInstance,paraClock);
}

/**
 * @brief Load packet with data and packet options.
 *
 * @param paraData Data to copy into the packet.
 * @param paraUsed Number of bytes of the WSABUF buffer (@a paraData) that are in use; not including @a paraOffset bytes (note that WSABUF.len indicates the size of the buffer, not the amount that has been filled).
 * @param paraOffset Number of bytes from the start of the buffer (@a paraData) that should be ignored (i.e. Not copied). Remember that @a paraUsed does not include @a paraOffset bytes.
 * @param paraClientFrom The client that the packet was received from. ClientFrom is set to this.
 * @param paraOperation The operation of the packet. ParaOperation is set to this.
 * @param paraInstance The instance that the packet was received on. Operation is set to this.
 * @param paraClock The age value of the packet. Clock is set to this.
 */
void Packet::LoadFull(const WSABUF & paraData, size_t paraUsed, size_t paraOffset, size_t paraClientFrom, size_t paraOperation, size_t paraInstance, clock_t paraClock)
{
	Enter();
	
	try
	{
		DefaultVariables(false);
		equalWSABUF(paraData,paraUsed,paraOffset);

		clientFrom = paraClientFrom;
		operation = paraOperation;
		instance = paraInstance;
		age = paraClock;
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief	Searches for a packet.
 *
 * @param	startPos		The position to start searching from within this object (inclusive).
 * @param	endPos			The position to stop searching within this object (inclusive).
 * If 0 then the search will search up to the end of the packet.
 * @param	findMe			The packet to find.
 * @param [out]	result		Set to the found position, if no position is found then this is not modified.
 * The found position will be the location that @a findMe begins. If NULL then is ignored.
 *
 * @return	true if @a findMe was found, false if not.
 */
bool Packet::Find( size_t startPos, size_t endPos, const Packet & findMe, size_t * result ) const
{
	bool found = false;

	Enter();
	findMe.Enter();
	try
	{
		if(endPos == 0)
		{
			endPos = this->GetUsedSize()-1;
		}

		_ErrorException((startPos >= this->GetUsedSize()),"searching a packet, start position is out of bounds",0,__LINE__,__FILE__);
		_ErrorException((endPos >= this->GetUsedSize()),"searching a packet, end position is out of bounds",0,__LINE__,__FILE__);

		if(findMe.GetUsedSize() > 0 && findMe.GetUsedSize() <= endPos+1)
		{
			for(size_t n = startPos; n <= endPos - findMe.GetUsedSize() + 1; n++)
			{
				// Check that all of findMe exists at this location.
				bool identical = true;
				for(size_t x = 0; x<findMe.GetUsedSize(); x++)
				{
					if(this->GetDataPtr()[n+x] != findMe.GetDataPtr()[x])
					{
						identical = false;
						break;
					}
				}

				if(identical == true)
				{
					found = true;

					if(result != NULL)
					{
						*result = n;
					}

					break;
				}
			}
		}
	}
	// Release control of all objects before throwing exception
	catch (ErrorReport & error){findMe.Leave(); Leave(); throw(error);}
	catch (...){findMe.Leave(); Leave(); throw(-1);}
	findMe.Leave();
	Leave();

	return found;
}


/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool Packet::TestClass()
{
	bool problem = false;
	cout << "Testing Packet class...\n";

	{
		Packet packet;

		// SetMemorySize, GetMemorySize, GetUsedSize, GetCursor
		packet.SetMemorySize(sizeof(int));

		if(packet.GetMemorySize() != sizeof(int))
		{
			cout << "GetMemorySize or SetMemorySize is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetMemorySize and SetMemorySize are good\n";
		}

		if(packet.GetUsedSize() != 0)
		{
			cout << "GetUsedSize or SetMemorySize is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetUsedSize and SetMemorySize are good\n";
		}

		if(packet.GetCursor() != 0)
		{
			cout << "GetCursor or SetMemorySize is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetCursor and SetMemorySize are good\n";
		}

		// Adding into preallocated memory
		packet.Add<int>(6);
		if(packet.GetMemorySize() != sizeof(int))
		{
			cout << "GetMemorySize or Add is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetMemorySize and Add are good\n";
		}

		if(packet.GetUsedSize() != sizeof(int))
		{
			cout << "GetUsedSize or Add is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetUsedSize and Add are good\n";
		}

		if(packet.GetCursor() != sizeof(int))
		{
			cout << "GetCursor or Add is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetCursor and Add are good\n";
		}

		// Changing memory size
		packet.ChangeMemorySize(sizeof(int)*2);
		if(packet.GetMemorySize() != sizeof(int)*2)
		{
			cout << "GetMemorySize or ChangeMemorySize is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetMemorySize and ChangeMemorySize are good\n";
		}

		if(packet.GetUsedSize() != sizeof(int))
		{
			cout << "GetUsedSize or ChangeMemorySize is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetUsedSize and ChangeMemorySize are good\n";
		}

		if(packet.GetCursor() != sizeof(int))
		{
			cout << "GetCursor or ChangeMemorySize is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetCursor and ChangeMemorySize are good\n";
		}

		// Changing cursor
		packet.SetCursor(0);
		if(packet.GetMemorySize() != sizeof(int)*2)
		{
			cout << "GetMemorySize or SetCursor is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetMemorySize and SetCursor are good\n";
		}

		if(packet.GetUsedSize() != sizeof(int))
		{
			cout << "GetUsedSize or SetCursor is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetUsedSize and SetCursor are good\n";
		}

		if(packet.GetCursor() != 0)
		{
			cout << "GetCursor or SetCursor is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetCursor and SetCursor are good\n";
		}

		// Getting data
		int result = packet.Get<int>();
	
		if(packet.GetCursor() != sizeof(int))
		{
			cout << "GetCursor or Get is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetCursor and Get are good\n";
		}

		if(result != 6)
		{
			cout << "Add or Get is bad\n";
			problem = true;
		}
		else
		{
			cout << "Add and Get are good\n";
		}

		// Adding into unallocated data
		packet.Add<int>(500);
		packet.Add<int>(600);
		if(packet.GetMemorySize() != sizeof(int)*3)
		{
			cout << "GetMemorySize or Add is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetMemorySize and Add are good\n";
		}

		if(packet.GetUsedSize() != sizeof(int)*3)
		{
			cout << "GetUsedSize or Add is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetUsedSize and Add are good\n";
		}

		if(packet.GetCursor() != sizeof(int)*3)
		{
			cout << "GetCursor or Add is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetCursor and Add are good\n";
		}

		packet.SetCursor(sizeof(int));
		int result1 = packet.Get<int>();
		int result2 = packet.Get<int>();

		if(result1 != 500 && result2 != 600)
		{
			cout << "Get or Add (unallocated) is bad\n";
			problem = true;
		}
		else
		{
			cout << "Get and Add (unallocated) are good\n";
		}

		// Get going out of bounds
		bool exceptionOccurred = false;
		try
		{
			packet.Get<int>();
			exceptionOccurred = true;
		}
		catch (ErrorReport & error)
		{
			cout << "Exception correctly occurred while going out of bounds in packet\n";
			exceptionOccurred = true;
		}

		if(exceptionOccurred == false)
		{
			cout << "Exception while going out of bounds in packet did not occur; this is bad\n";
		}

		// Clear
		packet.Clear();
		if(packet.GetUsedSize() != 0)
		{
			cout << "GetUsedSize or Clear is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetUsedSize and Clear are good\n";
		}

		// AddStringC and GetStringC with prefix, no length specified
		packet.AddStringC("hello world",0,true);

		packet.SetCursor(0);
		char * str = packet.GetStringC(0,1);

		if(strcmp(str,"hello world") != 0)
		{
			cout << "AddStringC or GetStringC (with prefix, no length specified) is bad\n";
			problem = true;
		}
		else
		{
			cout << "AddStringC and GetStringC (with prefix, no length specified) is good\n";
		}

		delete[] str;
	}

	{
		Packet packet;

		// AddStringC and GetStringC without prefix, length specified, preallocated destination
		packet.AddStringC("hello world",5,false);

		char * str = new char[10];

		packet.SetCursor(0);
		packet.GetStringC(str,5,true);

		if(strcmp(str,"hello") != 0)
		{
			cout << "AddStringC or GetStringC (with prefix, no length specified, preallocated destination) is bad\n";
			problem = true;
		}
		else
		{
			cout << "AddStringC and GetStringC (with prefix, no length specified, preallocated destination) is good\n";
		}

		delete str;
	}

	{
		Packet packet;

		// GetStringSize
		packet.AddStringC("roflcopter",0,true);

		packet.SetCursor(0);

		if(packet.GetStringSize() != 10)
		{
			cout << "GetStringSize is bad\n";
			problem = true;
		}
		else
		{
			cout << "GetStringSize is good\n";
		}
	}
	
	{
		Packet packet;

		// Simulating AddSizeT on 32 bit and GetSizeT on 64 bit (make sure compiled for 32bit).
		// It is assumed that other methods that work in the same way will work.
		size_t addMe = 6000;
		packet.AddSizeT(addMe);
		packet.SetCursor(0);
		unsigned long long int result = packet.Get<unsigned long long int>();

		if(result != addMe)
		{
			cout << "AddSizeT is bad with result of " << result << "\n";
			problem = true;
		}
		else
		{
			cout << "AddSizeT is good\n";
		}

		if(packet.GetCursor() != Utility::LargestSupportedBytesInt)
		{
			cout << "AddSizeT cursor change is bad with result of " << packet.GetCursor() << '\n';
			problem = true;
		}
		else
		{
			cout << "AddSizeT cursor change is good\n";
		}

		packet.Clear();

		// Simulating AddSizeT on 64 bit and GetSizeT on 32 bit (make sure compiled for 32 bit).
		// It is assumed that other methods that work in the same way will work.
		unsigned long long int addMe2 = 5000;
		packet.Add(addMe2);
		packet.SetCursor(0);
		size_t result4 = packet.GetSizeT();

		if(result4 != addMe2)
		{
			cout << "GetSizeT is bad with result of " << result4 << '\n';
			problem = true;
		}
		else
		{
			cout << "GetSizeT is good\n";
		}

		if(packet.GetCursor() != Utility::LargestSupportedBytesInt)
		{
			cout << "GetSizeT cursor change is bad with result of " << packet.GetCursor() << '\n';
			problem = true;
		}
		else
		{
			cout << "GetSizeT cursor change is good\n";
		}
	}

	{
		Packet packet;

		// Erase
		packet.AddStringC("hello everyone, this is a very good API!",0,false);

		packet.Erase(26,5);	// Erase the word 'very'

		packet.SetCursor(0);

		char * str = packet.GetStringC(packet.GetUsedSize(),true);

		if(strcmp(str,"hello everyone, this is a good API!") != 0)
		{
			cout << "Erase is bad with result of \'" << str << "\'\n";
			problem = true;
		}
		else
		{
			cout << "Erase is good\n";
		}

		delete[] str;
		
		// Insert
		packet.SetCursor(25);
		packet.Insert(11);
		packet.AddStringC("n amazingly",0,false);

		packet.SetCursor(0);
		str = packet.GetStringC(packet.GetUsedSize(),true);

		if(strcmp(str,"hello everyone, this is an amazingly good API!") != 0)
		{
			cout << "Insert is bad with result of \'" << str << "\'\n";
			problem = true;
		}
		else
		{
			cout << "Insert is good\n";
		}

		delete[] str;
	}

	{
		// Packet + operator and C string constructor
		Packet packet1("hello ");
		Packet packet2("world");

		Packet packetResult = packet1 + packet2;

		packetResult.SetCursor(0);
		char * str = packetResult.GetStringC(packetResult.GetUsedSize(),true);

		if(strcmp(str,"hello world") != 0 || packetResult.GetUsedSize() != 11)
		{
			cout << "+ operator or C string constructor is bad\n";
			problem = true;
		}
		else
		{
			cout << "+ operator and C string constructor are good\n";
		}

		delete[] str;
	
		// Packet == operator and != operator
		Packet packetResultCopy(packetResult);
		if(packetResult != packetResultCopy || packetResult == packet1)
		{
			cout << "== or != operator with packet is bad\n";
			problem = true;
		}
		else
		{
			cout << "== and != operator with packet are good\n";
		}

		// C string == operator and != operator
		if(packetResult != "hello world" || packetResult == "lolocopter")
		{
			cout << "== or != operator with C string is bad\n";
			problem = true;
		}
		else
		{
			cout << "== and != operator with C string are good\n";
		}

		// Packet copy constructor and packet += operator
		Packet packetResult2(packet1);
		packetResult2 += packet2;
		if(packetResult2 != packetResult)
		{
			cout << "Copy constructor with packet or += operator with packet is bad\n";
			problem = true;
		}
		else
		{
			cout << "Copy constructor with packet and += operator with packet are good\n";
		}
	}

	// equalWSABUF
	{
		WSABUF buf;
		buf.len = 11; // I am not including the null terminator in this.
		buf.buf = "hello world";

		Packet wsaBufPacket;
		wsaBufPacket.equalWSABUF(buf,5,6);

		if(wsaBufPacket != "world")
		{
			cout << "equalWSABUF is bad\n";
			problem = true;
		}
		else
		{
			cout << "equalWSABUF is good\n";
		}

		// addEqualWSABUF
		wsaBufPacket.addEqualWSABUF(buf,5);
		if(wsaBufPacket != "worldhello")
		{
			cout << "addEqualWSABUF is bad\n";
			problem = true;
		}
		else
		{
			cout << "addEqualWSABUF is good\n";
		}

		// compareWSABUF
		WSABUF buf2;
		buf2.len = 10;
		buf2.buf = "worldhello";
		if(wsaBufPacket.compareWSABUF(buf2,buf2.len) == false || wsaBufPacket.compareWSABUF(buf,buf.len) == true)
		{
			cout << "compareWSABUF is bad\n";
			problem = true;
		}
		else
		{
			cout << "compareWSABUF is good\n";
		}

		// addWSABUF
		wsaBufPacket = wsaBufPacket.addWSABUF(buf,5);
		if(wsaBufPacket != "worldhellohello")
		{
			cout << "addWSABUF is bad\n";
			problem = true;
		}
		else
		{
			cout << "addWSABUF is good\n";
		}
	}


	// Find
	{
		Packet haystack("Once upon a time, there was a boy names James, he said to his best friend Chris that he liked to say hello, but sometimes preferred to say goodbye! Chris and James will always be best friends, because they are both very good people[");
		Packet needle("hello");

		size_t position = 0;
		bool found = haystack.Find(0,0,needle,&position);
	
		if(found == false || position != 101)
		{
			haystack.SetCursor(position);
			char * str = haystack.GetStringC(needle.GetUsedSize(),true);
			cout << "Find is bad: " << str << "\n";
			delete[] str;
			problem = true;
		}
		else
		{
			cout << "Find is good\n";
		}

		needle = "[";
		position = 0;
		found = haystack.Find(haystack.GetUsedSize()-1,0,needle,&position);

		if(found == false || position != haystack.GetUsedSize()-1)
		{
			haystack.SetCursor(position);
			char * str = haystack.GetStringC(needle.GetUsedSize(),true);
			cout << "Find is bad: " << str << "\n";
			delete[] str;
			problem = true;
		}
		else
		{
			cout << "Find is good\n";
		}	
	
	
		haystack = "hello";
		needle = "hello";
		position = 0;
		found = haystack.Find(0,0,needle,&position);

		if(found == false || position != 0)
		{
			haystack.SetCursor(position);
			char * str = haystack.GetStringC(needle.GetUsedSize(),true);
			cout << "Find is bad: " << str << "\n";
			delete[] str;
			problem = true;
		}
		else
		{
			cout << "Find is good\n";
		}
	
		haystack = "o";
		needle = "o";
		position = 0;
		found = haystack.Find(0,0,needle,&position);

		if(found == false || position != 0)
		{
			haystack.SetCursor(position);
			char * str = haystack.GetStringC(needle.GetUsedSize(),true);
			cout << "Find is bad: " << str << "\n";
			delete[] str;
			problem = true;
		}
		else
		{
			cout << "Find is good\n";
		}

		needle = "i";
		found = haystack.Find(0,0,needle,&position);

		if(found == true)
		{
			haystack.SetCursor(position);
			char * str = haystack.GetStringC(needle.GetUsedSize(),true);
			cout << "Find is bad: " << str << "\n";
			delete[] str;
			problem = true;
		}
		else
		{
			cout << "Find is good\n";
		}	

		needle = "ii";
		found = haystack.Find(0,0,needle,&position);

		if(found == true)
		{
			haystack.SetCursor(position);
			char * str = haystack.GetStringC(needle.GetUsedSize(),true);
			cout << "Find is bad: " << str << "\n";
			delete[] str;
			problem = true;
		}
		else
		{
			cout << "Find is good\n";
		}	
	}

	{
		char * hexStr = "2b7e151628aed2a6";
		Packet hex;
		hex.AddHex(hexStr);
		char * result = hex.GetHex(0,hex.GetUsedSize());

		if(strcmp(hexStr,result) != 0)
		{
			cout << "AddHex or GetHex is bad\n";
			problem = true;
		}
		else
		{
			cout << "AddHex and GetHex are good\n";
		}

		delete[] result;
	}
	cout << "\n\n";
	return !problem;
}

/**
 * @brief	Modifies the packet so that its data is a NULL terminated string.
 *
 * This allows the Packet class to be used as a string class.
 *
 * @return a mutable pointer to Packet::data which is NULL terminated.
 */
char * Packet::GetNullTerminated()
{
	char * returnMe = NULL;
	Enter();
		if(IsNullTerminated() == false)
		{
			size_t originalCursor = GetCursor();
			this->SetCursor(GetUsedSize());
			this->Add('\0');
			this->SetCursor(originalCursor);
		}
		returnMe = data;
	Leave();
	return returnMe;
}

/**
 * @brief	Throws an exception if the contents of the packet is not NULL terminated.
 *
 * @return	a constant pointer to Packet::data which is NULL terminated.
 *
 * @throws clError If the contents of the packet are not NULL terminated.
 */
const char * Packet::GetNullTerminated() const
{
	char * returnMe = NULL;

	Enter();
		if(GetUsedSize() == 0)
		{
			returnMe = "";
		}
		else
		{
			if(IsNullTerminated() == true)
			{
				returnMe = data;
			}
			else
			{
				// This is an unfortunate situation to be. We have to violate const correctness to
				// make this work! Sorry to anyone impacted by this :(
				Packet * mThis = const_cast<Packet*>(this);
				returnMe = mThis->GetNullTerminated();
			}
		}
	Leave();

	// This was the original fall back option, before I violated const correctness. However, it lead to 
	// hard to pickup bugs as one wouldn't normally expect this behavior.
	_ErrorException((returnMe == NULL),"retrieving a null terminated representation of a packet, data is not NULL terminated",0,__LINE__,__FILE__);
	return returnMe;
}

/**
 * @brief	Determine whether the data of the packet is null terminated.
 *
 * This method checks the last byte of the packet to see if it is null terminated.
 *
 * @return	true if null terminated, false if not. 
 */
bool Packet::IsNullTerminated() const
{
	return GetUsedSize() > 0 && data[GetUsedSize()-1] == '\0';
}

/**
 * @brief	Outputs packet contents as a C string using cout.
 */
void Packet::OutputContents()
{
	size_t originalCursor = GetCursor();
	SetCursor(0);
	char * contents = GetStringC(GetUsedSize(),true);
	cout << contents << '\n';
	delete[] contents;
	SetCursor(originalCursor);
}

/**
 * @brief	Adds a NULL terminated string of hexadecimal e.g. 2b7e151628aed2a6 to the packet.
 *
 * @param	hex	NULL terminated hexadecimal string.
 */
void Packet::AddHex( const char * hex )
{
	Enter();
	try
	{
		StoreVector<unsigned char> result;
		Utility::ConvertFromHexToInt(hex,result);

		for(size_t n = 0;n<result.Size();n++)
		{
			Add(result[n]);
		}
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
}

/**
 * @brief	Retrieves a NULL terminated string of hexadecimal e.g. 2b7e151628aed2a6 from the packet.
 *
 * @param	startPos	The position to start retrieving from.
 * @param	numBytes	Number of bytes to retrieve.
 *
 * @return	hexadecimal string which should be deallocated using delete[]. 
 */
char * Packet::GetHex( size_t startPos, size_t numBytes ) const
{
	char * returnMe = NULL;
	Enter();
	try
	{
		_ErrorException((startPos + numBytes > GetUsedSize()),"retrieving a hexadecimal string from a packet, end position is out of bounds",0,__LINE__,__FILE__);
		returnMe = Utility::ConvertFromIntToHex((const unsigned char*)data + startPos,numBytes);
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){Leave();	throw(Error);}
	catch(...){Leave();	throw(-1);}

	Leave();
	return returnMe;
}

/**
 * @brief	Does not return until the last encryption operation has finished.
 */
void Packet::WaitUntilLastEncryptionOperationFinished()
{
	WaitUntilLastThreadOperationFinished();
}

/**
 * @brief	Determines whether the last encryption operation has finished.
 *
 * @return	true if last encryption operation finished, false if not.
 */
bool Packet::IsLastEncryptionOperationFinished()
{
	return IsLastThreadOperationFinished();
}