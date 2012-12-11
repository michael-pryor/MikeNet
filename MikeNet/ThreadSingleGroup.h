#pragma once
#include "StoreVector.h"
#include "ThreadSingle.h"

/**
 * @brief Manages a group of ThreadSingle objects.
 */
class ThreadSingleGroup : public StoreVector<ThreadSingle>
{
public:
	ThreadSingleGroup();
	virtual ~ThreadSingleGroup(void);

	void TerminateForce(DWORD exitCode);
	virtual void TerminateFriendly(bool block);
	void TerminateNormal(clock_t timeout);
	bool IsRunning() const;
	bool IsSuspended() const;
	void Resume();
	void Suspend();
	void WaitForThreadsToExit();

	static bool TestClass();
};

