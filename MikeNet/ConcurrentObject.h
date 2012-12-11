#pragma once
#include "ConcurrencyControl.h"

DWORD WINAPI ConcurrentObjectTestFunction(LPVOID lpParameter);

/**
 * @brief	Uses CriticalSection to safely control access to an object.
 * @remarks	Michael Pryor, 6/28/2010.
 */
template <class T>	
class ConcurrentObject: public CriticalSection
{
	/** @brief Object that is protected by critical section. */
	T object;

public:
	/**
	 * @brief Constructor.
	 */
	ConcurrentObject() : CriticalSection()
	{

	}

	/**
	 * @brief Constructor, protected object is initialized by copying a temporary object.
	 *
	 * @param paraObject Object to be protected.
	 */
	ConcurrentObject(T paraObject) : CriticalSection(), object(paraObject)
	{

	}

	/**
	 * @brief Constructor, protected object is initialized by copying a dereferenced pointer.
	 *
	 * @param paraObject Pointer to object to be protected, object contents is copied.
	 */
	ConcurrentObject(const T * paraObject) : CriticalSection(), object(*paraObject)
	{
		
	}

	/**
	 * @brief Copy constructor.
	 *
	 * @param copyMe Object to copy.
	 */
	ConcurrentObject(const ConcurrentObject<T> & copyMe) : CriticalSection(), object(copyMe.object)
	{

	}

	/**
	 * @brief Assignment operator.
	 *
	 * @param copyMe Object to copy.
	 *
	 * @return reference to this object.
	 */
	ConcurrentObject & operator= (T copyMe)
	{
		SetB(&copyMe);
		return (*this);
	}

	/**
	 * @brief Assignment operator.
	 *
	 * @param copyMe Object to copy.
	 *
	 * @return reference to this object.
	 */
	ConcurrentObject & operator= (const ConcurrentObject<T> & copyMe)
	{
		copyMe.Enter();
			SetB(&copyMe.object);
		copyMe.Leave();
		return (*this);
	}

	/**
	 * @brief	Equality operator. 
	 *
	 * @param	compare	Object to compare with this object.
	 *
	 * @return	true if @a compare is identical to this object. 
	 */
	bool operator==(const ConcurrentObject<T> & compare) const
	{
		this->Enter();
		compare.Enter();
			bool returnMe = (this->object == compare.object);
		compare.Leave();
		this->Leave();

		return returnMe;
	}

	/**
	 * @brief	Inequality operator. 
	 *
	 * @param	compare	Object to compare with this object.
	 *
	 * @return	false if @a compare is identical to this object. 
	 */
	bool operator!=(const ConcurrentObject<T> & compare) const
	{
		return !(*this == compare);
	}

	/**
	* @brief Changes contents of protected object, by copying data from a dereferenced pointer.
	* 
	* @param paraObject Pointer who's data will be copied into object.
	*/
	void ConcurrentObject::SetB(const T * paraObject)
	{
		Enter();
			object = *paraObject;
		Leave();
	}

	/**
	* @brief Changes contents of protected object, by copying data from a temporary object.
	* 
	* @param paraObject Object who's data will be copied.
	*/
	void ConcurrentObject::Set(T paraObject)
	{
		Enter();
			object = paraObject;
		Leave();
	}

	/**
	 * @brief Safely decreases protected object by specified amount.
	 *
	 * @param amount Amount to decrease protected object by.
	 */
	void ConcurrentObject::Decrease(int amount)
	{
		Enter();
			object-=amount;
		Leave();
	}

	/**
	 * @brief Safely increases protected object by specified amount.
	 *
	 * @param amount Amount to increase protected object by.
	 */
	void ConcurrentObject::Increase(int amount)
	{
		Enter();
			object+=amount;
		Leave();
	}

	/**
	 * @brief Retrieves copy of protected object.
	 *
	 * @return copy of object.
	 */
	T ConcurrentObject::Get() const
	{
		T returnMe;

		Enter();
			returnMe = object;
		Leave();

		return(returnMe);
	}

	/**
	 * @brief Retrieves copy of protected object.
	 * 
	 * @param [out] destination Destination to copy protected object into.
	 */
	void ConcurrentObject::GetB(T & destination)
	{
		Enter();
			destination = object;
		Leave();
	}

	/**
	 * @brief Retrieves mutable pointer to protected object, which is not thread safe.
	 *
	 * @warning Object can be used unsafely if this command is used!
	 * Take care to ensure that appropriate read and write protection is used
	 * when accessing protected object.
	 *
	 * @return mutable pointer to protected object.
	 */
	T * ConcurrentObject::GetPtr()
	{
		return(&object);
	}

	/**
	 * @brief Retrieves constant pointer to protected object, which is not thread safe.
	 *
	 * @warning Object can be used unsafely if this command is used!
	 * Take care to ensure that appropriate read and write protection is used.
	 *
	 * @note Did not overload GetPtr because visual studio compiler crashes in this case.
	 *
	 * @return constant pointer to protected object.
	 */
	const T * ConcurrentObject::GetPtrConst()
	{
		return(&object);
	}

	/**
	 * @brief Sets the specified bit of the protected object to on.
	 * 
	 * @exception ErrorReport If bitNumber > sizeof(object).
	 *
	 * @param bitNumber Bit number to toggle on.
	 */
	void ConcurrentObject::BitOn(int bitNumber)
	{
		_ErrorException((bitNumber > sizeof(T)),"attempting to signal object's bit to on, bitNumber is too high",0,__LINE__,__FILE__);
		Enter();
			BIT_ON(object,bitNumber);
		Leave();
	}

	/**
	 * @brief Sets the specified bit of the protected object to off.
	 * 
	 * @exception ErrorReport If bitNumber > sizeof(object).
	 *
	 * @param bitNumber Bit number to toggle off.
	 */
	void ConcurrentObject::BitOff(int bitNumber)
	{
		_ErrorException((bitNumber > sizeof(T)),"attempting to signal object's bit to off, bitNumber is too high",0,__LINE__,__FILE__);
		Enter();
			BIT_OFF(object,bitNumber);
		Leave();
	}

	/**
	 * @brief Toggles the specified bit of the protected object.
	 * 
	 * @exception ErrorReport If bitNumber > sizeof(object).
	 *
	 * @param bitNumber Bit number to flip.
	 */
	void ConcurrentObject::BitToggle(int bitNumber)
	{
		_ErrorException((bitNumber > sizeof(T)),"attempting to toggle object's bit, bitNumber is too high",0,__LINE__,__FILE__);
		Enter();
			BIT_TOGGLE(object,bitNumber);
		Leave();
	}

	/**
	 * @brief Retrieves the specified bit of the protected object.
	 * 
	 * @exception ErrorReport If bitNumber > sizeof(object).
	 *
	 * @param bitNumber Bit number to retrieve the value of.
	 * @return true if specified bit is signaled.
	 */
	bool ConcurrentObject::BitGet(int bitNumber) const
	{
		_ErrorException((bitNumber > sizeof(T)),"attempting to retrieve object's bit, bitNumber is too high",0,__LINE__,__FILE__);
		bool bReturn = false;
		Enter();
			bReturn = BIT_GET(object,bitNumber);
		Leave();

		return(bReturn);
	}

	/**
	 * @brief Retrieves an exact copy of this object.
	 *
	 * @return copy of this object.
	 */
	ConcurrentObject * Clone() const
	{
		ConcurrentObject * returnMe = NULL;
		Enter();
			returnMe = new (nothrow) ConcurrentObject(*this);
		Leave();
		Utility::DynamicAllocCheck(returnMe,__LINE__,__FILE__);

		return returnMe;
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
		cout << "Testing ConcurrentObject class...\n";
		bool problem = false;

		{
			cout << "Running threads\n";

			int integer = 5000;
			ConcurrentObject<int*> co(&integer);

			// Create group of threads
			const size_t numThreads = 5;

			ThreadSingleGroup threads;

			for(size_t n = 0;n<numThreads;n++)
			{
				ThreadSingle * thread = new ThreadSingle(&ConcurrentObjectTestFunction,&co);
				Utility::DynamicAllocCheck(thread,__LINE__,__FILE__);
				thread->Resume();
				threads.Add(thread);
				
			}

			// Wait for threads to exit
			threads.WaitForThreadsToExit();

			// Total up return value
			size_t total = 0;
			for(size_t n = 0;n<numThreads;n++)
			{
				total += threads[n].GetExitCode();
				cout << "Count for thread " << n << " is " << threads[n].GetExitCode() << '\n';
			}

			cout << "Total: " << total << '\n';
		}

		{
			ConcurrentObject<size_t> co;
			co.Set(0);

			// Test Increase()
			co.Increase(10);

			if(co.Get() != 10)
			{
				cout << "Increase is bad\n";
				problem = true;
			}
			else
			{
				cout << "Increase is good\n";
			}

			// Test Decrease()
			co.Decrease(5);

			if(co.Get() != 5)
			{
				cout << "Decrease is bad\n";
				problem = true;
			}
			else
			{
				cout << "Decrease is good\n";
			}

			co.Set(0);

			// Test BitOn()
			co.BitOn(2);
			if(co.BitGet(2) != true)
			{
				cout << "BitOn or BitGet is bad\n";
				problem = true;
			}
			else
			{
				cout << "BitOn and BitGet are good\n";
			}

			// Test BitOff()
			co.BitOff(2);
			if(co.BitGet(2) != false)
			{
				cout << "BitOff or BitGet is bad\n";
				problem = true;
			}
			else
			{
				cout << "BitOff and BitGet are good\n";
			}

			// Test BitToggle()
			co.BitToggle(2);
			if(co.BitGet(2) != true)
			{
				cout << "BitToggle or BitGet is bad\n";
				problem = true;
			}
			else
			{
				cout << "BitToggle and BitGet are good\n";
			}

			// Test BitToggle()
			co.BitToggle(2);
			if(co.BitGet(2) != false)
			{
				cout << "BitToggle or BitGet is bad\n";
				problem = true;
			}
			else
			{
				cout << "BitToggle and BitGet are good\n";
			}

			ConcurrentObject<size_t> copyOfCo(co);
			if(co != copyOfCo)
			{
				cout << "Copy constructor or equality operator is bad\n";
				problem = true;
			}
			else
			{
				cout << "Copy constructor and equality operator are good\n";
			}

			ConcurrentObject<size_t> copyOfCo2;
			copyOfCo2 = co;
			if(co != copyOfCo2)
			{
				cout << "Assignment operator or equality operator is bad\n";
				problem = true;
			}
			else
			{
				cout << "Assignment operator and equality operator are good\n";
			}
		}

		
		cout << "\n\n";
		return !problem;
	}


};

extern ConcurrentObject<int> ErrorMode;