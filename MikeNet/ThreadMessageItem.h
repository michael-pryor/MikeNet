#pragma once

/**
 * @brief	Represents an individual message that can be sent to a thread via ThreadSingleMessage.
 * @remarks	Michael Pryor, 9/16/2010. 
 *
 * Before a message is sent ThreadSingleMessage will call ThreadMessageItem::SetMessageInUseByThread with a
 * parameter of true. After the thread has finished using the message it must call SetMessageInUseByThread
 * with a parameter of false.
 */
class ThreadMessageItem
{
	/**
	 * @brief False when this message object is in use by the thread.
	 *
	 * Must use mtEventObjects when changing or checking state in order
	 * to synchronize with messageNotInUseBySender.
	 */
	ConcurrencyEvent messageNotInUseByThread;

	/**
	 * @brief False when the message object is not in use by the sender
	 * and so should be cleaned up by the thread.
	 *
	 * Must use mtEventObjects when changing or checking state in order
	 * to synchronize with messageNotInUseByThread.
	 */
	ConcurrencyEvent messageNotInUseBySender;

	/**
	 * @brief Synchronizes messageNotInUseByThread and messageNotInUseBySender so that
	 * actions can be taken based on both without worrying about their status changing independently.
	 *
	 * This is used only during the cleanup phase.
	 */
	CriticalSection mtEventObjects;
public:
	ThreadMessageItem();
	virtual ~ThreadMessageItem();

	void SetMessageInUseByThread(bool option);
	bool IsMessageInUseByThread() const;
	void WaitUntilNotInUseByThread(DWORD timeout) const;
	void WaitUntilNotInUseByThread() const;

	void SetMessageInUseBySender(bool option);
	bool IsMessageInUseBySender() const;
	void WaitUntilNotInUseBySender(DWORD timeout) const;
	void WaitUntilNotInUseBySender() const;

	bool ShouldThreadCleanup();
	bool ShouldSenderCleanup();


	virtual void * TakeAction();
};

