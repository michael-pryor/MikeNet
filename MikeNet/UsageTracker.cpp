#include "FullInclude.h"
#include "UsageTracker.h"

/**
 * @brief	Default constructor. 
 */
UsageTracker::UsageTracker(void)
{
	usageCount = 1;
}

/**
 * @brief	Destructor. 
 */
UsageTracker::~UsageTracker(void)
{
}

/**
 * @brief	Increments the usage count.
 */
void UsageTracker::IncrementUsageCount() const
{
	usageCount++;
}

/**
 * @brief	Decrements the usage count. 
 */
void UsageTracker::DecrementUsageCount() const
{
	usageCount--;
}

/**
 * @brief	Retrieves the usage count.
 *
 * @return	the usage count. 
 */
size_t UsageTracker::GetUsageCount() const
{
	return usageCount;
}
