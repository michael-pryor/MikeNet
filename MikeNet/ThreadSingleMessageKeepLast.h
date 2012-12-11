#pragma once
#include "threadsinglemessage.h"

/**
 * @brief	Message based thread where only the last message is stored and others are cleaned up.
 *
 * The system that ensures messages are cleaned up correctly is rather complex. Either the thread (receiver)
 * or the sender may clean up a message. A message is deemed to be still in use by the sender (and therefore not
 * cleaned up by the receiver) if it is stored in ThreadSingleMessageKeepLast::lastMessage. A message is stored
 * here if it is the last message to be sent using ThreadSingleMessageKeepLast::PostMessageItem. If two messages
 * are sent simultaneously then the sender will attempt to cleanup the first message that was sent, but if the receiver has
 * not finished using it then the sender will not clean it up. When the receiver finishes using the message, it will
 * see that the sender has finished using the message, and so it will clean up the message itself. However,
 * if the sender has not finished using the message then the receiver will not clean it up, and will let the sender
 * do this at a later time. Note that in the event that the last message sent does not change (i.e. New messages are not sent),
 * then it will always be deemed to be in use by the sender, so that users can poll on its status and determine whether its operation
 * has completed. In this case the destructor of this object will clean it up.
 *
 * @remarks	Michael Pryor, 9/17/2010. 
 */
class ThreadSingleMessageKeepLast :
	public ThreadSingleMessage
{
	/**
	 * @brief Last message that was sent to thread.
	 *
	 * Only the last message is stored since you would only ever need
	 * to poll on the last message. Messages are dealt with in order
	 * so by waiting for the last message to complete, you are also waiting
	 * for any previous messages to complete.
	 */
	ThreadMessageItem * lastMessage;

	/**
	 * @brief Points to ThreadSingleMessageKeepLast::lastMessage or a custom last message.
	 *
	 * Points to ThreadSingleMessageKeepLast::lastMessage if ThreadSingleMessageKeepLast::PostMessageItem
	 * is to be used by one thread only. If multiple threads may post messages simultaneously then a
	 * local last message should be stored and ThreadSingleMessageKeepLast::lastMessage should be NULL.
	 */
	ThreadMessageItem ** ptrLastMessage;

	ThreadMessageItem ** GetLastMessageToActOn(ThreadMessageItem ** customLastMessage);
	void CleanupLastMessage(ThreadMessageItem ** lastMessageToActOn);
public:
	ThreadSingleMessageKeepLast(LPTHREAD_START_ROUTINE function, void * parameter, size_t manualThreadID=0);
	virtual ~ThreadSingleMessageKeepLast();

	void PostMessageItem(ThreadMessageItem * message, ThreadMessageItem ** lastMessage);
	void PostMessageItem(ThreadMessageItem * message);

	const ThreadMessageItem * GetLastMessage() const;
	const ThreadMessageItem * ExtractLastMessage();

	static bool TestClass();
	void TerminateFriendly(bool block);
};

