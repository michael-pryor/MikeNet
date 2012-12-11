#include "FullInclude.h"
#include "MemoryUsageLogRestricted.h"

/**
 * @brief	Constructor. 
 *
 * @param	limit	The initial memory bloat limit (optional, default = INFINITE).
 */
MemoryUsageLogRestricted::MemoryUsageLogRestricted(size_t limit)
{
	SetMemoryLimit(limit);
}


/**
 * @brief	Deep copy constructor.
 *
 * @param	copyMe	Object to copy.
 */
MemoryUsageLogRestricted::MemoryUsageLogRestricted( const MemoryUsageLogRestricted & copyMe ) :
		MemoryUsageLog(copyMe),
		MemoryUsageRestricted(copyMe)
{
	
}

/**
 * @brief	Deep assignment operator. 
 *
 * @param	copyMe	Object to copy. 
 *
 * @return	a reference to this object.
 */
MemoryUsageLogRestricted & MemoryUsageLogRestricted::operator=( const MemoryUsageLogRestricted & copyMe )
{
	MemoryUsageLog::operator=(copyMe);
	MemoryUsageRestricted::operator=(copyMe);
	return *this;
}


/**
 * @brief	Destructor. 
 */
MemoryUsageLogRestricted::~MemoryUsageLogRestricted(void)
{
}

/**
 * @brief	Increases the memory usage of the object, checking that it doesn't go above the limit.
 *
 * @param	amount	The amount to increase by.
 */
void MemoryUsageLogRestricted::IncreaseMemorySize( size_t amount )
{
	Enter();
	try
	{
		EnforceMemoryLimit(GetNewMemorySizeAfterIncrease(amount));
		
		// Checks for overflow.
		MemoryUsageLog::IncreaseMemorySize(amount);
	}
	catch(ErrorReport & report) {Leave(); throw report;}
	catch(...) {Leave(); throw(-1);}
	Leave();
}

/**
 * @brief	Decreases the memory usage of the object, checking that it doesn't go below the limit.
 *
 * @param	amount	The amount to decrease by.
 */
void MemoryUsageLogRestricted::DecreaseMemorySize( size_t amount )
{
	Enter();
	try
	{
		// Checks for overflow.
		MemoryUsageLog::DecreaseMemorySize(amount);
	}
	catch(ErrorReport & report) {Leave(); throw report;}
	Leave();
}

/**
 * @brief Changes the amount of memory logged to be in use by this object, checking that the new
 * value is not too high.
 *
 * @param newSize The new size.
 */
void MemoryUsageLogRestricted::SetMemorySize(size_t newSize)
{
	Enter();
	try
	{
		EnforceMemoryLimit(newSize);
		MemoryUsageLog::SetMemorySize(newSize);
	}
	catch(ErrorReport & report) {Leave(); throw report;}
	Leave();
}
