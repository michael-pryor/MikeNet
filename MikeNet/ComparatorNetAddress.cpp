#include "FullInclude.h"
#include "ComparatorNetAddress.h"
#include "NetAddress.h"

/**
 * @brief	Default constructor. 
 */
ComparatorNetAddress::ComparatorNetAddress(void)
{
}

/**
 * @brief	Destructor. 
 */
ComparatorNetAddress::~ComparatorNetAddress(void)
{
}

/**
 * @brief	Retrieves a numerical value for the object for use in comparisons.
 *
 * Objects with higher weights will be sorted into higher element IDs by the Comparator class.
 *
 * Note that port does not alter weight. I have left space in the return value for ipV6 addresses
 * (when this is implemented).
 *
 * @param	sortableObject	The object to retrieve the value of. Must be castable to NetAddress.
 *
 * @return	the numerical value of the object for use in comparisons.
 */
__int64 ComparatorNetAddress::GetWeight( const void * sortableObject ) const
{
	const NetAddress * address = static_cast<const NetAddress*>(sortableObject);
	
	return address->GetByteRepresentationIP();
}
