#include "FullInclude.h"

/**
 * @brief Initializes object.
 *
 * @param p_decryptKey Pointer (casted to long long int) to a EncryptKey object to be used to decrypt UDP data.
 * @param memoryRecycleUDP		 If non NULL, memory recycle used to manage incoming UDP packets (default = NULL).
 * @param recvMemoryLimitTCP Maximum amount of memory that can be allocated to TCP receive operations (default = NetInstanceProfile::DEFAULT_RECV_MEMORY_LIMIT). See @ref securityPage "security" for more information.
 * @param recvMemoryLimitUDP Maximum amount of memory that can be allocated to UDP receive operations per client (default = NetInstanceProfile::DEFAULT_RECV_MEMORY_LIMIT). See @ref securityPage "security" for more information.
 * @param sendMemoryLimitTCP Maximum amount of memory that can be allocated to TCP send operations (default = NetInstanceProfile::DEFAULT_SEND_MEMORY_LIMIT). See @ref securityPage "security" for more information.
 * @param sendMemoryLimitUDP Maximum amount of memory that can be allocated to UDP send operations (default = NetInstanceProfile::DEFAULT_SEND_MEMORY_LIMIT). See @ref securityPage "security" for more information.
 *
 * @warning SocketUDP Must be initialized before using this method.
 */
void NetInstanceClient::Initialize(const EncryptKey * p_decryptKey, const MemoryRecyclePacketRestricted * memoryRecycleUDP, size_t recvMemoryLimitTCP, size_t recvMemoryLimitUDP, size_t sendMemoryLimitTCP, size_t sendMemoryLimitUDP)
{
	ExceptionValidateRecvSizeUDP();

	this->clientID = 0;
	this->maxClients = 0;
	this->handshakeErrorOccurred = false;
	this->timeoutMilliseconds = NULL;
	this->handshakeThread = NULL;

	this->SetSendMemoryLimitTCP(sendMemoryLimitTCP);
	this->SetRecvMemoryLimitTCP(recvMemoryLimitTCP);

	if(IsEnabledUDP() == true)
	{
		this->recvSizeUDP = socketUDP->GetRecvBufferLength();

		this->SetSendMemoryLimitUDP(sendMemoryLimitUDP);

		if(p_decryptKey == NULL)
		{
			this->decryptKey = NULL;
		}
		else
		{
			this->decryptKey = new (nothrow) EncryptKey(*p_decryptKey);
			Utility::DynamicAllocCheck(this->decryptKey,__LINE__,__FILE__);
		}

		if(memoryRecycle != NULL)
		{
			this->memoryRecycle = new (nothrow) MemoryRecyclePacketRestricted(*memoryRecycleUDP);
			Utility::DynamicAllocCheck(this->memoryRecycle,__LINE__,__FILE__);

			this->memoryRecycle->SetMemoryLimit(recvMemoryLimitUDP);
		}
		else
		{
			this->memoryRecycle = NULL;
		}
	}
	else
	{
		this->recvSizeUDP = 0;
		this->decryptKey = NULL;
		this->memoryRecycle = NULL;
	}
}
/**
 * @brief Constructor.
 *
 * @param [in] p_socketTCP Socket to use to send and receive TCP data. This pointer and its data
 * is now owned by this object and should not be used elsewhere.
 * @param [in] p_socketUDP Socket to use to send and receive UDP data, NULL if UDP is disabled.
 * This pointer and its data is now owned by this object and should not be used elsewhere.
 * @param memoryRecycleUDP		 If non NULL, memory recycler used to manage incoming UDP packets.
 * @param p_handshakeEnabled True if the TCP handshaking process is enabled, false if not.
 * For more information see @ref handshakePage "server/client handshaking process".
 * @param p_sendTimeout Length of time in milliseconds to wait for a send operation to complete before
 * disconnecting client (optional, default INFINITE).
 * @param p_decryptKey Pointer to a EncryptKey object to be used to decrypt UDP data (optional, default NULL).
 * @param p_instanceID Unique identifier for instance (optional, default 0).
 * @throws ErrorReport If p_socketTCP->GetRecvBufferLength() < GetRecvSizeMinTCP().
 */
NetInstanceClient::NetInstanceClient(NetSocketTCP * p_socketTCP, NetSocketUDP * p_socketUDP, const MemoryRecyclePacketRestricted * memoryRecycleUDP, bool p_handshakeEnabled, unsigned int p_sendTimeout, const EncryptKey * p_decryptKey, size_t p_instanceID) :
		connectionStatus(NetUtility::NOT_CONNECTED),
		NetInstance(p_instanceID,NetInstance::CLIENT,p_sendTimeout),
		NetInstanceImplementedTCP(p_socketTCP,p_handshakeEnabled),
		NetInstanceUDP(p_socketUDP)
{
	_ErrorException((ValidateRecvSizeTCP(p_socketTCP->GetRecvBufferLength()) != true),"initializing a TCP based instance of client type, receive buffer size is too small",0,__LINE__,__FILE__);
	Initialize(p_decryptKey,memoryRecycleUDP);
}

/**
 * @brief Constructor, using profile instead of individual parameters.
 *
 * @param p_profile Instance profile containing parameters.
 * @param p_instanceID Unique identifier for instance (optional, default 0).
 * @throws ErrorReport If p_profile.GetRecvSizeTCP() < GetRecvSizeMinTCP().
 */
NetInstanceClient::NetInstanceClient(const NetInstanceProfile & p_profile, size_t p_instanceID) :
		connectionStatus(NetUtility::NOT_CONNECTED),
		NetInstance(p_instanceID,NetInstance::CLIENT,p_profile.GetSendTimeout()),
		NetInstanceImplementedTCP
		(
			static_cast<NetSocketTCP*>
			(
				Utility::DynamicAllocCheck
				(
					new (nothrow) NetSocketTCP
					(
						p_profile.GetWsaRecvSizeTCP(),
						p_profile.GetLocalAddrTCP(),
						p_profile.IsNagleEnabled(),
						p_profile.IsGracefulDisconnectEnabled(),
						p_profile.GenerateObjectModeTCP(),
						p_profile.GetRecvFuncTCP()
					)
				,__LINE__,__FILE__)
			),

			p_profile.IsHandshakeEnabled()
		),

		NetInstanceUDP
		(
			p_profile.GenerateObjectSocketUDP
			(
				p_profile.GetRecvSizeUDP(),
				p_profile.GetLocalAddrUDP(),
				p_profile.IsReusableUDP(),
				NULL,
				p_profile.GetRecvFuncUDP()
			)
		)
{
	_ErrorException((ValidateRecvSizeTCP(p_profile.GetRecvSizeTCP()) != true),"initializing a TCP based instance of client type, receive buffer size is too small",0,__LINE__,__FILE__);

	Initialize(p_profile.GetDecryptKeyUDP(), &p_profile.GetMemoryRecyclePacketUDP(), p_profile.GetRecvMemoryLimitTCP(), p_profile.GetRecvMemoryLimitUDP(),p_profile.GetSendMemoryLimitTCP(),p_profile.GetRecvMemoryLimitUDP());
}

/**
 * @brief Destructor.
 *
 * CloseSockets must be used in the destructor.
 */
NetInstanceClient::~NetInstanceClient()
{
	const char * cCommand = "an internal function (~NetInstanceClient)";
	try
	{
		delete handshakeThread; // shutdown handshake thread first
		delete decryptKey;
		delete memoryRecycle;
		CloseSockets();
	}
	MSG_CATCH
}


/**
 * @brief	Closes any sockets in use by the instance.
 *
 * Must be called in the destructor so that instance is not cleaned up until completion
 * port is done using it.
 */
void NetInstanceClient::CloseSockets()
{
	NetInstanceUDP::CloseSockets();
	NetInstanceImplementedTCP::CloseSockets();
}

/**
 * @brief Retrieves the minimum TCP receive buffer size.
 * 
 * @return the minimum TCP receive buffer size.
 */
size_t NetInstanceClient::GetRecvSizeMinTCP() const
{
	return recvSizeMinTCP;
}

/**
 * @brief Retrieves the minimum UDP receive buffer size.
 * 
 * @return the minimum UDP receive buffer size.
 */
size_t NetInstanceClient::GetRecvSizeMinUDP() const
{
	return recvSizeMinUDP;
}

/**
 * @brief Length of time in milliseconds that handshaking process
 * should be allowed before aborting the process.
 *
 * @return length of time in milliseconds that handshaking process
 * should be allowed before aborting the process.
 */
size_t NetInstanceClient::GetConnectTimeout() const
{
	return timeoutMilliseconds;
}

/**
 * @brief Performs handshaking process with server.
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 * @note Method must be defined outside of class exactly as is for winsock to accept it.
 * @param [in] pointer Pointer to ThreadSingle object managing this thread. The parameter stored should be
 * a pointer to the NetInstanceClient object that should be used.
 *
 * @return NetUtility::CONNECTED If the handshaking process completed successfully and the client is now fully connected.
 * @return NetUtility::TIMED_OUT If the handshaking process timed out.
 * @return NetUtility::CONNECTION_ERROR If an error occurred.
 */
DWORD WINAPI ThreadHandshakeRoutineStatic(LPVOID lpParameter)
{
	ThreadSingle * thread = static_cast<ThreadSingle*>(lpParameter);
	ThreadSingle::ThreadSetCallingThread(thread);
	NetInstanceClient * client = static_cast<NetInstanceClient*>(thread->GetParameter());
	
	return client->ThreadHandshakeRoutine();
}

/**
 * @brief	Used by handshaking thread to determine whether the process should be aborted. 
 *
 * @param	timeout	Object used to determine when the connection process has timed out.
 *
 * @return NetUtility::TIME_OUT if the handshaking process timed out, or if the process was terminated upon request.
 * @return NetUtility::CONNECTION_ERROR if an error occurred in the completion port.
 * @return NetUtility::CONNECTING if the process is still in progress with no critical problems.
 */
NetUtility::ConnectionStatus NetInstanceClient::HelperHandshakeStatus(const Timer & timeout)
{
	ThreadSingle * thread = ThreadSingle::GetCallingThread();

	// Timeout or manual termination.
	if(timeout.GetState() == true || thread->GetTerminateRequest() == true)
	{
		return NetUtility::TIMED_OUT;
	}

	// Error occurred in completion port.
	if(connectionStatus.Get() != NetUtility::CONNECTING)
	{
		return NetUtility::CONNECTION_ERROR;
	}

	return NetUtility::CONNECTING;
}

/**
 * @brief Negotiates a connection with the server.
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @warning Receive operations are canceled when the thread who initiated them exits
 * this means that initiating a receive operation that is not guaranteed to have
 * completed before this thread exits is wrong. Note that if the operation completes
 * the completion port will then initiate a new one which is not bound to this thread.
 *
 * @return NetUtility::CONNECTED if the handshaking process completed successfully and the client is now fully connected.
 * @return NetUtility::TIMED_OUT if the handshaking process timed out or was manually aborted by manipulating @a thread.
 * @return NetUtility::CONNECTION_ERROR if an error occurred.
 */
NetUtility::ConnectionStatus NetInstanceClient::ThreadHandshakeRoutine()
{
	try
	{
		Timer connectionTimeout(static_cast<clock_t>(GetConnectTimeout()));
		NetUtility::ConnectionStatus status;

		// Connect via TCP
		bool inProgress = false;
		do
		{
			inProgress = socketTCP->PollConnect();

			status = HelperHandshakeStatus(connectionTimeout);
			if(status != NetUtility::CONNECTING)
			{
				return status;
			}
		} while(inProgress == true);

		/**
		 * If TCP handshake is not enabled then we don't need
		 * to wait for a TCP packet from the server and the connection
		 * process is now complete.
		 */
		if(IsHandshakeEnabled() == false)
		{
			return NetUtility::CONNECTED;
		}
		else
		{
			/**
			 * Start receiving via TCP.
			 * When thread exits further receive operations will be canceled.
			 * It is okay to do this here because thread will receive data before exiting.
			 */
			DoRecv(socketTCP);
		}
		
		// Loop until TCP packet is received by server
		while(true)
		{
			// Use less CPU
			Sleep(1);

			status = HelperHandshakeStatus(connectionTimeout);
			if(status != NetUtility::CONNECTING)
			{
				return status;
			}
			
			// Receive TCP data from server.
			Packet recvPacket;
			Packet sendPacket;
			size_t packets = GetPacketFromStoreTCP(&recvPacket);

			if(packets > 0)
			{
				// Retrieve information about server.
				NetMode::ProtocolMode mode;
				size_t numOperations;

				this->maxClients = recvPacket.GetSizeT();
				if(IsEnabledUDP() == true)
				{
					numOperations = recvPacket.GetSizeT();
					mode = static_cast<NetMode::ProtocolMode>(recvPacket.Get<char>());
				}
				clientID = recvPacket.GetSizeT();

				if(IsEnabledUDP() == true)
				{
					// Create UDP mode and pass this to socket.
					// Socket will now be fully operational.
					NetModeUdp * modeUDP = NetModeUdp::GenerateModeUDP(mode,maxClients.Get(),numOperations,recvSizeUDP,decryptKey,memoryRecycle);
					socketUDP->LoadMode(modeUDP);
					
					// Formulate packet to be sent via UDP so that the server can find our UDP address.
					sendPacket.SetMemorySize(Utility::LargestSupportedBytesInt + Utility::LargestSupportedBytesInt + (sizeof(int) * NetUtility::authenticationStrength));

					// Add prefix to indicate that this is a connection packet.
					sendPacket.AddSizeT(0);

					// Add client number.
					sendPacket.AddSizeT(clientID.Get());
					
					// Add authentication codes.
					for(size_t n = 0;n<NetUtility::authenticationStrength;n++)
					{
						int aux = recvPacket.Get<int>();
						sendPacket.Add<int>(aux);
					}
					
					/**
					 * Send UDP packet to confirm our connection.
					 * Done in this way to get traverse Network Address Translation enabled routers.
					 * Message sending is repeated due to possibility of UDP packet loss.
					 */
					while(true)
					{
						// Use less CPU.
						Sleep(10);

						status = HelperHandshakeStatus(connectionTimeout);
						if(status != NetUtility::CONNECTING)
						{
							return status;
						}

						// Receive TCP packet, this indicates that server received our UDP packet
						if(GetPacketFromStoreTCP(&recvPacket) > 0)
						{
							// Only packets of size 0 are acceptable.
							if(recvPacket.GetUsedSize() == 0)
							{
								// If the server received our UDP packet then we have succeeded
								return NetUtility::CONNECTED;
							}
							else
							{
								// If a packet containing data is received then something went wrong
								return NetUtility::CONNECTION_ERROR;
							}
						}

						DoRawSendUDP(sendPacket,true);
					}
				}
				else
				{
					return NetUtility::CONNECTED;
				}
				
			}
			else
			{
				if(packets == NetUtility::NET_ERROR)
				{
					return NetUtility::CONNECTION_ERROR;
				}
			}
		}
	}
	// Store error so that PollConnect can report it.
	// This enables error system to deal with error.
	catch(ErrorReport & error)
	{
		handshakeError = error;
		handshakeErrorOccurred = true;
	}
	catch(...)
	{
		handshakeError.LoadReport("handshaking with server",-1,__LINE__,__FILE__);
		handshakeErrorOccurred = true;
	}

	return(NetUtility::CONNECTION_ERROR);
}

/**
 * @brief Determines the status of handshaking process.
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @return NetUtility::STILL_CONNECTING if the handshaking process is in progress still.
 * @return NetUtility::CONNECTED if the handshaking process completed successfully and the client is now fully connected.
 * @return NetUtility::TIMED_OUT if the handshaking process timed out.
 * @return NetUtility::CONNECTION_ERROR if an error occurred.
 * @return NetUtility::REFUSED if the server is full and so the connection attempt failed.
 */
NetUtility::ConnectionStatus NetInstanceClient::PollConnect()
{
	NetUtility::ConnectionStatus returnMe = NetUtility::TIMED_OUT;

	_ErrorException((handshakeThread == NULL),"polling on the status of a connection attempt, connection process has not begun",0,__LINE__,__FILE__);
	DWORD exitCode = handshakeThread->GetExitCode();

	// Cleanup handshaking thread now that we are done with it.
	if(exitCode != STILL_ACTIVE)
	{
		delete handshakeThread;
		handshakeThread = NULL;
	}

	// Take action based upon exit code result.
	switch(exitCode)
	{
		case(STILL_ACTIVE):
			returnMe = NetUtility::STILL_CONNECTING;
		break;

		case(NetUtility::TIMED_OUT):
			this->RequestDestroy();
			returnMe = NetUtility::TIMED_OUT;
		break;

		case(NetUtility::CONNECTED):
		{
			returnMe = NetUtility::CONNECTED;

			// Having reached this stage it is possible that an error occurred
			// and so connectionStatus is no longer CONNECTING, so we double check this
			// here.
			bool bContinue = true;
			connectionStatus.Enter();
				if(connectionStatus.Get() == NetUtility::CONNECTING)
				{
					connectionStatus.Set(NetUtility::CONNECTED);
				}
				else
				{
					bContinue = false;
				}
			connectionStatus.Leave();

			if(bContinue == true)
			{
				if(IsEnabledUDP() == true)
				{
					/**
					 * Start receiving UDP packets.
					 * This is done here instead of during handshaking because once thread exits
					 * operation is aborted.
					 */
					DoRecv(socketUDP);
				}

				/**
				 * Connect temporarily disabled the user receive function
				 * so that the handshaking process can use the packet queue system.
				 * This reverses that process now that we are done with the packet queue.
				 */
				socketTCP->UndoRemoveRecvFunction();

				/**
				 * If handshake is enabled then we have already started receiving.
				 * TCP packets and so don't need to call DoRecvTCP.
				 *
				 * If handshake is enabled then the completion port will have
				 * passed all TCP packets to the packet queue system rather than
				 * any specified user function so that the handshaking thread
				 * can receive TCP data. This means that any packets left in the
				 * queue that were received while handshaking should now be
				 * passed to the user function if one exists.
				 */
				if(IsHandshakeEnabled() == true)
				{
					// Deal with TCP packets that may have been received during
					// handshaking process but should have been passed to function
					if(IsUserFunctionLoadedTCP() == true)
					{
						Packet aux;
						while(GetPacketFromStoreTCP(&aux) > 0)
						{
							GetUserFunctionTCP()(aux);
						}
					}
				}
				else
				{
					/**
					 * Starts a TCP receive operation.
					 * Must be done here because on thread exit receive operation
					 * would be canceled, since no packet is received on it first.
					 */
					DoRecv(socketTCP);
				}
			}
		}
		break;

		case(NetUtility::CONNECTION_ERROR):
			// Check to see if throwable errors have occurred
			// and throw exception if it has.
			if(handshakeErrorOccurred == true)
			{
				handshakeError.DoRethrow();
			}

			// Connection was refused by server
			if(connectionStatus.Get() == NetUtility::REFUSED)
			{
				returnMe = NetUtility::REFUSED;
			}
			// Connection attempt failed for an unknown reason
			else
			{
				returnMe = NetUtility::CONNECTION_ERROR;
			}

			this->RequestDestroy();
		break;

		default:
			_ErrorException(true,"trying to poll on a connection process. Invalid GetExitCodeThread result",0,__LINE__,__FILE__);
		break;
	}

	return(returnMe);
}

/**
 * @brief Cancels the handshaking process.
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 * The handshaking must be in progress otherwise this command will fail.
 *
 * @warning PollConnect should not be used after this command, it will fail .
 */
void NetInstanceClient::StopConnect()
{
	delete handshakeThread;
	handshakeThread = NULL;

	this->RequestDestroy();
}

/**
 * @brief Begins connecting to server.
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @param addressTCP TCP IP and port of server that we should attempt to connect to.
 * @param addressUDP UDP IP and port of server that we should attempt to connect to.
 * @param connectionTimeout Length of time in milliseconds that connection attempt should last before it should abort (applies to non blocking too).
 * @param block If true the connection attempt is done synchronously and the method will not return until 
 * the connection attempt completes or fails. If false the connection attempt is done asynchronously and the method
 * will return straight away, PollConnect and StopConnect should then be used to manage the connection attempt.
 *
 * @return NetUtility::CONNECTED if the handshaking process completed successfully and the client is now fully connected.
 * @return NetUtility::TIMED_OUT if the handshaking process timed out.
 * @return NetUtility::CONNECTION_ERROR if an error occurred.
 * @return NetUtility::REFUSED if the server is full and so the connection attempt failed.
 */
NetUtility::ConnectionStatus NetInstanceClient::Connect(const NetAddress * addressTCP, const NetAddress * addressUDP, size_t connectionTimeout, bool block)
{
	NetUtility::ConnectionStatus returnMe = NetUtility::STILL_CONNECTING;

	// Handshaking thread will exit if connectionStatus
	// changes to anything but NetUtility::CONNECTING.
	connectionStatus.Set(NetUtility::CONNECTING); 
	
	// Temporarily disable receive function because
	// handshake thread must use TCP packet queue.
	// PollConnect will reverse this.
	socketTCP->RemoveRecvFunction();

	// Connect to server.
	_ErrorException((addressTCP == NULL),"connecting to a TCP address, parameter is NULL",0,__LINE__,__FILE__);
	socketTCP->Connect(*addressTCP);

	if(IsEnabledUDP() == true)
	{
		_ErrorException((addressUDP == NULL),"connecting to a UDP address, parameter is NULL",0,__LINE__,__FILE__);
		socketUDP->Connect(*addressUDP);
	}

	// Handshaking thread will read this value.
	timeoutMilliseconds = connectionTimeout;

	// Create thread to finish connection process.
	handshakeThread = new (nothrow) ThreadSingle(&ThreadHandshakeRoutineStatic,this);
	Utility::DynamicAllocCheck(handshakeThread,__LINE__,__FILE__);
	handshakeThread->Resume();
	
	// Wait for process to finish.
	if(block == true)
	{
		do{
			returnMe = PollConnect();
			Sleep(1);
		} while(returnMe == NetUtility::STILL_CONNECTING);
	}

	return returnMe;
}

/**
 * @brief	Determines if the client is in the process of connecting to a server.
 *
 * @return	true if connecting, false if not. 
 */
bool NetInstanceClient::IsConnecting() const
{
	return (handshakeThread != NULL);
}

/**
 * @brief Deals with errors.
 *
 * Signals instance to be shutdown, ClientConnected will do this when it is next called.
 * The instance is not shutdown directly because it is important that instance shutdown is done
 * by the main process and not by a worker thread (a worker thread may call this method).
 *
 * @param clientID Ignored (optional, default = 0).
 */
void NetInstanceClient::ErrorOccurred(size_t clientID)
{
	connectionStatus.Enter();
	if(connectionStatus.Get() != NetUtility::NOT_CONNECTED)
	{
		connectionStatus.Set(NetUtility::DISCONNECTING);
	}
	connectionStatus.Leave();
}

/**
 * @brief Determines the status of the connection with the server.
 *
 * This method is part of the @ref gracefulDisconnectPage "graceful disconnect process".
 *
 * @return NetUtility::CONNECTED: Fully connected.
 * @return NetUtility::NOT_CONNECTED: Not connected at all.
 * @return NetUtility::NO_RECV: Connected but new data cannot be received because @c Shutdown has been used by recipient (only possible if graceful disconnect is enabled).
 * @return NetUtility::NO_SEND: Connected but data cannot be sent because @c Shutdown has been used (only possible if graceful disconnect is enabled).
 * @return NetUtility::NO_SEND_RECV: Connected but data cannot be sent or received because @c Shutdown has been used on both sides,
 * only data that has already been received and is in the packet queue can be received (only possible if graceful disconnect is enabled).
 */
NetUtility::ConnectionStatus NetInstanceClient::ClientConnected()
{
	NetUtility::ConnectionStatus returnMe = NetUtility::NOT_CONNECTED;

	// First check if client is connected
	switch(connectionStatus.Get())
	{
		// If client is connected.
		case(NetUtility::CONNECTED):
			returnMe = socketTCP->GetConnectionStatus();
		break;
		
		// If client is no longer connected and needs to be cleaned up.
		case(NetUtility::DISCONNECTING):
			this->RequestDestroy();
			returnMe = NetUtility::NOT_CONNECTED;
		break;

		// If client is not connected at all or at some stage of the handshaking process then we return 0 
		// because it is not properly connected yet.
		default:
			returnMe = NetUtility::NOT_CONNECTED;
		break;
	}

	return returnMe;
}

/**
 * @brief	Retrieves the state that the connection is in.
 *
 * Unlike NetInstanceClient::ClientConnected, the raw state of the connection
 * is returned and no further action is taken.
 *
 * @return	the connection state. 
 */
NetUtility::ConnectionStatus NetInstanceClient::GetConnectionState()
{
	return connectionStatus.Get();
}

/**
 * @brief Starts a receive operation and deals with errors appropriately.
 *
 * Calls NetSocket***::Recv. Use this INSTEAD OF socket***->Recv, never use socket***->Recv directly.
 *
 * @param socket [in,out] Pointer to socket that receive operation should be started on.
 * @param clientID Ignored (optional, default = 0).
 */
void NetInstanceClient::DoRecv(NetSocket * socket, size_t clientID)
{
	bool error = socket->Recv();

	if(error == true)
	{
		ErrorOccurred(NULL);
	}
}

/**
 * @brief Starts a raw send operation via UDP and deals with errors in a client specific way.
 *
 * The packet will be sent as is with no modification, regardless of UDP mode.\n\n
 *
 * Calls NetSocketUDP::RawSend. Use this INSTEAD OF socketUDP->RawSend, never use socketUDP->RawSend directly.
 *
 * @param packet Packet to send.
 * @param block If true the method will not return until @a packet is completely sent, note that this does not indicate that
 * the packet has been received by the recipient, instead it simply means the packet is in transit. \n
 * If false the method will return instantly even if the packet has not been sent.
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED if the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
int NetInstanceClient::DoRawSendUDP(const Packet & packet, bool block)
{
	ValidateIsEnabledUDP(__LINE__,__FILE__);
	NetUtility::SendStatus result = socketUDP->RawSend(packet,block,NULL,GetSendTimeout());
	if(result == NetUtility::SEND_FAILED_KILL)
	{
		ErrorOccurred(NULL);
	}
	return result;
}

/** 
 * @brief Sends a packet via UDP to the server.
 *
 * @param packet Packet to send.
 * @param block If true the method will not return until @a packet is completely sent, note that this does not indicate that
 * the packet has been received by the recipient, instead it simply means the packet is in transit. \n
 * If false the method will return instantly even if the packet has not been sent.
 * @param clientID Ignored.
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED if the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus NetInstanceClient::SendUDP(const Packet & packet, bool block, size_t clientID)
{
	ValidateIsEnabledUDP(__LINE__,__FILE__);
	NetUtility::SendStatus result = socketUDP->Send(packet,block,NULL,GetSendTimeout());
	if(result == NetUtility::SEND_FAILED_KILL)
	{
		ErrorOccurred(NULL);
	}
	return result;
}

/**
 * @brief Sends a packet via UDP to a remote address.
 *
 * @param address Remote address to send to.
 * @param packet Packet to send.
 * @param block If true the method will not return until @a packet is completely sent to all clients, note that this does not indicate that
 * the packet has been received by all clients, instead it simply means the packet is in transit. \n
 * If false the method will return instantly even if the packet has not been sent.
 *
 * @return NetUtility::SEND_COMPLETED If the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS If the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED If the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL If the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus NetInstanceClient::SendToUDP(const NetAddress & address, const Packet & packet, bool block)
{
	ValidateIsEnabledUDP(__LINE__,__FILE__);
	NetUtility::SendStatus result = socketUDP->Send(packet,block,&address,GetSendTimeout());
	if(result == NetUtility::SEND_FAILED_KILL)
	{
		ErrorOccurred(NULL);
	}
	return result;
}

/**
 * @brief	Called by the completion port when an error occurred during an operation.
 *
 * @param [in,out]	completionSocket	The socket responsible for the error. 
 * @param	clientID					Ignored (optional, default = 0).
 */
void NetInstanceClient::CompletionError(NetSocket * completionSocket, size_t clientID)
{
	// UDP
	if(completionSocket == socketUDP)
	{
		ErrorOccurred(0);
	}
	// TCP
	else
	{
		bool connectionRefused = (this->connectionStatus.Get() == NetUtility::CONNECTING &&
								 (WSAGetLastError() == ERROR_NETNAME_DELETED || WSAGetLastError() == WSAECONNREFUSED));

		if(connectionRefused == true)
		{
			connectionStatus.Set(NetUtility::REFUSED);
		}
		else
		{
			// connectionStatus.Get will return NetUtility::CONNECTED regardless of TCP socket connection state.
			// So, during graceful disconnection connectionStatus.Get will return NetUtility::CONNECTED.
			if(this->IsGracefulDisconnectEnabled() == false || this->connectionStatus.Get() != NetUtility::CONNECTED)
			{
				ErrorOccurred(0);
			}
		}
	}
}

/**
 * @brief When send and receive operations are completed on this instance, this method is called.
 * 
 * @param socket [in,out] Socket that operation was started by.
 * @param bytes Number of bytes of data transferred in operation.
 * @param clientID Ignored (optional, default = 0).
 */
void NetInstanceClient::DealCompletion(NetSocket * socket, DWORD bytes, size_t clientID)
{
	try
	{
		// Deal with received data
		socket->DealWithData(socket->recvBuffer,bytes,socket->GetRecvFunction(),NULL,this->GetInstanceID());
	}
	// Disconnect client in the event of an error
	catch(ErrorReport & error){	ErrorOccurred(NULL); }
	catch(...){ ErrorOccurred(NULL);}
}

/**
 * @brief Retrieves maximum number of clients that can be connected to server at any one time.
 *
 * @return @copydoc maxClients
 * @return 0 if the client is not fully connected to the server.
 */
size_t NetInstanceClient::GetMaxClients()
{
	return maxClients.Get();
}

/**
 * @brief Retrieves client ID allocated by server.
 *
 * @return @copydoc clientID
 * @return 0 if the client is not fully connected to the server.
 */
size_t NetInstanceClient::GetClientID()
{
	return clientID.Get();
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetInstanceClient::TestClass()
{
	cout << "Testing NetInstanceClient class...\n";
	bool problem = false;

	NetUtility::LoadEverything(2,0);

	NetInstanceProfile profile;
	profile.SetModeTCP(NetMode::TCP_RAW);
	profile.SetHandshakeEnabled(false);
	profile.SetGracefulDisconnectEnabled(true);

	NetAddress localAddr(NULL,6543);
	profile.SetLocalAddrTCP(localAddr);

	NetInstanceClient * client = new NetInstanceClient(profile);
	NetInstanceContainer container(client);

	NetAddress connectAddr = NetUtility::ConvertDomainNameToIP("www.google.com");
	connectAddr.SetPort(80);

	NetUtility::ConnectionStatus status = client->Connect(&connectAddr,NULL,3000,true);

	if(status != NetUtility::CONNECTED)
	{
		cout << "Failed to connect to google: " << status << "\n";
		problem = true;
	}
	else
	{
		cout << "Connected to google!\n";
	}
	
	Packet sendPacket;
	sendPacket.AddStringC("GET / HTTP/1.1\r\n",0,false);
	sendPacket.AddStringC("Host: www.google.com\r\n",0,false);
	sendPacket.AddStringC("Connection: close\r\n\r\n",0,false);

	client->SendTCP(sendPacket,false);

	clock_t startClock = clock();
	Packet recvPacket;
	while(client->ClientConnected() != NetUtility::NOT_CONNECTED)
	{
		// Check for graceful disconnect
		if(client->GetConnectionStateTCP() == NetUtility::NO_RECV)
		{
			// Wait for all data to be received before closing connection
			if(client->GetPacketAmountTCP() == 0)
			{
				client->ShutdownTCP();
			}
		}

		// Receive data
		if(client->GetPacketFromStoreTCP(&recvPacket) > 0)
		{
			if(recvPacket.GetUsedSize() > 0)
			{
				char * str = recvPacket.GetStringC(recvPacket.GetUsedSize(),true);
				cout << str << '\n';
				delete[] str;
			}
		}
	}

	NetUtility::UnloadEverything();

	cout << "\n\n";
	return !problem;
}
