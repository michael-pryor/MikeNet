#pragma once
#include "CriticalSection.h"

/**
 * @brief Simple version of ConcurrencyControl class which avoids the need for threadID parameters.
 * @remarks	Michael Pryor, 6/16/2010. 
 * 
 * This class is redundant since the use of thread local storage was introduced to ConcurrencyControl.
 *
 * This class is designed to maintain the high performance that ConcurrencyControl provides
 * but avoid the need for threadIDs which can reduce flexibility.\n\n
 *
 * Usage of the class follows the same rules as ConcurrencyControl.\n\n
 *
 * There are some drawbacks:
 * - Additional parameter to the EnterWrite(), EnterRead() and LeaveRead() methods.
 * - Slightly reduced performance.
 */
class ConcurrencyControlSimple
{
	/** @brief Controls access to ConcurrencyControlSimple::numReading with a single critical section. */
	CriticalSection mtControl;

	/** @brief Increases when a thread takes read control of this object and decreases when a thread releases read control of this object. */
	mutable size_t numReading;

	/** @brief A thread takes control of this object when writing. */
	CriticalSection writing;

	bool IsAnyoneElseReading(size_t readCount) const;
public:
	ConcurrencyControlSimple();
	virtual ~ConcurrencyControlSimple();

	void EnterRead(size_t & readCount) const;
	void LeaveRead(size_t & readCount) const;

	void EnterWrite(size_t readCount);
	void LeaveWrite();

	template<typename safeReadValueType>
	/**
	 * @brief Safely reads a variable using critical sections.
	 * @param readMe Reference to variable to read.
	 */
	safeReadValueType _safeReadValue(const safeReadValueType & readMe) const
	{
		safeReadValueType returnMe;

		// Do not need to get from input, EnterRead increases by 1 and LeaveRead decreases by 1
		// so readCount is not permanently changed by this method.
		size_t readCount = 0;

		EnterRead(readCount);
			returnMe = readMe;
		LeaveRead(readCount);
		return returnMe;	
	}

	template<typename safeWriteValueType>
	/**
	 * @brief Safely writes a value using critical sections to a non key variable.
	 *
	 * Non key means that it does not require write access when changing (see class description for reasoning).
	 *
	 * @param updateMe Reference to object to write to.
	 * @param newValue Pointer to value to copy into @a updateMe.
	 */
	void _safeWriteValueNK(safeWriteValueType & updateMe, const safeWriteValueType * newValue)
	{
		// Do not need to get from input, EnterRead increases by 1 and LeaveRead decreases by 1
		// so readCount is not permanently changed by this method.
		size_t readCount = 0;

		EnterRead(readCount); // IS OKAY
			updateMe = *newValue;
		LeaveRead(readCount); // IS OKAY
	}

	template<typename safeWriteValueType>
	/**
	 * @brief Safely writes a value using critical sections to a non key variable.
	 *
	 * Non key means that it does not require write access when changing (see class description for reasoning).
	 *
	 * @param [out] updateMe Reference to object to write to.
	 * @param newValue Value to copy into @a updateMe.
	 */
	void _safeWriteValueNK(safeWriteValueType & updateMe, safeWriteValueType newValue)
	{
		size_t readCount = 0;

		EnterRead(readCount); // IS OKAY
			updateMe = newValue;
		LeaveRead(readCount); // IS OKAY
	}

	template<typename safeWriteValueType>
	/**
	 * @brief Safely writes a value using critical sections to a key variable.
	 *
	 * Key means that it does require write access when changing.
	 *
	 * @param [out] updateMe Reference to object to write to.
	 * @param newValue Pointer to value to copy into @a updateMe.
	 * @param readCount This is used by a thread to keep track
	 * of how many calls to EnterRead() it has made, so that EnterWrite() can correctly
	 * take read control if it is the only thread that has read control.
	 */
	void _safeWriteValueKEY(safeWriteValueType & updateMe, const safeWriteValueType * newValue, size_t readCount)
	{
		EnterWrite(readCount);
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
	 * @param readCount This is used by a thread to keep track
	 * of how many calls to EnterRead() it has made, so that EnterWrite() can correctly
	 * take read control if it is the only thread that has read control.
	 */
	void _safeWriteValueKEY(safeWriteValueType & updateMe, safeWriteValueType newValue, size_t readCount)
	{
		EnterWrite(readCount);
			updateMe = newValue;
		LeaveWrite();
	}

	static bool TestClass();
};

