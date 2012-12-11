#include "FullInclude.h"
#include "Comparator.h"

/**
 * @brief	Default constructor. 
 */
Comparator::Comparator(void)
{
}

/**
 * @brief	Destructor. 
 */
Comparator::~Comparator(void)
{
}

/**
 * @brief	Compares two non NULL objects.
 *
 * @param object1 Object to be compared with @a object2. Must not be NULL.
 * @param object2 Object to be compared with @a object1. When binary search is performed
 * this is always the object being searched for (binary search is used by StoreVector::Find).
 * Must not be NULL.
 *
 * @return	true if the weight of object1 is less than that of object2. False otherwise.
 */
bool Comparator::Compare(const void * object1, const void * object2) const
{
	return GetWeight(object1) < GetWeight(object2);
}