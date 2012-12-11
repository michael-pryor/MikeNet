#pragma once

/**
 * @brief	Sorts items when used with StoreVector.
 *
 * @author	Michael Pryor
 * @date	13/04/2011.
 */
class Comparator
{
public:
	Comparator(void);
	virtual ~Comparator(void);

	/**
	 * @brief	Retrieves a numerical value for the object for use in comparisons.
	 *
	 * Objects with higher weights will be sorted into higher element IDs by the Comparator class.
	 *
	 * Note that it is expected that overriding classes with cast @a sortableObject
	 * as necessary.
	 *
	 * @param	sortableObject	The object to retrieve the value of. This should be cast as appropriate.
	 *
	 * @return	the numerical value of the object for use in comparisons.
	 */
	virtual __int64 GetWeight(const void * sortableObject) const = 0;

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
	virtual bool Compare(const void * object1, const void * object2) const;
};

