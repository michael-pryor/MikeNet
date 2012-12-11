#include "FullInclude.h"
#include "ComparatorServerClientFindByAddressUDP.h"
#include "StdComparator.h"


/**
 * @brief Initializes object.
 *
 * @param p_maxClients Maximum number of clients that can be connected to server at any one time.
 * @param p_handshakeEnabled True if the TCP handshaking process is enabled, false if not.
 * For more information see @ref handshakePage "server/client handshaking process".
 * @param p_connectionTimeout Time in milliseconds that a connection attempt will be waited on before giving up (optional, default DEFAULT_CONNECTION_TIMEOUT).
 * @param p_socketListening Socket to use to listen for new TCP connections.
 * @param recvMemoryLimitTCP Maximum amount of memory that can be allocated to TCP receive operations per client (default = NetInstanceProfile::DEFAULT_RECV_MEMORY_LIMIT). See @ref securityPage "security" for more information.
 * @param recvMemoryLimitUDP Maximum amount of memory that can be allocated to UDP receive operations per client (default = NetInstanceProfile::DEFAULT_RECV_MEMORY_LIMIT). See @ref securityPage "security" for more information.
 * @param sendMemoryLimitTCP Maximum amount of memory that can be allocated to TCP send operations per client (default = NetInstanceProfile::DEFAULT_SEND_MEMORY_LIMIT). See @ref securityPage "security" for more information.
 * @param sendMemoryLimitUDP Maximum amount of memory that can be allocated to UDP send operations (default = NetInstanceProfile::DEFAULT_SEND_MEMORY_LIMIT). See @ref securityPage "security" for more information.
 *
 * @warning SocketUDP Must be initialized before using this method (is done in super class constructor).
 * @throws ErrorReport If p_socketListening->GetSocket()->GetMode()->GetMaxPacketSize() < GetRecvSizeMinTCP().
 */
void NetInstanceServer::Initialize(size_t p_maxClients, bool p_handshakeEnabled, size_t p_connectionTimeout, NetSocketListening * p_socketListening, size_t recvMemoryLimitTCP, size_t recvMemoryLimitUDP, size_t sendMemoryLimitTCP, size_t sendMemoryLimitUDP)
{
	try
	{
		ExceptionValidateRecvSizeUDP();
		_ErrorException((ValidateRecvSizeTCP(p_socketListening->GetSocket()->GetMode()->GetMaxPacketSize()) != true),"initializing a TCP based instance of server type, receive buffer size is too small",0,__LINE__,__FILE__);

		this->maxClients = p_maxClients;
		this->handshakeEnabled = p_handshakeEnabled;
		this->timeout.Set(p_connectionTimeout);

		_ErrorException((p_socketListening == NULL),"loading a listening socket, parameter is NULL",0,__LINE__,__FILE__);
		this->socketListening = p_socketListening;

		// Setup client vector
		client.Resize(maxClients+1); // +1 for 0 indexed
		for(size_t n = 1; n<=maxClients; n++)
		{
			NetServerClient * addMe = new (nothrow) NetServerClient(n,socketListening->GetCopySocket());
			Utility::DynamicAllocCheck(addMe,__LINE__,__FILE__);

			client.Allocate(n,addMe);
			client[n].GetSocketTCP()->SetInstance(this);

			client[n].SetSendMemoryLimitTCP(sendMemoryLimitTCP);
			client[n].SetRecvMemoryLimitTCP(recvMemoryLimitTCP);

			if(IsEnabledUDP())
			{
				if(socketUDP->GetMode()->IsRecvMemorySizeSupported())
				{
					socketUDP->SetRecvMemoryLimit(recvMemoryLimitUDP,n);
				}
			}
		}

		clientSortedByAddressUDP.LinkShallow(client);
		clientSortedByAddressUdpNeedsResort = true;

		/**
		 * Server info packet contains:
		 * 1: Maximum number of clients.
		 * 2: Number of operations (UDP only).
		 * 3: UDP Mode (UDP only).
		 */
		if(IsEnabledUDP() == true)
		{
			serverInfo.SetMemorySize(Utility::LargestSupportedBytesInt + Utility::LargestSupportedBytesInt + sizeof(char));
		}
		else
		{
			serverInfo.SetMemorySize(Utility::LargestSupportedBytesInt);
		}

		serverInfo.AddSizeT(maxClients);
		if(IsEnabledUDP() == true)
		{
			serverInfo.AddSizeT(socketUDP->GetMode()->GetNumOperations());
			serverInfo.Add<char>(socketUDP->GetMode()->GetProtocolMode());
		}

		// Start receiving via UDP
		if(IsEnabledUDP() == true)
		{
			socketUDP->SetSendMemoryLimit(sendMemoryLimitUDP);
			DoRecv(socketUDP,0);
		}
	}
	catch(ErrorReport & report)
	{
		delete p_socketListening;
		throw report;
	}
}

/**
 * @brief Constructor.
 *
 * @param p_maxClients Maximum number of clients that can be connected to server at any one time.
 * @param p_socketListening Socket to use to listen for new TCP connections.
 * @param p_socketUDP Socket to use to send and receive UDP data, NULL if UDP is disabled.
 * @param p_handshakeEnabled True if the TCP handshaking process is enabled, false if not.
 * For more information see @ref handshakePage "server/client handshaking process".
 * @param p_sendTimeout Length of time in milliseconds to wait for a send operation to complete before disconnecting client (optional, default INFINITE).
 * @param p_connectionTimeout Time in milliseconds that a connection attempt will be waited on before giving up (optional, default DEFAULT_CONNECTION_TIMEOUT).
 * @param p_instanceID Unique identifier for instance (optional, default 0).
 * @throws ErrorReport If p_socketListening->GetSocket()->GetMode()->GetMaxPacketSize() < GetRecvSizeMinTCP().
 */
NetInstanceServer::NetInstanceServer(size_t p_maxClients, NetSocketListening * p_socketListening, NetSocketUDP * p_socketUDP, bool p_handshakeEnabled, unsigned int p_sendTimeout, size_t p_connectionTimeout, size_t p_instanceID) :
		client(),
		clientSortedByAddressUDP(),
		comparatorSort(true),
		comparatorFind(false),
		NetInstance(p_instanceID,NetInstance::SERVER,p_sendTimeout),
		NetInstanceTCP(p_handshakeEnabled),
		NetInstanceUDP(p_socketUDP),
		recvFailCounterUDP(Counter::DEFAULT_TIMEOUT,Counter::DEFAULT_LIMIT),
		timeout()
{
	Initialize(p_maxClients,p_handshakeEnabled,p_connectionTimeout,p_socketListening);
}

/**
 * @brief Constructor, using profile instead of individual parameters.
 *
 * @param p_maxClients Maximum number of clients that can be connected to server at any one time.
 * @param p_profile Instance profile containing parameters.
 * @param p_instanceID Unique identifier for instance (optional, default 0).
 * @throws ErrorReport If p_socketListening->GetSocket()->GetMode()->GetMaxPacketSize() < GetRecvSizeMinTCP().
 */
NetInstanceServer::NetInstanceServer(size_t p_maxClients, const NetInstanceProfile & p_profile, size_t p_instanceID) :
		client(),
		clientSortedByAddressUDP(),
		comparatorSort(true),
		comparatorFind(false),
		NetInstance(p_instanceID,NetInstance::SERVER,p_profile.GetSendTimeout()),
		NetInstanceTCP(p_profile.IsHandshakeEnabled()),
		NetInstanceUDP
		(
			p_profile.GenerateObjectSocketUDP
			(
				p_profile.GetRecvSizeUDP(),
				p_profile.GetLocalAddrUDP(),
				p_profile.IsReusableUDP(),
				p_profile.GenerateObjectModeUDP(p_maxClients,p_profile.GetNumOperations()),
				p_profile.GetRecvFuncUDP()
			)
		),
		recvFailCounterUDP(Counter::DEFAULT_TIMEOUT,Counter::DEFAULT_LIMIT),
		timeout()
{
	Initialize(
		p_maxClients,
		p_profile.IsHandshakeEnabled(),
		p_profile.GetConnectionToServerTimeout(),
		static_cast<NetSocketListening*>(
			Utility::DynamicAllocCheck(
					new (nothrow) NetSocketListening(
						p_profile.GetLocalAddrTCP(),
						static_cast<NetSocketTCP*>(
							Utility::DynamicAllocCheck(
								new (nothrow) NetSocketTCP(
									p_profile.GetWsaRecvSizeTCP(),
									p_profile.IsNagleEnabled(),
									p_profile.IsGracefulDisconnectEnabled(),
									static_cast<NetModeTcp*>(Utility::DynamicAllocCheck(p_profile.GenerateObjectModeTCP(),__LINE__,__FILE__)),
									p_profile.GetRecvFuncTCP()
								),
							__LINE__,__FILE__)
						)
					),
			__LINE__,__FILE__)
		),
		p_profile.GetRecvMemoryLimitTCP(),
		p_profile.GetRecvMemoryLimitUDP(),
		p_profile.GetSendMemoryLimitTCP(),
		p_profile.GetSendMemoryLimitUDP()
	);
}

/**
 * @brief Destructor.
 *
 * CloseSockets must be used in the destructor.
 */
NetInstanceServer::~NetInstanceServer()
{
	const char * cCommand = "an internal function (~NetInstanceServer)";
	try
	{
		CloseSockets();
		delete socketListening;
	}
	MSG_CATCH
}

/**
 * @brief	Changes the maximum amount of memory the specified
 * client is allowed to use for asynchronous TCP send operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	newLimit	The new limit in bytes. 
 * @param	clientID	ID of the specified client.
 */
void NetInstanceServer::SetSendMemoryLimitTCP(size_t newLimit, size_t clientID)
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	client[clientID].GetSocketTCP()->SetSendMemoryLimit(newLimit);
}


/**
 * @brief	Changes the maximum amount of memory the specified
 * client is allowed to use for TCP receive operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	newLimit	The new limit in bytes. 
 * @param	clientID	ID of the specified client.
 */
void NetInstanceServer::SetRecvMemoryLimitTCP(size_t newLimit, size_t clientID)
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	client[clientID].GetSocketTCP()->SetRecvMemoryLimit(newLimit);	
}

/**
 * @brief	Retrieves the maximum amount of memory the
 * specified client is allowed to use for TCP send operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	clientID	ID of the specified client.
 * @return the maximum amount of memory in bytes.
 */
size_t NetInstanceServer::GetSendMemoryLimitTCP(size_t clientID) const
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return client[clientID].GetSocketTCP()->GetSendMemoryLimit();	
}

/**
 * @brief	Retrieves the maximum amount of memory the
 * specified client is allowed to use for TCP receive operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	clientID	ID of the specified client.
 * @return the maximum amount of memory in bytes.
 */
size_t NetInstanceServer::GetRecvMemoryLimitTCP(size_t clientID) const
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return client[clientID].GetSocketTCP()->GetRecvMemoryLimit();	
}

/**
 * @brief	Retrieves the estimated amount of memory that the
 * specified client is currently using for TCP send operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	clientID	ID of the specified client.
 * @return the maximum amount of memory in bytes.
 */
size_t NetInstanceServer::GetSendMemorySizeTCP(size_t clientID) const
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return client[clientID].GetSocketTCP()->GetSendMemorySize();	
}

/**
 * @brief	Retrieves the estimated amount of memory that the
 * specified client is currently using for TCP receive operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	clientID	ID of the specified client.
 * @return the maximum amount of memory in bytes.
 */
size_t NetInstanceServer::GetRecvMemorySizeTCP(size_t clientID) const
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return client[clientID].GetSocketTCP()->GetRecvMemorySize();	
}

/**
 * @brief Retrieves the minimum UDP receive buffer size.
 *
 * @return the minimum UDP receive buffer size.
 */
size_t NetInstanceServer::GetRecvSizeMinUDP() const
{
	return recvSizeMinUDP;
}

/**
 * @brief Retrieves the minimum TCP receive buffer size.
 *
 * @return the minimum TCP receive buffer size.
 */
size_t NetInstanceServer::GetRecvSizeMinTCP() const
{
	return recvSizeMinTCP;
}

/**
 * @brief Packet that is sent to clients during the handshaking process,
 * containing information about the server.
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 * @return %Packet containing information about the server.
 */
const Packet & NetInstanceServer::GetServerInfo() const
{
	return serverInfo;
}

/**
 * @brief Adds a client to the disconnect list, indicating that it has recently been disconnected.
 *
 * @param client Client ID.
 */
void NetInstanceServer::AddDisconnect(size_t client)
{
	disconnected.Add(Utility::CopyObject(client));	
}

/**
 * @brief Retrieve a client from the disconnect list, this client has been recently disconnected.
 * @return client ID.
 */
size_t NetInstanceServer::GetDisconnect()
{
	size_t returnMe = 0;
	disconnected.Get(&returnMe);
	return(returnMe);
}

/**
 * @brief Retrieves the TCP function that is executed when complete TCP packets are received.
 *
 * @return the method set by the user that should
 * be executed when data is received.
 * @return NULL if there is no method.
 */
NetSocket::RecvFunc NetInstanceServer::GetUserFunctionTCP() const
{
	return socketListening->GetSocket()->GetRecvFunction();
}

/**
 * @brief Determines whether a TCP function has been loaded.
 *
 * If no TCP function has been loaded received packets are put into a queue and
 * retrieved using GetPacketFromStoreTCP.
 * 
 * @return true if a method to be executed when data is received has been set.
 * @return false if no method has been set.
 */
bool NetInstanceServer::IsUserFunctionLoadedTCP() const
{
	return socketListening->GetSocket()->IsRecvFunctionLoaded();
}


/**
 * @brief Changes the auto resize option for all clients.
 *
 * @param paraAutoResize If true then if a packet larger than
 * the maximum is received, the maximum will be increased silently. \n
 * if false then an exception will be thrown.
 */
void NetInstanceServer::SetAutoResize(bool paraAutoResize)
{
	for(size_t n = 1;n<=maxClients;n++)
	{
		client[n].SetAutoResizeTCP(paraAutoResize);
	}
}

/**
 * @brief Deals with errors.
 *
 * Client @a clientID is disconnected.
 * @param clientID ID of client to disconnected, if 0 then no action is taken.
 */
void NetInstanceServer::ErrorOccurred(size_t clientID)
{
	if(clientID != 0)
	{
		client[clientID].ErrorOccurred();
	}
}


/**
 * @brief From the moment that a client first communicates with the server, it is allowed 'server timeout' number of milliseconds
 * to complete the handshaking process before the process is aborted and the client is forcefully silently
 * disconnected.
 *
 * Note that ClientLeft will not receive notification since ClientJoined will not have. \n\n
 *
 * This method is part of the @ref handshakePage "server/client handshaking process". \n\n
 *
 * This method is part of a security precaution. If the server is spammed with connection attempts
 * that never complete it would eventually throw an error due to running out of memory. \n\n
 *
 * @param milliseconds Number of milliseconds that connection attempt should
 * be allowed to complete before entity is forcefully disconnected.
 */
void NetInstanceServer::SetServerTimeout(size_t milliseconds)
{
	timeout.Set(milliseconds);
}

/**
 * @brief From the moment that a client first communicates with the server, it is allowed 'server timeout'
 * number of milliseconds to complete the handshaking process before the process is aborted and the client is forcefully silently
 * disconnected.
 *
 * Note that ClientLeft will not receive notification since ClientJoined will not have.\n\n
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @return number of milliseconds that connection attempt should be
 * allowed to complete before entity is forcefully disconnected. /n/n.
 */
size_t NetInstanceServer::GetServerTimeout() const
{
	return timeout.Get();
}

/**
 * @brief Determines whether graceful disconnection is enabled.
 *
 * This method is part of the @ref gracefulDisconnectPage "graceful disconnect process".
 *
 * @return if true graceful disconnection is possible, where data is allowed
 * to finish sending and receiving before the connection is closed.
 */
bool NetInstanceServer::IsGracefulDisconnectEnabled() const
{
	return socketListening->GetSocket()->IsGracefulDisconnectEnabled();
}

/**
 * @brief Checks that client ID is within range and throws an exception if it is not.
 *
 * @param clientID Client ID to check.
 * @param line Line number that method was called at.
 * @param file File that method was called in.
 * @exception ErrorReport If clientID > maxClients.
 */
void NetInstanceServer::ValidateClientID(size_t clientID, size_t line, const char * file) const
{
	_ErrorException((clientID > maxClients || clientID == 0),"performing a client related function on the server side. Invalid client ID", 0, line, file);
}

/** 
 * @brief Disconnects specified client.
 * 
 * @param clientID ID of client to disconnect.
 */
void NetInstanceServer::DisconnectClient(size_t clientID)
{
	ValidateClientID(clientID,__LINE__,__FILE__);

	// Add client to list of disconnected clients
	if(client[clientID].WasFullyConnected() == true)
	{
		AddDisconnect(clientID);
	}

	clientSortedByAddressUDP.Enter();
	try
	{
		// Reset client's data
		client[clientID].Disconnect();

		// UDP address will have changed, so resort vector.
		RequestResortClientsByAddressUDP();
	}
	catch(ErrorReport & error){	clientSortedByAddressUDP.Leave(); throw(error); }
	catch(...){ clientSortedByAddressUDP.Leave(); throw(-1); }
	clientSortedByAddressUDP.Leave();

	if(IsEnabledUDP() == true)
	{
		socketUDP->Reset(clientID);
	}
}

/**
 * @brief Determines the status of a clients connection.
 *
 * This method is part of the @ref gracefulDisconnectPage "graceful disconnect process".
 *
 * @param clientID ID of client who's connection status should be checked.
 *
 * @return NetUtility::CONNECTED: Fully connected.
 * @return NetUtility::NOT_CONNECTED: Not connected at all.
 * @return NetUtility::NO_RECV: Connected but new data cannot be received because @c Shutdown has been used by recipient (only possible if graceful disconnect is enabled).
 * @return NetUtility::NO_SEND: Connected but data cannot be sent because @c Shutdown has been used (only possible if graceful disconnect is enabled).
 * @return NetUtility::NO_SEND_RECV: Connected but data cannot be sent or received because @c Shutdown has been used on both sides,
 * only data that has already been received and is in the packet queue can be received (only possible if graceful disconnect is enabled).
 */
NetUtility::ConnectionStatus NetInstanceServer::ClientConnected(size_t clientID) const
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return client[clientID].GetConnectionState();
}


/**
 * @brief This command manages incoming connections from clients.
 *
 * For a client to connect, this command must have been called several times
 * during the handshaking process. It is best to call this continuously in 
 * a loop that runs this networking module. \n\n
 * 
 * Connection requests will be rejected if the maximum number of clients
 * has been reached.\n\n
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @return 0 if no new clients joined during the method call.
 * @return >0 if a new client joined, this number is the ID of the client.
 */
size_t NetInstanceServer::ClientJoined()
{
	size_t returnMe = NULL;

	/**
	 * When a TCP connection is accepted, an unused client ID needs to be found.
	 * It is more efficient only to have one loop checking the status of clients
	 * and this loop occurs before WSAAccept. The loop will fill unusedClientID
	 * with the first unused client ID that it finds, so that a new client can
	 * be given the ID.
	 *
	 * If unusedClientID remains 0 then no free client ID was found.
	 */
	size_t unusedClientID = 0;

	for(size_t clientID = 1; clientID <= maxClients; clientID++)
	{
		switch(client[clientID].GetConnectionState())
		{
			// Unused client ID
			case(NetUtility::NOT_CONNECTED):
				if(unusedClientID == 0)
				{
					unusedClientID = clientID;
				}
			break;

			// When all TCP data has been used then we should cleanup.
			case(NetUtility::CONNECTED):
				if(IsGracefulDisconnectEnabled() == true)
				{
					if(client[clientID].GetConnectionStateTCP() == NetUtility::NOT_CONNECTED)
					{
						DisconnectClient(clientID);
					}
				}
			break;

			// An error has occurred and the client should now be disconnected. It is important
			// that the client is disconnected from the main process to prevent deadlock.
			case(NetUtility::DISCONNECTING):
				DisconnectClient(clientID);
			break;
				
			// Client is fully connected but is awaiting confirmation since
			// this method must announce that it is joined before it is officially connected.
			case(NetUtility::CONNECTED_AC):
				if(returnMe == NULL)
				{
					// Notify client that we received their UDP packet so they are fully connected.
					// Send operation MUST block because we don't want to change connection status until
					// this message has been sent.
					Packet notifyCompletion;
					NetUtility::SendStatus status = client[clientID].SendTCP(notifyCompletion,true);
					_ErrorException((status != NetUtility::SEND_COMPLETED),"notifying a client that it has finished connecting",WSAGetLastError(),__LINE__,__FILE__);

					returnMe = clientID;
					client[clientID].SetConnectionState(NetUtility::CONNECTED);
				}
			break;

			// Connection process timeouts
			case(NetUtility::CONNECTING):
				if(clock() - client[clientID].GetClockStarted() > (clock_t)timeout.Get())
				{
					DisconnectClient(clientID);
				}
			break;
		}
	}

	// Deal with new TCP connection attempts
	NetAddress newClientAddr;
	SOCKET newClientSocket = socketListening->AcceptConnection(unusedClientID,&newClientAddr);

	// If a request was accepted then continue setting up this client
	if(newClientSocket != INVALID_SOCKET)
	{
		client[unusedClientID].LoadTCP(newClientSocket,newClientAddr,IsEnabledUDP());
		DoRecv(client[unusedClientID].GetSocketTCP(),unusedClientID); // Starts TCP receive operation

		if(handshakeEnabled == true)
		{
			NetUtility::SendStatus status = client[unusedClientID].SendHandshakingPacket(GetServerInfo(),IsEnabledUDP());
			if(status == NetUtility::SEND_FAILED || status == NetUtility::SEND_FAILED_KILL)
			{
				clientSortedByAddressUDP.Enter();
				try
				{
					client[unusedClientID].Disconnect();

					// Just in case a UDP address was loaded before disconnection, although
					// this probably never happens.
					RequestResortClientsByAddressUDP();
				}
				catch(ErrorReport & error){	clientSortedByAddressUDP.Leave(); throw(error); }
				catch(...){ clientSortedByAddressUDP.Leave(); throw(-1); }
				clientSortedByAddressUDP.Leave();
			}
		}
	}

	return(returnMe);
}


/** 
 * @brief Retrieves the maximum number of clients that can be connected to the server at any one time.
 *
 * @return the maximum number of clients that can be connected to the server at any one time (@c maxClients).
 * Client IDs range from 1 inclusive to this value inclusive, client ID 0 is not used.
 */
size_t NetInstanceServer::GetMaxClients() const
{
	return maxClients;
}

/**
 * @brief Calls NetSocketTCP::Recv or NetSocketUDP::Recv and deals with errors in a server specific way.
 *
 * Use this INSTEAD OF socket***->Recv, never use socket***->Recv directly.
 *
 * @param socket Pointer to socket that receive operation should be started on.
 * @param clientID ID of client that receive operation should apply to.
 */
void NetInstanceServer::DoRecv(NetSocket * socket, size_t clientID)
{
	// UDP socket
	if(socket == socketUDP)
	{
		NetInstanceUDP::ValidateIsEnabledUDP(__LINE__,__FILE__);
RetryRecv:
		bool error = socketUDP->Recv();

		if(error == true)
		{
			int error = WSAGetLastError();

			// I've left recvFailCounterUDP in here even though its not really needed,
			// in case there are some other winsock errors which should be ignored but are not.
			// Note that ERROR_PORT_UNREACHABLE and WSAECONNRESET can occur if a UDP packet
			// fails to be sent. There is a way of turning this off using WSAIoctl but this
			// looks like more trouble than its worth, but maybe do this at a later date.
			if(error == ERROR_PORT_UNREACHABLE || error == WSAECONNRESET || recvFailCounterUDP.Increment() == false)
			{
				//Utility::output.Enter();
				//cout << "Failed to start receive operation, trying again: " << WSAGetLastError() << '\n';
				//Utility::output.Leave();
				goto RetryRecv;
			}
			else
			{
				_ErrorException(true,"trying to start a receive operation via UDP in server state, maximum number of attempts exceeded",WSAGetLastError(),__LINE__,__FILE__);
			}
		}
		else
		{
			recvFailCounterUDP.Reset();
		}
	}
	// TCP socket
	else
	{
		client[clientID].DoRecv(socket);
	}

}

/**
 * @brief Retrieves the amount of data that can be received in one winsock receive operation.
 *
 * @param clientID ID of client to use.
 * @return the buffer size of the WSABUF used by the TCP socket.
 * This is the maximum amount of data that can be received with one winsock receive operation.
 */
size_t NetInstanceServer::GetRecvBufferLengthTCP(size_t clientID) const
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return client[clientID].GetRecvBufferLengthTCP();
}

/**
 * @brief Retrieves the amount of the current partial packet that has been received in bytes.
 *
 * @param clientID ID of client to use.
 * @return the current size of the TCP partial packet being received.
 * i.e. The number of bytes of the packet that have been received.
 */
size_t NetInstanceServer::GetPartialPacketCurrentSizeTCP(size_t clientID) const
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return client[clientID].GetPartialPacketCurrentSizeTCP();
}

/**
 * @brief Retrieves the largest TCP packet that can be received without error or increase in buffer size.
 *
 * @param clientID ID of client to use.
 * @return	the size of the largest packet that can be received, packets larger than this
 * will require an increase in memory size or an exception will be thrown.
 */
size_t NetInstanceServer::GetMaxPacketSizeTCP(size_t clientID) const
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return client[clientID].GetMaxPacketSizeTCP();
}

/**
 * @brief Changes the maximum packet size that can be received.
 *
 * @param newMaxSize The size of the largest packet that can be received, packets larger than this
 * will require an increase in memory size or an exception will be thrown.
 * @param clientID ID of client to use.
 *
 * @throws ErrorReport If newMaxSize < GetRecvSizeMinTCP().
 */
void NetInstanceServer::SetMaxPacketSizeTCP(size_t newMaxSize, size_t clientID)
{
	_ErrorException((ValidateRecvSizeTCP(newMaxSize) != true),"changing the TCP packet receive buffer size for a client in server state, new size is too small",0,__LINE__,__FILE__);
	ValidateClientID(clientID,__LINE__,__FILE__);
	client[clientID].SetMaxPacketSizeTCP(newMaxSize);
}

/**
 * @brief Determines whether auto resize is enabled, if enabled the TCP receive
 * buffer size will increase as necessary to accommodate large packets.
 *
 * @param clientID ID of client to use.
 * @return when true, if a packet larger than the max TCP packet
 * size is received then the max size will be increased silently.
 * When false, in this case an exception will be thrown.
 */
bool NetInstanceServer::GetAutoResizeTCP(size_t clientID) const
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return client[clientID].GetAutoResizeTCP();
}

/**
 * @brief Enables or disables auto resize, if enabled the TCP receive
 * buffer size will increase as necessary to accommodate large packets.
 *
 * @param newAutoResizeTCP When true, if a packet larger than the max TCP packet
 * size is received then the max size will be increased silently.
 * When false, in this case an exception will be thrown.
 * @param clientID ID of client to use.
 */
void NetInstanceServer::SetAutoResizeTCP(bool newAutoResizeTCP, size_t clientID)
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	client[clientID].SetAutoResizeTCP(newAutoResizeTCP);
}

/**
 * @brief Retrieves the TCP mode in use by the instance. Clients connecting
 * must use the same TCP mode to connect successfully.
 *
 * @return protocol mode in use by instance.
 */
NetMode::ProtocolMode NetInstanceServer::GetProtocolModeTCP() const
{
	return socketListening->GetSocket()->GetMode()->GetProtocolMode();
}

/** 
 * @brief Retrieve the local TCP address in use by the specified client.
 *
 * @param clientID ID of client to use.
 * @return TCP local address information of the specified client.
 * This reference will remain valid and unmodified until the next call.
 * @note Is okay to return reference because NetAddress is thread safe.
 */
const NetAddress & NetInstanceServer::GetClientLocalAddressTCP(size_t clientID) const
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return client[clientID].GetLocalAddressTCP();
}

/**
 * @brief Retrieve the local TCP address in use by the server.
 * Connecting clients should attempt to connect to this address.
 * 
 * @return TCP local address information of the specified client.
 * This reference will remain valid and unmodified until the next call.
 * @note Is okay to return reference because NetAddress is thread safe.
 */
const NetAddress & NetInstanceServer::GetLocalAddressTCP() const
{
	return socketListening->GetLocalAddress();
}

/** 
 * @brief Retrieve the TCP address that the specified client
 * is connected to.
 *
 * @param clientID ID of client to use.
 * @return TCP remote address information of the specified client.
 * This reference will remain valid and unmodified until the next call.
 * @note Is okay to return reference because NetAddress is thread safe.
 */
const NetAddress & NetInstanceServer::GetConnectAddressTCP(size_t clientID) const
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return client[clientID].GetConnectAddressTCP();
}

/**
 * @brief Retrieve the UDP address that the specified client
 * is connected to.
 *
 * @param clientID ID of client to use.
 * @return UDP remote address information of the specified client.
 * This reference will remain valid and unmodified until the next call.
 * @note Is okay to return reference because NetAddress is thread safe.
 */
const NetAddress & NetInstanceServer::GetConnectAddressUDP(size_t clientID) const
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return client[clientID].GetConnectedAddressUDP();
}

/**
 * @brief Empties the TCP received packet queue.
 *
 * When TCP packets are received and a receive function has not been set (in the constructor),
 * packets are put into a queue and received using GetPacketFromStoreTCP. If the queue gets too big
 * newer packets may not be received for some time and so it can be useful to empty the queue.
 * This command discards everything in the queue.\n\n
 *
 * Has no impact on the partial packet store. \n\n
 *
 * @param clientID ID of client to use.
 */
void NetInstanceServer::FlushRecvTCP(size_t clientID)
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	client[clientID].FlushRecvTCP();
}

/** 
 * @brief Retrieves the number of packets in the received TCP packet queue.
 *
 * @param clientID ID of client to use.
 * @return number of fully received TCP packets in the packet store.
 */
size_t NetInstanceServer::GetPacketAmountTCP(size_t clientID) const
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return client[clientID].GetPacketAmountTCP();
}

/**
 * @brief Starts the graceful disconnection process.
 *
 * Stops sending on TCP socket so that all further send operations will fail.
 * All data sent up till now will be received by recipient.
 * This method is part of the @ref gracefulDisconnectPage "graceful disconnect process".
 *
 * @param clientID ID of client to use.
 */
void NetInstanceServer::ShutdownTCP(size_t clientID)
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	client[clientID].ShutdownTCP();
}


/**
 * @brief Retrieves a complete packet from the TCP packet store.
 *
 * @param [out] destination Destination to copy into.
 * @param clientID ID of client to use.
 *
 * @return number of packets in packet store before this method call.
 */
size_t NetInstanceServer::GetPacketFromStoreTCP(Packet * destination, size_t clientID)
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return client[clientID].GetPacketFromStoreTCP(destination);
}

/**
 * @brief Sends a packet via TCP to specified client.
 *
 * @param packet Packet to send.
 * @param block If true the method will not return until @a packet is completely sent, note that this does not indicate that
 * the packet has been received by the recipient, instead it simply means the packet is in transit. \n
 * If false the method will return instantly even if the packet has not been sent.
 * @param clientID ID of client to send to.
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED if the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus NetInstanceServer::SendTCP(const Packet & packet, bool block, size_t clientID)
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	NetUtility::SendStatus returnMe = client[clientID].SendTCP(packet,block,GetSendTimeout());
	if(returnMe == NetUtility::SEND_FAILED_KILL)
	{
		ErrorOccurred(clientID);
	}
	return returnMe;
}

/**
 * @brief Sends a packet via TCP to all connected clients.
 *
 * @param packet Packet to send.
 * @param block If true the method will not return until @a packet is completely sent to all clients, note that this does not indicate that
 * the packet has been received by all clients, instead it simply means the packet is in transit. \n
 * If false the method will return instantly even if the packet has not been sent.
 * @param excludeClient Client ID of client not to send to.
 */
void NetInstanceServer::SendAllTCP(const Packet & packet, bool block, size_t excludeClient)
{
	for(size_t cl = 1;cl<=maxClients;cl++)
	{
		if(excludeClient != cl)
		{
			if(ClientConnected(cl) == NetUtility::CONNECTED)
			{
				SendTCP(packet,block,cl);
			}
		}
	}
}

/** 
 * @brief Sends a packet via UDP to the specified client.
 *
 * @param packet Packet to send.
 * @param block If true the method will not return until @a packet is completely sent, note that this does not indicate that
 * the packet has been received by the recipient, instead it simply means the packet is in transit. \n
 * If false the method will return instantly even if the packet has not been sent.
 * @param clientID ID of client to send to.
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED if the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus NetInstanceServer::SendUDP(const Packet & packet, bool block, size_t clientID)
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	NetInstanceUDP::ValidateIsEnabledUDP(__LINE__,__FILE__);

	// Ensures that the correct counter is increased when there is a counter per client.
	Packet * aux = const_cast<Packet*>(&packet);
	size_t originalClientFrom = aux->GetClientFrom();
	aux->SetClientFrom(clientID);

	NetUtility::SendStatus returnMe = socketUDP->Send(packet,block,&client[clientID].GetConnectedAddressUDP(),GetSendTimeout());
	if(returnMe == NetUtility::SEND_FAILED_KILL)
	{
		ErrorOccurred(clientID);
	}

	aux->SetClientFrom(originalClientFrom);
	return returnMe;
}

/**
 * @brief Sends a packet via UDP to all connected clients.
 *
 * @param packet Packet to send.
 * @param block If true the method will not return until @a packet is completely sent to all clients, note that this does not indicate that
 * the packet has been received by all clients, instead it simply means the packet is in transit. \n
 * If false the method will return instantly even if the packet has not been sent.
 * @param excludeClient ClientID of client not to send to.
 */
void NetInstanceServer::SendAllUDP(const Packet & packet, bool block, size_t excludeClient)
{
	for(size_t cl = 1;cl<=maxClients;cl++)
	{
		if(excludeClient != cl)
		{
			if(ClientConnected(cl) == NetUtility::CONNECTED)
			{
				SendUDP(packet,block,cl);
			}
		}
	}
}

/**
 * @brief Sends a packet via UDP to specified address.
 *
 * @param address Remote address to send to.
 * @param packet Packet to send.
 * @param block If true the method will not return until @a packet is completely sent to all clients, note that this does not indicate that
 * the packet has been received by all clients, instead it simply means the packet is in transit. \n
 * If false the method will return instantly even if the packet has not been sent.
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED if the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus NetInstanceServer::SendToUDP(const NetAddress & address, const Packet & packet, bool block)
{
	NetInstanceUDP::ValidateIsEnabledUDP(__LINE__,__FILE__);
	return socketUDP->Send(packet,block,&address,GetSendTimeout());
}

/**
 * @brief Retrieves the number of packets in the UDP received packet queue.
 *
 * @return the number of packets in the packet store.
 * @param clientID ID of client to use.
 * @param operationID Operation ID of operation to use, may be ignored.
 * @note Reimplemented to enforce multithreaded control rules.
 */
size_t NetInstanceServer::GetPacketAmountUDP(size_t clientID, size_t operationID) const
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return NetInstanceUDP::GetPacketAmountUDP(clientID,operationID);
}

/** 
 * @brief Empties the UDP received packet queue.
 *
 * When UDP packets are received and a receive function has not been set (in the constructor),
 * packets are put into a queue and received using GetPacketFromStoreUDP. If the queue gets too big
 * newer packets may not be received for some time and so it can be useful to empty the queue.
 * This command discards everything in the queue.\n\n
 *
 * @param clientID ID of client to use.
 * @note Reimplemented to enforce multithreaded control rules.
 */
void NetInstanceServer::FlushRecvUDP(size_t clientID)
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	NetInstanceUDP::FlushRecvUDP(clientID);
}

/**
 * @brief Retrieves a complete packet from the TCP received packet queue.
 *
 * @param [out] destination Destination to copy into.
 * @param clientID ID of client to use.
 * @param operationID ID of operation to use (optional, default 0).
 *
 * @return number of packets in packet store before this method call.
 * @note Reimplemented to enforce multithreaded control rules.
 */
size_t NetInstanceServer::GetPacketFromStoreUDP(Packet * destination, size_t clientID, size_t operationID)
{
	ValidateClientID(clientID,__LINE__,__FILE__);
	return NetInstanceUDP::GetPacketFromStoreUDP(destination, clientID,operationID);
}

/**
 * @brief	Sorts the vector clientSortedByAddressUDP in order of UDP address so
 * that it can be efficiently searched through.
 */
void NetInstanceServer::RequestResortClientsByAddressUDP()
{
	clientSortedByAddressUdpNeedsResort = true;
}

/** 
 * @brief Searches all connected clients and determines if the specified address
 * a remote UDP address belonging to one of them. If it is then the client
 * can be deemed connected via UDP.
 *
 * This method is used as part of the @ref handshakePage "handshaking process".
 *
 * @param addr Address to search for.
 *
 * @return 0 if no client was found.
 * @return >0 if client was found, this is the client ID of the client.
 */
size_t NetInstanceServer::FindClientByAddressUDP(const NetAddress & addr)
{
	clientSortedByAddressUDP.Enter();
	size_t returnMe = 0;
	try
	{
		size_t idWithinVector = clientSortedByAddressUDP.Find(comparatorSort,comparatorFind,&addr,clientSortedByAddressUdpNeedsResort);

		// Vector will have been sorted if necessary by Find.
		clientSortedByAddressUdpNeedsResort = false;

		if(idWithinVector >= clientSortedByAddressUDP.Size() ||
		   !clientSortedByAddressUDP.IsAllocated(idWithinVector) ||
		   clientSortedByAddressUDP[idWithinVector].GetConnectedAddressUDP() != addr)
		{
			returnMe = 0;
		}
		else
		{
			returnMe = clientSortedByAddressUDP[idWithinVector].GetClientID();
		}
	}
	catch(ErrorReport & Error){	clientSortedByAddressUDP.Leave(); throw(Error); }
	catch(...){ clientSortedByAddressUDP.Leave(); throw(-1); }
		
	clientSortedByAddressUDP.Leave();

	return returnMe;

	/* // OLD CODE <-- does work, but not as efficiently as above replacement.
	size_t returnMe = 0;
	for(size_t n = 1;n<client.Size();n++)
	{
		if(ClientConnected(n) != NetUtility::NOT_CONNECTED)
		{
			if(addr == client[n].GetConnectedAddressUDP())
			{
				returnMe = n;
				break;
			}
		}
	}
	
	return returnMe;
	*/
}


/**
 * @brief Determines whether UDP is enabled.
 *
 * @return true if UDP is enabled, false if not.
 */
bool NetInstanceServer::IsEnabledUDP() const
{
	return (this->socketUDP != NULL);
}

/**
 * @brief Retrieves the protocol mode in use.
 *
 * @return the UDP protocol mode in use, describing how data is manipulated during sending and receiving.
 */
NetMode::ProtocolMode NetInstanceServer::GetModeUDP() const
{
	NetInstanceUDP::ValidateIsEnabledUDP(__LINE__,__FILE__);
	return socketUDP->GetMode()->GetProtocolMode();
}

/**
 * @brief Retrieves the percentage of the partial packet that has been received.
 *
 * @param clientID ID of client to use, may be ignored.
 *
 * @return the percentage of the partial packet that has been received
 * i.e. Between 0 and 100 where 50 indicates that half the packet has
 * been received and we are now waiting for the second half to be received.
 */
double NetInstanceServer::GetPartialPacketPercentageTCP(size_t clientID) const
{
	return client[clientID].GetPartialPacketPercentageTCP();
}

/**
 * @brief Determines whether the nagle algorithm is enabled or disabled.
 *
 * @return true if the nagle algorithm is enabled for the instance.
 */
bool NetInstanceServer::GetNagleEnabledTCP() const
{
	return socketListening->GetSocket()->IsNagleEnabled();
}

/**
 * @brief Retrieves the TCP postfix in use in TCP mode NetMode::TCP_POSTFIX.
 *
 * If TCP mode is not NetMode::TCP_POSTFIX this command will fail.
 *
 * @return the postfix in use by the TCP instance.
 */
const Packet & NetInstanceServer::GetPostfixTCP() const
{
	const NetModeTcpPostfix * aux = NULL;
	try
	{
		aux = dynamic_cast<const NetModeTcpPostfix*>(socketListening->GetSocket()->GetMode());
	}
	catch (bad_cast exception)
	{
		_ErrorException(true,"retrieving the TCP postfix in use, invalid TCP mode",0,__LINE__,__FILE__);
	}
	return aux->GetPostfix();
}

/**
 * @brief Retrieves the state that the specified client's TCP connection is in.
 *
 * This method is part of the @ref gracefulDisconnectPage "graceful disconnect process".
 *
 * @param clientID ID of client to use.
 *
 * @return connection state of client purely looking at the TCP socket.
 *
 * @note GetConnectionState() and this method are unlinked.
 */
NetUtility::ConnectionStatus NetInstanceServer::GetConnectionStateTCP(size_t clientID) const
{
	return client[clientID].GetConnectionStateTCP();
}

/**
 * @brief	Called by the completion port when an error occurred during an operation.
 *
 * @param [in,out]	completionSocket	The socket responsible for the error. 
 * @param	clientID					The client ID responsible for the error.
 */
void NetInstanceServer::CompletionError(NetSocket * completionSocket, size_t clientID)
{
	// UDP
	if(completionSocket == socketUDP)
	{
		ErrorOccurred(clientID);
	}
	// TCP
	else
	{
		// GetConnectionState will return NetUtility::CONNECTED regardless of TCP socket connection state.
		// So, during graceful disconnection GetConnectionState will return NetUtility::CONNECTED.
		if(this->IsGracefulDisconnectEnabled() == false || client[clientID].GetConnectionState() != NetUtility::CONNECTED)
		{
			ErrorOccurred(clientID);
		}
	}
}

/**
 * @brief When send and receive operations are completed on this instance, this method is called.
 * When data is received from an unlisted UDP address, i.e. An address that is not stored
 * under any connected client ID, this method performs part of the @ref handshakePage "handshaking process".
 * 
 * @param socket [in,out] Socket that operation was started by.
 * @param bytes Number of bytes of data transferred in operation.
 * @param clientID ID of client that owns socket.
 */
void NetInstanceServer::DealCompletion(NetSocket * socket, DWORD bytes, size_t clientID)
{
	switch(socket->GetProtocol())
	{
	case(NetSocketSimple::UDP):
		{
			NetSocketUDP * completionSocket = static_cast<NetSocketUDP*>(socket);

			size_t clientID = FindClientByAddressUDP(completionSocket->GetRecvAddress());
			bool clientAlreadyConnected = (clientID > 0);
			if(clientAlreadyConnected)
			{
				try
				{
					completionSocket->DealWithData(completionSocket->recvBuffer,bytes,completionSocket->GetRecvFunction(),clientID,this->GetInstanceID());
				}
				// Disconnect client in the event of an error
				catch(ErrorReport & error){	ErrorOccurred(clientID); }
				catch(...){ ErrorOccurred(clientID);}
			}
			// !clientAlreadyConnected
			else
			{
				try
				{
					// Interface with packet using Packet class
					// newPacket and socket's WSABUF are now linked.
					Packet newPacket;
					newPacket.SetDataPtr(completionSocket->recvBuffer.buf,bytes,bytes);

					/**
					 * Discard prefix.
					 *
					 * We do this because after connection there may be connection UDP packets that are received.
					 * The prefix is always 0 which allows us to differentiate between connection
					 * packets and normal packets.
					 */
					newPacket.GetSizeT();

					// Determine what client this UDP packet claims to be from
					size_t clientID = newPacket.GetSizeT();

					// If clientID is out of bounds then ignore packet
					ValidateClientID(clientID,__LINE__,__FILE__);

					// Authenticate client
					// We need to resort later, but must take control of lock
					// in this order always, to avoid deadlock.
					clientSortedByAddressUDP.Enter();
					try
					{
						// Take control in case multiple threads reach this point at the same time
						client[clientID].Enter(); 

						try
						{
							_ErrorException((client[clientID].GetConnectionState() != NetUtility::CONNECTING),"handshaking with a client, client ID specified is not connecting",0,__LINE__,__FILE__);

							vector<int> connectCode;
							connectCode.resize(NetUtility::authenticationStrength);
							for(size_t n = 0;n<connectCode.size();n++)
							{
								connectCode[n] = newPacket.Get<int>();
							}

							_ErrorException((client[clientID].Authenticate(connectCode) == false),"handshaking with a client, client failed to authenticate",0,__LINE__,__FILE__);
							
							try
							{
								// Finish setting up client by finalizing its UDP configuration
								// Note that LoadUDP changes the connection status of the client.
								// Ensure this is the last thing done so that connection state changes last.
								// ClientJoined will send confirmation to client.
								client[clientID].LoadUDP(completionSocket->GetRecvAddress());
							}
							catch(ErrorReport & Error){	ErrorOccurred(clientID); throw(Error); }
							catch(...){ ErrorOccurred(clientID); throw(-1); }

							// Not in above try/catch because not client specific.
							RequestResortClientsByAddressUDP();
						}
						// Release control of all objects before throwing final exception
						catch(ErrorReport & Error){	client[clientID].Leave(); throw(Error); }
						catch(...){ client[clientID].Leave(); throw(-1); }
						client[clientID].Leave();

					}
					catch(ErrorReport & Error){	clientSortedByAddressUDP.Leave(); throw(Error); }
					catch(...){ clientSortedByAddressUDP.Leave(); throw(-1); }
					clientSortedByAddressUDP.Leave();	
				}
				// If an exception occurs then ignore packet silently
				catch(ErrorReport & error){}
			}
		}
		break;

	case(NetSocketSimple::TCP):
		{
			NetSocketTCP * completionSocket = static_cast<NetSocketTCP*>(socket);
				
			try
			{
				// Client must be connected, or connecting
				if(client[clientID].GetConnectionState() != NetUtility::NOT_CONNECTED)
				{
					// Deal with received data
					completionSocket->DealWithData(completionSocket->recvBuffer,bytes,completionSocket->GetRecvFunction(),clientID,this->GetInstanceID());
				}
			}
			// Disconnect client in the event of an error
			catch(ErrorReport & error){ ErrorOccurred(clientID); }
			catch(...){ ErrorOccurred(clientID);}
		}
		break;

	default:
		_ErrorException(true,"Dealing with the result of a receive operation, invalid protocol",0,__LINE__,__FILE__);
		break;
	}
}

/**
 * @brief	Closes any sockets in use by the instance.
 *
 * Must be called in the destructor so that instance is not cleaned up until completion
 * port is done using it.
 */
void NetInstanceServer::CloseSockets()
{
	for(size_t n = 1;n<client.Size();n++) // element 0 is not allocated
	{
		client[n].CloseSockets();
	}
	
	socketListening->Close();
	NetInstanceUDP::CloseSockets();
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetInstanceServer::TestClass()
{
	const char * cCommand = "NetInstanceServer::TestClass";
	cout << "Testing NetInstanceServer class...\n";
	bool problem = false;


	NetUtility::LoadEverything(2,0);
	{
		NetAddress localHost = NetUtility::ConvertDomainNameToIP("localhost");

		// Setup server
		NetInstanceProfile profileServer;
		NetAddress localAddrServer(localHost.GetIP(),6500);
		profileServer.SetLocalAddrTCP(localAddrServer);
		profileServer.SetLocalAddrUDP(localAddrServer);
		profileServer.SetMemoryRecycleTCP(3,20);
		profileServer.SetMemoryRecycleUDP(2,10);
	
		NetInstanceServer * server = new NetInstanceServer(10,profileServer);

		try
		{
			NetInstanceServer * server2 = new NetInstanceServer(10,profileServer);
		}
		catch(ErrorReport & report)
		{
			cout << "Expected error occurred\n";
		}


		// Setup client and connect it to server
		NetInstanceProfile profileClient;
		profileClient.SetMemoryRecycleTCP(3,20);
		profileServer.SetMemoryRecycleUDP(2,10);

		StoreVector<NetInstanceClient> client;
		client.Add(new NetInstanceClient(profileClient));
		client.Add(new NetInstanceClient(profileClient));
		client.Add(new NetInstanceClient(profileClient));
		client.Add(new NetInstanceClient(profileClient));
		client.Add(new NetInstanceClient(profileClient));

		for(size_t n = 0;n<client.Size();n++)
		{
			client[n].Connect(&localAddrServer,&localAddrServer,10000,false);
		}

		// Main loop
		Packet recvPacket;

		Timer loopRunTime(20000);
		Timer sendFreqTCP(1000);
		Timer sendFreqUDP(1000);
		Timer clientDisconnectFreq(2000);
		Timer clientReconnectFreq(4000);

		size_t disconnectCounter = 0;

		while(loopRunTime.GetState() == false)
		{
			for(size_t n = 0;n<client.Size();n++)
			{
				if(client.IsAllocated(n) && client[n].IsConnecting() == true)
				{
					NetUtility::ConnectionStatus connectionProgress = client[n].PollConnect();

					if(connectionProgress == NetUtility::CONNECTED)
					{
						cout << "Client connected to server\n";
					}
					else
					{
						if(connectionProgress != NetUtility::STILL_CONNECTING)
						{
							cout << "Client failed to connect to server: " << connectionProgress << "\n";
							break;
						}
					}
				}
			}

			// Accept new connections
			size_t newClient = server->ClientJoined();

			if(newClient > 0)
			{
				cout << "A new client has joined: " << newClient << " T_RML " << server->GetRecvMemoryLimitTCP(newClient) << ", T_RMU " << server->GetRecvMemorySizeTCP(newClient) << ", T_SML " << server->GetSendMemoryLimitTCP(newClient) << ", T_SMU " << server->GetSendMemorySizeTCP(newClient) << ", U_SML " << server->GetSendMemoryLimitUDP() << ", U_SMU " << server->GetSendMemorySizeUDP() << ", U_RML " << server->GetRecvMemoryLimitUDP(newClient) << ", U_RMU " << server->GetRecvMemorySizeUDP(newClient) << ".\n";
			}

			// Deal with leaving clients
			size_t clientLeft = server->GetDisconnect();

			if(clientLeft > 0)
			{
				cout << "A client has left: " << clientLeft << "\n";
			}

			// Server receive TCP data
			for(size_t n = 1;n<server->GetMaxClients();n++)
			{
				if(server->ClientConnected(n) == NetUtility::CONNECTED)
				{
					if(server->GetPacketFromStoreTCP(&recvPacket,n) > 0)
					{
						clock_t clock = recvPacket.GetClockT();
						char * str = recvPacket.GetStringC(0,true);
						cout << "TCP packet received by server from client " << recvPacket.GetClientFrom() << ": " << clock << ", " << str << ", T_RMU " << server->GetRecvMemorySizeTCP(n) << ", T_SMU " << server->GetSendMemorySizeTCP(n) << ".\n";
						delete[] str;
					}

					// Receive UDP data
					if(server->GetPacketFromStoreUDP(&recvPacket,n) > 0)
					{
						clock_t clock = recvPacket.GetClockT();
						char * str = recvPacket.GetStringC(0,true);
						cout << "UDP packet received by server from client " << recvPacket.GetClientFrom() << ": " << clock << " with age of " << recvPacket.GetAge() << ", U_SMU " << server->GetSendMemorySizeUDP() << ", U_RMU " << server->GetRecvMemorySizeUDP(n) << ".\n";
						delete[] str;
					}
				}
			}

			// Disconnect
			if(clientDisconnectFreq.GetState() == true)
			{
				server->DisconnectClient((disconnectCounter % client.Size())+1);
				disconnectCounter++;
			}

			// Clients
			for(size_t n = 0;n<client.Size();n++)
			{
				if(client.IsAllocated(n))
				{
					if(client[n].ClientConnected() == NetUtility::CONNECTED)
					{
						// Receive TCP data
						if(client[n].GetPacketFromStoreTCP(&recvPacket) > 0)
						{
							clock_t clock = recvPacket.GetClockT();
							char * str = recvPacket.GetStringC(0,true);
							cout << "TCP packet received by client " << client[n].GetClientID() << ": " << clock << ", " << str << ", T_RMU " << client[n].GetRecvMemorySizeTCP(n) << ", T_SMU " << client[n].GetSendMemorySizeTCP(n) << ".\n";
							delete[] str;

							// Send back TCP data.
							client[n].SendTCP(recvPacket,false);
						}

						// Receive UDP data
						if(client[n].GetPacketFromStoreUDP(&recvPacket) > 0)
						{
							clock_t clock = recvPacket.GetClockT();
							char * str = recvPacket.GetStringC(0,true);
							cout << "UDP packet received by client " << client[n].GetClientID() << ": " << clock << ", " << str << " with age of " << recvPacket.GetAge() << ", U_SMU " << client[n].GetSendMemorySizeUDP() << ", U_RMU " << client[n].GetRecvMemorySizeUDP(n) << ".\n";
							delete[] str;

							// Send back UDP data.
							client[n].SendUDP(recvPacket,false);
						}
					}
					else
					{
						if(client[n].GetDestroyRequest() == true)
						{
							client.Deallocate(n);
						}
					}
				}
				else
				{
					// Reconnect
					if(clientReconnectFreq.GetState() == true)
					{
						client.Allocate(n,new NetInstanceClient(profileClient));
						client[n].Connect(&localAddrServer,&localAddrServer,10000,false);
					}
				}
			}


			// Send TCP data from server to client
			if(sendFreqTCP.GetState() == true)
			{
				Packet sendMe;
				sendMe.AddClockT(clock());
				sendMe.AddStringC("Hello WORLD",0,true);
				server->SendAllTCP(sendMe,false,0);
			}

			if(sendFreqUDP.GetState() == true)
			{
				Packet sendMe;
				sendMe.AddClockT(clock());
				sendMe.AddStringC("hello universe..",0,true);
				server->SendAllUDP(sendMe,false,0);
			}
		}

		client.Clear();
		delete server;
	}
	
	NetUtility::UnloadEverything();
	
	cout << "\n\n";
	return !problem;
}
