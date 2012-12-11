#pragma once
#include "Comparator.h"

/**
 * @brief	Sorts integers from highest to lowest.
 *
 * @author	Michael Pryor
 * @date	13/04/2011.
 */
class ComparatorInteger :
	public Comparator
{
public:
	ComparatorInteger(void);
	virtual ~ComparatorInteger(void);

private:
	__int64 GetWeight( const void * sortableObject ) const;
};

