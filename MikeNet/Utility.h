#pragma once
#include "CriticalSection.h"
#include "StoreVector.h"
/**
 * @brief	%Utility namespace providing methods for basic and commonly used jobs.
 * @remarks	Michael Pryor, 6/28/2010. 
 */
namespace Utility
{
	/**
	 * @brief When multiple threads what to use @c cout at the same time, they
	 * should use output while using @c cout.
	 */
	extern CriticalSection output;

	/** @brief The version that this project is currently at. */
	extern const char * VERSION;

	/** @brief The names of all significant contributors in order of importance. */
	extern const char * CREDITS;

	/** @brief Largest supported number of bytes in an integer. */
	const unsigned int LargestSupportedBytesInt = sizeof(__int64);
	
	/** @brief Largest supported number of bits in an integer. */
	const unsigned int LargestSupportedBitsInt = LargestSupportedBytesInt * 8;

	/** @brief Hexadecimal lookup table used when converting hexadecimal. */
	extern const char HexadecimalLookupTable[16];

	char * ConcatArray(const char ** textElements, size_t numElements);
	void DisplayErrorMB(const char * messageBoxText);

	void * DynamicAllocCheck(const void * checkMe, size_t line, const char * file);

	char * ConvertFromIntToString(int integer);
	char * ConvertFromIntToString(__int64 integer);

	void ConvertFromHexToInt(const char * string, StoreVector<unsigned char> & outputVector);
	char * ConvertFromIntToHex(const unsigned char * convertMe, size_t length);

	void SplitDouble(double dbl, WORD & integer, WORD & fraction);
	double MakeDouble(WORD integer, WORD fraction);

	size_t Log2(size_t logMe);

	/**
	 * @brief Generates a copy of the specified object.
	 *
	 * @param copyMe Object to copy.
	 * 
	 * @return copy of @a copyMe.
	 */
	template<typename T>
	T * CopyObject(const T & copyMe)
	{
		T * returnMe = new (nothrow) T(copyMe);
		Utility::DynamicAllocCheck(returnMe,__LINE__,__FILE__);
		return returnMe;
	}

	bool TestClass();
};
