#pragma once
#include "threadsinglegroup.h"
#include "CompletionKey.h"

DWORD WINAPI NetManageCompletionPort(LPVOID lpParameter);

/**
 * @brief Manages a completion port and the threads associated with it.
 */
class CompletionPort :
	protected ThreadSingleGroup
{
	/**
	 * @brief Completion port.
	 */
	HANDLE completionPort;
public:
	CompletionPort(size_t numThreads, LPTHREAD_START_ROUTINE function);
	virtual ~CompletionPort(void);

	void PostCompletionStatus(const CompletionKey & key, DWORD numberOfBytesTransferred, OVERLAPPED * overlapped);
	void PostCompletionStatusToAll(const CompletionKey & key, DWORD numberOfBytesTransferred, OVERLAPPED * overlapped);

	void TerminateFriendly(bool block);

	bool GetCompletionStatus(CompletionKey *& key, DWORD & bytes, LPOVERLAPPED & overlapped);

	size_t Size();
	void Associate(HANDLE object, const CompletionKey & key);

	static bool TestClass();

};
