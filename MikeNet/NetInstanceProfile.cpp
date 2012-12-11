#include "FullInclude.h"	

const NetSocket::RecvFunc NetInstanceProfile::DEFAULT_RECV_FUNC = NULL;
const Packet NetInstanceProfile::DEFAULT_POSTFIX_TCP = "\r\n";

/** @brief Resets all variables to default. */
void NetInstanceProfile::DefaultVariables()
{
	wsaRecvSizeTCP = DEFAULT_BUFFER_SIZE;
	recvSizeTCP = DEFAULT_BUFFER_SIZE+wsaRecvSizeTCP;
	recvSizeUDP = DEFAULT_BUFFER_SIZE;
	localAddrTCP.Clear();
	localAddrUDP.Clear();
	enabledUDP = DEFAULT_ENABLED_UDP;
	decryptKeyUDP = DEFAULT_DECRYPT_KEY_UDP;
	tcpRecvFunc = DEFAULT_RECV_FUNC;
	udpRecvFunc = DEFAULT_RECV_FUNC;
	handshakeEnabled = DEFAULT_HANDSHAKE_ENABLED;
	modeTCP = DEFAULT_MODE_TCP;
	modeUDP = DEFAULT_MODE_UDP;
	autoResizeTCP = DEFAULT_AUTO_RESIZE_TCP;
	sendTimeout = DEFAULT_SEND_TIMEOUT;
	gracefulDisconnect = DEFAULT_GRACEFUL_DISCONNECT;
	nagleEnabled = DEFAULT_NAGLE_ENABLED;
	postFixTCP = DEFAULT_POSTFIX_TCP;
	reusableUDP = DEFAULT_REUSABLE_UDP;
	connectionToServerTimeout = DEFAULT_CONNECTION_TO_SERVER_TIMEOUT;
	numOperations = DEFAULT_NUM_OPERATIONS;
	sendMemoryLimitTCP = DEFAULT_SEND_MEMORY_LIMIT;
	sendMemoryLimitUDP = DEFAULT_SEND_MEMORY_LIMIT;
	recvMemoryLimitTCP = DEFAULT_RECV_MEMORY_LIMIT;
	recvMemoryLimitUDP = DEFAULT_RECV_MEMORY_LIMIT;
	packetRecycleMemorySizeOfPacketsTCP = DEFAULT_PACKET_RECYCLE_MEMORY_SIZE_TCP;
	packetRecycleNumberOfPacketsTCP = DEFAULT_PACKET_RECYCLE_MEMORY_SIZE_TCP;
	packetRecycleUDP = new (nothrow) MemoryRecyclePacketRestricted(DEFAULT_PACKET_RECYCLE_NUM_OF_PACKETS_UDP,DEFAULT_PACKET_RECYCLE_MEMORY_SIZE_UDP);
	Utility::DynamicAllocCheck(packetRecycleUDP,__LINE__,__FILE__);
}

/** @brief Constructor. */
NetInstanceProfile::NetInstanceProfile()
{
	DefaultVariables();
}

/**
 * @brief Copies the contents of the specified instance profile.
 *
 * @param a Object to copy.
 */
void NetInstanceProfile::Copy(const NetInstanceProfile & a)
{
	a.Enter();
	this->Enter();
		wsaRecvSizeTCP = a.wsaRecvSizeTCP;
		recvSizeTCP = a.recvSizeTCP;
		recvSizeUDP = a.recvSizeUDP;
		localAddrTCP = a.localAddrTCP;
		localAddrUDP = a.localAddrUDP;
		enabledUDP = a.enabledUDP;
		decryptKeyUDP = a.decryptKeyUDP;
		tcpRecvFunc = a.tcpRecvFunc;
		udpRecvFunc = a.udpRecvFunc;
		handshakeEnabled = a.handshakeEnabled;
		modeTCP = a.modeTCP;
		modeUDP = a.modeUDP;
		autoResizeTCP = a.autoResizeTCP;
		sendTimeout = a.sendTimeout;
		gracefulDisconnect = a.gracefulDisconnect;
		nagleEnabled = a.nagleEnabled;
		postFixTCP = a.postFixTCP;
		reusableUDP = a.reusableUDP;
		connectionToServerTimeout = a.connectionToServerTimeout;
		numOperations = a.numOperations;
		
		packetRecycleUDP = new (nothrow) MemoryRecyclePacketRestricted(*a.packetRecycleUDP);
		Utility::DynamicAllocCheck(packetRecycleUDP,__LINE__,__FILE__);

		packetRecycleMemorySizeOfPacketsTCP = a.packetRecycleMemorySizeOfPacketsTCP;
		packetRecycleNumberOfPacketsTCP = a.packetRecycleNumberOfPacketsTCP;
	this->Leave();
	a.Leave();
}

/**
 * @brief Compares this object with another instance profile.
 *
 * @return true if @a a is identical to this object.
 * @return false if @a a is different in any way to this object.
 */
bool NetInstanceProfile::operator==( const NetInstanceProfile & a ) const
{
	this->Enter();
	a.Enter();
	bool returnMe =
		   (wsaRecvSizeTCP == a.wsaRecvSizeTCP && 
			recvSizeTCP == a.recvSizeTCP && 
			recvSizeUDP == a.recvSizeUDP && 
			localAddrTCP == a.localAddrTCP && 
			localAddrUDP == a.localAddrUDP && 
			enabledUDP == a.enabledUDP && 
			decryptKeyUDP == a.decryptKeyUDP && 
			tcpRecvFunc == a.tcpRecvFunc && 
			udpRecvFunc == a.udpRecvFunc && 
			handshakeEnabled == a.handshakeEnabled && 
			modeTCP == a.modeTCP && 
			modeUDP == a.modeUDP && 
			autoResizeTCP == a.autoResizeTCP && 
			sendTimeout == a.sendTimeout && 
			gracefulDisconnect == a.gracefulDisconnect && 
			nagleEnabled == a.nagleEnabled && 
			postFixTCP == a.postFixTCP && 
			reusableUDP == a.reusableUDP && 
			connectionToServerTimeout == a.connectionToServerTimeout && 
			numOperations == a.numOperations && 
			packetRecycleMemorySizeOfPacketsTCP == a.packetRecycleMemorySizeOfPacketsTCP &&
			packetRecycleNumberOfPacketsTCP == a.packetRecycleNumberOfPacketsTCP &&
			packetRecycleUDP->GetMaxNumberOfPackets() == a.packetRecycleUDP->GetMaxNumberOfPackets() &&
			packetRecycleUDP->GetMemorySize() == a.packetRecycleUDP->GetMemorySize());
	a.Leave();
	this->Leave();

	return returnMe;
}

/**
 * @brief Deep copy constructor.
 *
 * Copies contents of @a copyMe into this object (deep).
 * @param copyMe Object to copy.
 */
NetInstanceProfile::NetInstanceProfile(const NetInstanceProfile & copyMe)
{
	Copy(copyMe);
}

/**
 * @brief Deep assignment operator.
 *
 * Copies contents of @a copyMe into this object (deep).
 * @param copyMe Object to copy.
 */
NetInstanceProfile & NetInstanceProfile::operator=(const NetInstanceProfile & copyMe)
{
	Copy(copyMe);
	return *this;
}

/**
 * @brief Retrieves the WSA winsock buffer size i.e. The amount of TCP data
 * that can be received in one winsock TCP receive operation.
 *
 * @return @copydoc wsaRecvSizeTCP
 */
size_t NetInstanceProfile::GetWsaRecvSizeTCP() const
{
	return _safeReadValue(wsaRecvSizeTCP);
}

/**
 * @brief Sets the WSA winsock buffer size i.e. The amount of TCP data
 * that can be received in one winsock TCP receive operation.
 *
 * @param newWsaRecvSizeTCP @copydoc wsaRecvSizeTCP
 */
void NetInstanceProfile::SetWsaRecvSizeTCP(size_t newWsaRecvSizeTCP)
{
	_safeWriteValue(wsaRecvSizeTCP,newWsaRecvSizeTCP);
}

/**
 * @brief Retrieves the TCP receive buffer size i.e. The maximum
 * size a packet can be for it to be received without causing an error
 * or needing the buffer to be increased in size.
 *
 * @return @copydoc recvSizeTCP
 */
size_t NetInstanceProfile::GetRecvSizeTCP() const
{
	return _safeReadValue(recvSizeTCP);
}

/**
 * @brief Sets the TCP receive buffer size i.e. The maximum
 * size a packet can be for it to be received without causing an error
 * or needing the buffer to be increased in size.
 *
 * @param newRecvSizeTCP @copydoc recvSizeTCP
 */
void NetInstanceProfile::SetRecvSizeTCP(size_t newRecvSizeTCP)
{
	_safeWriteValue(recvSizeTCP,newRecvSizeTCP);
}

/**
 * @brief Retrieves the UDP receive buffer size i.e. The maximum
 * size a packet can be for it to be received without causing an error.
 * 
 * @return @copydoc recvSizeUDP
 */
size_t NetInstanceProfile::GetRecvSizeUDP() const
{
	return _safeReadValue(recvSizeUDP);
}

/**
 * @brief Sets the UDP receive buffer size i.e. The maximum
 * size a packet can be for it to be received without causing an error.
 *
 * @param newRecvSizeUDP @copydoc recvSizeUDP
 */
void NetInstanceProfile::SetRecvSizeUDP(size_t newRecvSizeUDP)
{
	_safeWriteValue(recvSizeUDP,newRecvSizeUDP);
}

/**
 * @brief Retrieves the local TCP address that the instance should be bound to.
 *
 * @return @copydoc localAddrTCP
 * @note Is okay to return reference because NetAddress is thread safe.
 */
const NetAddress & NetInstanceProfile::GetLocalAddrTCP() const
{
	Enter();
	const NetAddress & returnMe = localAddrTCP;
	Leave();
	return returnMe;
}

/**
 * @brief Sets the local TCP address that the instance should be bound to.
 *
 * @param newLocalAddrTCP @copydoc localAddrTCP
 */
void NetInstanceProfile::SetLocalAddrTCP(const NetAddress & newLocalAddrTCP)
{
	_safeWriteValue(localAddrTCP,newLocalAddrTCP);
}

/**
 * @brief Retrieves the local UDP address that the instance should be bound to.
 *
 * @return @copydoc localAddrUDP
 * @note Is okay to return reference because NetAddress is thread safe.
 */
const NetAddress & NetInstanceProfile::GetLocalAddrUDP() const
{
	size_t readCount = 0;
	Enter();
	const NetAddress & returnMe = localAddrUDP;
	Leave();
	return returnMe;
}

/**
 * @brief Sets the local UDP address that the instance should be bound to.
 *
 * @param newLocalAddrUDP @copydoc localAddrUDP
 */
void NetInstanceProfile::SetLocalAddrUDP(const NetAddress & newLocalAddrUDP)
{
	_safeWriteValue(localAddrUDP, newLocalAddrUDP);
}

/**
 * @brief Determines whether UDP is enabled.
 *
 * @return @copydoc enabledUDP
 */
bool NetInstanceProfile::IsEnabledUDP() const
{
	return _safeReadValue(enabledUDP);
}

/**
 * @brief Enables or disables UDP.
 *
 * @param newEnabledUDP @copydoc enabledUDP
 */
void NetInstanceProfile::SetEnabledUDP(bool newEnabledUDP)
{
	_safeWriteValue(enabledUDP, newEnabledUDP);
}

/**
 * @brief Retrieves a pointer to the decryption key used to decrypt incoming UDP packets.
 *
 * @return @copydoc decryptKeyUDP
 */
const EncryptKey * NetInstanceProfile::GetDecryptKeyUDP() const
{
	return _safeReadValue(decryptKeyUDP);
}

/**
 * @brief Sets the pointer to the decryption key used to decrypt incoming UDP packets.
 *
 * Note that the contents of @a newDecryptKeyUDP is copied into this object.
 *
 * @param newDecryptKeyUDP @copydoc decryptKeyUDP
 */
void NetInstanceProfile::SetDecryptKeyUDP(const EncryptKey * newDecryptKeyUDP)
{
	Enter();

	delete decryptKeyUDP;
	if(newDecryptKeyUDP == NULL)
	{
		decryptKeyUDP = NULL;
	}
	else
	{
		decryptKeyUDP = new (nothrow) EncryptKey(*newDecryptKeyUDP);
		Utility::DynamicAllocCheck(decryptKeyUDP,__LINE__,__FILE__);
	}

	Leave();
	_safeWriteValue(decryptKeyUDP, newDecryptKeyUDP);
}

/**
 * @brief Determines whether a decryption key has been set.
 *
 * @return true if decrypt key UDP is loaded, default = false.
 */
bool NetInstanceProfile::IsDecryptKeyLoadedUDP()
{
	size_t readCount = 0;
	Enter();
	bool result = (decryptKeyUDP != NULL);
	Leave();
	return result;
}

/**
 * @brief Retrieves the TCP receive function that should be called
 * when an incoming TCP packet is received.
 *
 * @return @copydoc tcpRecvFunc
 */
NetSocket::RecvFunc NetInstanceProfile::GetRecvFuncTCP() const
{
	return _safeReadValue(tcpRecvFunc);
}

/**
 * @brief Sets the TCP receive function that should be called
 * when an incoming TCP packet is received.
 *
 * @param newRecvFuncTCP @copydoc tcpRecvFunc
 */
void NetInstanceProfile::SetRecvFuncTCP(NetSocket::RecvFunc newRecvFuncTCP)
{
	_safeWriteValue(tcpRecvFunc, newRecvFuncTCP);
}

/**
 * @brief Retrieves the UDP receive function that should be called
 * when an incoming UDP packet is received.
 *
 * @return @copydoc udpRecvFunc
 */
NetSocket::RecvFunc NetInstanceProfile::GetRecvFuncUDP() const
{
	return _safeReadValue(udpRecvFunc);
}

/**
 * @brief Sets the UDP receive function that should be called
 * when an incoming UDP packet is received.
 *
 * @param newRecvFuncUDP @copydoc udpRecvFunc
 */
void NetInstanceProfile::SetRecvFuncUDP(NetSocket::RecvFunc newRecvFuncUDP)
{
	_safeWriteValue(udpRecvFunc, newRecvFuncUDP);
}

/**
 * @brief Determines whether the handshake process is enabled.
 *
 * @return @copydoc handshakeEnabled
 */
bool NetInstanceProfile::IsHandshakeEnabled() const
{
	return _safeReadValue(handshakeEnabled);
}

/**
 * @brief Enables or disables the handshake process.
 *
 * @param newhandshakeEnabled  @copydoc handshakeEnabled
 */
void NetInstanceProfile::SetHandshakeEnabled(bool newhandshakeEnabled)
{
	Enter();
		_safeWriteValue(handshakeEnabled, newhandshakeEnabled);

		if(handshakeEnabled == false)
		{
			SetEnabledUDP(false);
		}
	Leave();
}

/**
 * @brief Retrieves the TCP mode that should be used by the instance.
 *
 * @return @copydoc modeTCP
 */
NetMode::ProtocolMode NetInstanceProfile::GetModeTCP() const
{
	return _safeReadValue(modeTCP);
}

/**
 * @brief Sets the TCP mode that should be used by the instance.
 *
 * @param newModeTCP @copydoc modeTCP
 * @throws ErrorReport If @a newModeUDP is not a valid TCP mode.
 */
void NetInstanceProfile::SetModeTCP(NetMode::ProtocolMode newModeTCP)
{
	_ErrorException((NetMode::ValidateProtocolModeTCP(newModeTCP) == false),"changing the TCP mode of a profile, invalid mode",0,__LINE__,__FILE__);
	_safeWriteValue(modeTCP, newModeTCP);
}

/**
 * @brief Retrieves the UDP mode that should be used by the instance.
 *
 * @return @copydoc modeUDP
 */
NetMode::ProtocolMode NetInstanceProfile::GetModeUDP() const
{
	return _safeReadValue(modeUDP);
}

/**
 * @brief Sets the UDP mode that should be used by the instance.
 *
 * @param newModeUDP @copydoc modeUDP
 * @throws ErrorReport If @a newModeUDP is not a valid UDP mode.
 */
void NetInstanceProfile::SetModeUDP(NetMode::ProtocolMode newModeUDP)
{
	_ErrorException((NetMode::ValidateProtocolModeUDP(newModeUDP) == false),"changing the UDP mode of a profile, invalid mode",0,__LINE__,__FILE__);
	_safeWriteValue(modeUDP, newModeUDP);
}

/**
 * @brief Determines whether the auto resize option is enabled. If it is
 * then if a packet larger than the TCP receive buffer size is received,
 * then the buffer will automatically increase in size.
 *
 * @return @copydoc autoResizeTCP
 */
bool NetInstanceProfile::GetAutoResizeTCP() const
{
	return _safeReadValue(autoResizeTCP);
}

/**
 * @brief Enables or disables the auto resize option is enabled. If it is enabled
 * then if a packet larger than the TCP receive buffer size is received,
 * then the buffer will automatically increase in size.
 *
 * @param newAutoResizeTCP @copydoc autoResizeTCP
 */
void NetInstanceProfile::SetAutoResizeTCP(bool newAutoResizeTCP)
{
	_safeWriteValue(autoResizeTCP, newAutoResizeTCP);
}

/**
 * @brief Retrieves the number of milliseconds that send operations
 * will be allowed to complete before they are canceled and the
 * entity that initiated the operation is forcefully disconnected.
 *
 * @return @copydoc sendTimeout
 */
unsigned int NetInstanceProfile::GetSendTimeout() const
{
	return _safeReadValue(sendTimeout);
}

/**
 * @brief Sets the number of milliseconds that send operations
 * will be allowed to complete before they are canceled and the
 * entity that initiated the operation is forcefully disconnected.
 *
 * @param newSendTimeout @copydoc sendTimeout
 */
void NetInstanceProfile::SetSendTimeout(unsigned int newSendTimeout)
{
	_safeWriteValue(sendTimeout, newSendTimeout);
}

/**
 * @brief Determines whether graceful disconnection is enabled.
 *
 * @return @copydoc gracefulDisconnect
 */
bool NetInstanceProfile::IsGracefulDisconnectEnabled() const
{
	return _safeReadValue(gracefulDisconnect);
}

/**
 * @brief Enables or disables graceful disconnection.
 *
 * @param newGracefulDisconnect @copydoc gracefulDisconnect
 */
void NetInstanceProfile::SetGracefulDisconnectEnabled(bool newGracefulDisconnect)
{
	_safeWriteValue(gracefulDisconnect, newGracefulDisconnect);
}

/**
 * @brief Determines whether the nagle algorithm is enabled or disabled.
 *
 * @return @copydoc nagleEnabled
 */
bool NetInstanceProfile::IsNagleEnabled() const
{
	return _safeReadValue(nagleEnabled);
}

/**
 * @brief Enables or disables the nagle algorithm.
 *
 * @param newNagleEnabled @copydoc nagleEnabled
 */
void NetInstanceProfile::SetNagleEnabled(bool newNagleEnabled)
{
	_safeWriteValue(nagleEnabled, newNagleEnabled);
}


/**
 * @brief Retrieves the TCP postfix in use in NetModeTcp::TCP_POSTFIX TCP mode.
 *
 * @return @copydoc postFixTCP
 */
Packet NetInstanceProfile::GetPostFixTCP() const
{
	return _safeReadValue(postFixTCP);
}

/**
 * @brief Sets the TCP postfix in use in NetModeTcp::TCP_POSTFIX TCP mode.
 *
 * This command changes the postfix option and has no impact on instances not in NetMode::TCP_POSTFIX mode.
 * @param newPostFixTCP @copydoc postFixTCP
 */
void NetInstanceProfile::SetPostFixTCP(const Packet & newPostFixTCP)
{
	_safeWriteValue(postFixTCP, newPostFixTCP);
}

/**
 * @brief Determines whether UDP should be reusable. If reusable other applications, instances
 * and sockets can bind to the same UDP address as this instance.
 *
 * @return @copydoc reusableUDP
 */
bool NetInstanceProfile::IsReusableUDP() const
{
	return _safeReadValue(reusableUDP);
}

/**
 * @brief Enables or disables the reusable UDP option. If reusable other applications, instances
 * and sockets can bind to the same UDP address as this instance.
 *
 * @param newReusableUDP @copydoc reusableUDP
 */
void NetInstanceProfile::SetReusableUDP(bool newReusableUDP)
{
	_safeWriteValue(reusableUDP, newReusableUDP);
}


/**
 * @brief Specifies the number of UDP operations in NetModeUdp::UDP_PER_CLIENT_PER_OPERATION.
 *
 * @param newNumOperations @copydoc numOperations
 */
void NetInstanceProfile::SetNumOperations(size_t newNumOperations)
{
	_safeWriteValue(numOperations, newNumOperations);
}

/**
 * @brief Retrieves the number of UDP operations in NetModeUdp::UDP_PER_CLIENT_PER_OPERATION.
 *
 * @return @copydoc numOperations
 */
size_t NetInstanceProfile::GetNumOperations() const
{
	return _safeReadValue(numOperations);
}

/**
 * @brief Sets the number of milliseconds that a client is allowed to handshake with the server
 * before it is forcefully disconnected.
 *
 * @param newConnectionToServerTimeout @copydoc connectionToServerTimeout
 */
void NetInstanceProfile::SetConnectionToServerTimeout(size_t newConnectionToServerTimeout)
{
	_safeWriteValue(connectionToServerTimeout, newConnectionToServerTimeout);
}

/**
 * @brief Retrieves the number of milliseconds that a client is allowed to handshake with the server
 * before it is forcefully disconnected.
 *
 * @return @copydoc connectionToServerTimeout
 */
size_t NetInstanceProfile::GetConnectionToServerTimeout() const
{
	return _safeReadValue(connectionToServerTimeout);
}

/**
 * @brief	Specifies the maximum amount of memory that send operations of
 * a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	memoryLimitTCP	The memory limit in bytes. Set to 0 if no change is needed.
 * @param	memoryLimitUDP	The memory limit in bytes. Set to 0 if no change is needed.
 */
void NetInstanceProfile::SetSendMemoryLimit( size_t memoryLimitTCP , size_t memoryLimitUDP )
{
	if(memoryLimitTCP > 0)
	{
		_safeWriteValue(this->sendMemoryLimitTCP,memoryLimitTCP);
	}

	if(memoryLimitUDP > 0)
	{
		_safeWriteValue(this->sendMemoryLimitUDP,memoryLimitUDP);
	}
}

/**
 * @brief	Specifies the maximum amount of memory that receive operations of
 * a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	memoryLimitTCP	The memory limit in bytes. Set to 0 if no change is needed.
 * @param	memoryLimitUDP	The memory limit in bytes. Set to 0 if no change is needed.
 */
void NetInstanceProfile::SetRecvMemoryLimit( size_t memoryLimitTCP , size_t memoryLimitUDP )
{
	if(memoryLimitTCP > 0)
	{
		_safeWriteValue(this->recvMemoryLimitTCP,memoryLimitTCP);
	}

	if(memoryLimitUDP > 0)
	{
		_safeWriteValue(this->recvMemoryLimitUDP,memoryLimitUDP);
	}
}

/**
 * @brief Retrieves the maximum amount of memory that TCP send operations 
 * of a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @return @copydoc sendMemoryLimitTCP
 */
size_t NetInstanceProfile::GetSendMemoryLimitTCP() const
{
	return _safeReadValue(this->sendMemoryLimitTCP);
}

/**
 * @brief Retrieves the maximum amount of memory that TCP receive operations 
 * of a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @return @copydoc sendMemoryLimitTCP
 */
size_t NetInstanceProfile::GetRecvMemoryLimitTCP() const
{
	return _safeReadValue(this->recvMemoryLimitTCP);
}

/**
 * @brief Retrieves the maximum amount of memory that UDP send operations 
 * of a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @return @copydoc sendMemoryLimitUDP
 */
size_t NetInstanceProfile::GetSendMemoryLimitUDP() const
{
	return _safeReadValue(this->sendMemoryLimitUDP);
}

/**
 * @brief Retrieves the maximum amount of memory that TCP receive operations 
 * of a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @return @copydoc sendMemoryLimitUDP
 */
size_t NetInstanceProfile::GetRecvMemoryLimitUDP() const
{
	return _safeReadValue(this->recvMemoryLimitUDP);
}

/**
 * @brief	Sets up the memory recycle to be used when receiving TCP packets.
 *
 * A separate memory recycle is allocated to each client, each individually created
 * with the parameters specified here.\n\n
 *
 * See MemoryRecyclePacket for more information.
 *
 * @param	numberOfPackets		Maximum number of packets which can be stored by memory recycle. 
 * @param	memorySizeOfPackets	Memory size of packets in memory recycle.
 */
void NetInstanceProfile::SetMemoryRecycleTCP(size_t numberOfPackets, size_t memorySizeOfPackets)
{
	_safeWriteValue(packetRecycleNumberOfPacketsTCP,numberOfPackets);
	_safeWriteValue(packetRecycleMemorySizeOfPacketsTCP,memorySizeOfPackets);
}

/**
 * @brief	Sets up the memory recycle to be used when receiving UDP packets.
 *
 * A separate memory recycle is allocated to each client, each individually created
 * with the parameters specified here.\n\n
 *
 * See MemoryRecyclePacket for more information.
 *
 * @param	numberOfPackets		Maximum number of packets which can be stored by memory recycle. 
 * @param	memorySizeOfPackets	Memory size of packets in memory recycle.
 */
void NetInstanceProfile::SetMemoryRecycleUDP(size_t numberOfPackets, size_t memorySizeOfPackets)
{
	MemoryRecyclePacketRestricted * recycle = new (nothrow) MemoryRecyclePacketRestricted(numberOfPackets,memorySizeOfPackets);
	Utility::DynamicAllocCheck(recycle,__LINE__,__FILE__);

	Enter();
	delete packetRecycleUDP;
	_safeWriteValue(packetRecycleUDP,recycle);
	Leave();
}

/**
 * @brief	Retrieves the number of packets that the TCP receiving memory recycle can store.
 *
 * See MemoryRecyclePacket for more information.
 *
 * @return	number of packets.
 */
size_t NetInstanceProfile::GetMemoryRecycleNumberOfPacketsTCP() const
{
	return _safeReadValue(packetRecycleNumberOfPacketsTCP);
}

/**
 * @brief	Retrieves the amount of memory that each packet in the TCP recycle has allocated to it.
 *
 * See MemoryRecyclePacket for more information.
 *
 * @return	amount of memory (in bytes).
 */
size_t NetInstanceProfile::GetMemoryRecycleMemorySizeOfPacketsTCP() const
{
	return _safeReadValue(packetRecycleMemorySizeOfPacketsTCP);
}

/**
 * @brief	Retrieves the number of packets that the UDP receiving memory recycle can store.
 *
 * See MemoryRecyclePacket for more information.
 *
 * @return	number of packets.
 */
size_t NetInstanceProfile::GetMemoryRecycleNumberOfPacketsUDP() const
{
	return _safeReadValue(packetRecycleUDP)->GetMaxNumberOfPackets();
}

/**
 * @brief	Retrieves the amount of memory that each packet in the UDP recycle has allocated to it.
 *
 * See MemoryRecyclePacket for more information.
 *
 * @return	amount of memory (in bytes).
 */
size_t NetInstanceProfile::GetMemoryRecycleMemorySizeOfPacketsUDP() const
{
	return _safeReadValue(packetRecycleUDP)->GetPacketMemorySize();
}

/**
 * @brief	Retrieves the UDP packet recycle receive object.
 *
 * See MemoryRecyclePacket for more information.
 *
 * @return	the object.
 */
const MemoryRecyclePacketRestricted & NetInstanceProfile::GetMemoryRecyclePacketUDP() const
{
	return *_safeReadValue(packetRecycleUDP);
}

/**
 * @brief Generates a NetModeUdp object based on local variables.
 *
 * @param numClients Number of clients that data may be received for (optional, default 1).
 * @param numOperations Number of operations that data may be received for (optional, default 1).
 * Ignored in NetMode::UDP_CATCH_ALL and NetMode::UDP_CATCH_ALL_NO UDP modes.
 * @return object.
 *
 * @return netModeUdp object if UDP is enabled.
 * @return NULL if UDP is disabled.
 */
NetModeUdp * NetInstanceProfile::GenerateObjectModeUDP(size_t numClients, size_t numOperations) const
{
	if(IsEnabledUDP() == true)
	{
		switch(GetModeUDP())
		{
		case NetMode::UDP_CATCH_ALL:
			return static_cast<NetModeUdp*>(Utility::DynamicAllocCheck(new (nothrow) NetModeUdpCatchAll(numClients,&GetMemoryRecyclePacketUDP()),__LINE__,__FILE__));
			break;

		case NetMode::UDP_CATCH_ALL_NO:
			return static_cast<NetModeUdp*>(Utility::DynamicAllocCheck(new (nothrow) NetModeUdpCatchAllNo(numClients,&GetMemoryRecyclePacketUDP()),__LINE__,__FILE__));
			break;

		case NetMode::UDP_PER_CLIENT:
			return static_cast<NetModeUdp*>(Utility::DynamicAllocCheck(new (nothrow) NetModeUdpPerClient(GetRecvSizeUDP(),numClients,numOperations,false,GetDecryptKeyUDP()),__LINE__,__FILE__));
			break;

		case NetMode::UDP_PER_CLIENT_PER_OPERATION:
			return static_cast<NetModeUdp*>(Utility::DynamicAllocCheck(new (nothrow) NetModeUdpPerClient(GetRecvSizeUDP(),numClients,numOperations,true,GetDecryptKeyUDP()),__LINE__,__FILE__));
			break;

		default:
			_ErrorException(true,"generating a NetModeUdp object, invalid UDP mode",0,__LINE__,__FILE__);
			return NULL;
			break;
		}
	}
	else
	{
		return NULL;
	}
}

/**
 * @brief Generates a NetModeTcp object based on local variables.
 *
 * @return object.
 */
NetModeTcp * NetInstanceProfile::GenerateObjectModeTCP() const
{
	MemoryRecyclePacket * memoryRecycle = new (nothrow) MemoryRecyclePacket(packetRecycleNumberOfPacketsTCP,packetRecycleMemorySizeOfPacketsTCP);
	Utility::DynamicAllocCheck(memoryRecycle,__LINE__,__FILE__);

	switch(GetModeTCP())
	{
	case NetMode::TCP_PREFIX_SIZE:
		return static_cast<NetModeTcp*>(Utility::DynamicAllocCheck(new (nothrow) NetModeTcpPrefixSize(GetRecvSizeTCP(),GetAutoResizeTCP(),memoryRecycle),__LINE__,__FILE__));
		break;

	case NetMode::TCP_POSTFIX:
		return static_cast<NetModeTcp*>(Utility::DynamicAllocCheck(new (nothrow) NetModeTcpPostfix(GetRecvSizeTCP(),GetAutoResizeTCP(),GetPostFixTCP(),memoryRecycle),__LINE__,__FILE__));
		break;

	case NetMode::TCP_RAW:
		return static_cast<NetModeTcp*>(Utility::DynamicAllocCheck(new (nothrow) NetModeTcpRaw(memoryRecycle),__LINE__,__FILE__));
		break;

	default:
		delete memoryRecycle;
		_ErrorException(true,"generating a NetModeTcp object, invalid TCP mode",0,__LINE__,__FILE__);
		return NULL;
		break;
	}

	delete memoryRecycle;
}

/**
 * @brief Creates a normal UDP socket.
 *
 * The purpose of this method is for class constructors. If UDP is disabled NULL needs to be set.
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
 *
 * @return a dynamically allocated socketUDP object.
 * @return NULL if UDP is disabled.
 */
NetSocketUDP * NetInstanceProfile::GenerateObjectSocketUDP( size_t bufferLength, const NetAddress & localAddr, bool reusable, NetModeUdp * udpMode, NetSocket::RecvFunc recvFunc) const
{
	NetSocketUDP * returnMe = NULL;

	if(this->IsEnabledUDP() == true)
	{
		returnMe = new NetSocketUDP(bufferLength,localAddr,reusable,udpMode,recvFunc);
		Utility::DynamicAllocCheck(returnMe,__LINE__,__FILE__);
	}

	return returnMe;
}

/**
 * @brief	Destructor. 
 */
NetInstanceProfile::~NetInstanceProfile()
{
	delete decryptKeyUDP;
	delete packetRecycleUDP;
}

