#pragma once
#include "Store.h"
#include <math.h>
#include "StdComparator.h"
using namespace std;

/**
 * @brief	Manages a vector of objects efficiently and safely.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * The vector manages the objects using pointers, making it very efficient.\n\n
 *
 * Only the vector itself is thread safe, not the use of the objects in the vector.\n\n
 */
template<class T>
class StoreVector: public Store<T>
{
	/** @brief Vector, storing pointers to elements. */
	vector<T*> data;

	/** @brief If true this object cannot deallocate elements of the data vector. */
	bool doNotDeallocate;

	/**
	 * @brief	Sets variables to default.
	 */
	void DefaultVariables()
	{
		doNotDeallocate = false;
	}

public:
	/** @brief Constructor. */
	StoreVector() : Store()
	{
		DefaultVariables();
	}

	/**
	 * @brief Constructor.
	 *
	 * @param doNotDeallocate if true items added to this vector will not be deallocated.
	 */
	StoreVector(bool doNotDeallocate) : Store()
	{
		DefaultVariables();
		this->doNotDeallocate = true;
	}

	/**
	 * @brief Deep copy constructor.
	 *
	 * @param	copyMe	Object to copy.
	 */
	StoreVector(const StoreVector & copyMe) : Store(copyMe)
	{
		copyMe.Enter();
		this->Enter();

		try
		{
			DefaultVariables();

			this->doNotDeallocate = copyMe.doNotDeallocate;

			this->data.resize(copyMe.data.size());
			for(size_t n = 0;n<copyMe.data.size();n++)
			{
				if(copyMe.IsAllocated(n) == true)
				{
					this->data[n] = new (nothrow) T(*copyMe.data[n]);
					Utility::DynamicAllocCheck(this->data[n],__LINE__,__FILE__);
				}
				else
				{
					this->data[n] = NULL;
				}
			}
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & Error){ copyMe.Leave(); this->Leave(); throw(Error); }
		catch(...){	copyMe.Leave(); this->Leave(); throw(-1); }

		this->Leave();
		copyMe.Leave();
	}

	/**
	 * Deep assignment operator.
	 *
	 * @param	copyMe	Object to copy. 
	 *
	 * @return	reference to this object.
	 */
	StoreVector & operator= (const StoreVector & copyMe)
	{
		this->Enter();
		copyMe.Enter();

		try
		{
			DefaultVariables();

			this->doNotDeallocate = copyMe.doNotDeallocate;

			// Resize vector
			this->Resize(copyMe.Size());

			// Copy data
			for(size_t n = 0;n<this->Size();n++)
			{
				if(copyMe.IsAllocated(n) == true)
				{
					Allocate(n,copyMe[n]);
				}
				else
				{
					Deallocate(n);
				}
			}
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & Error){ copyMe.Leave(); this->Leave(); throw(Error); }
		catch(...){	copyMe.Leave(); this->Leave(); throw(-1); }

		copyMe.Leave();
		this->Leave();

		return *this;
	}


	/** @brief Destructor. */
	virtual ~StoreVector()
	{
		const char * cCommand = "an internal function (~StoreVector)";
		try
		{
			Clear();
		}
		MSG_CATCH
	}

	/**
	 * @brief	Links this vector with another.
	 *
	 * The vectors themselves are not linked, so if elements are added to this vector
	 * after it linking then these elements will not appear in the vector that it is
	 * linked with. Similarly objects added after linking, to the vector that this object
	 * is linked with, will not appear in this object.\n\n
	 *
	 * After linking, only objects that were stored by the object we linked with will
	 * be present in this vector (note that any previous content will be overwritten).
	 * Changes made to the object, regardless of which vector
	 * was used to access the object, will be seen by both vectors (i.e. the memory address
	 * of objects is shared between both vectors).\n\n
	 *
	 * For this reason the vector with the shallow link is not allowed to deallocate elements
	 * and only the original vector is responsible for deallocation.
	 *
	 * This method is intended for use where two identical vectors are needed but should
	 * be differently ordered (a key use is for sorting and searching).
	 *
	 * @param loadMe	The internal vector to load. 
	 */
	void LinkShallow(const StoreVector & loadMe)
	{
		Enter();
		loadMe.Enter();
			data = loadMe.data;
			doNotDeallocate = true;
		loadMe.Leave();
		Leave();
	}

	/**
	 * @brief	Determines whether this vector will deallocate memory
	 * of its items.
	 *
	 * If deallocation is disabled then commands such as Deallocate,
	 * which are intended solely for deallocation will fail, but other
	 * commands such as Erase will succeed but without deallocating memory.
	 *
	 * @return	true if deallocation is allowed, false if not. 
	 */
	bool IsDeallocationAllowed()
	{
		return !doNotDeallocate;
	}
private:

	/**
	 * @brief	Sorts the specified std::vector.
	 *
	 * @param [in,out]	sortMe	The std::vector to sort.
	 * @param	comparator		The comparator which determines how elements should be sorted.
	 */
	void VectorSort(vector<T*> & sortMe, const Comparator & comparator) const
	{
		StdComparator<T,T> stdComparator(comparator);

		std::sort(sortMe.begin(),sortMe.end(),stdComparator);
	}

	/**
	 * @brief	Sorts and searches for an element within the specified std::vector, returning its element ID within the sorted vector.
	 *
	 * @param [in,out]	range	The std::vector to sort.
	 * @param	comparatorSort		The comparator which determines how elements should be sorted.
	 * Its compare method must accept both object1 and object2 as type T.
	 * @param   comparatorFind      The comparator used when searching for an element within the sorted
	 * vector. Its compare method must accept object1 of type T and object2 of type T2.
	 * @param	elementToFind	The element to find within @a range. 
	 * @param   sortVector			If true the vector will be sorted before searching. If the vector
	 * is already sorted set this to false to improve efficiency.
	 *
	 * @return the element ID associated with the specified element.
	 */
	template <class T2>
	size_t VectorFind(vector<T*> & range, const Comparator & comparatorSort, const Comparator & comparatorFind, const T2 * elementToFind, bool sortVector) const
	{
		if(sortVector)
		{
			StdComparator<T,T> stdComparatorSort(comparatorSort);
			std::sort(range.begin(),range.end(),stdComparatorSort);
		}

		StdComparator<T,T2> stdComparatorFind(comparatorFind);
		vector<T*>::iterator iterator = std::lower_bound(range.begin(),range.end(),elementToFind,stdComparatorFind);
		return static_cast<size_t>(iterator - range.begin());
	}

	/**
	 * @brief	Sorts and searches for an element within the specified std::vector, determining whether it is in the vector.
	 *
	 * @param [in,out]	range	The std::vector to sort.
	 * @param	comparatorSort		The comparator which determines how elements should be sorted.
	 * Its compare method must accept both object1 and object2 as type T.
	 * @param   comparatorFind      The comparator used when searching for an element within the sorted
	 * vector. Its compare method must accept object1 of type T and object2 of type T2.
	 * @param	elementToFind	The element to find within @a range. 
	 * @param   sortVector			If true the vector will be sorted before searching. If the vector
	 * is already sorted set this to false to improve efficiency.
	 *
	 * @return the element ID associated with the specified element.
	 */
	template <class T2>
	bool VectorExists(vector<T*> & range, const Comparator & comparatorSort, const Comparator & comparatorFind, const T2 * elementToFind, bool sortVector) const
	{
		if(sortVector)
		{
			StdComparator<T,T> stdComparatorSort(comparatorSort);
			std::sort(range.begin(),range.end(),stdComparatorSort);
		}

		StdComparator<T,T2> stdComparatorFind(comparatorFind);
		bool result = std::binary_search(range.begin(),range.end(),elementToFind,stdComparatorFind);
	
		return result;
	}
public:

	/**
	 * @brief	Sorts the vector using the specified comparator.
	 *
	 * std::sort is used from the C++ libraries.
	 *
	 * @param	comparator	The comparator to use, this decides how to order elements.
	 */
	void Sort(const Comparator & comparator)
	{
		Enter();
		try
		{
			VectorSort(data,comparator);
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();
	}

	/**
	 * @brief	Retrieves the element ID of the specified object within this vector.
	 *
	 * @param	comparatorSort		The comparator which determines how elements should be sorted.
	 * Its compare method must accept both object1 and object2 as type T.
	 * @param   comparatorFind      The comparator used when searching for an element within the sorted
	 * vector. Its compare method must accept object1 of type T and object2 of type T2.
	 * @param   findMe				Element to search to, this will be passed as object2 to comparatorFind's compare method.		
	 * @param   sortVector			If true the vector will be sorted before searching. If the vector
	 * is already sorted set this to false to improve efficiency.
	 *
	 * @return the element ID of @a findMe within this vector if it exists in the vector.
	 * @return undefined output if @a findMe is not in the vector.
	 */
	template <class T2>
	size_t Find(const Comparator & comparatorSort, const Comparator & comparatorFind, const T2 * findMe, bool sortVector)
	{
		size_t returnMe;

		Enter();
		try
		{
			returnMe = VectorFind(data, comparatorSort, comparatorFind, findMe, sortVector);
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();

		return returnMe;
	}

	/**
	 * @brief	Retrieves the element ID of the specified object within this vector.
	 *
	 * @param	comparator		The comparator which determines how elements should be sorted.
	 * Its compare method must accept both object1 and object2 as type T.
	 * @param	findMe			The object to find.
	 * @param   sortVector		If true the vector will be sorted before searching. If the vector
	 * is already sorted set this to false to improve efficiency.
	 *
	 * @return the element ID of @a findMe within this vector if it exists in the vector.
	 * @return undefined output if @a findMe is not in the vector.
	 */
	size_t Find(const Comparator & comparator, const T * findMe, bool sortVector)
	{
		size_t returnMe;

		Enter();
		try
		{
			returnMe = VectorFind(data, comparator, comparator, findMe, sortVector);
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();

		return returnMe;
	}

	/**
	 * @brief	Determines whether the specified object exists within this vector.
	 *
	 * @param	comparatorSort		The comparator which determines how elements should be sorted.
	 * Its compare method must accept both object1 and object2 as type T.
	 * @param   comparatorFind      The comparator used when searching for an element within the sorted
	 * vector. Its compare method must accept object1 of type T and object2 of type T2.
	 * @param   findMe				Element to search for, this will be passed as object2 to comparatorFind's compare method.		
	 * @param   sortVector			If true the vector will be sorted before searching. If the vector
	 * is already sorted set this to false to improve efficiency.
	 *
	 * @return the element ID of @a findMe within this vector if it exists in the vector.
	 * @return undefined output if @a findMe is not in the vector.
	 */
	template <class T2>
	size_t Exists(const Comparator & comparatorSort, const Comparator & comparatorFind, const T2 * findMe, bool sortVector)
	{
		size_t returnMe;

		Enter();
		try
		{
			returnMe = VectorExists(data, comparatorSort, comparatorFind, findMe, sortVector);
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();

		return returnMe;
	}

	/**
	 * @brief	Determines whether the specified object exists within this vector.
	 *
	 * @param	comparator  		The comparator which determines how elements should be sorted.
	 * @param	findMe				The object to find.
	 * @param   sortVector			If true the vector will be sorted before searching. If the vector
	 * is already sorted set this to false to improve efficiency.
	 *
	 * @return true if the object exists, false if not.
	 */
	bool Exists(const Comparator & comparator, const T * findMe, bool sortVector)
	{
		bool returnMe;

		Enter();

		try
		{
			returnMe = VectorExists(data,comparator,comparator,findMe,sortVector);
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();

		return returnMe;
	}

	/**
	 * @brief	Determines whether the specified object exists within this vector without
	 * modifying its contents.
	 *
	 * @param	comparator		The comparator used to order elements.
	 * @param	findMe			The object to find.
	 *
	 * @return the element ID of @a findMe within this vector.
	 */
	bool Exists(const Comparator & comparator, const T * findMe) const
	{
		vector<T*> copy;

		Enter();
		try
		{
			copy = data;
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();

		return VectorExists(copy,comparator,comparator,findMe,true);
	}

	/**
	 * @brief	Retrieves a pointer to an internal pointer to an element of data.
	 *
	 * This is used where the pointer itself needs to be directly modified. Use with caution
	 * as the retrieved pointer may become invalid if the vector changes in size.
	 *
	 * @param	element	The element.
	 *
	 * @return	a pointer to an internal pointer to an element of data. 
	 */
	T ** GetPtr(size_t element)
	{
		T ** returnMe = NULL;
		Enter();
		try
		{
			_ErrorException((element >= this->Size()),"retrieving a vector element's pointer, element specified is out of bounds",0,__LINE__,__FILE__);
			returnMe = &data[element];
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();
		return returnMe;
	}

	/**
	 * @brief [] Operator, returns constant pointer to element in vector.
	 *
	 * @note MUST return reference for multidimensional elements so that
	 * subsequent dimensions can be accessed in this fashion: Vec[][].
	 *
	 * @param element Element to access.
	 * @return object at @a element.
	 *
	 * @exception ErrorReport If element is out of bounds.
	 */
	const T & operator[](size_t element) const
	{
		T * returnMe = NULL;

		Enter();
		try
		{
			_ErrorException((element >= this->Size()),"accessing element of vector, element specified is out of bounds",0,__LINE__,__FILE__);
			_ErrorException((IsAllocated(element) == false),"accessing element of vector, element is not allocated",0,__LINE__,__FILE__);
			returnMe = data[element];
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}

		Leave();
		return(*returnMe);
	}

	/**
	 * @brief [] Operator, returns mutable pointer to element in vector.
	 *
	 * @note MUST return reference for multidimensional elements so that
	 * subsequent dimensions can be accessed in this fashion: Vec[][].
	 *
	 * @param element Element to access.
	 * @return object at @a element.
	 *
	 * @exception ErrorReport If element is out of bounds.
	 */
	T & operator[](size_t element)
	{
		T * returnMe = NULL;

		Enter();
		try
		{
			_ErrorException((element >= this->Size()),"accessing element of vector, element specified is out of bounds",0,__LINE__,__FILE__);
			_ErrorException((IsAllocated(element) == false),"accessing element of vector, element is not allocated",0,__LINE__,__FILE__);
			returnMe = data[element];
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}

		Leave();
		return(*returnMe);
	}
	
private:

	/**
	 * @brief	Throws an exception if deallocation is not allowed.
	 *
	 * @param	line	The line at which this method is called.
     * @param	file	The file in which this method is called.
  	 */
	void CheckDeallocationAllowed(INT_PTR line, const char * file)
	{
		_ErrorException((!IsDeallocationAllowed()),"attempting to deallocate memory from the vector, this is not allowed as this object is a shallow copy",0,line,file);
	}
public:

	/**
	 * @brief Remove element from vector, decreasing vector size by 1.
	 *
	 * Use Deallocate() to erase the contents of the element but not remove it from the vector.
	 *
	 * @param element Element to remove.
	 *
	 * @throws ErrorReport If @a element is out of bounds.
	 */
	void Erase(size_t element)
	{
		Enter();
		try
		{
			_ErrorException((element >= this->Size()),"erasing element of vector, element specified is out of bounds",0,__LINE__,__FILE__);

			if(IsDeallocationAllowed())
			{
				Deallocate(element);
			}

			data.erase(data.begin()+element);
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}

		Leave();
	}

	/**
	 * @brief	Removes and returns the specified element from the vector without deallocating its memory.
	 *
	 * @param	element	The element to extract.
	 *
	 * @return	the extracted element.
	 */
	T * Extract(size_t element)
	{
		T * returnMe = NULL;
		Enter();
		try
		{
			_ErrorException((element >= this->Size()),"extracting element from vector, element specified is out of bounds",0,__LINE__,__FILE__);

			returnMe = data[element];
			data.erase(data.begin()+element);
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}

		Leave();
		return returnMe;
	}

	/**
	 * @brief	Inserts empty (NULL) elements into the vector.
	 *
	 * @param	element	The element ID at which to start inserting elements.
	 * @param	amount	The amount of elements to insert.
	 */
	void Insert(size_t element, size_t amount)
	{
		Enter();

		try
		{
			this->data.insert(data.begin()+element,amount,NULL);
			vectorNeedsResorting = true;
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}

		Leave();
	}

	/**
	 * @brief	Inserts elements constructed with the default constructor into the vector.
	 *
	 * @param	element	The element ID at which to start inserting elements.
	 * @param	amount	The amount of elements to insert.
	 */
	void InsertAllocate(size_t element, size_t amount)
	{
		Enter();

		try
		{
			Insert(element,amount);

			for(size_t n = element;n<element+amount;n++)
			{
				Allocate(n);
			}
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}

		Leave();	
	}

	/**
	 * @brief Allocate memory to specified element, overwriting anything currently stored.
	 *
	 * New memory is allocated using default constructor.
	 *
	 * @param element Element to allocate to.
	 */
	void Allocate(size_t element)
	{
		Enter();
		try
		{
			_ErrorException((element >= this->Size()),"allocating element of vector with default constructor, element specified is out of bounds",0,__LINE__,__FILE__);
			Deallocate(element);

			data[element] = new (nothrow) T();
			Utility::DynamicAllocCheck(data[element],__LINE__,__FILE__);
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}

		Leave();
	}

	/**
	 * @brief Allocate memory to specified element, overwriting anything currently stored.
	 *
	 * @param element Element to overwrite.
	 * @param [in] newElement Reference to object to copy from.
	 */
	void Allocate(size_t element, const T & newElement)
	{
		Enter();
		
		try
		{
			_ErrorException((element >= this->Size()),"allocating memory to element of vector, element specified is out of bounds",0,__LINE__,__FILE__);
			
			Deallocate(element);

			data[element] = new (nothrow) T(newElement);
			Utility::DynamicAllocCheck(data[element],__LINE__,__FILE__);
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}

		Leave();
	}

	/**
	 * @brief Allocate memory to specified element, overwriting anything currently stored.
	 *
	 * @param element Element to overwrite.
	 * @param [in] newElement Pointer that is now owned by this object and should not be referenced elsewhere.
	 */
	void Allocate(size_t element, T * newElement)
	{
		Enter();
		
		try
		{
			_ErrorException((element >= this->Size()),"allocating memory to element of vector with pointer, element specified is out of bounds",0,__LINE__,__FILE__);

			Deallocate(element);

			data[element] = newElement;
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}

		Leave();
	}

	/**
	 * @brief Deallocate element from vector setting it to NULL, without decreasing size of vector.
	 *
	 * Use Erase() to completely remove an element from the vector, decreasing the size of the vector.
	 *
	 * @param element To deallocate.
	 */
	void Deallocate(size_t element)
	{
		Enter();

		try
		{
			_ErrorException((element >= data.size()),"deallocating element of vector, element specified is out of bounds",0,__LINE__,__FILE__);

			if(data[element] != NULL)
			{
				CheckDeallocationAllowed(__LINE__,__FILE__);
			}

			delete data[element];
			data[element] = NULL;
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}

		Leave();
	}

	/**
	 * @brief Empty the contents of the vector.
	 */
	void Clear()
	{
		Enter();

		while(data.size() > 0)
		{
			Erase(0);
		}

		Leave();		
	}


	/**
	 * @brief Get object from front of queue and removes it from the vector.
	 *
	 * @param [out] destination Location to copy retrieved object.
	 *
	 * @return number of objects in collection before this method was called.
	 */
	size_t Get(T * destination)
	{
		size_t returnMe;

		Enter();
		try
		{
			returnMe = data.size();

			if(data.size() > 0)
			{
				*destination = *data[0];
				Erase(0);
			}
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();

		return(returnMe);
	}

	/**
	 * @brief	Retrieves an element from this vector.
	 *
	 * @param	element	ID of the element to retrieve. 
	 *
	 * @return	element.
	 */
	T & GetElement(size_t element)
	{
		return (*this)[element];
	}

	/**
	 * @brief Add object to back of vector.
	 *
	 * @param [in] object Object to add to collection. @a object is now owned by this object
	 * and should not be referenced elsewhere.
	 */
	void Add(T * object)
	{
		_ErrorException((object == NULL),"adding a NULL object to a vector",0,__LINE__,__FILE__);
		Enter();
			data.push_back(object);
		Leave();
	}

	/**
	 * @brief Retrieve the number of objects in the vector.
	 * 
	 * @return number of objects in vector.
	 */
	size_t Size() const
	{
		size_t returnMe = 0;

		Enter();
			returnMe = data.size();
		Leave();

		return(returnMe);
	}
	
	/**
	 * @brief Retrieve the object at the front of the vector (without removing it).
	 *
	 * @return element at front.
	 */
	T * Front() const
	{
		T * returnMe;

		Enter();
		try
		{
			_ErrorException((data.size() == 0),"retrieving an element from the front of a vector, vector is empty",0,__LINE__,__FILE__);
			returnMe = data.front();
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();

		return(returnMe);
	}

	/**
	 * @brief Retrieve the object at the back of the vector (without removing it).
	 *
	 * @return element at back.
	 */
	T * Back() const
	{
		T * returnMe;

		Enter();
		try
		{
			_ErrorException((data.size() == 0),"retrieving an element from the back of a vector, vector is empty",0,__LINE__,__FILE__);
			returnMe = data.back();
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();

		return(returnMe);
	}

	/**
	 * @brief Resizes vector, allocating memory to new elements.
	 *
	 * Memory is allocated using default constructor.
	 *
	 * @param newSize New size of vector.
	 */
	void ResizeAllocate(size_t newSize)
	{
		Enter();
		try
		{
			size_t originalSize = data.size();
			data.resize(newSize);
			for(size_t n = originalSize;n<newSize;n++)
			{
				Allocate(n);
			}
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}

		Leave();
	}

	/**
	 * @brief Resizes vector, allocating memory to new elements.
	 *
	 * Memory is allocated using the copy constructor.
	 *
	 * @param newSize New size of vector.
	 * @param copyMe Object to pass to copy constructor of T when allocating new objects.
	 */
	void ResizeAllocate(size_t newSize, const T & copyMe)
	{
		Enter();
		try
		{
			size_t originalSize = data.size();
			data.resize(newSize);
			for(size_t n = originalSize;n<newSize;n++)
			{
				Allocate(n,copyMe);
			}
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}

		Leave();
	}

	/**
	 * @brief Determines whether the specified element has memory allocated to it.
	 * 
	 * @return true if specified element has memory allocated to it (is not NULL).
	 * @param element Element to check.
	 */
	bool IsAllocated(size_t element) const
	{
		bool returnMe;

		Enter();
		try
		{
			_ErrorException((element >= data.size()),"determining whether an element is allocated, element specified is out of bounds",0,__LINE__,__FILE__);
			returnMe = (data[element] != NULL);
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();
		return returnMe;
	}

	/**
	 * @brief Resizes array without allocating memory (setting new elements to NULL).
	 *
	 * @param newSize New size of vector.
	 */
	void Resize(size_t newSize)
	{
		Enter();
			size_t originalSize = data.size();

			// Resize array and set new elements to NULL
			data.resize(newSize);
			for(size_t n = originalSize;n<newSize;n++)
			{
				data[n] = NULL;
			}
		Leave();
	}

	/**
	 * @brief	Swaps two elements in the vector.
	 *
	 * @param	element1	The first element to swap with the second element.
	 * @param	element2	The second element to swap with the first element.
	 */
	void Swap(size_t element1, size_t element2)
	{
		Enter();
		try
		{
			_ErrorException((element1 >= Size()),"swapping two elements of a vector, element1 is out of bounds",0,__LINE__,__FILE__);
			_ErrorException((element2 >= Size()),"swapping two elements of a vector, element2 is out of bounds",0,__LINE__,__FILE__);

			// Perform real swap.
			T * aux = data[element2];
			data[element2] = data[element1];
			data[element1] = aux;
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();
	}

	/**
	 * @brief Tests class.
	 *
	 * @return true if no problems while testing were found, false if not.
	 * Note that not all tests automatically check for problems so some tests
	 * require manual verification.
	 */
	static bool TestClass()
	{


		cout << "Testing StoreVector class...\n";
		bool problem = false;
		{
		size_t size = 5;

		StoreVector<size_t> vec;
		vec.Resize(size);

		if(vec.Size() != size)
		{
			cout << "Resize or Size is bad\n";
			problem = true;
		}
		else
		{
			cout << "Resize and Size are good\n";
		}

		bool allocated = false;
		for(size_t n = 0;n<vec.Size();n++)
		{
			if(vec.IsAllocated(n) == true)
			{
				allocated = true;
				break;
			}
		}

		if(allocated == true)
		{
			cout << "Resize or IsAllocated is bad\n";
			problem = true;
		}
		else
		{
			cout << "Resize and IsAllocated are good\n";
		}
		
		vec.Allocate(0);
		if(vec.IsAllocated(0) == false)
		{
			cout << "Allocate or IsAllocated is bad\n";
			problem = true;
		}
		else
		{
			cout << "Allocate and IsAllocated are good\n";
		}

		size_t newElement = 1;
		size_t newElementValue = 4000;
		vec.Allocate(newElement,newElementValue);
		if(vec[newElement] != newElementValue)
		{
			cout << "Allocate or [] is bad\n";
			problem = true;
		}
		else
		{
			cout << "Allocate and [] are good\n";
		}

		newElement = 2;
		vec.Allocate(newElement,new size_t(newElementValue));
		if(vec[newElement] != newElementValue)
		{
			cout << "Allocate or [] is bad\n";
			problem = true;
		}
		else
		{
			cout << "Allocate and [] are good\n";
		}

		vec.Deallocate(newElement);
		if(vec.IsAllocated(newElement) == true)
		{
			cout << "Deallocate or IsAllocated is bad\n";
			problem = true;
		}
		else
		{
			cout << "Deallocate and IsAllocated are good\n";
		}

		vec.Clear();
		if(vec.Size() > 0)
		{
			cout << "Clear or Size is bad\n";
			problem = true;
		}
		else
		{
			cout << "Clear and Size are good\n";
		}

		vec.Add(new size_t(10));
		vec.Add(new size_t(20));

		if(vec.Size() != 2 || vec[0] != 10 || vec[1] != 20)
		{
			cout << "Add is bad\n";
			problem = true;
		}
		else
		{
			cout << "Add is good\n";
		}

		if(*vec.Front() != vec[0] || *vec.Back() != vec[1])
		{
			cout << "Front or Back is bad\n";
			problem = true;
		}
		else
		{
			cout << "Front and Back are good\n";
		}

		size_t destination;
		size_t result = vec.Get(&destination);
		if(vec.Size() != 1 || vec[0] != 20)
		{
			cout << "Get is bad\n";
			problem = true;
		}
		else
		{
			cout << "Get is good\n";
		}

		vec.Clear();
		
		vec.Resize(5);
		vec.ResizeAllocate(10,100);

		allocated = false;
		for(size_t n = 0;n<5;n++)
		{
			if(vec.IsAllocated(n) == true)
			{
				allocated = true;
				break;
			}
		}

		if(allocated == true)
		{
			cout << "Resize or ResizeAllocate is bad\n";
			problem = true;
		}
		else
		{
			cout << "Resize and ResizeAllocate are good\n";
		}

		allocated = true;
		for(size_t n = 5;n<vec.Size();n++)
		{
			if(vec.IsAllocated(n) == false)
			{
				allocated = true;
				break;
			}
		}

		if(allocated == false)
		{
			cout << "ResizeAllocate is bad\n";
			problem = true;
		}
		else
		{
			cout << "ResizeAllocate is good\n";
		}

		StoreVector<size_t> copyOperator(vec);
		bool same = (copyOperator.Size() == vec.Size());
		if(same == true)
		{
			for(size_t n = 0;n<copyOperator.Size();n++)
			{
				if(copyOperator.IsAllocated(n) == true && vec.IsAllocated(n) == true)
				{
					if(copyOperator[n] != vec[n])
					{
						same = false;
						break;
					}
				}
				else
				{
					if(copyOperator.IsAllocated(n) == true || vec.IsAllocated(n) == true)
					{
						same = false;
						break;
					}
				}
			}
		}

		if(same == false)
		{
			cout << "Copy constructor is bad\n";
			problem = true;
		}
		else
		{
			cout << "Copy constructor is good\n";
		}

		StoreVector<size_t> assignmentOperator;
		assignmentOperator = vec;
		same = (assignmentOperator.Size() == vec.Size());
		if(same == true)
		{
			for(size_t n = 0;n<assignmentOperator.Size();n++)
			{
				if(assignmentOperator.IsAllocated(n) == true && vec.IsAllocated(n) == true)
				{
					if(assignmentOperator[n] != vec[n])
					{
						same = false;
						break;
					}
				}
				else
				{
					if(assignmentOperator.IsAllocated(n) == true || vec.IsAllocated(n) == true)
					{
						same = false;
						break;
					}
				}
			}
		}

		if(same == false)
		{
			cout << "Assignment operator is bad\n";
			problem = true;
		}
		else
		{
			cout << "Assignment operator is good\n";
		}

		}


		// Test sorting
		{
			ComparatorInteger comparator;

			StoreVector<int> vec;
			vec.Add(new int(rand()));
			vec.Add(new int(rand()));
			vec.Add(new int(rand()));
			vec.Add(new int(rand()));
			vec.Add(new int(rand()));
			vec.Add(new int(vec[0]));
			vec.Add(new int(rand()));
			vec.Add(new int(rand()));

			cout << "Before sorting..\n";
			for(size_t n = 0;n<vec.Size();n++)
			{
				cout << vec[n] << '\n';
			}
			cout << "\n\n";

			vec.Sort(comparator);

			cout << "After sorting..\n";
			for(size_t n = 0;n<vec.Size();n++)
			{
				cout << vec[n] << '\n';
			}
			cout << "\n\n";

			for(size_t n = 1;n<vec.Size();n++)
			{
				if(vec[n] >= vec[n-1])
				{
					cout << "Sorting is good\n";
				}
				else
				{
					cout << "Sorting is bad\n";
					problem = true;
				}
			}
		}

		// Test finding
		{
			ComparatorInteger comparator;

			StoreVector<int> vec;
			vec.Add(new int(rand()));
			vec.Add(new int(rand()));
			vec.Add(new int(rand()));
			vec.Add(new int(rand()));
			vec.Add(new int(rand()));
			vec.Add(new int(rand()));
			vec.Add(new int(rand()));
			vec.Add(new int(rand()));

			vec.Deallocate(3);
			vec.Deallocate(1);

			vec.Sort(comparator);

			for(size_t n = 0;n<vec.Size();n++)
			{
				size_t pos = 0;
				bool exists;
				if(vec.IsAllocated(n))
				{ 
					pos = vec.Find(comparator,&vec[n],true);
					exists = vec.Exists(comparator,&vec[n]);
				}
				else
				{
					pos = vec.Find(comparator,NULL,true);
					exists = vec.Exists(comparator,NULL);
				}

				if(vec.IsAllocated(n))
				{
					cout << "At position " << pos << " is " << vec[n] << ". ";
				}
				else
				{
					cout << "At position " << pos << " is " << "NULL" << ". ";
				}
				cout << "Exists: " << exists << '\n';
				
			}

			for(size_t n = 0;n<vec.Size();n++)
			{
				if(vec.IsAllocated(n))
				{
					cout << n << "--> " << vec[n] << '\n';
				}
				else
				{
					cout << n << "--> " << "NULL" << '\n';
				}
			}
		}

		cout << "\n\n";
		return !problem;
	}
};