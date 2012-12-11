#pragma once

/**
 * @brief	Manages a queue of objects efficiently and safely.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * The queue manages the objects using pointers, making it very efficient.\n\n
 *
 * Only the queue itself is thread safe, not the use of the objects in the queue.
 */
template <class T>
class StoreQueue: public Store<T>
{
	/** @brief Queue, storing pointers to elements. */
	queue<T*> data;
public:

	/**
	 * @brief Empty the contents of the queue.
	 */
	void Clear()
	{
		Enter();
			while(data.size() > 0)
			{
				delete data.front();
				data.pop();
			}
		Leave();
	}


	/**
	 * @brief Constructor.
	 */
	StoreQueue() : Store()
	{

	}

	/**
	 * @brief Deep copy constructor.
	 *
	 * @param	copyMe	Object to copy.
	 */
	StoreQueue(const StoreQueue & copyMe) : Store(copyMe)
	{
		copyMe.Enter();
			queue<T*> aux = copyMe.data;
		copyMe.Leave();
		
		this->Enter();
		while(aux.size() > 0)
		{
			T * object = new (nothrow) T(*aux.front());
			Utility::DynamicAllocCheck(object,__LINE__,__FILE__);

			aux.pop();
			this->data.push(object);
		}
		this->Leave();
	}

	/**
	 * @brief Deep assignment operator.
	 *
	 * @param	copyMe	Object to copy.
	 *
	 * @return	reference to this object.
	 */
	StoreQueue & operator= (const StoreQueue & copyMe)
	{
		this->Enter();
		copyMe.Enter();

		try
		{
			Clear();

			queue<T*> aux = copyMe.data;

			while(aux.size() > 0)
			{
				T * object = new (nothrow) T(*aux.front());
				Utility::DynamicAllocCheck(object,__LINE__,__FILE__);

				aux.pop();
				this->data.push(object);
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
	virtual ~StoreQueue()
	{
		const char * cCommand = "an internal function (~StoreQueue)";
		try
		{
			Clear();
		}
		MSG_CATCH
	}

	/**
	 * @brief Get object from front of queue and remove it from queue.
	 *
	 * @param [out] destination Location to copy retrieved object.
	 *
	 * @return number of objects in collection before this method was called.
	 */
	size_t Get(T * destination)
	{
		size_t returnMe;

		Enter();
			returnMe = data.size();

			if(data.size() > 0)
			{
				*destination = *data.front();
				delete data.front();
				data.pop();
			}
		Leave();

		return(returnMe);
	}

	/**
	 * @brief Add object to back of queue.
	 *
	 * @param [in] object Object to add to collection. @a obj is now owned by this object
	 * and should not be referenced elsewhere.
	 */
	void Add(T * object)
	{
		_ErrorException((object == NULL),"adding an object to an object store queue, NULL pointer received",0,__LINE__,__FILE__);
		Enter();
			data.push(object);
		Leave();
	}

	/**
	 * @brief Retrieve the number of objects in the queue.
	 * 
	 * @return number of objects in queue.
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
	 * @brief Retrieve the object at the front of the queue (without removing it).
	 *
	 * @return element at front.
	 */
	T * Front() const
	{
		T * returnMe;

		Enter();
		try
		{
			_ErrorException((data.size() == 0),"retrieving an element from the front of a queue, queue is empty",0,__LINE__,__FILE__);
			returnMe = data.front();
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();

		return(returnMe);
	}

	/**
	 * @brief	Removes the element at the front of the queue (deallocating it).
	 */
	void RemoveFront()
	{
		Enter();
		try
		{
			_ErrorException((data.size() == 0),"retrieving an element from the front of a queue, queue is empty",0,__LINE__,__FILE__);
			delete data.front();
			data.pop();
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();
	}

	/**
	 * @brief	Removes the element at the front of the queue and returns it.
	 */
	T * ExtractFront()
	{
		T * returnMe = NULL;

		Enter();
		try
		{
			_ErrorException((data.size() == 0),"extracting an element from the front of a queue, queue is empty",0,__LINE__,__FILE__);
			returnMe = data.front();
			data.pop();
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();

		return returnMe;
	}

	/**
	 * @brief	Determines whether this object is empty or not.
	 *
	 * @return	true if empty, false if not.
	 */
	bool IsEmpty() const
	{
		Enter();
		bool returnMe = data.empty();
		Leave();
		return returnMe;
	}

	/**
	 * @brief Retrieve the object at the back of the queue (without removing it).
	 *
	 * @return element at back.
	 */
	T * Back() const
	{
		T * returnMe;

		Enter();
		try
		{
			_ErrorException((data.size() == 0),"retrieving an element from the back of a queue, queue is empty",0,__LINE__,__FILE__);
			returnMe = data.back();
		}
		// Release control of all objects before throwing final exception
		catch(ErrorReport & error){Leave(); throw(error);}
		catch(...){Leave(); throw(-1);}
		Leave();

		return(returnMe);
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
		cout << "Testing StoreQueue class...\n";
		bool problem = false;

		size_t size = 5;
		size_t result = 0;

		StoreQueue<size_t> queue;
		queue.Add(new size_t(50));
		queue.Add(new size_t(40));

		queue.Clear();
		if(queue.Size() > 0)
		{
			cout << "Clear or Size is bad\n";
			problem = true;
		}
		else
		{
			cout << "Clear and Size are good\n";
		}

		queue.Add(new size_t(10));
		queue.Add(new size_t(20));

		if(*queue.Front() != 10 || *queue.Back() != 20)
		{
			cout << "Front or Back is bad\n";
			problem = true;
		}
		else
		{
			cout << "Front and Back are good\n";
		}

		if(queue.Size() != 2 || queue.Get(&result) != 2 || result != 10 || queue.Get(&result) != 1 || result != 20)
		{
			cout << "Add or Get is bad\n";
			problem = true;
		}
		else
		{
			cout << "Add and Get are good\n";
		}

		queue.Clear();
		
		queue.Add(new size_t(1));
		queue.Add(new size_t(100));
		queue.Add(new size_t(5121));
		queue.Add(new size_t(32));
		queue.Add(new size_t(64));
		queue.Add(new size_t(1337));

		StoreQueue<size_t> copyOperator(queue);
		bool same = true;
		if(copyOperator.Get(&result) != 6 && result != 1)
		{
			same = false;
		}
		if(copyOperator.Get(&result) != 5 && result != 100)
		{
			same = false;
		}
		if(copyOperator.Get(&result) != 4 && result != 5121)
		{
			same = false;
		}
		if(copyOperator.Get(&result) != 3 && result != 32)
		{
			same = false;
		}
		if(copyOperator.Get(&result) != 2 && result != 64)
		{
			same = false;
		}
		if(copyOperator.Get(&result) != 1 && result != 1337)
		{
			same = false;
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

		StoreQueue<size_t> assignmentOperator;
		queue.Add(new size_t(10));
		queue.Add(new size_t(20));

		assignmentOperator = queue;
		same = true;
		if(assignmentOperator.Get(&result) != 6 && result != 1)
		{
			same = false;
		}
		if(assignmentOperator.Get(&result) != 5 && result != 100)
		{
			same = false;
		}
		if(assignmentOperator.Get(&result) != 4 && result != 5121)
		{
			same = false;
		}
		if(assignmentOperator.Get(&result) != 3 && result != 32)
		{
			same = false;
		}
		if(assignmentOperator.Get(&result) != 2 && result != 64)
		{
			same = false;
		}
		if(assignmentOperator.Get(&result) != 1 && result != 1337)
		{
			same = false;
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

		cout << "\n\n";
		return !problem;
	}
};