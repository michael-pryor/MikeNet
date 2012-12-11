#include "FullInclude.h"
#include "ThreadSingleMessage.h"
#include "ThreadMessageItem.h"
#include "ThreadMessageItemShutdown.h"

const HWND ThreadSingleMessage::THIS_QUEUE_ONLY((HWND)-1);

/**
 * @brief Constructor, creates a suspended thread.
 *
 * @param function Entry point for thread. A pointer to this object will be passed as a parameter.
 * @param parameter Parameter to be passed to thread. 
 * @param manualThreadID Custom thread ID that can be retrieved by thread (optional, default = 0).
 */
ThreadSingleMessage::ThreadSingleMessage(LPTHREAD_START_ROUTINE function, void * parameter, size_t manualThreadID) : threadReady(false), ThreadSingle(function,parameter,manualThreadID)
{

}

/**
 * @brief	Destructor. 
 */
ThreadSingleMessage::~ThreadSingleMessage()
{
	const char * cCommand = "An internal function (~ThreadSingleMessage)";
	try
	{
		TerminateFriendly(true);
	}
	MSG_CATCH
}

/**
 * @brief	Does not return until thread is ready for messages to be sent.
 */
void ThreadSingleMessage::WaitForThreadToBeReady()
{
	this->Resume();
	threadReady.WaitUntilSignaled();
}

/**
 * @brief	Posts a message to the thread to be received using GetMessageItem().
 *
 * @param	message	The message to post.
 */
void ThreadSingleMessage::PostMessageItem( ThreadMessageItem * message )
{
	WaitForThreadToBeReady();

	message->SetMessageInUseByThread(true);
	message->SetMessageInUseBySender(true);

	BOOL result = PostThreadMessage(GetThreadID(),NULL,NULL,reinterpret_cast<LPARAM>(message));
	_ErrorException((result == FALSE),"posting a thread message item",WSAGetLastError(),__LINE__,__FILE__);
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
void ThreadSingleMessage::TerminateFriendly(bool block)
{
	if(IsRunning() == true)
	{
		// Post message so that thread stops waiting and checks GetTerminateRequest.
		//
		// Note for compatibility with ThreadSingleMessageKeepLast, it does not matter
		// that terminateMessage is not dynamically allocated. The thread will never
		// try to clean it up, because it will be seen as in use by the sender.
		ThreadMessageItemShutdown terminateMessage(this);

		// Help thread exit.
		PostMessageItem(&terminateMessage);

		// Wait for thread to exit
		this->WaitForThreadToExit();

		// In case thread did not use terminateMessage to help it exit.
		terminateMessage.SetMessageInUseByThread(false);

		// Cleanup message
		terminateMessage.ShouldSenderCleanup();

		// Set running to false after shutting down thread
		// because we don't want thread to terminate unexpectedly,
		// causing PostMessageItem to fail.
		running = false;
	}
}


/**
 * @brief	Retrieves a message from the message queue.
 *
 * Messages are posted using PostMessageItem.
 *
 * @return	the message item that was retrieved.
 */
ThreadMessageItem * ThreadSingleMessage::GetMessageItem()
{
getAnotherMessage:
	MSG message;

	// Finish setting up message queue.
	if(threadReady.Get() == false)
	{
		PeekMessage(&message, THIS_QUEUE_ONLY, 0, 0, PM_NOREMOVE);
		threadReady.Set(true);
	}

	BOOL result = GetMessage(&message, THIS_QUEUE_ONLY, 0, 0);
	_ErrorException((result == -1),"retrieving a message item from the queue",WSAGetLastError(),__LINE__,__FILE__);

	// Sometimes NULL messages are received, we ignore these
	if(message.lParam == NULL)
	{
		goto getAnotherMessage;
	}

	return reinterpret_cast<ThreadMessageItem*>(message.lParam);
}

/**
 * @brief	Determines whether the message queue is empty.
 *
 * @return	true if the queue is empty, false if not. 
 */
bool ThreadSingleMessage::IsQueueEmpty() const
{
	MSG message;
	BOOL result = PeekMessage(&message, THIS_QUEUE_ONLY, 0, 0, PM_NOREMOVE);
	return result == FALSE;
}

/**
 * @brief	Determines whether the thread should exit.
 *
 * The thread should check this regularly and exit when this method returns true.\n\n
 *
 * This method will only return true when the message queue is empty.
 *
 * @return	true if the thread should exit, false if not.
 */
bool ThreadSingleMessage::GetTerminateRequest() const
{
	return IsQueueEmpty() && ThreadSingle::GetTerminateRequest();
}

/**
 * @brief	Thread message item test. 
 * @remarks	Michael Pryor, 9/16/2010. 
 */
class ThreadMessageItemTest : public ThreadMessageItem
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
	ThreadMessageItemTest(clock_t valueToDisplay)
	{
		storedValue = valueToDisplay;
	}

	/**
	 * @brief	Write text to the screen and display our stored value.
	 *
	 * @return	NULL.
	 */
	void * TakeAction()
	{
		Utility::output.Enter();
		cout << "Action is being taken and we have a stored value of " << storedValue << '\n';
		Utility::output.Leave();
		return NULL;
	}
};



/**
 * @brief Test function used by threads.
 *
 * @param lpParameter Pointer to ThreadSingle object.
 */
DWORD WINAPI ThreadSingleMessageTestFunction(LPVOID lpParameter)
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

		if(item != NULL)
		{
			item->TakeAction();

			bool shouldCleanup = item->ShouldThreadCleanup();
			if(shouldCleanup == true)
			{
				Utility::output.Enter();
				cout << "Thread is deleting a message\n";
				Utility::output.Leave();
				delete item;
			}
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
bool ThreadSingleMessage::TestClass()
{
	cout << "Testing ThreadSingleMessage class...\n";
	bool problem = false;

	{
		ThreadSingleMessage thread(&ThreadSingleMessageTestFunction,NULL);
		thread.Resume();
	
		Timer loopTimer(10000);
		while(loopTimer.GetState() != true)
		{
			Sleep(100);
			ThreadMessageItemTest * message = new ThreadMessageItemTest(clock());
			thread.PostMessageItem(message);

			message->WaitUntilNotInUseByThread();

			bool shouldCleanup = message->ShouldSenderCleanup();
			if(shouldCleanup == true)
			{
				Utility::output.Enter();
				cout << "Sender is deleting a message\n";
				Utility::output.Leave();
				delete message;
			}
		}
	}

	cout << "\n\n";
	return !problem;
}