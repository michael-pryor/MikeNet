#pragma once

/**
 * @brief	Critical section class with Enter and Leave methods.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * Wraps around the CRITICAL_SECTION object. \n\n
 *
 * When reading or writing to protected section, use Enter()
 * and when done use Leave(). \n\n
 * 
 * Remember to use Leave() or deadlock will occur.\n\n
 *
 * When debug is enabled (_DEBUG is defined, done automatically by visual studio)
 * additional checks will be performed to help identify when:
 * - A thread uses Leave() without first taking control using Enter().
 * - A CriticalSeciton object is cleaned up while in use by another thread.
 */
class CriticalSection
{
	/**
	 * @brief Critical section, at the core this API's multithreaded architecture.
	 */
	mutable CRITICAL_SECTION CT;

#ifdef _DEBUG
	/**
	 * @brief Every time a thread uses Enter() and is able to take control or already has control,
	 * this increases by one. Every time a thread uses Leave() and is able to release control,
	 * this decreases by 1.
	 */
	mutable size_t controlCount;
#endif

public:
	/**
	 * @brief Spin count.
	 */
	const static DWORD SPIN_COUNT = 50;

	CriticalSection();
	~CriticalSection();

	void Enter() const;
	void Leave() const;

	template<typename safeReadValueType>
	/**
	 * @brief Safely reads a value using critical section.
	 *
	 * @param readMe Reference to value to read.
	 */
	safeReadValueType _safeReadValue(const safeReadValueType & readMe) const
	{
		safeReadValueType returnMe;
		Enter();
			returnMe = readMe;
		Leave();
		return returnMe;	
	}

	template<typename safeWriteValueType>
	/**
	 * @brief Safely writes a value using critical section.
	 *
	 * @param updateMe Reference to object to write to.
	 * @param newValue Reference to value to copy into @a updateMe.
	 */
	void _safeWriteValue(safeWriteValueType & updateMe, const safeWriteValueType & newValue)
	{
		Enter();
			updateMe = newValue;
		Leave();
	}

	static bool TestClass();
};