#include "FullInclude.h"

/**
 * @brief	Constructor. 
 *
 * @param [in]	socket			Socket to use to send and receive TCP data. This pointer and its data
 * is now owned by this object and should not be used elsewhere.
 * @param	handshakeEnabled	True if the @ref handshakePage "TCP handshake process" is enabled, false if not. 
 */
NetInstanceImplementedTCP::NetInstanceImplementedTCP(NetSocketTCP * socket, bool handshakeEnabled) : NetInstanceTCP(handshakeEnabled)
{
	_ErrorException((socket == NULL),"loading a socket into a TCP instance, parameter is NULL",0,__LINE__,__FILE__);
	this->socketTCP = socket;
	socketTCP->SetInstance(this);
}

/**
 * @brief	Destructor. 
 */
NetInstanceImplementedTCP::~NetInstanceImplementedTCP()
{
	const char * cCommand = "an internal function (~NetInstanceImplementedTCP)";
	try
	{
		CloseSockets();
		delete socketTCP;
	}
	MSG_CATCH
}

/**
 * @brief Retrieves the TCP function that is executed when complete TCP packets are received.
 *
 * @return the method set by the user that should
 * be executed when data is received.
 * @return NULL if there is no method.
 */
NetSocket::RecvFunc NetInstanceImplementedTCP::GetUserFunctionTCP() const
{
	return socketTCP->GetRecvFunction();
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
bool NetInstanceImplementedTCP::IsUserFunctionLoadedTCP() const
{
	return socketTCP->IsRecvFunctionLoaded();
}

/** 
 * @brief Determines whether @ref gracefulDisconnectPage "graceful disconnection" is enabled.
 *
 * @return true if @ref gracefulDisconnectPage "graceful disconnection" is possible, where data is allowed
 * to finish sending and receiving before the connection is closed.
 */
bool NetInstanceImplementedTCP::IsGracefulDisconnectEnabled() const
{
	return socketTCP->IsGracefulDisconnectEnabled();
}

/**
 * @brief Retrieves the size of the largest packet that can be received without resizing the receive buffer. 
 *
 * @param clientID ID of client to use. Ignored in this implementation but derived class may use ID when overriding (optional, default = 0).
 * @return	the size of the largest packet that can be received, packets larger than this
 * will require an increase in memory size or an exception will be thrown.
 */
size_t NetInstanceImplementedTCP::GetMaxPacketSizeTCP(size_t clientID) const
{
	return socketTCP->GetMode()->GetMaxPacketSize();
}

/**
 * @brief Retrieves the maximum amount of data that can be received in one TCP socket receive operation.
 *
 * @param clientID ID of client to use. Ignored in this implementation but derived class may use ID when overriding (optional, default = 0).
 * @return the buffer size of the WSABUF used by the TCP socket. \n
 * This is the maximum amount of data that can be received with one WSARecv operation.
 */
size_t NetInstanceImplementedTCP::GetRecvBufferLengthTCP(size_t clientID) const
{
	return socketTCP->GetRecvBufferLength();
}

/**
 * @brief Retrieves the amount of the current partial packet that has been received in bytes.
 *
 * @param clientID ID of client to use. Ignored in this implementation but derived class may use ID when overriding (optional, default = 0).
 * @return the current size of the TCP partial packet being received.
 * i.e. The number of bytes of the packet that have been received.
 */
size_t NetInstanceImplementedTCP::GetPartialPacketCurrentSizeTCP(size_t clientID) const
{
	return socketTCP->GetMode()->GetPartialPacketUsedSize();
}

/**
 * @brief Manually changes the maximum packet size that can be received.
 *
 * Note that data loss will not occur, the buffer will be decreased in
 * size as much as possible without causing data loss in the case that @a newMaxSize < current.
 *
 * @param newMaxSize The size of the largest packet that can be received, packets larger than this
 * will require an increase in memory size or an exception will be thrown.
 * @param clientID ID of client to use. Ignored in this implementation but derived class may use ID when overriding (optional, default = 0).
 *
 * @throws ErrorReport If newMaxSize < GetRecvSizeMinTCP().
 */
void NetInstanceImplementedTCP::SetMaxPacketSizeTCP(size_t newMaxSize, size_t clientID)
{
	_ErrorException((ValidateRecvSizeTCP(newMaxSize) != true),"changing the TCP packet receive buffer size in client state, new size is too small",0,__LINE__,__FILE__);
	socketTCP->GetMode()->ChangePartialPacketMemorySize(newMaxSize);
}

/**
 * @brief Determines whether the auto resize TCP option is enabled.
 * 
 * @param clientID ID of client to use. Ignored in this implementation but derived class may use ID when overriding (optional, default = 0).
 *
 * @return when true, if a packet larger than the max TCP packet
 * size is received then the max size will be increased silently.
 * When false, in this case an exception will be thrown.
 */
bool NetInstanceImplementedTCP::GetAutoResizeTCP(size_t clientID) const
{
	return socketTCP->GetMode()->IsAutoResizeEnabled();
}

/**
 * @brief Enables or disables the auto resize TCP option.
 *
 * @param newAutoResizeTCP When true, if a packet larger than the max TCP packet
 * size is received then the max size will be increased silently.
 * When false, an exception will be thrown.
 * @param clientID ID of client to use. Ignored in this implementation but derived class may use ID when overriding (optional, default = 0).
 */
void NetInstanceImplementedTCP::SetAutoResizeTCP(bool newAutoResizeTCP, size_t clientID)
{
	socketTCP->GetMode()->SetAutoResize(newAutoResizeTCP);
}

/**
 * @brief Retrieves the TCP protocol mode in use.
 *
 * @return protocol mode in use by instance.
 */
NetMode::ProtocolMode NetInstanceImplementedTCP::GetProtocolModeTCP() const
{
	return socketTCP->GetMode()->GetProtocolMode();
}

/** 
 * @brief Retrieves the local TCP address of the instance.
 *
 * @return TCP local address information. This reference should remain
 * valid and unmodified until the next call.
 * @note Is okay to return reference because NetAddress is thread safe.
 */
const NetAddress & NetInstanceImplementedTCP::GetLocalAddressTCP() const
{
	return socketTCP->GetLocalAddress();
}

/** 
 * @brief Retrieves the TCP address that the instance is connected to.
 *
 * @param clientID ID of client to use. Ignored in this implementation but derived class may use ID when overriding (optional, default = 0).
 * @return TCP remote address information of the specified client.
 * This reference will remain valid and unmodified until the next call.
 * @note Is okay to return reference because NetAddress is thread safe.
 */
const NetAddress & NetInstanceImplementedTCP::GetConnectAddressTCP(size_t clientID) const
{
	return socketTCP->GetAddressConnected();
}

/**
 * @brief Empties the received TCP packet queue.
 *
 * When TCP packets are received and a receive function has not been set (in the constructor),
 * packets are put into a queue and received using GetPacketFromStoreTCP. If the queue gets too big
 * newer packets may not be received for some time and so it can be useful to empty the queue.
 * This command discards everything in the queue.\n\n
 *
 * Has no impact on the partial packet store. \n\n
 *
 * @param clientID ID of client to use. Ignored in this implementation but derived class may use ID when overriding (optional, default = 0).
 */
void NetInstanceImplementedTCP::FlushRecvTCP(size_t clientID)
{
	socketTCP->GetMode()->ClearPacketStore();
}

/** 
 * @brief Retrieves the number of packets in the received TCP packet queue.
 * 
 * @param clientID ID of client to use. Ignored in this implementation but derived class may use ID when overriding (optional, default = 0).
 * @return number of fully received UDP packets in the packet store.
 */
size_t NetInstanceImplementedTCP::GetPacketAmountTCP(size_t clientID) const
{
	return socketTCP->GetMode()->GetPacketAmount();
}

/**
 * @brief Starts the graceful disconnection process.
 *
 * Stops sending on TCP socket so that all further send operations will fail.
 * All data sent up until now will be received by recipient.\n\n
 *
 * This method is part of the @ref gracefulDisconnectPage "graceful disconnect process".
 *
 * @param clientID ID of client to use. Ignored in this implementation but derived class may use ID when overriding (optional, default = 0).
 */
void NetInstanceImplementedTCP::ShutdownTCP(size_t clientID)
{
	socketTCP->Shutdown();
}

/**
 * @brief Retrieves a complete packet from the TCP packet store.
 *
 * @param [out] destination Destination to copy retrieved packet into.
 * @return number of packets in packet store before this method call.
 * @param clientID ID of client to use. Ignored in this implementation but derived class may use ID when overriding (optional, default = 0).
 */
size_t NetInstanceImplementedTCP::GetPacketFromStoreTCP(Packet * destination, size_t clientID)
{
	return socketTCP->GetMode()->GetPacketFromStore(destination);
}

/**
 * @brief Retrieves the amount of the current partial packet that has been received as a percentage.
 *
 * @param clientID ID of client to use. Ignored in this implementation but derived class may use ID when overriding (optional, default = 0).
 * @return the percentage of the partial packet that has been received
 * i.e. Between 0 and 100 where 50 indicates that half the packet has
 * been received and we are now waiting for the second half to be received.
 */
double NetInstanceImplementedTCP::GetPartialPacketPercentageTCP(size_t clientID) const
{
	return socketTCP->GetMode()->GetPartialPacketPercentage();
}

/**
 * @brief Determines whether the nagle algorithm is enabled.
 *
 * @return true if the nagle algorithm is enabled for the instance.
 */
bool NetInstanceImplementedTCP::GetNagleEnabledTCP() const
{
	return socketTCP->IsNagleEnabled();
}

/**
 * @brief Retrieves the TCP postfix in use. 
 *
 * If TCP mode is not NetMode::TCP_POSTFIX this command will fail.
 * @return the postfix in use by the TCP instance.
 */
const Packet & NetInstanceImplementedTCP::GetPostfixTCP() const
{
	const NetModeTcpPostfix * aux = NULL;
	try
	{
		aux = dynamic_cast<const NetModeTcpPostfix*>(socketTCP->GetMode());
	}
	catch (bad_cast exception)
	{
		_ErrorException(true,"retrieving the TCP postfix in use, invalid TCP mode",0,__LINE__,__FILE__);
	}
	return aux->GetPostfix();
}


/**
 * @brief Sends a packet via TCP to the server.
 *
 * @param packet Packet to send.
 * @param block If true the method will not return until @a packet is completely sent, note that this does not indicate that
 * the packet has been received by the recipient, instead it simply means the packet is in transit. \n
 * If false the method will return instantly even if the packet has not been sent.
 * @param clientID ID of client to use. Ignored in this implementation but derived class may use ID when overriding (optional, default = 0).
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED if the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus NetInstanceImplementedTCP::SendTCP(const Packet & packet, bool block, size_t clientID)
{
	return socketTCP->Send(packet,block,NULL,GetSendTimeout());
}


/**
 * @brief Retrieves the state that the TCP connection is in.
 *
 * This method is part of the @ref gracefulDisconnectPage "graceful disconnect process".
 *
 * @param clientID ID of client to use. Ignored in this implementation but derived class may use ID when overriding (optional, default = 0).
 *
 * @return TCP connection state.
 *
 * @note GetConnectionState() and this method are unlinked.
 */
NetUtility::ConnectionStatus NetInstanceImplementedTCP::GetConnectionStateTCP(size_t clientID) const
{
	return socketTCP->GetConnectionStatus();
}

/**
 * @brief	Closes any sockets in use by the instance.
 */
void NetInstanceImplementedTCP::CloseSockets()
{
	socketTCP->Close();
}


/**
 * @brief	Changes the maximum amount of memory that the instance
 * is allowed to use for asynchronous TCP send operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	newLimit	The new limit in bytes. 
 * @param	clientID	Ignored (default = 0).
 */
void NetInstanceImplementedTCP::SetSendMemoryLimitTCP(size_t newLimit, size_t clientID)
{
	socketTCP->SetSendMemoryLimit(newLimit);
}


/**
 * @brief	Changes the maximum amount of memory that the instance is
 * allowed to use for TCP receive operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	newLimit	The new limit in bytes.
 * @param	clientID	Ignored (default = 0).
 */
void NetInstanceImplementedTCP::SetRecvMemoryLimitTCP(size_t newLimit, size_t clientID)
{
	socketTCP->SetRecvMemoryLimit(newLimit);
}

/**
 * @brief	Retrieves the maximum amount of memory that the instance
 * is allowed to use for TCP send operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	clientID	Ignored (default = 0).
 * @return the maximum amount of memory in bytes.
 */
size_t NetInstanceImplementedTCP::GetSendMemoryLimitTCP(size_t clientID) const
{
	return socketTCP->GetSendMemoryLimit();
}

/**
 * @brief	Retrieves the maximum amount of memory the
 * instance is allowed to use for TCP receive operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	clientID	Ignored (default = 0).
 * @return the maximum amount of memory in bytes.
 */
size_t NetInstanceImplementedTCP::GetRecvMemoryLimitTCP(size_t clientID) const
{
	return socketTCP->GetRecvMemoryLimit();	
}

/**
 * @brief	Retrieves the estimated amount of memory that the
 * instance is currently using for TCP send operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	clientID	Ignored (default = 0).
 * @return the maximum amount of memory in bytes.
 */
size_t NetInstanceImplementedTCP::GetSendMemorySizeTCP(size_t clientID) const
{
	return socketTCP->GetSendMemorySize();	
}

/**
 * @brief	Retrieves the estimated amount of memory that the instance
 * is currently using for TCP receive operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	clientID	Ignored (default = 0).
 * @return the maximum amount of memory in bytes.
 */
size_t NetInstanceImplementedTCP::GetRecvMemorySizeTCP(size_t clientID) const
{
	return socketTCP->GetRecvMemorySize();	
}