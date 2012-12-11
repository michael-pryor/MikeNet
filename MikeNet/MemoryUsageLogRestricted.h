#pragma once
#include "MemoryUsageLog.h"
#include "MemoryUsageRestricted.h"

/**
 * @brief	Keeps a running total of memory usage and throws an exception when usage
 * exceeds the defined limit.
 *
 * @author	Michael Pryor
 * @date	07/01/2011.
 */
class MemoryUsageLogRestricted : public MemoryUsageLog, public MemoryUsageRestricted
{
public:
	MemoryUsageLogRestricted(size_t limit = INFINITE);
	MemoryUsageLogRestricted(const MemoryUsageLogRestricted & copyMe);
	MemoryUsageLogRestricted & operator=(const MemoryUsageLogRestricted & copyMe);
	virtual ~MemoryUsageLogRestricted(void);

	void IncreaseMemorySize(size_t amount);
	void DecreaseMemorySize(size_t amount);

	void SetMemorySize(size_t size);
};

