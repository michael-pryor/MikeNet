#include "FullInclude.h"
#include "ComString.h"
#include "Packet.h"

/**
 * @brief	Deep copy constructor.
 *
 * @param	comStr	COM string to copy.
 */
ComString::ComString(const ComString & comStr)
{
	this->comString = NULL;
	Allocate(comStr.comString);
}

/**
 * @brief	Deep copy constructor. 
 *
 * @param	str	NULL terminated string to copy.
 */
ComString::ComString( const char * str )
{
	this->comString = NULL;
	Allocate(str);
}

/**
 * @brief	Default constructor. 
 */
ComString::ComString()
{
	this->comString = NULL;
}

/**
 * @brief	Deep assignment operator.
 *
 * @param	comStr	COM string to copy. 
 *
 * @return	a reference to this object.
 */
ComString & ComString::operator=( const ComString & comStr )
{
	Allocate(comStr.comString);
	return *this;
}

/**
 * @brief	Deep assignment operator.
 *
 * @param	str	NULL terminated C string to copy. 
 *
 * @return	a reference to this object.
 */
ComString & ComString::operator=( const char * str )
{
	Allocate(str);
	return *this;
}

/**
 * @brief	Destructor. 
 */
ComString::~ComString()
{
	Cleanup();
}

/**
 * @brief	Allocates memory to COM string.
 *
 * @param	length	The amount of memory to allocate. 
 */
void ComString::Allocate( size_t length )
{
	Cleanup();

	comString = SysAllocStringLen(NULL,static_cast<UINT>(length));
	_ErrorException((comString == NULL),"allocating memory to a COM string, not enough memory available",0,__LINE__,__FILE__);
}

/**
 * @brief	Copies a BSTR.
 *
 * @param	copyString	The string to copy. 
 */
void ComString::Allocate( BSTR copyString )
{
	Cleanup();

	if(copyString != NULL)
	{
		comString = SysAllocString(copyString);
		_ErrorException((comString == NULL),"allocating memory to a COM string while copying, not enough memory available",0,__LINE__,__FILE__);
	}
	else
	{
		copyString = NULL;
	}
}

/**
 * @brief	Copies a C string.
 *
 * @param	str	The NULL terminated C string. 
 */
void ComString::Allocate( const char * str )
{
	Cleanup();

	size_t length = strlen(str);
	Allocate(length);

	if(length > 0)
	{
		int iResult = MultiByteToWideChar(CP_ACP,MB_COMPOSITE,str,static_cast<int>(length),comString,static_cast<int>(length));
		_ErrorException((iResult==NULL), "converting from CSTR to BSTR", WSAGetLastError(), __LINE__,__FILE__);
	}
}

/**
 * @brief	Cleans up the COM string.
 */
void ComString::Cleanup()
{
	if(comString != NULL)
	{
		SysFreeString(comString);
		comString = NULL;
	}
}

/**
 * @brief	Determines the length of the string.
 *
 * @return	the length of the string.
 */
size_t ComString::GetLength() const
{
	// Works fine if comString is NULL.
	return SysStringLen(comString);
}

/**
 * @brief	Copies the string into the specified packet, overwriting any existing contents.
 *
 * A null terminator is appended to the string stored within the packet.
 *
 * @param [out]	packet	Packet to copy into.
 */
void ComString::CopyIntoPacket( Packet & packet ) const
{
	size_t length = GetLength();

	packet.Enter();
		// +1 leaves space for NULL terminator.
		packet.SetMemorySize(length+1);
		packet.SetUsedSize(length+1);
		char * packetDataPtr = packet.GetDataPtr();

		if(length > 0)
		{
			int result = WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,comString,static_cast<int>(length),packetDataPtr,static_cast<int>(length),NULL,NULL);
			_ErrorException((result == NULL), "converting from BSTR to CSTR", WSAGetLastError(), __LINE__,__FILE__);
		}
	
		// Null terminator
		packetDataPtr[length] = '\0';
	packet.Leave();
}

/**
 * @brief	Returns a pointer to the BSTR that this object is managing.
 *
 * @return	pointer to string.
 */
BSTR * ComString::GetPtrBSTR()
{
	return &comString;
}

/**
 * @brief	Determines whether the com string is loaded with anything (i.e. is not NULL).
 *
 * Note that IsLoaded may return true even if the string length is 0, since a BSTR
 * can be non NULL but empty.
 *
 * @return	true if loaded, false if not. 
 */
bool ComString::IsLoaded() const
{
	return comString != NULL;
}

/**
 * @brief	Returns the BSTR that this object is managing.
 *
 * @return	string.
 */
BSTR ComString::GetBSTR()
{
	return comString;
}

