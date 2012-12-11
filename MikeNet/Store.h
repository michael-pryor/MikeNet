#pragma once
#include "CriticalSection.h"

/**
 * @brief	Base class for any object that safely controls access to a number of objects.
 * @remarks	Michael Pryor, 6/28/2010. 
 *
 * Base class for objects that manage access to T objects. \n\n
 *
 * Only the store itself is thread safe, not the use of the objects in the store.
 */
template<class T>
class Store : public CriticalSection
{
public:
	/**
	 * @brief Constructor.
	 */
	Store()
	{
		
	}

	/**
	 * @brief Destructor.
	 */
	virtual ~Store()
	{
		
	}

	/**
	 * @brief Deep copy constructor.
	 *
	 * @param	copyMe	Object to copy.
	 */
	Store(const Store & copyMe)
	{
		
	}

	/**
	 * @brief Deep assignment operator.
	 *
	 * @param	copyMe	Object to copy. 
	 *
	 * @return	reference to this object.
	 */
	Store & operator= (const Store & copyMe)
	{
		return *this;
	}

	/**
	 * @brief Empty contents of collection.
	 */
	virtual void Clear()=0;

	/**
	 * @brief Get object from front of collection and remove it from collection.
	 *
	 * @param [out] destination Location to copy retrieved object.
	 *
	 * @return number of objects in collection before this method was called.
	 */
	virtual size_t Get(T * destination)=0;

	/**
	 * @brief Add object to back of collection.
	 *
	 * @param [in] obj Object to add to collection. @a obj is now owned by this object
	 * and should not be referenced elsewhere.
	 */
	virtual void Add(T * obj)=0;

	/**
	 * @brief Retrieve the number of objects in the collection.
	 * 
	 * @return number of objects in collection.
	 */
	virtual size_t Size() const =0;

	/**
	 * @brief Retrieve the object at the front of the collection (without removing it).
	 *
	 * @return element at front.
	 */
	virtual T * Front()const =0 ;

	/**
	 * @brief Retrieve the object at the back of the collection (without removing it).
	 *
	 * @return element at back.
	 */
	virtual T * Back() const =0;
};