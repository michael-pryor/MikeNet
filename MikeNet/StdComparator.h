#pragma once
#include "Comparator.h"

/**
 * @brief	Interfaces with the std namespace and sorts items in conjunction with Comparator and StoreVector.
 *
 * @author	Michael Pryor
 * @date	12/04/2011.
 */
template<class T1, class T2>
class StdComparator
{
protected:
	const Comparator * comparator;
	
public:

	/**
	 * @brief	Constructor. 
	 *
	 * @param	comparator	Object used to retrieve the 'weight' of objects being compared.
	 * Objects with higher weights have a higher element number.
	 */
	StdComparator(const Comparator & comparator)
	{
		this->comparator = &comparator;
	}

	/**
	 * @brief	Default constructor. 
	 */
	StdComparator()
	{
		comparator = NULL;
	}

	/**
	 * @brief	Determine if a comparator is loaded into this object.
	 *
	 * @return	true if comparator loaded, false if not. 
	 */
	bool IsComparatorLoaded()
	{
		return comparator != NULL;
	}

	/**
	 * @brief	Loads a comparator into this object.
	 *
	 * @param [in]	comparator	The comparator to load. 
	 */
	void LoadComparator(const Comparator & comparator)
	{
		this->comparator = &comparator;
	}

	/**
	 * @brief	Retrieves a comparator from this object.
	 *
	 * @return	the comparator.
	 */
	const Comparator & GetComparator()
	{
		_ErrorException((!IsComparatorLoaded()),"retrieving a comparator object, no comparator is loaded",0,__LINE__,__FILE__);
		return *comparator;
	}

	/**
	 * @brief	Shallow copy constructor.
	 *
	 * Necessary because std::sort uses it.
	 *
	 * @param	p_comparator	Object to copy. 
	 */
	StdComparator(const StdComparator & p_comparator)
	{
		this->comparator = p_comparator.comparator;
	}

	/**
	 * @brief	Destructor. 
	 */
	virtual ~StdComparator()
	{

	}

	/**
	 * @brief	Comparison operater used by std::sort.
	 *
	 * @param object1 Object to be compared with @a object2.
	 * @param object2 Object to be compared with @a object1. When binary search is performed
	 * this is always the object being searched for (binary search is used by StoreVector::Find).
	 *
	 * @return	true if the weight of object1 is less than that of object2. False otherwise.
	 */
	bool operator() ( const T1 * object1, const T2 * object2 ) const
	{
		// object1 < object2
		if(object1 == NULL && object2 != NULL)
		{
			return true;
		}
		// object1 > object2
		else if(object1 != NULL && object2 == NULL)
		{
			return false;
		}
		// object1 = object2
		else if(object1 == NULL && object2 == NULL)
		{
			return false;
		}
		else if(object1 != NULL && object2 != NULL)
		{
			return comparator->Compare(object1,object2);
		}

		// Should be unreachable.
		return false;
	}
};
