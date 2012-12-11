#pragma once
#include "Comparator.h"

/**
 * @brief	Sorts NetAddress objects.
 *
 * @author	Michael Pryor
 * @date	17/04/2011.
 */
class ComparatorNetAddress :
	public Comparator
{
public:
	ComparatorNetAddress(void);
	virtual ~ComparatorNetAddress(void);

	__int64 GetWeight(const void * sortableObject) const;
};

