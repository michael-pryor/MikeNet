#include "FullInclude.h"
#include "MemoryUsageRestricted.h"

/**
 * @brief	Default constructor.
 *
 * Memory usage limit is defaulted to INFINITE (maximum storable integer).
 */
MemoryUsageRestricted::MemoryUsageRestricted(void)
{
	memoryUsageLimit = INFINITE;
}

/**
 * @brief	Constructor. 
 *
 * @param	limit	Memory limit of object.
 */
MemoryUsageRestricted::MemoryUsageRestricted(size_t limit)
{
	memoryUsageLimit = limit;
}

/**
 * @brief	Deep copy constructor.
 *
 * @param	copyMe	Object to copy.
 */
MemoryUsageRestricted::MemoryUsageRestricted( const MemoryUsageRestricted & copyMe )
	: MemoryUsage(copyMe)
{
	copyMe.Enter();
	this->memoryUsageLimit = copyMe.memoryUsageLimit;
	copyMe.Leave();
}

/**
 * @brief	Deep assignment operator. 
 *
 * @param	copyMe	Object to copy. 
 *
 * @return	a reference to this object.
 */
MemoryUsageRestricted & MemoryUsageRestricted::operator=( const MemoryUsageRestricted & copyMe )
{
	MemoryUsage::operator=(copyMe);
	Enter();
	copyMe.Enter();
		this->memoryUsageLimit = copyMe.memoryUsageLimit;
	copyMe.Leave();
	Leave();
	return *this;
}
/**
 * @brief	Destructor. 
 */
MemoryUsageRestricted::~MemoryUsageRestricted(void)
{
}


/**
 * @brief	Changes the memory bloat limit (default is INFINITE). 
 *
 * @param	limit	The new memory bloat limit. 
 */
void MemoryUsageRestricted::SetMemoryLimit( size_t limit )
{
	Enter();
	try
	{
		memoryUsageLimit = limit;
		this->EnforceMemoryLimit(); // Memory limit may have decreased.
	}
	catch(ErrorReport & report) {Leave(); throw report;}
	catch(...) {Leave(); throw(-1);}
	Leave();
}

/**
 * @brief	Retrieves the memory bloat limit (default is INFINITE). 
 *
 * @return	the memory bloat limit. 
 */
size_t MemoryUsageRestricted::GetMemoryLimit() const
{
	Enter();
	size_t returnMe = memoryUsageLimit;
	Leave();
	return returnMe;
}

/**
 * @brief Throws an exception if the desired memory size is too high.
 *
 * @param desiredSize Desired memory size.
 */
void MemoryUsageRestricted::EnforceMemoryLimit(size_t desiredSize) const
{
	_ErrorException((desiredSize > this->GetMemoryLimit()),"enforcing memory limit, memory usage is too high for this object",0,__LINE__,__FILE__);
}

/**
 * @brief Throws an exception if the current memory size is too high.
 */
void MemoryUsageRestricted::EnforceMemoryLimit() const
{
	_ErrorException((this->GetMemorySize() > this->GetMemoryLimit()),"enforcing memory limit, memory usage is too high for this object",0,__LINE__,__FILE__);
}

/**
 * @brief Throws an exception if the desired memory size after an increase is too high.
 *
 * @param desiredIncrease Desired increase in memory size.
 */
void MemoryUsageRestricted::EnforceMemoryLimitIncrease( size_t desiredIncrease ) const
{
	EnforceMemoryLimit(this->GetMemorySize() + desiredIncrease);
}
