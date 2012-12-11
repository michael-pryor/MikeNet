#include "FullInclude.h"

/**
 * @brief	Constructor. 
 *
 * @param	clientID			ID assigned to client. 
 * @param [in]	socketTCP		Socket to use to send and receive TCP data. This pointer and its data
 * is now owned by this object and should not be used elsewhere.
 * @param sendTimeout			Length of time in milliseconds that send operations will be allowed to complete
 * before giving up and disconnecting the client.
 */
NetServerClient::NetServerClient(size_t clientID, NetSocketTCP * socketTCP, unsigned int sendTimeout) :
		NetInstanceImplementedTCP(socketTCP,false),
		NetInstance(clientID,NetInstance::SERVER_CLIENT,sendTimeout)
{
	_ErrorException((socketTCP == NULL),"constructing a NetServerClient object, socketTCP parameter must not be NULL",0,__LINE__,__FILE__);
	
	SetConnectionState(NetUtility::NOT_CONNECTED);

	this->socketTCP->SetClientID(clientID);
	
	clockStarted = 0;
	for(size_t n = 0;n<NetUtility::authenticationStrength;n++)
	{
		connectCode[n] = 0;
	}
}

/**
 * @brief	Destructor. 
 */
NetServerClient::~NetServerClient()
{

}

/**
 * @brief Disconnects client from server forcefully
 * and prepares this object so that it can store another client.
 */
void NetServerClient::Disconnect()
{
	remoteAddrUDP.Clear();
	socketTCP->Close();
	SetConnectionState(NetUtility::NOT_CONNECTED);
}

/**
 * @brief	Takes control of NetServerClient::connectionState critical section.
 */
void NetServerClient::Enter()
{
	connectionState.Enter();
}

/**
 * @brief	Releases control of NetServerClient::connectionState critical section.
 */
void NetServerClient::Leave()
{
	connectionState.Leave();
}

/**
 * @brief Retrieves the ID assigned to this client during construction.
 *
 * @return client ID of this client.
 */
size_t NetServerClient::GetClientID() const
{
	// Instance ID is used to store client ID, since no valid instance ID
	// applies to this object.
	return GetInstanceID();
}

/**
 * @brief Retrieves the state that the connection is currently in.
 *
 * @return connection state of client.
 */
NetUtility::ConnectionStatus NetServerClient::GetConnectionState() const
{
	return connectionState.Get();
}

/**
 * @brief	Determines whether the client has ever been fully connected (i.e. if
 * currently NetUtility::DISCONNECTING, then was it ever NetUtility::CONNECTED).
 *
 * @return	true if client is connected (at least partially) and if that client was ever fully connected.
 */
bool NetServerClient::WasFullyConnected() const
{
	return wasFullyConnected;
}

/**
 * @brief Changes the connection state that the client is in.
 *
 * @param state New connection state.
 */
void NetServerClient::SetConnectionState(NetUtility::ConnectionStatus state)
{
	// A client is now fully connected.
	if(state == NetUtility::CONNECTED)
	{
		wasFullyConnected = true;
	}
	else
	{
		// Not client is connected at all.
		if(state == NetUtility::NOT_CONNECTED)
		{
			wasFullyConnected = false;
		}
	}

	connectionState.Set(state);
}

/**
 * @brief Retrieves the clock() value stored at the time that the client first began communicating with the server.
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @return clock value at which client began connecting.
 */
clock_t NetServerClient::GetClockStarted() const
{
	return clockStarted;
}

/**
 * @brief Stores clock() value, indicating when the client first began communicating with the server. 
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 */
void NetServerClient::SetClockStarted()
{
	clockStarted = clock();
}

/**
 * @brief Sets a connection code, later to be used to authenticate the client's UDP connection.
 *
 * See Authenticate() for details on how the connection is authenticated.\n\n
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @param element Connection code to set.
 * @param code Code to set it to.
 */
void NetServerClient::SetConnectCode(size_t element, int code)
{
	_ErrorException((element >= NetUtility::authenticationStrength),"setting a connection code, element out of bounds",0,__LINE__,__FILE__);
	
	connectCode[element] = code;
}

/**
 * @brief Retrieves a stored connection code, to be used to authenticate the client's UDP connection.
 *
 * See Authenticate() for details on how the connection is authenticated.\n\n
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @param element Connection code to retrieve.
 * @return connection code at element @a element.
 */
int NetServerClient::GetConnectCode(size_t element) const
{
	_ErrorException((element >= NetUtility::authenticationStrength),"getting a connection code, element out of bounds",0,__LINE__,__FILE__);
	return connectCode[element];
}

/**
 * @brief Authenticates the client's UDP connection by comparing received (via UDP)
 * authentication codes with those sent to the client (via TCP).
 *
 * This is necessary because a client's UDP connection can originate from a different IP
 * and port, giving no way of confirming that the unknown UDP connection does indeed
 * belong to the client it claims to belong to.\n\n
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @param codes Vector containing NetUtility::authenticationStrength number of connection
 * codes received from the client via UDP.
 */
bool NetServerClient::Authenticate(vector<int> codes) const
{
	if(codes.size() != NetUtility::authenticationStrength)
	{
		return false;
	}

	for(size_t n = 0;n<codes.size();n++)
	{
		if(codes[n] != this->connectCode[n])
		{
			return false;
		}
	}

	return true;
}

/**
 * @brief Retrieves a mutable pointer that is not thread safe to the stored TCP socket.
 *
 * @warning Returned pointer must be accessed with at least read control.
 * @return pointer to TCP socket.
 */
NetSocketTCP * NetServerClient::GetSocketTCP()
{
	return socketTCP;
}

/**
 * @brief Retrieves a constant pointer that is not thread safe to the stored TCP socket.
 *
 * @warning Returned pointer must be accessed with at least read control.
 * @return pointer to TCP socket.
 */
const NetSocketTCP * NetServerClient::GetSocketTCP() const
{
	return socketTCP;
}

/**
 * @brief Loads a new TCP socket and address of socket end point into this object and updates connection status.
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @param socket TCP winsock socket to load.
 * @param addr TCP address of client.
 * @param enabledUDP True if UDP is enabled for this client (we need to know this so that we know how to change the connection status).
 */
void NetServerClient::LoadTCP(SOCKET socket, const NetAddress & addr, bool enabledUDP)
{
	GetSocketTCP()->LoadSOCKET(socket,addr);

	// Update connection status
	if(enabledUDP == true)
	{
		// Need to wait for UDP connection to complete
		SetConnectionState(NetUtility::CONNECTING);
	}
	else
	{
		// Do not need to wait for UDP connection
		SetConnectionState(NetUtility::CONNECTED_AC);
	}
}

/**
 * @brief Loads client's UDP remote address and updates connection status.
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @param addr Remote UDP address of client.
 */
void NetServerClient::LoadUDP(const NetAddress & addr)
{
	remoteAddrUDP = addr;
	SetConnectionState(NetUtility::CONNECTED_AC);
}

/**
 * @brief Sends a packet via TCP which contains information about the server, the client, 
 * and authentication codes to authenticate the UDP connection.
 *
 * See Authenticate() for more information on the authentication process.
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @param serverInfo Information about the server.
 * @param enabledUDP True if UDP is enabled for this client.
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED if the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus NetServerClient::SendHandshakingPacket(const Packet & serverInfo, bool enabledUDP)
{
	NetUtility::SendStatus result;

	/**
	 * Create packet containing:
	 * 1: Server info.
	 * 2: Client number.
	 * 3-6: Authentication codes (UDP only).
	 * 
	 * To send to client.
	 */
	size_t numConCodes;

	if(enabledUDP)
	{
		numConCodes = NetUtility::authenticationStrength;
	}
	else
	{
		numConCodes = 0;
	}

	Packet packet;
	packet.SetMemorySize(serverInfo.GetUsedSize() + Packet::prefixSizeBytes + (sizeof(int)*numConCodes));

	// Add server information
	packet += serverInfo;
			
	// Add client ID
	packet.AddSizeT(GetClientID());

	// Add authentication codes
	if(enabledUDP)
	{
		for(size_t element = 0;element<numConCodes;element++)
		{
			SetConnectCode(element,rand());
			packet.Add<int>(GetConnectCode(element));
		}
	}

	// Send data
	result = socketTCP->Send(packet,false,NULL,GetSendTimeout());

	// If no error
	if(result != NetUtility::SEND_FAILED && result != NetUtility::SEND_FAILED_KILL && enabledUDP == true)
	{
		// Store clock value so that client can time out
		SetClockStarted();
	}

	return result;
}

/**
 * @brief Retrieves the UDP remote address of the client.
 *
 * @note Is okay to return reference because NetAddress is thread safe.
 */
const NetAddress & NetServerClient::GetConnectedAddressUDP() const
{
	return remoteAddrUDP;
}

/**
 * @brief	Assignment operator.
 *
 * @param [in]	copyMe	Object to copy.
 *
 * @return	reference to this object.
 */
NetServerClient & NetServerClient::operator=( NetServerClient & copyMe )
{
	_ErrorException(true,"using the NetInstanceServer assignment operator, this should never be used and is intended to please StoreVector",0,__LINE__,__FILE__);
	return *this;
}

/**
 * @brief Retrieves the minimum TCP receive buffer size.
 *
 * @return the minimum TCP receive buffer size.
 */
size_t NetServerClient::GetRecvSizeMinTCP() const
{
	return NetInstanceServer::recvSizeMinTCP;
}

/**
 * @brief Calls NetSocketTCP::Recv and deals with errors in a server specific way.
 *
 * Use this INSTEAD OF socketTCP->Recv, never use socketTCP->Recv directly.
 *
 * @param socket [in,out] Pointer to socket that receive operation should be started on.
 * @param clientID Ignored (optional, default = 0).
 */
void NetServerClient::DoRecv(NetSocket * socket, size_t clientID)
{
	_ErrorException((socket != socketTCP),"starting a server side TCP receive operation, invalid socket pointer",0,__LINE__,__FILE__);

	bool error = socketTCP->Recv();

	if(error == true)
	{
		ErrorOccurred(clientID);
	}
}

/**
 * @brief Method should never be called, required by base class.
 *
 * @param [in,out]	completionSocket	Ignored.
 * @param	clientID					Ignored.
 */
void NetServerClient::CompletionError(NetSocket * completionSocket, size_t clientID)
{
	_ErrorException(true,"attempting to call NetServerClient::CompletionError, all completion port matters are dealt with by the NetInstanceServer object encapsulating this object",0,__LINE__,__FILE__);
}

/**
 * @brief Method should never be called, required by base class.
 * 
 * @param socket Ignored.
 * @param bytes Ignored.
 * @param clientID Ignored.
 */
void NetServerClient::DealCompletion(NetSocket * socket, DWORD bytes, size_t clientID)
{
	_ErrorException(true,"attempting to call NetServerClient::DealCompletion, all data is dealt with by the NetInstanceServer object encapsulating this object",0,__LINE__,__FILE__);
}

/**
 * @brief Deals with errors.
 *
 * @param clientID Ignored (optional, default = 0).
 */
void NetServerClient::ErrorOccurred(size_t clientID)
{
	connectionState.Enter();
	if(this->GetConnectionState() != NetUtility::NOT_CONNECTED)
	{
		this->SetConnectionState(NetUtility::DISCONNECTING);
	}
	connectionState.Leave();
}
