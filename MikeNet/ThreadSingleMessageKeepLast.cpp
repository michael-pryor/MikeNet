#include "FullInclude.h"

/**
 * @brief Constructor, creates a suspended thread.
 *
 * @param function Entry point for thread. A pointer to this object will be passed as a parameter.
 * @param parameter Parameter to be passed to thread. 
 * @param manualThreadID Custom thread ID that can be retrieved by thread (optional, default = 0).
 */
ThreadSingleMessageKeepLast::ThreadSingleMessageKeepLast(LPTHREAD_START_ROUTINE function, void * parameter, size_t manualThreadID)
	: ThreadSingleMessage(function, parameter, manualThreadID)
{
	lastMessage = NULL;
}

/**
 * @brief	Destructor. 
 */
ThreadSingleMessageKeepLast::~ThreadSingleMessageKeepLast()
{
	const char * cCommand = "An internal function (~ThreadSingleMessageKeepLast)";
	
	try
	{
		TerminateFriendly(true);

		if(lastMessage != NULL)
		{
			lastMessage->WaitUntilNotInUseByThread();
			lastMessage->ShouldSenderCleanup();
			delete lastMessage;
		}
	}
	MSG_CATCH
}

/**
 * @brief	Retrieves the last message that was sent to the thread.
 *
 * @return	the last message sent. This pointer will become invalid when the next message is sent.
 * @return  NULL if no message has been sent yet.
 */
const ThreadMessageItem * ThreadSingleMessageKeepLast::GetLastMessage() const
{
	return lastMessage;
}

/**
 * @brief	Extracts the last message that was sent to the controller thread,
 * so that it will not be automatically cleaned up.
 *
 * @return	the last message sent. This pointer should be cleaned up using delete. Make sure
 * to set in use by sender to false before attempting to cleanup.
 * @return  NULL if no message has been sent yet.
 */
const ThreadMessageItem * ThreadSingleMessageKeepLast::ExtractLastMessage()
{
	ThreadMessageItem * returnMe = lastMessage;
	lastMessage = NULL;
	return returnMe;
}

/**
 * @brief	Posts a message to the thread to be received using ThreadSingleMessage::GetMessageItem().
 *
 * @param	message	The message to post.
 * @param [in]	customLastMessage	Non NULL if this local last message should be used, NULL if the global last message should be used.
 */
void ThreadSingleMessageKeepLast::PostMessageItem( ThreadMessageItem * message, ThreadMessageItem ** customLastMessage)
{
	ThreadMessageItem ** lastMessageToActOn = GetLastMessageToActOn(customLastMessage);

	CleanupLastMessage(lastMessageToActOn);

	*lastMessageToActOn = message;
	ThreadSingleMessage::PostMessageItem(message);
}

/**
 * @brief	Posts a message to the thread to be received using ThreadSingleMessage::GetMessageItem().
 *
 * We must have this method in order to override base class.
 *
 * @param	message	The message to post.
 */
void ThreadSingleMessageKeepLast::PostMessageItem( ThreadMessageItem * message )
{
	PostMessageItem(message, NULL);
}

/**
 * @brief	Retrieves the last message that should be acted on.
 *
 * @param [in]	customLastMessage	Non NULL if this local last message should be used, NULL if the global last message should be used.
 *
 * @return	the last message that should be acted on.
 */
ThreadMessageItem ** ThreadSingleMessageKeepLast::GetLastMessageToActOn( ThreadMessageItem ** customLastMessage )
{
	ThreadMessageItem ** lastMessageToActOn = NULL;
	if(customLastMessage == NULL)
	{
		lastMessageToActOn = &lastMessage;
	}
	else
	{
		lastMessageToActOn = customLastMessage;
	}
	return lastMessageToActOn;
}


/**
 * @brief	Cleans up the message that was last sent.
 *
 * @param [in]	lastMessageToActOn	Last message to cleanup.
 */
void ThreadSingleMessageKeepLast::CleanupLastMessage(ThreadMessageItem ** lastMessageToActOn)
{
	if(*lastMessageToActOn != NULL)
	{
		bool shouldCleanup = (*lastMessageToActOn)->ShouldSenderCleanup();
		if(shouldCleanup == true)
		{
			delete *lastMessageToActOn;
			*lastMessageToActOn = NULL;
		}
	}
}

/**
 * @brief Requests that the thread exits.
 *
 * The thread is not guaranteed to exit as it may not be checking GetTerminateRequest().
 * If it is performing an operation it may also not exit straight away.
 * Use TerminateForce to guarantee quick thread exit.
 *
 * @param block Ignored, will always block.
 */
void ThreadSingleMessageKeepLast::TerminateFriendly(bool block)
{
	if(IsRunning() == true)
	{
		ThreadSingleMessage::TerminateFriendly(block);

		// ThreadSingleMessage::TerminateFriendly cleans up last send operation.
		lastMessage = NULL;
	}
}

/**
 * @brief	Thread message item test. 
 * @remarks	Michael Pryor, 9/16/2010. 
 */
class ThreadMessageItemTestTwo : public ThreadMessageItem
{
	/**
	 * @brief Value for TakeAction to use.
	 */
	clock_t storedValue;
public:
	/**
	 * @brief	Constructor. 
	 *
	 * @param	valueToDisplay	The value to display. 
	 */
	ThreadMessageItemTestTwo(clock_t valueToDisplay)
	{
		storedValue = valueToDisplay;
	}

	/**
	 * @brief	Does nothing.
	 *
	 * @return	NULL.
	 */
	void * TakeAction()
	{
		return NULL;
	}
};

/**
 * @brief Test function used by threads.
 *
 * @param lpParameter Pointer to ThreadSingle object.
 */
DWORD WINAPI ThreadSingleMessageKeepLastTestFunction(LPVOID lpParameter)
{
	ThreadSingleMessage * thread = (ThreadSingleMessage*)lpParameter;
	ThreadSingle::ThreadSetCallingThread(thread);

	// Parameter
	Utility::output.Enter();
	cout << "Thread started\n";
	Utility::output.Leave();

	// Friendly termination
	while(thread->GetTerminateRequest() == false)
	{
		ThreadMessageItem * item = thread->GetMessageItem();

		item->TakeAction();

		bool shouldCleanup = item->ShouldThreadCleanup();
		if(shouldCleanup == true)
		{
			Utility::output.Enter();
			cout << "Thread is cleaning up a message!\n";
			Utility::output.Leave();
			delete item;
		}
		else
		{
			Utility::output.Enter();
			cout << "Sender is cleaning up a message!\n";
			Utility::output.Leave();
		}
	}

	Utility::output.Enter();
	cout << "Thread terminating..\n";
	Utility::output.Leave();

	return (1234);
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool ThreadSingleMessageKeepLast::TestClass()
{
	cout << "Testing ThreadSingleMessageKeepLast class...\n";
	bool problem = false;

	{
		ThreadSingleMessageKeepLast thread(&ThreadSingleMessageKeepLastTestFunction,NULL,false);
	
		Timer loopTimer(10000);
		while(loopTimer.GetState() != true)
		{
			Sleep(100);
			thread.PostMessageItem(new ThreadMessageItemTestTwo(clock()));
			thread.PostMessageItem(new ThreadMessageItemTestTwo(clock()));
			thread.PostMessageItem(new ThreadMessageItemTestTwo(clock()));
		}
	}

	cout << "\n\n";
	return !problem;
}