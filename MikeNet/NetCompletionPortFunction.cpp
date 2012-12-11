#include "FullInclude.h"

/**
 * @brief Thread function which manages a completion port,
 * cleaning up send operations and dealing with received data.
 */
DWORD WINAPI NetManageCompletionPort(LPVOID lpParameter)
{
	const char * cCommand = "an internal function (NetManageCompletionPort)";


	try
	{
		// Get all parameters
		ThreadSingle * thread = static_cast<ThreadSingle*>(lpParameter);
		size_t threadID = thread->GetManualThreadID();
		CompletionPort * completionPort = static_cast<CompletionPort*>(thread->GetParameter());
		ThreadSingle::ThreadSetCallingThread(thread);

		bool bRunning = true;
		while(bRunning == true)
		{
			// Get completion status
			CompletionKey * completionKey = NULL;
			DWORD completionBytes = 0;
			OVERLAPPED * completionOverlapped = NULL;

			//Utility::output.Enter();
			//cout << "Thread " << threadID << " getting completion status...\n";
			//Utility::output.Leave();

			bool success = completionPort->GetCompletionStatus(completionKey,completionBytes,completionOverlapped);
			_ErrorException((completionKey == NULL),"retrieving a completion status",WSAGetLastError(),__LINE__,__FILE__);

			size_t getCompletionStatusLastError = WSAGetLastError();

			//Utility::output.Enter();
			//cout << "Thread " << threadID << " got completion status...\n";
			//Utility::output.Leave();

			// Determine whether instance is shutting down
			if(completionBytes == 0)
			{
				success = false;
			}
			bool shuttingDown = (success == false && (getCompletionStatusLastError == WSA_OPERATION_ABORTED));

			// Instance initiated operation
			if(completionKey->IsOwnedByInstance() == true)
			{
				NetInstance * instance = completionKey->GetInstance();

				try
				{
					bool isRecvOperation = completionKey->GetSocket()->IsOurOverlapped(completionOverlapped);
					bool isSendOperation = !isRecvOperation;
					size_t clientID = completionKey->GetClientID();
					NetSocket * socket = completionKey->GetSocket();

					try
					{
						if(completionBytes == 0)
						{
							success = false;
						}

						shuttingDown = (success == false && (getCompletionStatusLastError == WSA_OPERATION_ABORTED));

						if(isRecvOperation == true)
						{
							if(success == false)
							{
								//Utility::output.Enter();
								//cout << "Could not deal with receive operation, reason: " << getCompletionStatusLastError << '\n';
								//Utility::output.Leave();

								if(shuttingDown == false)
								{
									instance->CompletionError(socket,clientID);
								}

								// Indicate that we have completely finished receiving and dealing with receive data
								// Socket will wait until this has happened before cleaning up
								socket->SetCompletionPortFinishRecvNotification();

								// UDP receive operations sometimes fail and are not catastrophic,
								// so we should always try to start another receive operation
								// unless the instance is shutting down.
								if(shuttingDown == false && socket->GetProtocol() == NetSocket::UDP)
								{
									instance->DoRecv(socket,clientID);
								}
							}
							else
							{
								instance->DealCompletion(socket,completionBytes,clientID);

								// Start another receive operation	
								instance->DoRecv(socket,clientID);
							}
						}
						// isSendOperation = true
						else
						{
							instance->CompletedSendOperation(socket,completionOverlapped,success,shuttingDown,clientID);
						}
					}
					// Try to terminate individual client.
					catch(ErrorReport & error) {instance->CompletionError(socket,clientID);}
				}
				// Shutdown entire instance because critical error occurred.
				catch(ErrorReport & error){instance->RequestDestroy();}
				catch(...){instance->RequestDestroy();}
			}
			// Instance did not initiate operation
			else
			{
				switch(completionKey->GetType())
				{
				case(CompletionKey::SOCKET):
				{
					bool isRecvOperation = completionKey->GetSocket()->IsOurOverlapped(completionOverlapped);
					bool isSendOperation = !isRecvOperation;
					
					if(isRecvOperation == true)
					{
						if(success == false)
						{
							if(shuttingDown == false)
							{
								// Request that socket be closed
								completionKey->GetSocket()->CompletionPortRequestClose();
							}

							// Indicate that we have completely finished receiving and dealing with receive data
							// Socket will wait until this has happened before cleaning up
							completionKey->GetSocket()->SetCompletionPortFinishRecvNotification();

							// UDP receive operations sometimes fail and are not catastrophic,
							// so we should always try to start another receive operation
							// unless the instance is shutting down.
							if(shuttingDown == false && completionKey->GetSocket()->GetProtocol() == NetSocket::UDP)
							{
								completionKey->GetSocket()->Recv();
							}
						}
						else
						{
							try
							{
								// Deal with received data
								completionKey->GetSocket()->DealWithData(completionKey->GetSocket()->recvBuffer,completionBytes,completionKey->GetSocket()->GetRecvFunction(),0,0);

								// Indicate that we have completely finished receiving and dealing with receive data
								// MUST do before starting new receive operation
								completionKey->GetSocket()->SetCompletionPortFinishRecvNotification();

								// Start another receive operation	
								bool error = completionKey->GetSocket()->Recv();

								// Request that socket be closed upon error
								if(error && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAENOTSOCK)
								{
									completionKey->GetSocket()->CompletionPortRequestClose();
								}
							}
							// Request that socket be closed in the event of an error
							catch(ErrorReport & error){	completionKey->GetSocket()->CompletionPortRequestClose(); }
							catch(...){ completionKey->GetSocket()->CompletionPortRequestClose(); }
						}
					}
					// isSendOperation = true
					else
					{
						completionKey->GetSocket()->CompletedSendOperation(completionOverlapped,success,shuttingDown);
					}
				}
					break;

				case(CompletionKey::SHUTDOWN):
					bRunning = false;
					break;
				}
			}
		}
	}
	MSG_CATCH

	return(0);
}
