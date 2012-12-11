#pragma once
#include "Packet.h"


/**
 * @brief	Manages a COM string (BSTR).
 * @remarks	Michael Pryor, 9/16/2010. 
 */
class ComString
{
	/**
	 * @brief COM string to manage.
	 */
	BSTR comString;

	void Allocate(size_t length);
	void Allocate(BSTR copyString);
	void Allocate(const char * str);
public:
	void Cleanup();
	bool IsLoaded() const;
	size_t GetLength() const;
	void CopyIntoPacket(Packet & packet) const;

	ComString();

	ComString(const ComString & comStr);
	ComString & operator=(const ComString & comStr);

	ComString(const char * str);
	ComString & operator=(const char * str);

	BSTR GetBSTR();
	BSTR * GetPtrBSTR();
	

	virtual ~ComString();
};

