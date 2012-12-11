#include "FullInclude.h"
#include "MemoryUsageLog.h"

/**
 * @brief	Default constructor. 
 */
MemoryUsageLog::MemoryUsageLog(void)
{
	this->memoryUsage = 0;
}

/**
 * @brief	Deep copy constructor.
 *
 * @param	copyMe	Object to copy.
 */
MemoryUsageLog::MemoryUsageLog( const MemoryUsageLog & copyMe )
{
	this->memoryUsage = copyMe.memoryUsage;
}

/**
 * @brief	Deep assignment operator. 
 *
 * @param	copyMe	Object to copy. 
 *
 * @return	a reference to this object.
 */
MemoryUsageLog & MemoryUsageLog::operator=( const MemoryUsageLog & copyMe )
{
	this->memoryUsage = copyMe.memoryUsage;
	return *this;
}


/**
 * @brief	Destructor. 
 */
MemoryUsageLog::~MemoryUsageLog(void)
{
}


/**
 * @brief	Increases the amount of memory usage logged.
 *
 * @throws ErrorReport	If integer overflow will occur.
 *
 * @param	amount	The amount to increase memory usage logged by.
 */
void MemoryUsageLog::IncreaseMemorySize( size_t amount )
{
	Enter();

	try
	{
		size_t newMemoryUsage = this->GetMemorySize() + amount;
		_ErrorException((amount > 0 && newMemoryUsage <= this->GetMemorySize()),"increasing the logged memory usage of an object, integer overflow will occur",0,__LINE__,__FILE__);

		this->memoryUsage = newMemoryUsage;
	}
	catch(ErrorReport & report) {Leave(); throw report;}
	catch(...) {Leave(); throw(-1);}
	Leave();
}

/**
 * @brief	Decreases the amount of memory memory usage logged.
 *
 * @throws ErrorReport	If integer overflow will occur.
 *
 * @param	amount	The amount to decrease memory usage logged by.
 */
void MemoryUsageLog::DecreaseMemorySize( size_t amount )
{
	Enter();

	try
	{
		size_t newMemoryUsage = this->GetMemorySize() - amount;
		_ErrorException((amount > 0 && newMemoryUsage >= this->GetMemorySize()),"decreasing the logged memory usage of an object, integer overflow will occur",0,__LINE__,__FILE__);

		this->memoryUsage = newMemoryUsage;
	}
	catch(ErrorReport & report) {Leave(); throw report;}
	catch(...) {Leave(); throw(-1);}
	Leave();
}

/**
 * @brief	Retrieves the amount of memory in use by this object.
 *
 * @return	the amount of memory in use by this object.
 */
size_t MemoryUsageLog::GetMemorySize() const
{
	Enter();
	size_t returnMe = this->memoryUsage;
	Leave();
	return returnMe;
}

/**
 * @brief	Changes the amount of memory logged as in use by this object.
 *
 * @param	newMemorySize	New memory size.
 */
void MemoryUsageLog::SetMemorySize(size_t newMemorySize)
{
	Enter();
	this->memoryUsage = newMemorySize;
	Leave();
}

/**
 * @brief	Retrieves the new memory size after an increase, without 
 * actually changing the memory size.
 *
 * @param	amount	The amount to increase by.
 *
 * @return	the new memory size after increase. 
 */
size_t MemoryUsageLog::GetNewMemorySizeAfterIncrease( size_t amount ) const
{
	size_t returnMe;

	Enter();
		returnMe = GetMemorySize() + amount;
	Leave();

	return returnMe;
}

/**
 * @brief	Retrieves the new memory size after an decrease, without 
 * actually changing the memory size.
 *
 * @param	amount	The amount to decrease by.
 *
 * @return	the new memory size after decrease. 
 */
size_t MemoryUsageLog::GetNewMemorySizeAfterDecrease( size_t amount ) const
{
	size_t returnMe;

	Enter();
		returnMe = GetMemorySize() - amount;
	Leave();

	return returnMe;
}
