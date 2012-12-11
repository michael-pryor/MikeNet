#include "FullInclude.h"
#include "ComparatorInteger.h"

/**
 * @brief	Default constructor. 
 */
ComparatorInteger::ComparatorInteger(void)
{
}

/**
 * @brief	Destructor. 
 */
ComparatorInteger::~ComparatorInteger(void)
{
}

/**
 * @brief	Retrieves a numerical value for the object for use in comparisons.
 *
 * Objects with higher weights will be sorted into higher element IDs by the Comparator class.
 *
 * @param	sortableObject	The object to retrieve the value of. Must be castable to int type.
 *
 * @return	the numerical value of the object for use in comparisons.
 */
__int64 ComparatorInteger::GetWeight( const void * sortableObject ) const
{
	const int * sortableInteger = static_cast<const int*>(sortableObject);
	return *sortableInteger;
}

