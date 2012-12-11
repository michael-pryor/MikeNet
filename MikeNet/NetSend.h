#pragma once
#include "ConcurrencyEvent.h"

/**
 * @brief Base class for sendable objects which can be used in conjunction with winsock WSASend.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * This class reduces the complexity of interacting with winsock's send mechanisms.\n\n
 *
 * This class inherits CriticalSection because: After this object is added to the send
 * cleanup list, we must have control of this object until it has finished using its own data,
 * because otherwise it may be cleaned up before it is done using them.
 */
class NetSend : public CriticalSection
{
	/** @brief True if send operation should be synchronous, false if send operation should be asynchronous. */
	bool block;

public:
	/** @brief Event object that is signaled when the send operation completes, and non signaled in its duration. */
	ConcurrencyEvent overlappedEvent;

	/** @brief Overlapped object passed to WSASend. */
	WSAOVERLAPPED overlapped;

	/** @brief Filled with number of bytes that were transferred upon completion of send operation. */
	DWORD bytes;

	NetSend(bool block);
	virtual ~NetSend();
	
	NetUtility::SendStatus _WaitForCompletion(unsigned int sendTimeout);
	size_t GetTotalBufferLength();
	bool IsBlocking() const;

	/** 
	 * @brief Retrieves an array of WSABUF structures containing
	 * data to send.
	 *
	 * @return an array of WSABUF containing data to be sent. The
	 * sent packet or data stream will consist of a combination
	 * of all elements of the array, starting from element 0.
	 */
	virtual WSABUF * GetBuffer() = 0;

	/** 
	 * @brief Retrieves the number of elements in the array returned by GetBuffer().
	 *
	 * @return number of elements.
	 */
	virtual size_t GetBufferAmount() const = 0;

	static bool TestClass();
};
