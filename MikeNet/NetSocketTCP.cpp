#include "FullInclude.h"

/**
 * @brief	Associates event object with FD_CLOSE notification if @ref gracefulDisconnectPage "graceful disconnection" is enabled. 
 */
void NetSocketTCP::AssociateGracefulDisconnect()
{
	// Associate event with FD_CLOSE notification
	if(IsGracefulDisconnectEnabled() == true)
	{
		int result = WSAEventSelect(winsockSocket,gracefulShutdown->GetEventHandle(),FD_CLOSE);
		_ErrorException((result == SOCKET_ERROR),"associating an event object with FD_CLOSE notification",WSAGetLastError(),__LINE__,__FILE__);
	}
}

/**
 * @brief Initializes object, contains code that is shared between constructors.
 *
 * @param [in]		modeTCP				Describes how received data should be dealt with and how
 * 										sent data should be modified. Must not be NULL.
 * @param	gracefulDisconnectEnabled	If true @ref gracefulDisconnectPage "graceful
 * 										disconnection" is possible. 
 */
void NetSocketTCP::Initialize(bool gracefulDisconnectEnabled, NetModeTcp * modeTCP)
{
	_ErrorException((modeTCP==NULL),"constructing NetSocketTCP object, modeTCP must not be NULL",0,__LINE__,__FILE__);
	
	this->modeTCP = modeTCP;

	sendPossible = true;

	if(gracefulDisconnectEnabled == true)
	{
		// Create event
		// Starts off unsignaled and is signaled when FD_CLOSE notification is detected
		gracefulShutdown = new (nothrow) ConcurrencyEvent(false);
		Utility::DynamicAllocCheck(gracefulShutdown,__LINE__,__FILE__);
	}
	else
	{
		gracefulShutdown = NULL;
	}
}

/**
 * @brief	Constructor, sets up socket ready to be connected and transfer data. 
 *
 * @param	wsaBufferLength				Length of receive buffer in bytes;
 * 										this is the maximum amount of data that can be received
 * 										in one receive operation. 
 * @param	localAddr					Local address to bind to, if IP or port is set to 0 then
 * 										winsock will automatically find one. 
 * @param	nagleEnabled				If true then the nagle algorithm will be enabled. See.
 * 										NetSocketSimple::nagleEnabled for more information. 
 * @param	gracefulDisconnectEnabled	If true @ref gracefulDisconnectPage "graceful
 * 										disconnection" is possible. 
 * @param [in]		modeTCP				Describes how received data should be dealt with and how
 * 										sent data should be modified. Must not be NULL.
 * @param [in]		recvFunc			Function that any received packets will be passed to. (optional, default NULL). 
 *										Care must be taken to ensure that this function is thread safe.
 *										If NULL then packets will not be passed to a function, and instead
 *										will be put into a queue.
 */
NetSocketTCP::NetSocketTCP(size_t wsaBufferLength, const NetAddress & localAddr, bool nagleEnabled, bool gracefulDisconnectEnabled, NetModeTcp * modeTCP, NetSocket::RecvFunc recvFunc) : NetSocket(wsaBufferLength, recvFunc)
{
	try
	{
		Initialize(gracefulDisconnectEnabled,modeTCP);

		Setup(NetSocketSimple::TCP);
		SetReusable();
		AssociateCompletionPort();

		if(!gracefulDisconnectEnabled)
		{
			SetHardShutdown();
		}
		else
		{
			AssociateGracefulDisconnect();
		}

		if(!nagleEnabled)
		{
			DisableNagle();
		}

		Bind(localAddr);
	}
	catch(ErrorReport & errorReport)
	{
		delete modeTCP;
		throw errorReport;
	}
}

/**
 * @brief Constructor, sets up socket ready to be loaded (via LoadSOCKET()) with a fully operational socket.
 *
 * @warning This socket is not ready to be used yet, LoadSOCKET MUST be used before attempting to transfer data.
 *
 * @note NetServerClient has an object constructed in this way as it is more efficient to allocate memory to the WSABUF
 * during initial instance setup, since typically this buffer will not be reallocated during the lifetime of the
 * instance. An alternative method (which is not implemented in MikeNet) would be to pass a new NetSocketTCP object
 * after WSAAccept accepts a connection but this has the overhead of WSABUF allocation during connection and equally.
 * WSABUF deallocation during disconnection of a client.
 *
 * @param	wsaBufferLength				Length of receive buffer in bytes;
 * 										this is the maximum amount of data that can be received
 * 										in one receive operation. 
 * @param	nagleEnabled				True if the nagle algorithm should be enabled, false if not.
 *										See NetSocketSimple::nagleEnabled for more information.
 * @param	gracefulDisconnectEnabled	If true @ref gracefulDisconnectPage "graceful
 * 										disconnection" is possible. 
 * @param [in]		modeTCP				Describes how received data should be dealt with and how
 * 										sent data should be modified. Must not be NULL.
 * @param [in]		recvFunc			Function that any received packets will be passed to. (optional, default NULL). 
 *										Care must be taken to ensure that this function is thread safe.
  *										If NULL then packets will not be passed to a function, and instead
 *										will be put into a queue.
 */
NetSocketTCP::NetSocketTCP(size_t wsaBufferLength, bool nagleEnabled, bool gracefulDisconnectEnabled, NetModeTcp * modeTCP, NetSocket::RecvFunc recvFunc) : NetSocket(wsaBufferLength, recvFunc)
{
	try
	{
		Initialize(gracefulDisconnectEnabled,modeTCP);
		this->hardShutdown = !gracefulDisconnectEnabled;
		this->nagleEnabled = nagleEnabled;
		this->reusable = true; // Only set to true if listening socket uses SetReusable on itself.
	}
	catch(ErrorReport & errorReport)
	{
		delete modeTCP;
		throw errorReport;
	}
}

/**
 * @brief Copy constructor / assignment operator helper method.
 *
 * @note Does not copy modeTCP, do this elsewhere.
 *
 * @param	copyMe	Object to copy.
 */
void NetSocketTCP::Copy(const NetSocketTCP & copyMe)
{
	this->sendPossible = copyMe.sendPossible;

	if(copyMe.IsGracefulDisconnectEnabled() == true)
	{
		if(gracefulShutdown != NULL)
		{
			*gracefulShutdown = *copyMe.gracefulShutdown;
		}
		else
		{
			gracefulShutdown = new (nothrow) ConcurrencyEvent(*copyMe.gracefulShutdown);
			Utility::DynamicAllocCheck(gracefulShutdown,__LINE__,__FILE__);
		}
	}
	else
	{
		gracefulShutdown = NULL;
	}
}

/**
 * @brief Deep copy constructor.
 *
 * Socket will not be ready to send or receive data, even if @a copyMe is.
 * First LoadSOCKET must be used to load a fully setup winsock socket object.\n\n
 *
 * NetSocketSimple::winsockSocket is not copied.
 *
 * @param	copyMe	Object to copy.
 */
NetSocketTCP::NetSocketTCP(const NetSocketTCP & copyMe) : NetSocket(copyMe)
{
	Initialize(copyMe.IsGracefulDisconnectEnabled(), copyMe.GetMode()->Clone());
	Copy(copyMe);
}

/**
 * @brief Deep assignment operator.
 *
 * NetSocketSimple::winsockSocket is not copied.
 *
 * @param	copyMe	Object to copy. 
 *
 * @return	reference to this object.
 */
NetSocketTCP & NetSocketTCP::operator= (const NetSocketTCP & copyMe)
{
	Copy(copyMe);

	delete modeTCP;
	modeTCP = copyMe.GetMode()->Clone();

	NetSocket::operator=(copyMe);
	return *this;
}

/**
 * @brief Destructor.
 */
NetSocketTCP::~NetSocketTCP()
{
	const char * cCommand = "an internal function (~NetSocketTCP)";
	try
	{	
		Close(); // must close socket before cleaning up
		delete modeTCP;
		delete gracefulShutdown;
	}
	MSG_CATCH
}

/**
 * @brief Loads a TCP winsock socket that is already fully setup and connected.
 *
 * This is used when @c WSAAccept accepts a connection.
 * WSAAccept does not associate the socket with the completion port, so this is done by this method.
 *
 * @param paraSocket TCP winsock socket that is fully setup.
 * @param addr Address that socket is connected to.
 */
void NetSocketTCP::LoadSOCKET(SOCKET paraSocket, const NetAddress & addr)
{
	NetSocketSimple::LoadSOCKET(paraSocket,addr);
	NetSocket::AssociateCompletionPort();
	AssociateGracefulDisconnect();
}

/**
 * @brief Starts a receive operation via TCP.
 *
 * This method calls the winsock method @c WSARecv.\n\n
 *
 * If @c WSARecv is successful the result of the operation (which will probably not complete instantly) will be passed to the completion port.
 * Upon completion NetSocket::recvBuffer will be filled with received data.\n\n
 * 
 * If @c WSARecv is unsuccessful the operation will not complete so the completion port will receive no notification. This
 * means that we must manually set the overlapped event in the case of initial failure.
 * 
 * @warning Receive operations will fail when the thread exits, so if you start a receive operation and the thread that started
 * it completes then the operation will fail and no data will be received.
 *
 * @return true if an error occurred and data was not properly received, false if no error occurred.
 */
bool NetSocketTCP::Recv()
{
	int iResult;

	// Prepare for new receive operation
	ClearRecv();

	// Start new receive operation
	notDealingWithData.Set(false);
	iResult = WSARecv(winsockSocket, &recvBuffer, 1, NULL, &flags, &recvOverlapped, NULL);

	// WSA_IO_PENDING indicates that receive operation was started, but did not complete instantly
	// The receive operation may still be successful however and complete at a later time.
	bool error = (iResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING);
	if(error)
	{
		size_t error = WSAGetLastError();
		SetRecvOverlappedEvent();
		notDealingWithData.Set(true);
	}
	return error;
}

/**
 * @brief Checks the status of the TCP handshake routine started by NetSocketSimple::Connect().
 * 
 * @return true if the connection is in progress.
 * @return false if the connection has completed.
 */
bool NetSocketTCP::PollConnect() const
{
	int returnMe = -1;

	timeval tvTimeout; 
	tvTimeout.tv_sec = 0;
	tvTimeout.tv_usec = 100000; // 100 milliseconds

	fd_set fdSetWrite;
	FD_ZERO(&fdSetWrite);
	FD_SET(winsockSocket,&fdSetWrite);

	returnMe = select(NULL,NULL,&fdSetWrite,NULL,&tvTimeout);
	_ErrorException((returnMe==SOCKET_ERROR),"checking the status of the connection process",WSAGetLastError(),__LINE__,__FILE__);

	return(returnMe == 0);
}

/**
 * @brief Halts sending on socket so that all further send operations will fail.
 *
 * See NetSocketTCP::sendPossible for more information.
 * 
 * @throws ErrorReport If graceful disconnect is disabled.
 */
void NetSocketTCP::StopSend()
{
	_ErrorException((IsGracefulDisconnectEnabled() == false),"stopping send operations on a TCP socket, graceful disconnect must be enabled",0,__LINE__,__FILE__);

	// Stop sending on socket and send FD_CLOSE notification
	int result = shutdown(winsockSocket,SD_SEND);
	_ErrorException((result == SOCKET_ERROR),"shutting down a socket",WSAGetLastError(),__LINE__,__FILE__);

	sendPossible = false;
}

/**
 * @brief Calls StopSend().
 */
void NetSocketTCP::Shutdown()
{
	StopSend();
}

/**
 * @brief Determines whether further send operations can be initiated.
 *
 * See NetSocketTCP::sendPossible for more information.
 *
 * @return true if further send operations can be initiated.
 * @return false if further send operations will fail.
 */
bool NetSocketTCP::IsSendPossible() const
{
	return sendPossible;
}

/**
 * @brief Determines whether further receive operations can be initiated.
 *
 * See NetSocketTCP::gracefulShutdown for more information.
 *
 * @return true if further receive operations can be initiated.
 * @return false if further receive operations will fail.
 */
bool NetSocketTCP::IsRecvPossible() const
{
	if(IsGracefulDisconnectEnabled() == false)
	{
		return true;
	}
	else
	{
		return !gracefulShutdown->Get(); // gracefulShutdown is signaled when receiving is no longer possible
	}
}

/** 
 * @brief Sends a packet using this socket.
 *
 * @param packet Packet to send.
 * @param block If true the method will not return until @a packet is completely sent, note that this does not indicate that
 * the packet has been received by the recipient, instead it simply means the packet is in transit. \n
 * If false the method will return instantly even if the packet has not been sent.
 * @param sendToAddr Ignored.
 * @param timeout Length of time in milliseconds to wait before canceling send operation.
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED if the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus NetSocketTCP::Send(const Packet & packet, bool block, const NetAddress * sendToAddr,unsigned int timeout)
{
	return NetSocket::Send(modeTCP->GetSendObject(&packet,block),NULL,timeout);
}

/**
 * @brief Closes socket and resets NetSocketTCP::modeTCP to unused state. 
 */
void NetSocketTCP::Close()
{
	NetSocket::Close();
	modeTCP->ClearData();
}

/**
 * @brief Queries the status of the connection.
 *
 * This method is part of the @ref gracefulDisconnectPage "graceful disconnect process".
 *
 * @return NetUtility::CONNECTED: Fully connected.
 * @return NetUtility::NOT_CONNECTED: Not connected at all.
 * @return NetUtility::NO_RECV: Connected but new data cannot be received because Shutdown() has been used by the end point that we are connected to (only possible if @ref gracefulDisconnectPage "graceful disconnection" is enabled).
 * @return NetUtility::NO_SEND: Connected but data cannot be sent because Shutdown() has been used by the end point that we are connected to (only possible if @ref gracefulDisconnectPage "graceful disconnection" is enabled).
 * @return NetUtility::NO_SEND_RECV: Connected but data cannot be sent or received because Shutdown() has been used on both sides,
 * only data that has already been received and is in the packet queue can be received (only possible if @ref gracefulDisconnectPage "graceful disconnection" is enabled).
 */
NetUtility::ConnectionStatus NetSocketTCP::GetConnectionStatus() const
{
	NetUtility::ConnectionStatus returnMe;

	if(this->IsFullyOperational() == false)
	{
		returnMe = NetUtility::NOT_CONNECTED;
	}
	else
	{
		// Connected but graceful shutdown may be in progress
		if(IsGracefulDisconnectEnabled() == true)
		{
			// If send is possible but receive is not then other end started
			// graceful shutdown process
			// If both are possible then graceful disconnect is not in progress
			// and so we are fully connected
			if(IsSendPossible() == true)
			{
				if(notDealingWithData.Get() == true && IsRecvPossible() == false)
				{
					returnMe = NetUtility::NO_RECV;
				}
				else
				{
					returnMe = NetUtility::CONNECTED;
				}
			}
			else
			{
				// If receive is possible but send is not then we started graceful
				// shutdown process
				// If both are possible then graceful disconnect is not in progress
				// and so we are fully connected
				if(IsRecvPossible() == true)
				{
					// IsSendPossible will DEFINATELY RETURN FALSE because we have already
					// checked it above

					//if(IsSendPossible() == false)
						returnMe = NetUtility::NO_SEND;
					//else
						//returnMe = NetUtility::CONNECTED;
				}
				// If neither receive or send is possible then graceful shutdown process has finished
				else
				{
					// All data has been used and we should cleanup
					if(notDealingWithData.Get() == true && modeTCP->GetPacketAmount() == 0)
					{
						returnMe = NetUtility::NOT_CONNECTED;
					}
					// Not all data has been used, but we cannot send
					// or receive via winsock
					else
					{
						returnMe = NetUtility::NO_SEND_RECV;
					}
				}
			}
		}
		// Connected
		else
		{
			returnMe = NetUtility::CONNECTED;
		}
	}

	return returnMe;
}

/**
 * @brief Retrieves constant pointer to TCP mode object.
 *
 * @return	returns pointer to object (which cannot be modified) representing TCP mode specific options.
 */
const NetModeTcp * NetSocketTCP::GetMode() const
{
	return modeTCP;
}

/**
 * @brief Retrieves mutable pointer to TCP mode object.
 *
 * @return	returns pointer to object representing TCP mode specific options.
 */
NetModeTcp * NetSocketTCP::GetMode()
{
	return modeTCP;
}

/**
 * @brief Determines whether @ref gracefulDisconnectPage "graceful disconnection" is enabled.
 *
 * See NetSocketTCP::gracefulDisconnectEnabled for more information.
 *
 * @return gracefulDisconnectEnabled true if @ref gracefulDisconnectPage "graceful disconnection" is possible.
 */
bool NetSocketTCP::IsGracefulDisconnectEnabled() const
{
	return (gracefulShutdown != NULL);
}

/**
 * @brief Retrieves the protocol type that the socket represents as an enum.
 *
 * @return NetSocketSimple::TCP.
 */
NetSocket::Protocol NetSocketTCP::GetProtocol() const
{
	return NetSocketSimple::TCP;
}

/**
 * @brief Deals with newly received data using the stored NetModeTcp object.
 *
 * @param buffer Newly received data.
 * @param completionBytes Number of bytes of new data stored in NetSocket::buffer.
 * @param [in] recvFunc Method will be executed and data not added to the queue if this is non NULL.
 * @param instanceID Instance that data was received on.
 * @param clientID ID of client that data was received from, set to 0 if not applicable.
 */
void NetSocketTCP::DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc recvFunc, size_t clientID, size_t instanceID)
{
	try
	{
		modeTCP->DealWithData(buffer,completionBytes,recvFunc,clientID,instanceID);
	}
	// Indicate that we are no longer dealing with data in the event of an error
	catch(ErrorReport & error){	notDealingWithData.Set(true); throw error;}
	catch(...){ notDealingWithData.Set(true); throw -1; }

	notDealingWithData.Set(true);
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetSocketTCP::TestClass()
{
	cout << "Testing NetSocketTCP class...\n";
	bool problem = false;

	NetUtility::StartWinsock();
	NetUtility::SetupCompletionPort(2);
	{
		NetSocketTCP socket(1024, false, true, new NetModeTcpPrefixSize(2048,true));

		cout << "Testing NetSocketTCP copy constructor and assignment operator..\n";
		NetSocketTCP copyConstructor(socket);
	
		if(copyConstructor.GetRecvBufferLength() != 1024 ||
		   copyConstructor.IsGracefulDisconnectEnabled() != true ||
		   copyConstructor.GetMode()->GetMaxPacketSize() != 2048 ||
		   copyConstructor.GetMode()->IsAutoResizeEnabled() != true)
		{
			cout << " Copy constructor is bad\n";
			problem = true;
		}
		else
		{
			cout << " Copy constructor is good\n";
		}
	
		Packet postfix;
		NetSocketTCP assignmentOperator(4000,true,false,new NetModeTcpPostfix(8000,false,postfix));
		assignmentOperator = socket;
	
		if(assignmentOperator.GetRecvBufferLength() != 1024 ||
		   assignmentOperator.IsGracefulDisconnectEnabled() != true ||
		   assignmentOperator.GetMode()->GetMaxPacketSize() != 2048 ||
		   assignmentOperator.GetMode()->IsAutoResizeEnabled() != true ||
		   assignmentOperator.GetMode()->GetProtocolMode() != NetMode::TCP_PREFIX_SIZE)
		{
			cout << " Assignment operator is bad\n";
			problem = true;
		}
		else
		{
			cout << " Assignment operator is good\n";
		}
	}
	
	//clock_t lastClock = clock(); // Uncomment for stress test
	//while(clock() - lastClock < 40000) // Uncomment for stress test
	{
		const char * localHost = NetUtility::ConvertDomainNameToIP("localhost").GetIP();
		NetAddress localAddrListening(localHost,14000);
		NetAddress localAddrClient(localHost,5432);

		bool gracefulDisconnectEnabled = true;
		bool nagleEnabled = true;

		// Setup client that is connected to listening socket
		cout << "Setting up client that is connected to a listening socket..\n";
		NetSocketListening listeningSocket(localAddrListening,new NetSocketTCP(1024,nagleEnabled,gracefulDisconnectEnabled,new NetModeTcpPrefixSize(2048,false)));
		NetSocketTCP listeningSocketClient(1024,nagleEnabled,gracefulDisconnectEnabled,new NetModeTcpPrefixSize(2048,false));

		NetSocketTCP client(1024,localAddrClient,nagleEnabled,gracefulDisconnectEnabled,new NetModeTcpPrefixSize(2048,false));

		if(NetSocketListening::HelperTestClass(listeningSocket,listeningSocketClient,client) == false)
		{
			problem = true;
		}

		// Start receive operations
		cout << "Starting receive operations..\n";
		client.Recv();
		listeningSocketClient.Recv();

		
		// Send data from client to server
		cout << "Sending data from client to server..\n";
		Packet sentPacket("hello world");
		NetUtility::SendStatus status = client.Send(sentPacket,false,NULL,INFINITE);
		if(status != NetUtility::SEND_COMPLETED && status != NetUtility::SEND_IN_PROGRESS)
		{
			cout << " Send is bad: " << status << "\n";
			problem = true;
		}
		else
		{
			cout << " Send is good: " << status << "\n";
		}

		// Wait for data to be received by server
		cout << "Waiting for data to be received by server..\n";
		Packet receivedPacket;
		while(listeningSocketClient.GetMode()->GetPacketFromStore(&receivedPacket) == 0)
		{
			Sleep(10);
		}

		if(receivedPacket != sentPacket)
		{
			cout << " Packet received is bad\n";
			problem = true;
		}
		else
		{
			cout << " Packet received is good!\n";
		}

		// GRACEFUL DISCONNECTION

		// FULLY CONNECTED
		if(listeningSocketClient.IsRecvPossible() == false ||
		   listeningSocketClient.IsSendPossible() == false ||
		   listeningSocketClient.GetConnectionStatus() != NetUtility::CONNECTED ||
		   client.IsRecvPossible() == false ||
		   client.IsSendPossible() == false ||
		   client.GetConnectionStatus() != NetUtility::CONNECTED)
		{
			cout << "Connection status is good (fully connected)\n";
			problem = true;
		}
		else
		{
			cout << "Connection status is good (fully connected)\n";
		}
		
		// SHUTDOWN ON SERVER SIDE
		// Data can no longer be sent from the server, or received by the client.
		// Data can be received by the server and sent by the client.
		cout << "Starting graceful disconnection on server side..\n";
		listeningSocketClient.Shutdown();

		// Wait for shutdown notification to be received by client
		cout << " Waiting for notification to be received by client..\n";
		while(client.IsRecvPossible() == true)
		{
			Sleep(1);
		}
		cout << " Notification received\n";

		if(listeningSocketClient.IsRecvPossible() == false ||
		   listeningSocketClient.IsSendPossible() == true ||
		   listeningSocketClient.GetConnectionStatus() != NetUtility::NO_SEND ||
		   client.IsRecvPossible() == true ||
		   client.IsSendPossible() == false ||
		   client.GetConnectionStatus() != NetUtility::NO_RECV)
		{
			cout << "Connection status is good is bad (shutdown)\n";
			problem = true;
		}
		else
		{
			cout << "Connection status is good is good (shutdown)\n";
		}

		// Send data from client to server
		cout << "Sending data from client to server..\n";
		sentPacket = "hihihi everyone!";
		status = client.Send(sentPacket,false,NULL,INFINITE);
		if(status != NetUtility::SEND_COMPLETED && status != NetUtility::SEND_IN_PROGRESS)
		{
			cout << " Send is bad: " << status << "\n";
			problem = true;
		}
		else
		{
			cout << " Send is good: " << status << "\n";
		}

		// Wait for data to be received by server
		cout << " Waiting for data to be received by server..\n";
		while(listeningSocketClient.GetMode()->GetPacketFromStore(&receivedPacket) == 0)
		{
			Sleep(10);
		}

		if(receivedPacket != sentPacket)
		{
			cout << " Packet received is bad\n";
			problem = true;
		}
		else
		{
			cout << " Packet received is good!\n";
		}

		// SHUTDOWN ON CLIENT SIDE
		// First send a packet to the server. This allows us to test
		// NO_SEND_RECV connection status which is only seen when packets remain
		// in the queue and Shutdown has been used on both sides.
		cout << "Shutting down connection on client side..\n";
		status = client.Send(sentPacket,false,NULL,INFINITE);
		if(status != NetUtility::SEND_COMPLETED && status != NetUtility::SEND_IN_PROGRESS)
		{
			cout << " Send is bad: " << status << "\n";
			problem = true;
		}
		else
		{
			cout << " Send is good: " << status << "\n";
		}

		client.Shutdown();
		
		// Wait for shutdown notification to be received by server
		cout << " Waiting for notification to be received by server..\n";
		while(listeningSocketClient.IsRecvPossible() == true)
		{
			Sleep(1);
		}
		cout << " Notification received\n";

		if(listeningSocketClient.IsRecvPossible() == true ||
		   listeningSocketClient.IsSendPossible() == true ||
		   listeningSocketClient.GetConnectionStatus() != NetUtility::NO_SEND_RECV ||
		   client.IsRecvPossible() == true ||
		   client.IsSendPossible() == true ||
		   client.GetConnectionStatus() != NetUtility::NOT_CONNECTED)
		{
			cout << "Connection status is bad (shutdown both sides, packet remaining)\n";
			cout << client.GetConnectionStatus() << '\n';
			cout << listeningSocketClient.GetConnectionStatus() << '\n';
			problem = true;
		}
		else
		{
			cout << "Connection status is good (shutdown both sides, packet remaining)\n";
		}

		// Wait for data to be received by server
		cout << " Waiting for data to be received by server..\n";
		while(listeningSocketClient.GetMode()->GetPacketFromStore(&receivedPacket) == 0)
		{
			Sleep(10);
		}

		if(receivedPacket != sentPacket)
		{
			cout << " Packet received is bad\n";
			problem = true;
		}
		else
		{
			cout << " Packet received is good!\n";
		}

		// Now that there is no data in the queue and Shutdown has been
		// used on both sides the sockets are fully disconnected.
		if(listeningSocketClient.IsRecvPossible() == true ||
		   listeningSocketClient.IsSendPossible() == true ||
		   listeningSocketClient.GetConnectionStatus() != NetUtility::NOT_CONNECTED ||
		   client.IsRecvPossible() == true ||
		   client.IsSendPossible() == true ||
		   client.GetConnectionStatus() != NetUtility::NOT_CONNECTED)
		{
			cout << "Connection status is bad (shutdown both sides, no packet)\n";
			cout << client.GetConnectionStatus() << '\n';
			cout << listeningSocketClient.GetConnectionStatus() << '\n';
			problem = true;
		}
		else
		{
			cout << "Connection status is good (shutdown both sides, no packet)\n";
		}
	}



	NetUtility::DestroyCompletionPort();
	NetUtility::FinishWinsock();

	cout << "\n\n";
	return !problem;
}

/**
 * @brief	Changes the maximum amount of memory receiving is allowed to consume.
 *
 * Consumed memory includes:
 * - The partial packet store, used to store incomplete fragments of packets. If
 * automatically resizing is enabled, this may expand too far.
 *
 * - The received packet queue. If more packets are being received than can be
 * dealt with then too much memory may be used.
 *
 * If too many much memory is in use the server would normally crash.
 * However, if a limit is imposed then the client will be disconnected and any memory
 * in use will be freed.
 *
 * By default there is no limit.
 *
 * @param	newLimit	The maximum amount of memory that can be dedicated to receiving (in bytes).
 */
void NetSocketTCP::SetRecvMemoryLimit( size_t newLimit )
{
	this->GetMode()->SetMemoryLimit(newLimit);
}

/**
 * @brief	Retrieves the maximum amount of memory that receiving is allowed to consume.
 *
 * See SetRecvMemoryLimit for more information.
 *
 * @return	the memory limit in bytes.
 */
size_t NetSocketTCP::GetRecvMemoryLimit() const
{
	return this->GetMode()->GetMemoryLimit();
}

/**
 * @brief	Retrieves the estimated amount of memory that receiving is currently using.
 *
 * See SetRecvMemoryLimit for more information.
 *
 * @return	the estimated amount of memory that is currently being used in bytes.
 */
size_t NetSocketTCP::GetRecvMemorySize() const
{
	return this->GetMode()->GetMemorySize();
}
