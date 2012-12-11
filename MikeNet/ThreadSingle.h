#pragma once
#include "ConcurrentObject.h"
#include "ErrorReport.h"

/**
 * @brief	A single thread, the thread's life time is the life time of this object.
 * @remarks	Michael Pryor, 8/25/2010. 
 */
class ThreadSingle
{
	/** 
	 * @brief Handle to thread, used to reference thread when taking action on it.
	 */
	HANDLE handle;

	/**
	 * @brief After thread is created, this is loaded with an ID unique to that thread.
	 */
	DWORD threadID;

	/**
	 * @brief Pointer to this object is passed to thread, so thread can access this parameter,
	 * which is set in the constructor.
	 */
	void * parameter;

protected:
	/**
	 * @brief Set to false when the thread should exit.
	 */
	ConcurrentObject<bool> running;
public:
	/**
	 * @brief True if the thread is suspended.
	 *
	 * A thread can be created suspended by the constructor.
	 */
	bool suspended;

	/**
	 * @brief Thread ID manually set.
	 */
	size_t manualThreadID;

	/**
	 * @brief The number of threads using thread local storage, if 0 then thread local storage is not allocated.
	 *
	 * Increases by 1 with every AllocateThreadLocalStorage, decreases by 1 with every DeallocateThreadLocalStorage.
	 */
	static ConcurrentObject<size_t> threadLocalStorageAllocated;

	/**
	 * @brief Index of thread local storage, used to identify storage space.
	 */
	static DWORD threadLocalStorageIndex;

	/**
	 * @brief Non NULL if an error occurred in the thread.
	 */
	ConcurrentObject<ErrorReport*> threadError;

public:
	static void AllocateThreadLocalStorage();
	static void DeallocateThreadLocalStorage();
	static bool IsThreadLocalStorageAllocated();

private:
	static DWORD GetThreadLocalStorageIndex();

	static void ThreadSetLocalStorageValue(void * value);
	static void * ThreadGetLocalStorageValue();
public:
	static ThreadSingle * GetCallingThread();
	static void ThreadSetCallingThread(ThreadSingle * object);

	ThreadSingle(LPTHREAD_START_ROUTINE function, void * parameter, size_t manualThreadID = 0);
	virtual ~ThreadSingle(void);

	void * GetParameter() const;
	DWORD GetThreadID() const;
	size_t GetManualThreadID() const;
	void TerminateForce(DWORD exitCode);
	virtual void TerminateFriendly(bool block);
	void TerminateNormal(clock_t timeout);
	bool IsRunning() const;
	virtual bool GetTerminateRequest() const;
	void RequestTerminate();
	DWORD GetExitCode() const;
	bool IsSuspended() const;
	void Resume();
	void Suspend();

	void WaitForThreadToExit();

	void SetError(ErrorReport * report);
	ErrorReport * GetError() const;
	bool IsErrorStored() const;
	void ClearError();

	static size_t GetNumLogicalCores();

	static bool TestClass();
};

