#pragma once

/**
 * @brief Interface for objects who know their memory usage, or an estimate of this.
 */
class MemoryUsage
{
public:

	/**
	 * @brief	Retrieves the amount of memory in use by, or being logged by this object.
	 *
	 * @return	The amount of memory in use by this object.
	 */
	virtual size_t GetMemorySize() const=0;
};