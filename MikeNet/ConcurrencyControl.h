#pragma once
class CriticalSection;

/**
 * @brief	Advanced concurrency control object.
 * @remarks	Michael Pryor, 6/28/2010. 

 * ConcurrencyControl is an advanced concurrency control object. \n
 * It relies on the following key multithreading principals:
 * - Data can be read by multiple threads at the same time.
 * - Data cannot be written and read from at the same time.
 * - Data cannot be written to > once at the same time.
 *
 * Threads should EnterRead when reading protected data, and LeaveRead when done reading the protected data.
 * Threads should EnterWrite when writing to a protected location, and LeaveWrite when done writing.
 * When in write control (EnterWrite) the thread can also read from the protected location.\n\n
 * 
 * All threads using this object should have a manual thread ID set in their associated ThreadSingle object
 * ranging from inclusive 0 to exclusive maximum number of threads.
 * The main process is given a virtual thread ID = to the maximum number of threads.
 * Two threads MUST NOT have the same ID, deadlock may occur otherwise.\n\n
 *
 * Calling threads must have first used ThreadSingle::ThreadSetCallingThread(). The main process must
 * call AllocateThreadLocalStorage and use ThreadSingle::ThreadSetCallingThread with a parameter of NULL.
 * This is done automatically by NetUtility::SetupCompletionPort.
 *
 * You must ensure the following when using ConcurrencyControl objects:
 * - A thread must release control at some point.
 * - A thread should only take control while it is using the protected objects.
 *
 * Note that a thread can EnterRead or EnterWrite multiple times but must call LeaveRead and LeaveWrite
 * for each individual EnterRead or/and EnterWrite.
 */
class ConcurrencyControl
{
	/**
	 * @brief Number of threads that can use object.
	 *
	 * Thread IDs range from 0 (inclusive) to maximum number of
	 * threads (non inclusive).
	 */
	size_t threads;

	/**
	 * @brief Critical section object, used to control access to object.
	 *
	 * Each thread will have one private critical section.
	 */
	CriticalSection * access;

	/**
	 * @brief Keeps a count of how many levels of read control each thread has.
	 *
	 * e.g. If a thread uses EnterRead() 4 times and then LeaveRead() twice, at this
	 * point it now has 2 levels of read control which is noted by this object.\n\n
	 * 
	 * Each thread will have its own private read count.
	 */
	size_t * accessRead;

	/**
	 * @brief Keeps a count of how many levels of write control each thread has.
	 *
	 * e.g. If a thread uses EnterWrite() 4 times and then LeaveWrite() twice, at this
	 * point it now has 2 levels of write control which is noted by this object.\n\n
	 * 
	 * Each thread will have its own private write count.
	 */
	size_t * accessWrite;

	/**
	 * @brief True if ConcurrencyControl::Construct has been used.
	 */
	bool constructed;

	void DefaultVariables();

	void ValidateThreadID(size_t threadID) const;

	size_t GetThreadID() const;
public:
	void Construct(size_t numThreads);
	ConcurrencyControl();
	ConcurrencyControl(size_t paraThreads);

private:
	void Cleanup();
public:
	~ConcurrencyControl();

	size_t GetNumThreads() const;

	void EnterRead() const;
	void LeaveRead() const;
	void EnterWrite();
	void LeaveWrite();

	template<typename safeReadValueType>
	/**
	 * @brief Safely reads a value using critical sections.
	 *
	 * @param readMe Reference to value to read.
	 */
	safeReadValueType _safeReadValue(const safeReadValueType & readMe) const
	{
		safeReadValueType returnMe;
		EnterRead();
			returnMe = readMe;
		LeaveRead();
		return returnMe;	
	}

	template<typename safeWriteValueType>
	/**
	 * @brief Safely writes a value using critical sections to a non key variable.
	 *
	 * Non key means that it does not require write access when changing (see class description for reasoning).
	 * @param updateMe Reference to object to write to.
	 * @param newValue Pointer to value to copy into @a updateMe.
	 */
	void _safeWriteValueNK(safeWriteValueType & updateMe, const safeWriteValueType * newValue)
	{
		EnterRead(); // IS OKAY
			updateMe = *newValue;
		LeaveRead(); // IS OKAY
	}

	template<typename safeWriteValueType>
	/**
	 * @brief Safely writes a value using critical sections to a non key variable.
	 *
	 * Non key means that it does not require write access when changing (see class description for reasoning).
	 * @param [out] updateMe Reference to object to write to.
	 * @param newValue Value to copy into @a updateMe.
	 */
	void _safeWriteValueNK(safeWriteValueType & updateMe, safeWriteValueType newValue)
	{
		EnterRead(); // IS OKAY
			updateMe = newValue;
		LeaveRead(); // IS OKAY
	}

	template<typename safeWriteValueType>
	/**
	 * @brief Safely writes a value using critical sections to a key variable.
	 *
	 * Key means that it does require write access when changing.
	 * @param [out] updateMe Reference to object to write to.
	 * @param newValue Pointer to value to copy into @a updateMe.
	 */
	void _safeWriteValueKEY(safeWriteValueType & updateMe, const safeWriteValueType * newValue)
	{
		EnterWrite();
			updateMe = *newValue;
		LeaveWrite();
	}

	template<typename safeWriteValueType>
	/**
	 * @brief Safely writes a value using critical sections to a key variable.
	 *
	 * Key means that it does require write access when changing.
	 * @param [out] updateMe Reference to object to write to.
	 * @param newValue Value to copy into @a updateMe.
	 */
	void _safeWriteValueKEY(safeWriteValueType & updateMe, safeWriteValueType newValue)
	{
		EnterWrite();
			updateMe = newValue;
		LeaveWrite();
	}

	static bool TestClass();
};

