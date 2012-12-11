#include "FullInclude.h"

/**
 * @brief	Constructor.
 *
 * @param numThreads Number of worker threads that will manage completion port. Threads must
 * exit without dealing with further completion status' when CompletionKey::SHUTDOWN notification is
 * received.
 * @param function Function to be called by worker threads. Function is passed a pointer to
 * the ThreadSingle object that is managing it. ThreadSingle::GetParameter() will return a
 * a pointer to the CompletionPort object that it is associated with. ThreadSingle::GetManualThreadID()
 * will return a unique thread ID that should be used by the thread when calling any method requiring a thread ID.
 */
CompletionPort::CompletionPort(size_t numThreads, LPTHREAD_START_ROUTINE function)
{
	_ErrorException((numThreads == 0),"starting the completion port, number of threads is 0",0,__LINE__,__FILE__);

	// Setup completion port
	completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,static_cast<DWORD>(numThreads));
	_ErrorException((completionPort==NULL),"creating the completion port",WSAGetLastError(),__LINE__,__FILE__);

	// Setup threads that manage completion port
	for(size_t t = 0;t<numThreads;t++)
	{
		ThreadSingle * newThread = new (nothrow) ThreadSingle(function,this,t);
		Utility::DynamicAllocCheck(newThread,__LINE__,__FILE__);

		newThread->Resume();

		this->Add(newThread);
	}
}

/**
 * @brief	Destructor. 
 */
CompletionPort::~CompletionPort(void)
{
	const char * cCommand = "an internal function (~CompletionPort)";
	try
	{
		this->TerminateFriendly(true);

		BOOL bResult = CloseHandle(completionPort);
		_ErrorException((bResult == FALSE),"closing a completion port handle",WSAGetLastError(),__LINE__,__FILE__);
	}
	MSG_CATCH
}

/**
 * @brief	Posts a completion status. 
 *
 * @param	key							Key to use. 
 * @param	numberOfBytesTransferred	Number of bytes transferred. 
 * @param   [in]	overlapped			Overlapped structure.
 */
void CompletionPort::PostCompletionStatus(const CompletionKey & key, DWORD numberOfBytesTransferred, OVERLAPPED * overlapped)
{
	BOOL result = PostQueuedCompletionStatus(completionPort,numberOfBytesTransferred,(ULONG_PTR)&key,overlapped);
	_ErrorException((result == 0),"posting a completion status",WSAGetLastError(),__LINE__,__FILE__);
}

/**
 * @brief	Posts a completion status to all threads.
 *
 * It is not necessarily guaranteed that each thread will receive the status. If a thread
 * quickly receives a status, acts and then attempts to retrieve another status it may be
 * quick enough to receive two notifications, thus meaning a thread misses out.
 *
 * @param	key							Key to use. 
 * @param	numberOfBytesTransferred	Number of bytes transferred. 
 * @param   [in]	overlapped			Overlapped structure.
 */
void CompletionPort::PostCompletionStatusToAll(const CompletionKey & key, DWORD numberOfBytesTransferred, OVERLAPPED * overlapped)
{
	for(size_t n = 0;n<this->Size();n++)
	{
		PostCompletionStatus(key,numberOfBytesTransferred,overlapped);
	}
}

/**
 * @brief Requests that all threads in this group exit, by posting a completion status.
 *
 * @param block If true this method will not return until the thread has exited.
 */
void CompletionPort::TerminateFriendly( bool block )
{
	// Notify all threads that they should exit.
	// Notification will be received by each thread because
	// thread will not attempt to retrieve another completion status
	// after it has received this notification meaning that it cannot
	// 'steal' other threads notification (see PostCompletionStatusToAll()
	// documentation.
	PostCompletionStatusToAll(CompletionKey::shutdownKey,NULL,NULL);

	// Wait for all threads to exit.
	this->WaitForThreadsToExit();
}

/**
 * @brief	Dequeues a completion status.
 *
 * @param [out]	key			Destination that a pointer to the key of the completion status will be stored.
 * @param [out]	bytes		Destination that number of bytes transferred of completion status will be stored.
 * @param [out]	overlapped	Destination that a pointer to the overlapped structure of the completion status will be stored.
 *
 * @return	true if a status was successfully dequeued, false if not. 
 */
bool CompletionPort::GetCompletionStatus(CompletionKey *& key, DWORD & bytes, LPOVERLAPPED & overlapped)
{
	ULONG_PTR completionKey; // Stores a pointer to CompletionKey object
	BOOL success = GetQueuedCompletionStatus(completionPort,&bytes,&completionKey,&overlapped,INFINITE);

	key = (CompletionKey*)completionKey;

	return (success != FALSE);
}

/**
 * @brief Associates an object with the completion port, so that status indicators
 * can be received by the completion port about that object.
 *
 * @param object Object to associate with completion port.
 * @param key Key associated with object, to uniquely identify it.
 */
void CompletionPort::Associate(HANDLE object, const CompletionKey & key)
{
	HANDLE hResult = CreateIoCompletionPort(object,completionPort,(ULONG_PTR)&key,NULL);
	_ErrorException((hResult==NULL),"associating a socket with the completion port",WSAGetLastError(),__LINE__,__FILE__);
}


/**
 * @brief Test function used by threads.
 *
 * @param lpParameter Pointer to ThreadSingle object, which contains pointer to CompletionPort object to use.
 * @return number of Enter and Leave operations within hard coded time period.
 */
DWORD WINAPI CompletionPortTestFunction(LPVOID lpParameter)
{
	// Get all parameters
	ThreadSingle * thread = static_cast<ThreadSingle*>(lpParameter);
	size_t threadID = thread->GetManualThreadID();
	CompletionPort * completionPort = static_cast<CompletionPort*>(thread->GetParameter());
	ThreadSingle::ThreadSetCallingThread(thread);

	Utility::output.Enter();
	cout << "Completion port worker thread started with ID of " << threadID << '\n';
	cout << "ThreadSingle address: " << thread << ", " << "TLS address: " << ThreadSingle::GetCallingThread() << '\n';
	cout << '\n';
	Utility::output.Leave();

	// Loop until told to shutdown
	while(true)
	{
		// Get completion status
		CompletionKey * completionKey = NULL;
		DWORD completionBytes = 0;
		OVERLAPPED * completionOverlapped = NULL;

		bool success = completionPort->GetCompletionStatus(completionKey,completionBytes,completionOverlapped);

		if(completionKey != NULL)
		{
			// Deal with completion status
			Utility::output.Enter();
			cout << "Completion key of type: " << completionKey->GetType() << " received\n";
			cout << " threadID: " << threadID << '\n';
			cout << " success: " << success << '\n';
			cout << " bytes: " << completionBytes << '\n';
			cout << " overlapped address: " << (size_t)completionOverlapped << '\n';
			cout << '\n';
			Utility::output.Leave();

			// Check for shutdown indication
			if(completionKey->GetType() == CompletionKey::SHUTDOWN)
			{
				Utility::output.Enter();
				cout << "Shutdown request received, thread is exiting\n\n";
				Utility::output.Leave();
				return CompletionKey::SHUTDOWN;
			}
		}
		else
		{
			Utility::output.Enter();
			cout << "Error: " << WSAGetLastError() << '\n';
			system("PAUSE");
			Utility::output.Leave();
		}
	}
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool CompletionPort::TestClass()
{
	cout << "Testing CompletionPort class...\n";

	DWORD speed = 0;
	
	{
		CompletionKey key1(CompletionKey::SOCKET);

		CompletionPort port(4,&CompletionPortTestFunction);

		Sleep(speed);
	
		port.PostCompletionStatus(key1,1234,(OVERLAPPED*)5000);

		Sleep(speed);

		port.PostCompletionStatus(key1,4321,(OVERLAPPED*)6000);

		Sleep(speed);

		// Do not expect each separate thread to receive this completion status.
		// A single thread may deal with multiple status.
		port.PostCompletionStatusToAll(key1,1000,NULL);

		Sleep(speed);
	}

	cout << "\n\n";
	return true;
}


/**
 * @brief Retrieve the number of threads associated with this completion port.
 * 
 * @return number of threads associated with this completion port.
 */
size_t CompletionPort::Size()
{
	return ThreadSingleGroup::Size();
}

