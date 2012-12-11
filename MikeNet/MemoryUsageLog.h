#pragma once
#include "memoryusage.h"
#include "CriticalSection.h"

/**
 * @brief Keeps a running total of memory usage.
 */
class MemoryUsageLog :
	public virtual MemoryUsage, public virtual CriticalSection
{
	size_t memoryUsage;

public:
	MemoryUsageLog(void);
	MemoryUsageLog(const MemoryUsageLog & copyMe);
	MemoryUsageLog & operator=(const MemoryUsageLog & copyMe);
	virtual ~MemoryUsageLog(void);

	virtual void IncreaseMemorySize(size_t amount);
	virtual void DecreaseMemorySize(size_t amount);

	size_t GetNewMemorySizeAfterIncrease(size_t amount) const;
	size_t GetNewMemorySizeAfterDecrease(size_t amount) const;

	virtual void SetMemorySize(size_t size);

	virtual size_t GetMemorySize() const;
};

