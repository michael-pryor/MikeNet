// Simple functions used throughout MikeNet
#include "FullInclude.h"
#include <sstream>
#include <intsafe.h>

const char * Utility::VERSION = "Release v2.0.2";
const char * Utility::CREDITS = "Michael Pryor";
CriticalSection Utility::output;
const char Utility::HexadecimalLookupTable[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

/**
 * @brief Combines array of C strings together into single string that is returned.
 *
 * @param [in] textElements Array of strings.
 * @param numElements Number of elements in @a textElements.
 *
 * @return single string that should be deallocated with delete[] when no longer needed.
 */
char * Utility::ConcatArray(const char ** textElements, size_t numElements)
{
	char * sFullMessage = NULL;

	// Calculate total size of message
	size_t iFullMessageSize = 0;
	for(size_t n = 0; n<numElements; n++)
	{
		iFullMessageSize +=	strlen(textElements[n]);
	}

	iFullMessageSize++; // For null terminator

	// Allocate memory to store full message
	sFullMessage = new (nothrow) char[iFullMessageSize];
	if(sFullMessage == NULL)
	{
		MessageBox(NULL,"Error whilst error reporting, could not allocate memory to store error message","DarkNet - Unexpected Error",MB_OK | MB_ICONERROR);
	}

	// Combine separate parts of message
	size_t iCursor = 0;
	for(size_t n = 0; n<numElements; n++)
	{
		// strcpy will append null terminator automatically
		strcpy_s(sFullMessage + iCursor, iFullMessageSize - iCursor, textElements[n]);
		iCursor += strlen(textElements[n]);
	}
	
	return(sFullMessage);
}


/**
 * @brief Displays an error message box.
 *
 * @param [in] messageBoxText Message to be displayed.
 */
void Utility::DisplayErrorMB(const char * messageBoxText)
{
	// Setup parts of full message
	const size_t NoParts = 2;
	const char * textElements[NoParts];

	textElements[0] = messageBoxText;
	textElements[1] = ". Would you like to exit?";

	// Combines parts together
	char * sFullMessage = Utility::ConcatArray(textElements, NoParts);

	// Display error message box
	int iResult = MessageBox(NULL, sFullMessage, "DarkNet - Unexpected Error", MB_YESNO | MB_ICONERROR);

	// Exit if user clicked 'Yes'
	if(iResult == IDYES)
	{
		exit(-1);
	}

	// Deallocate memory
	delete[] sFullMessage;
}

/**
 * @brief Checks that memory was successfully allocated to a pointer.
 *
 * @param [in]	checkMe	Object to check. When dynamic allocation fails, this will be NULL.
 * @param		line	Line at which this method was called (use @c __LINE__ macro).
 * @param [in]  file	Where this method was called (use @c __FILE__ macro).
 *
 * @return @a checkMe.
 *
 * @throws ErrorReport If @a checkMe is NULL.
 */
void * Utility::DynamicAllocCheck(const void * checkMe, size_t line, const char * file)
{
	_ErrorException((checkMe==NULL),"dynamically allocating memory",0,line,file);
	return (void*)checkMe;
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool Utility::TestClass()
{
	bool problem = false;
	cout << "Testing Utility class...\n";

	// All methods assumed simple enough not to test except ones below.

	// ConcatArray
	const char * str1 = "hello ";
	const char * str2 = "world, ";
	const char * str3 = "isn't this a good API?";

	const char* arr[] = {str1, str2, str3};

	char * combination = ConcatArray(arr,3);

	if(strcmp(combination, "hello world, isn't this a good API?") != 0)
	{
		cout << "ConcatArray is bad with result of " << combination << '\n';
		problem = true;
	}
	else
	{
		cout << "ConcatArray is good\n";
	}

	delete[] combination;

	// ConvertFromIntToString
	int integerToConvert = 1000;
	char * result = ConvertFromIntToString(integerToConvert);
	cout << "Converting " << integerToConvert << " to string, result: " << result << '\n';
	delete[] result;

	__int64 anotherIntegerToConvert = -9223372036854775807;
	char * result2 = ConvertFromIntToString(anotherIntegerToConvert);
	cout << "Converting " << anotherIntegerToConvert << " to string, result: " << result2 << '\n';
	delete[] result2;

	// ConvertFromHexToInt
	StoreVector<unsigned char> converted;
	ConvertFromHexToInt("2b7e151628aed2a6",converted);

	cout << "Conversion from hex into byte array:\n";
	for(size_t n = 0;n<converted.Size();n++)
	{
		printf("hex: %x\n",converted[n]);
	}

	// ConvertFromIntToHex
	unsigned char convertMe[] = {converted[0], converted[1], converted[2], converted[3], converted[4], converted[5], converted[6], converted[7]};
	char * conversion = ConvertFromIntToHex(convertMe,8);
	cout << "Conversion back into hex string: " << conversion << '\n';
	delete[] conversion;

	// SplitDouble
	WORD integer;
	WORD fraction;
	SplitDouble(5.5,integer,fraction);

	if(integer != 5 || fraction != USHORT_MAX / 2)
	{
		cout << "SplitDouble is bad\n";
		problem = true;
	}
	else
	{
		cout << "SplitDouble is good\n";
	}

	// MakeDouble
	double dblResult = MakeDouble(integer,fraction);
	if(dblResult > 5.6 || dblResult < 5.4)
	{
		cout << "MakeDouble is bad " << dblResult << "\n";
		problem = true;
	}
	else
	{
		cout << "MakeDouble is good " << dblResult << "\n";
	}

	size_t log2_32 = Log2(32);
	if(log2_32 != 5)
	{
		cout << "Log2(32) is bad " << log2_32 << "\n";
		problem = true;
	}
	else
	{
		cout << "Log2(32) is good " << log2_32 << '\n';
	}

	cout << "\n\n";
	return !problem;
}

/**
 * @brief Converts an integer into a NULL terminated C string.
 *
 * @param integer Integer to convert.
 *
 * @return string representation of @a integer.
 * This string must be deallocated using delete[].
 */
char * Utility::ConvertFromIntToString( int integer )
{
	char * returnMe;

	// 11 is the maximum number of digits in an integer of this type
	const size_t size = 11;
	returnMe = new (nothrow) char[size]; 
	Utility::DynamicAllocCheck(returnMe,__LINE__,__FILE__);

	_itoa_s(integer,returnMe,size,10);

	return returnMe;
}

/**
 * @brief Converts an __int64 into a NULL terminated C string.
 *
 * @param integer Integer to convert.
 *
 * @return string representation of @a integer.
 * This string must be deallocated using delete[].
 */
char * Utility::ConvertFromIntToString( __int64 integer )
{
	char * returnMe;

	// 11 is the maximum number of digits in an integer of this type
	const size_t size = 21;
	returnMe = new (nothrow) char[size]; 
	Utility::DynamicAllocCheck(returnMe,__LINE__,__FILE__);

	_i64toa_s(integer,returnMe,size,10);

	return returnMe;
}

/**
 * @brief	Convert from hexadecimal to int. 
 *
 * @param	string					String to convert.
 * @param [in,out]	outputVector	Vector which each hexadecimals is individually stored in. 
 */
void Utility::ConvertFromHexToInt(const char * string, StoreVector<unsigned char> & outputVector)
{
	_ErrorException((string == NULL),"converting hexadecimal to integer, input string is NULL",0,__LINE__,__FILE__);

	size_t strLength = strlen(string);
	_ErrorException((strLength % 2 != 0),"converting hexadecimal to integer, invalid input string",0,__LINE__,__FILE__);

	size_t numElements = strLength / 2;

	// Allocate memory for the output array
	outputVector.ResizeAllocate(numElements);

	// Populate output array
	std::stringstream sstr(string);
	for(size_t n=0; n < numElements; n++)
	{
		char ch1, ch2;
		sstr >> ch1 >> ch2;

		int dig1, dig2;
		if(isdigit(ch1) != 0)
		{
			dig1 = ch1 - '0';
		}
		else
		{
			if(ch1>='A' && ch1<='F')
			{
				dig1 = ch1 - 'A' + 10;
			}
			else
			{
				if(ch1>='a' && ch1<='f')
				{
					dig1 = ch1 - 'a' + 10;
				}
			}
		}

		if(isdigit(ch2) != 0)
		{
			dig2 = ch2 - '0';
		}
		else
		{
			if(ch2>='A' && ch2<='F')
			{
				dig2 = ch2 - 'A' + 10;
			}
			else
			{
				if(ch2>='a' && ch2<='f')
				{
					dig2 = ch2 - 'a' + 10;
				}
			}
		}

		outputVector[n] = dig1*16 + dig2;
	}
}

/**
 * @brief	Convert from int to hexadecimal. 
 *
 * @param  convertMe			The string to convert.
 * @param	length				The length. 
 *
 * @return	string representation of hexadecimal, needs to be deallocated using delete[].
 */
char * Utility::ConvertFromIntToHex( const unsigned char * convertMe, size_t length)
{
	_ErrorException((convertMe == NULL),"converting integer to hexadecimal, input string is NULL",0,__LINE__,__FILE__);

	char * returnMe = new (nothrow) char[length*2+1];
	returnMe[length*2] = '\0';
	Utility::DynamicAllocCheck(returnMe,__LINE__,__FILE__);

	for(size_t j = 0; j < length; j++)
	{
		returnMe[j*2] = HexadecimalLookupTable[((convertMe[j] >> 4) & 0xF)];
		returnMe[(j*2) + 1] = HexadecimalLookupTable[(convertMe[j]) & 0x0F];
	}

	return returnMe;
}

/**
 * @brief	Splits a double into its integer and fraction parts.
 *
 * @param	dbl					The double to split. 
 * @param [out]	integer			Destination to store integer part. If @a dbl is 15.5 this will be 15. 
 * @param [out]	fraction		Destination to store fractional part. If @a dbl is 15.5 this will be set to USHORT_MAX / 2.
 * If @a dbl is 15.25 this will be set to USHORT_MAX / 4.
 */
void Utility::SplitDouble(double dbl, WORD & integer, WORD & fraction)
{
	double floored = floor(dbl);
	double dblFraction = dbl - floored;
	double dblMax = USHORT_MAX;

	fraction = static_cast<WORD>(dblMax * dblFraction);
	integer = static_cast<WORD>(floored);
}

/**
 * @brief	Creates a double by combining two words describing a fraction.
 *
 * @param integer			Destination to store integer part. If return value is 15.5 this will be 15. 
 * @param fraction			Destination to store fractional part. If return value is 15.5 this will be USHORT_MAX / 2.
 * If return value is 15.25 this will be USHORT_MAX / 4.
 * @return double representation of @a integer and @a fraction.
 */
double Utility::MakeDouble( WORD integer, WORD fraction )
{
	double returnMe = integer;
	
	double dblFraction = fraction;
	double dblMax = USHORT_MAX;

	double result = dblFraction / dblMax;

	returnMe = integer + result;
	return returnMe;
}

/**
 * @brief	Retrieves the logarithm (base 2) of the specified number.
 *
 * @param	logMe	Number to use.
 *
 * @return	logarithm (base 2) of number.
 */
size_t Utility::Log2( size_t logMe )
{
	size_t log = 0;

	while(logMe > 1)
	{
		logMe /= 2;
		log++;
	}

	return log;	
}
