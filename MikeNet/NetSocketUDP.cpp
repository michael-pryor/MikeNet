#include "FullInclude.h"

/**
 * @brief Constructor, sets up socket to be a normal UDP socket.
 *
 * @param	bufferLength				Length of receive buffer in bytes;
 * 										this is the maximum amount of data that can be received
 * 										in one receive operation. 
 * @param	localAddr					Local address to bind to, if IP or port is set to 0 then
 * 										winsock will automatically find one. 
 * @param	reusable					If true socket local address can be reused. See.
 *										NetSocketSimple::reusable for more information.
 * @param [in] udpMode				Describes how received data should be dealt with and how
 * 										sent data should be modified. If NULL, LoadMode() must later be used.
 * @param [in]		recvFunc			Function that any received packets will be passed to. (optional, default NULL). 
 *										Care must be taken to ensure that this function is thread safe.
 *										If NULL then packets will not be passed to a function, and instead
 *										will be put into a queue.
 */
NetSocketUDP::NetSocketUDP(size_t bufferLength, const NetAddress & localAddr, bool reusable, NetModeUdp * udpMode, NetSocket::RecvFunc recvFunc) : NetSocket(bufferLength, recvFunc), recvAddr()
{
	try
	{
		modeUDP = udpMode;

		Setup(NetSocketSimple::UDP);

		if(reusable)
		{
			SetReusable();
		}

		AssociateCompletionPort();

		Bind(localAddr);
	}
	catch(ErrorReport & error)
	{
		Close();
		delete udpMode;
		throw error;
	}
}

/**
 * @brief Constructor, sets up socket to be a broadcasting socket.
 *
 * @param	bufferLength				Length of receive buffer in bytes;
 * 										this is the maximum amount of data that can be received
 * 										in one receive operation. 
 * @param	localAddr					Local address to bind to, if IP or port is set to 0 then
 * 										winsock will automatically find one. 
 * @param [in]		recvFunc			Function that any received packets will be passed to. (optional, default NULL). 
 *										Care must be taken to ensure that this function is thread safe.
 *										If NULL then packets will not be passed to a function, and instead
 *										will be put into a queue.
 */
NetSocketUDP::NetSocketUDP(size_t bufferLength, const NetAddress & localAddr, NetSocket::RecvFunc recvFunc) : NetSocket(bufferLength, recvFunc), recvAddr()
{
	NetModeUdpCatchAll * mode = new (nothrow) NetModeUdpCatchAll(1);
	Utility::DynamicAllocCheck(mode,__LINE__,__FILE__);

	modeUDP.Set(mode);

	Setup(NetSocketSimple::UDP);
	SetReusable();

	SetBroadcasting();

	AssociateCompletionPort();

	Bind(localAddr);
}

/**
 * @brief Copy constructor / assignment operator helper method.
 *
 * @note Does not copy modeUDP, do this elsewhere.
 *
 * @param	copyMe	Object to copy.
 */
void NetSocketUDP::Copy(const NetSocketUDP & copyMe)
{
	this->recvAddr = copyMe.recvAddr;
}

/**
 * @brief Deep copy constructor.
 *
 * @param	copyMe	Object to copy.
 */
NetSocketUDP::NetSocketUDP(const NetSocketUDP & copyMe) : NetSocket(copyMe), recvAddr(), modeUDP(copyMe.modeUDP)
{
	Copy(copyMe);
}

/**
 * @brief Deep assignment operator.
 *
 * @param	copyMe	Object to copy. 
 *
 * @return	reference to this object.
 */
NetSocketUDP & NetSocketUDP::operator= (const NetSocketUDP & copyMe)
{
	Copy(copyMe);
	this->modeUDP = copyMe.modeUDP;

	NetSocket::operator=(copyMe);
	return *this;
}

/**
 * @brief Destructor.
 */
NetSocketUDP::~NetSocketUDP()
{
	const char * cCommand = "an internal function (~NetSocketTCP)";
	try
	{
		Close();

		modeUDP.Enter();
		delete modeUDP.Get();
		modeUDP.Leave();
	}
	MSG_CATCH
}

/**
 * @brief Clears NetSocket::recvOverlapped and NetSocketUDP::recvAddr ready for next receive.
 *
 * This must be done after every receive operation.
 */
void NetSocketUDP::ClearRecv()
{
	NetSocket::ClearRecv();
	recvAddr.Clear();
}

/**
 * @brief Starts a receive operation via TCP.
 *
 * This method calls the winsock method @c WSARecvFrom.\n\n
 *
 * If @c WSARecvFrom is successful the result of the operation (which will probably not complete instantly) will be passed to the completion port.
 * Upon completion NetSocket::recvBuffer will be filled with received data and NetSocket::recvAddr will be filled with the address that the packet
 * came from, which can be different to the address we are connected to.\n\n
 * 
 * If @c WSARecvFrom is unsuccessful the operation will not complete so the completion port will receive no notification. This
 * means that we must manually set the overlapped event in the case of initial failure.
 * 
 * @warning Receive operations will fail when the thread exits, so if you start a receive operation and the thread that started
 * it completes then the operation will fail and no data will be received.
 *
 * @return true if an error occurred and data was not properly received, false if no error occurred.
 */
bool NetSocketUDP::Recv()
{
	ValidateModeLoaded(__LINE__,__FILE__);

	// Prepare for new receive operation
	ClearRecv();

	// Start new receive operation
	// Note: no point in using multithreaded controls on recvAddr
	// No way of telling when winsock is done with it
	// This does not matter though due to the way we are using recvAddr
	notDealingWithData.Set(false);
	int iResult = WSARecvFrom(winsockSocket, &recvBuffer, 1, NULL, &flags, (SOCKADDR*)recvAddr.GetAddrPtr(), NetUtility::GetSizeSOCKADDR(), &recvOverlapped, NULL);

	// WSA_IO_PENDING indicates that receive operation was started, but did not complete instantly
	// The receive operation may still be successful however and complete at a later time.
	bool error = (iResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING);
	if(error == true)
	{
		SetRecvOverlappedEvent();
		notDealingWithData.Set(true);
	}
	return error;
}

/** 
 * @brief Sends a packet using this socket.
 *
 * @param packet Packet to send.
 * @param block If true the method will not return until @a packet is completely sent, note that this does not indicate that
 * the packet has been received by the recipient, instead it simply means the packet is in transit. \n
 * If false the method will return instantly even if the packet has not been sent.
 * @param sendToAddr Address to send to, if NULL then object is sent to address that socket is connected to.
 * @param timeout Length of time in milliseconds to wait before canceling send operation.
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED if the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus NetSocketUDP::Send(const Packet & packet, bool block, const NetAddress * sendToAddr, unsigned int timeout)
{
	ValidateModeLoaded(__LINE__,__FILE__);
	return NetSocket::Send(modeUDP.Get()->GetSendObject(&packet,block),sendToAddr,timeout);
}

/** 
 * @brief Sends an unmodified packet, ignoring the UDP mode.
 *
 * @param packet Packet to send.
 * @param block If true the method will not return until @a packet is completely sent, note that this does not indicate that
 * the packet has been received by the recipient, instead it simply means the packet is in transit. \n
 * If false the method will return instantly even if the packet has not been sent.
 * @param sendToAddr Address to send to, if NULL then object is sent to address that socket is connected to.
 * @param timeout Length of time in milliseconds to wait before canceling send operation.
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED if the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus NetSocketUDP::RawSend(const Packet & packet, bool block, const NetAddress * sendToAddr, unsigned int timeout)
{
	NetSend * sendObject = new (nothrow) NetSendRaw(&packet,block);
	Utility::DynamicAllocCheck(sendObject,__LINE__,__FILE__);

	return NetSocket::Send(sendObject,sendToAddr,timeout);
}

/**
 * @brief Closes socket and resets NetSocketTCP::modeUDP to unused state. 
 */
void NetSocketUDP::Close()
{
	NetSocket::Close();

	if(IsModeLoaded() == true)
	{
		modeUDP.Get()->Reset();
	}
}

/**
 * @brief Retrieves constant pointer to UDP mode object.
 *
 * @return	returns pointer to object (which cannot be modified) representing UDP mode specific options.
 */
const NetModeUdp * NetSocketUDP::GetMode() const
{
	ValidateModeLoaded(__LINE__,__FILE__);
	return modeUDP.Get();
}

/**
 * @brief Resets client data store.
 *
 * @param clientID ID of client to reset.
 */
void NetSocketUDP::Reset(size_t clientID)
{
	ValidateModeLoaded(__LINE__,__FILE__);
	modeUDP.Get()->Reset(clientID);
}

/**
 * @brief Retrieves a packet from the specified packet store.
 *
 * @param [out] destination Destination to copy packet into. 
 * @param clientID ID of client to use, may be ignored.
 * @param operationID ID of operation to use, may be ignored.
 *
 * @return the number of packets in packet queue before this method was called.
 */
size_t NetSocketUDP::GetPacketFromStore(Packet * destination, size_t clientID, size_t operationID)
{
	ValidateModeLoaded(__LINE__,__FILE__);
	return modeUDP.Get()->GetPacketFromStore(destination,clientID,operationID);
}

/**
 * @brief Determines whether a UDP mode has been loaded.
 *
 * @return true if UDP mode is loaded.
 */
bool NetSocketUDP::IsModeLoaded() const
{
	bool returnMe = modeUDP.Get() != NULL;
	return returnMe;
}

/**
 * @brief Throws an exception if a UDP mode is not loaded.
 *
 * @throws ErrorReport If mode is not loaded.
 * @param line Line at which method was called.
 * @param file File in which method was called.
 */
void NetSocketUDP::ValidateModeLoaded(size_t line, const char * file) const
{
	_ErrorException((IsModeLoaded() == false),"performing a UDP operation, UDP mode is not loaded and must be",0,line,file);
}

/**
 * @brief Loads a UDP mode.
 *
 * Once loaded, the mode cannot be overwritten by another mode.
 *
 * @param mode Mode to load, must not be NULL. This is now owned by this object and should not be referenced elsewhere.
 *
 * @throws ErrorReport If a mode is already loaded.
 */
void NetSocketUDP::LoadMode(NetModeUdp * mode)
{
	_ErrorException((IsModeLoaded() == true),"loading a UDP mode, UDP is already loaded and cannot be changed during run time",0,__LINE__,__FILE__);
	_ErrorException((mode == NULL),"loading a UDP mode, mode parameter must not be NULL",0,__LINE__,__FILE__);

	this->modeUDP.Set(mode);
}

/**
 * @brief Retrieves the protocol type that the socket represents as an enum.
 *
 * @return NetSocketSimple::UDP.
 */
NetSocket::Protocol NetSocketUDP::GetProtocol() const
{
	return NetSocketSimple::UDP;
}

/**
 * @brief Determines where the packet that was last received from came from.
 *
 * @return the remote address that the last received packet was sent from.
 */
const NetAddress & NetSocketUDP::GetRecvAddress() const
{
	return recvAddr;
}


/**
 * @brief Deals with newly received data using the socket's UDP mode.
 *
 * @param buffer Newly received data.
 * @param completionBytes Number of bytes of new data stored in @a buffer.
 * @param [in] recvFunc Method will be executed and data not added to the queue if this is non NULL.
 * @param instanceID Instance that data was received on.
 * @param clientID ID of client that data was received from, set to 0 if not applicable.
 */
void NetSocketUDP::DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc recvFunc, size_t clientID, size_t instanceID)
{
	ValidateModeLoaded(__LINE__,__FILE__);

	try
	{
		modeUDP.Get()->DealWithData(buffer,completionBytes,recvFunc,clientID,instanceID);
	}
	// Indicate that we are no longer dealing with data in the event of an error
	catch(ErrorReport & error){	notDealingWithData.Set(true); throw error;}
	catch(...){ notDealingWithData.Set(true); throw -1; }

	notDealingWithData.Set(true);
}

/**
 * @brief	Changes the maximum amount of memory that receiving is allowed to use.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	clientID	The client that the restriction should apply to.
 * @param	newLimit	The maximum amount of memory that can be dedicated to receiving (in bytes).
 */
void NetSocketUDP::SetRecvMemoryLimit( size_t clientID, size_t newLimit )
{
	ValidateModeLoaded(__LINE__,__FILE__);
	modeUDP.Get()->SetRecvMemoryLimit(clientID, newLimit);
}

/**
 * @brief	Retrieves the maximum amount of memory that receiving is allowed to consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	clientID	The ID of the client to use.
 * @return	the memory limit in bytes.
 */
size_t NetSocketUDP::GetRecvMemoryLimit(size_t clientID) const
{
	ValidateModeLoaded(__LINE__,__FILE__);
	return modeUDP.Get()->GetRecvMemoryLimit(clientID);
}

/**
 * @brief	Retrieves the estimated amount of memory that the specified client is currently using for receiving.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param clientID ID of client to use.
 *
 * @return	the estimated amount of memory.
 */
size_t NetSocketUDP::GetRecvMemorySize(size_t clientID) const
{
	ValidateModeLoaded(__LINE__,__FILE__);
	return modeUDP.Get()->GetRecvMemorySize(clientID);
}


/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetSocketUDP::TestClass()
{
	cout << "Testing NetSocketUDP class...\n";
	bool problem = false;

	NetUtility::SetupCompletionPort(2);
	NetUtility::StartWinsock();
	{
		cout << "Setting up two clients..\n";
		const char * localHost = NetUtility::ConvertDomainNameToIP("localhost").GetIP();
		NetAddress localAddrClient1(localHost,14000);
		NetAddress localAddrClient2(localHost,0);

		NetSocketUDP client1(1024,localAddrClient1,false,new NetModeUdpCatchAllNo(2));
		NetSocketUDP client2(1024,localAddrClient2,false,new NetModeUdpCatchAllNo(2));

		client1.Connect(client2.GetLocalAddress());
		client2.Connect(client1.GetLocalAddress());

		// Start receive operations
		cout << "Starting receive operations..\n";
		client1.Recv();
		client2.Recv();

		// Sending data 1 to 2
		cout << "Sending data from client1 to client2..\n";
		Packet sendPacket("hello world UDP");
		NetUtility::SendStatus status = client1.Send(sendPacket,false,NULL,INFINITE);

		// Note that test may fail if sending takes place too quickly
		// and send operation is cleaned up before client1.GetSendMemorySize.
		size_t memSize = client1.GetSendMemorySize();
		if(memSize == 0)
		{
			cout << " GetSendMemorySize is bad " << memSize << "\n";
			problem = true;
		}
		else
		{
			cout << " GetSendMemorySize is good: " << memSize << "\n";
		}

		if(status != NetUtility::SEND_COMPLETED && status != NetUtility::SEND_IN_PROGRESS)
		{
			cout << " Send is bad: " << status << "\n";
			problem = true;
		}
		else
		{
			cout << " Send is good: " << status << "\n";
		}

		// Wait for data to be received by client 2
		cout << "Waiting for data to be received by client2..\n";
		Packet receivedPacket;
		while(client2.GetPacketFromStore(&receivedPacket,0,0) == 0)
		{
			Sleep(10);
		}

		sendPacket.SetAge(NetModeUdpCatchAllNo::INITIAL_COUNTER_VALUE);
		if(receivedPacket != sendPacket)
		{
			cout << " Packet received is bad\n";
			problem = true;
		}
		else
		{
			cout << " Packet received is good!\n";
		}

		// After packet is sent, memory size should decrease.
		if(client1.GetSendMemorySize() != 0)
		{
			cout << " GetSendMemorySize is bad\n";
			problem = true;
		}
		else
		{
			cout << " GetSendMemorySize is good\n";
		}

		// Sending data 2 to 1
		cout << "Sending data from client2 to client1..\n";
		sendPacket = "hello everyoneee";
		status = client2.Send(sendPacket,false,NULL,INFINITE);
		if(status != NetUtility::SEND_COMPLETED && status != NetUtility::SEND_IN_PROGRESS)
		{
			cout << " Send is bad: " << status << "\n";
			problem = true;
		}
		else
		{
			cout << " Send is good: " << status << "\n";
		}

		// Wait for data to be received by client 1
		cout << "Waiting for data to be received by client1..\n";
		while(client1.GetPacketFromStore(&receivedPacket,0,0) == 0)
		{
			Sleep(10);
		}

		sendPacket.SetAge(NetModeUdpCatchAllNo::INITIAL_COUNTER_VALUE);
		if(receivedPacket != sendPacket)
		{
			cout << " Packet received is bad\n";
			problem = true;
		}
		else
		{
			cout << " Packet received is good!\n";
		}
	}
	NetUtility::FinishWinsock();
	NetUtility::DestroyCompletionPort();


	cout << "\n\n";
	return !problem;
}
