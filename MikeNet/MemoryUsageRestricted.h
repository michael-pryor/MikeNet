#pragma once
#include "memoryusage.h"
#include "CriticalSection.h"

/**
 * @brief	Restricts memory usage of objects which know their memory usage.
 *
 * @author	Michael Pryor
 * @date	09/04/2011.
 */
class MemoryUsageRestricted :
	public virtual MemoryUsage, public virtual CriticalSection
{
	/**
	 * @brief Maximum amount of bloat that is allowed.
	 *
	 * IncreaseMemorySize and DecreaseMemorySize will throw an exception if this is exceeded.
	 */
	size_t memoryUsageLimit;

public:
	MemoryUsageRestricted(void);
	MemoryUsageRestricted(size_t limit);
	MemoryUsageRestricted(const MemoryUsageRestricted & copyMe);
	MemoryUsageRestricted & operator=(const MemoryUsageRestricted & copyMe);
	virtual ~MemoryUsageRestricted(void);

	void SetMemoryLimit(size_t limit);
	size_t GetMemoryLimit() const;

	void EnforceMemoryLimit(size_t desiredSize) const;
	void EnforceMemoryLimit() const;
	void EnforceMemoryLimitIncrease(size_t desiredIncrease) const;
};

