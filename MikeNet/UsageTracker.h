#pragma once

/**
 * @brief	Keeps track of an object's usage.
 *
 * This was designed for keeping track of memory used by shallow
 * copies of an object, so that memory is only deallocated once.
 *
 * Methods are constant so that the extending class may increment
 * usage for constant objects too.
 *
 * Not thread safe on the grounds that thread safe objects
 * should use their own critical sections to encapsulate
 * these methods.
 *
 * @author	Michael Pryor
 * @date	13/04/2011.
 */
class UsageTracker
{
	mutable size_t usageCount;
public:
	UsageTracker(void);
	virtual ~UsageTracker(void);

	void IncrementUsageCount() const;
	void DecrementUsageCount() const;
	size_t GetUsageCount() const;
};

