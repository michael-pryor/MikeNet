#pragma once
#include "MemoryRecyclePacketRestricted.h"
/**
 * @brief	Manages instance startup information.
 * @remarks	Michael Pryor, 6/5/2010. 
 *
 * Part of the instance system. \n\n
 *
 * An instance profile is an object that contains a variety of options
 * specific to instances. Any instance can have an instance profile loaded into
 * it during initialization. Each option has a default value, which often does not
 * need to be changed. By using instance profiles lengthy constructors are avoided.
 * As well as this, similar profiles with similar settings can use the same profile;
 * this is a neat way of sharing options between instances.\n\n
 *
 * This class is thread safe.
 *
 */
class NetInstanceProfile: protected CriticalSection
{
public:
	/**
	 * @brief Default number of bytes to allocate to receive buffers.
	 *
	 * @warning Must be >= minimum of any instance.
	 */
	static const size_t DEFAULT_BUFFER_SIZE = 1024;
private:
	/**
	 * @brief Maximum amount of data that can be received via TCP in one operation.
	 *
	 * Default is NetInstanceProfile::DEFAULT_BUFFER_SIZE.
	 */
	size_t wsaRecvSizeTCP;

	/**
	 * @brief Maximum size an incoming TCP packet can be.
	 *
	 * Default is NetInstanceProfile::DEFAULT_BUFFER_SIZE + NetInstanceProfile::wsaRecvSizeTCP.
	 */
	size_t recvSizeTCP;

	/**
	 * @brief Maximum size an incoming UDP packet can be.
	 *
	 * Default is NetInstanceProfile::DEFAULT_BUFFER_SIZE.
	 */
	size_t recvSizeUDP;

	/**
	 * @brief Local TCP address of instance.
	 *
	 * Default initialized using NetAddress default constructor.
	 */
	NetAddress localAddrTCP;

	/**
	 * @brief Local UDP address of instance.
	 *
	 * Default initialized using NetAddress default constructor.
	 */
	NetAddress localAddrUDP;

public:
	/** @brief Default value for NetInstanceProfile::enabledUDP. */
	static const bool DEFAULT_ENABLED_UDP = true;
private:
	/**
	 * @brief If true UDP is enabled.
	 *
	 * Default is NetInstanceProfile::DEFAULT_ENABLED_UDP.
	 */
	bool enabledUDP;

public:
	/** @brief Default value for NetInstanceProfile::decryptKeyUDP. */
	static const INT_PTR DEFAULT_DECRYPT_KEY_UDP = NULL;
private:

	/**
	 * @brief Pointer to EncryptKey object used when decrypting UDP packets.
	 *
	 * NULL if no key exists. \n\n
	 *
	 * Default is NetInstanceProfile::DEFAULT_DECRYPT_KEY_UDP. \n\n
	 *
	 * Note that the key is not necessarily used by.
	 * UDP instances, only if the mode in use specifically specifies in its documentation
	 * will the key ever be used.
	 */
	const EncryptKey * decryptKeyUDP;

public:
	/** @brief Default value for NetInstanceProfile::tcpRecvFunc and NetInstanceProfile::udpRecvFunc. */
	static const NetSocket::RecvFunc DEFAULT_RECV_FUNC;
private:
	/**
	 * @brief This function will be called whenever TCP data is received on the instance.
	 *
	 * Care must be taken to ensure that this function is thread safe.
	 *
	 * NULL if no function exists. \n\n
	 *
	 * Default is NetInstanceProfile::DEFAULT_RECV_FUNC.
	 */
	NetSocket::RecvFunc tcpRecvFunc;

	/**
	 * @brief This function will be called whenever UDP data is received on the instance.
	 *
	 * Care must be taken to ensure that this function is thread safe.
	 *
	 * NULL if no function exists.\n\n
	 *
	 * Default is NetInstanceProfile::DEFAULT_RECV_FUNC.
	 */
	NetSocket::RecvFunc udpRecvFunc;

public:
	/** @brief Default value for NetInstanceProfile::handshakeEnabled. */
	static const bool DEFAULT_HANDSHAKE_ENABLED = true;
private:
	/**
	 * @brief If true the TCP handshaking process is enabled.
	 * If false the TCP handshaking process is disabled and so connection will occur with no additional data transfer.
	 *
	 * For more information see @ref handshakePage "server/client handshaking process".\n\n
	 *
	 * Default is NetInstanceProfile::DEFAULT_HANDSHAKE_ENABLED.
	 */
	bool handshakeEnabled;

public:
	/** @brief Default value for NetInstanceProfile::modeTCP. */
	static const NetMode::ProtocolMode DEFAULT_MODE_TCP = NetMode::TCP_PREFIX_SIZE;
private:

	/**
	 * @brief TCP mode in use.
	 *
	 * Default is NetInstanceProfile::DEFAULT_MODE_TCP.
	 */
	NetMode::ProtocolMode modeTCP;

public:
	/** @brief Default value for NetInstanceProfile::autoResizeTCP. */
	static const bool DEFAULT_AUTO_RESIZE_TCP = false;
private:
	/**
	 * @brief If true maximum size of incoming TCP packet will increase if necessary to store an incoming packet.
	 * If false an error will occur when a TCP packet that is too large is receive.
	 *
	 * Default is NetInstanceProfile::DEFAULT_AUTO_RESIZE_TCP.
	 */
	bool autoResizeTCP;

public:
	/** @brief Default value for NetInstanceProfile::sendTimeout. */
	static const unsigned int DEFAULT_SEND_TIMEOUT = INFINITE;
private:
	/**
	 * @brief Length of time that send operation will wait before canceling and disconnecting.
	 *
	 * Default is NetInstanceProfile::DEFAULT_SEND_TIMEOUT.
	 */
	unsigned int sendTimeout;

public:
	/** @brief Default value for NetInstanceProfile::gracefulDisconnect. */
	static const bool DEFAULT_GRACEFUL_DISCONNECT = false;
private:
	/**
	 * @brief If true graceful disconnecting is enabled.
	 *
	 * Default is NetInstanceProfile::DEFAULT_GRACEFUL_DISCONNECT.
	 */
	bool gracefulDisconnect;

public:
	/** @brief Default value for NetInstanceProfile::nagleEnabled. */
	static const bool DEFAULT_NAGLE_ENABLED = true;
private:
	/**
	 * @brief If true the nagle algorithm is enabled.
	 *
	 * Default is NetInstanceProfile::DEFAULT_NAGLE_ENABLED.
	 */
	bool nagleEnabled;
	
public:
	/** @brief Default value for NetInstanceProfile::postFixTCP. */
	static const Packet DEFAULT_POSTFIX_TCP;
private:
	/**
	 * @brief The postfix to apply in TCP mode NetMode::TCP_POSTFIX.
	 *
	 * Default is NetInstanceProfile::DEFAULT_POSTFIX_TCP.
	 */
	Packet postFixTCP;

public:
	/** @brief Default value for NetInstanceProfile::reusableUDP. */
	static const bool DEFAULT_REUSABLE_UDP = false;
private:
	/**
	 * @brief True if UDP should be reusable. When reusable other applications (including this one) may bind to the local port in use.
	 *
	 * Default is NetInstanceProfile::DEFAULT_REUSABLE_UDP.
	 */
	bool reusableUDP;

public:
	/** @brief Default value for NetInstanceProfile::numOperations. */
	static const size_t DEFAULT_NUM_OPERATIONS = 1;
private:
	/**
	 * @brief In NetMode::UDP_PER_CLIENT_PER_OPERATION this variable is used, describing the number of operations.
	 *
	 * Default is NetInstanceProfile::DEFAULT_NUM_OPERATIONS.
	 */
	size_t numOperations;

public:
	/** @brief Default value for NetInstanceProfile::nagleEnabled. */
	static const NetMode::ProtocolMode DEFAULT_MODE_UDP = NetMode::UDP_CATCH_ALL_NO;
private:
	/** 
     * @brief UDP mode to use.
	 *
	 * Default is NetInstanceProfile::DEFAULT_MODE_UDP.
	 */
	NetMode::ProtocolMode modeUDP;

public:
	/** @brief Default value for NetInstanceProfile::connectionToServerTimeout. */
	static const size_t DEFAULT_CONNECTION_TO_SERVER_TIMEOUT = 10000;
private:
	/**
	 * @brief Time in milliseconds that a connection attempt will be waited on before giving up.
	 *
	 * Default is NetInstanceProfile::DEFAULT_CONNECTION_TO_SERVER_TIMEOUT.
	 */
	size_t connectionToServerTimeout;

public:
	/** @brief Default value for NetInstanceProfile::sendMemoryLimitTCP and NetInstanceProfile::sendMemoryLimitUDP. */
	static const size_t DEFAULT_SEND_MEMORY_LIMIT = INFINITE;
private:
	/** 
     * @brief Maximum amount of memory in bytes that TCP send operations
	 * can consume.
	 *
	 * See @ref securityPage "security" for more information.
	 *
	 * Default is NetInstanceProfile::DEFAULT_SEND_MEMORY_LIMIT.
	 */
	size_t sendMemoryLimitTCP;

	/** 
     * @brief Maximum amount of memory in bytes that TCP send operations
	 * can consume.
	 *
	 * See @ref securityPage "security" for more information.
	 *
	 * Default is NetInstanceProfile::DEFAULT_SEND_MEMORY_LIMIT.
	 */
	size_t sendMemoryLimitUDP;

public:
	/** @brief Default value for NetInstanceProfile::recvMemoryLimitTCP and NetInstanceProfile::recvMemoryLimitUDP. */
	static const size_t DEFAULT_RECV_MEMORY_LIMIT = INFINITE;
private:
	/** 
     * @brief Maximum amount of memory in bytes that TCP receive operations can consume.
	 *
     * See @ref securityPage "security" for more information.
	 *
	 * Default is NetInstanceProfile::DEFAULT_RECV_MEMORY_LIMIT.
	 */
	size_t recvMemoryLimitTCP;

	/** 
     * @brief Maximum amount of memory in bytes that UDP receive operations can consume.
	 *
	 * See @ref securityPage "security" for more information.
	 *
	 * Default is NetInstanceProfile::DEFAULT_RECV_MEMORY_LIMIT.
	 */
	size_t recvMemoryLimitUDP;

	/**
	 * @brief Default value for NetInstanceProfile::packetRecycleNumberOfPacketsTCP.
	 */
	const static size_t DEFAULT_PACKET_RECYCLE_NUM_OF_PACKETS_TCP = 0;

	/**
	 * @brief Default value for NetInstanceProfile::packetRecycleMemorySizeOfPacketsTCP.
	 */
	const static size_t DEFAULT_PACKET_RECYCLE_MEMORY_SIZE_TCP = 0;

	/**
	 * @brief Default value used in the construction of NetInstanceProfile::packetRecycleUDP.
	 */
	const static size_t DEFAULT_PACKET_RECYCLE_NUM_OF_PACKETS_UDP = 0;

	/**
	 * @brief Default value used in the construction of NetInstanceProfile::packetRecycleUDP.
	 */
	const static size_t DEFAULT_PACKET_RECYCLE_MEMORY_SIZE_UDP = 0;

	/**
	 * @brief Maximum number of packets which can be stored in the memory
	 * recycle to be used while receiving TCP packets.
	 */
	size_t packetRecycleNumberOfPacketsTCP;

	/**
	 * @brief Size of packets which can be stored in the memory
	 * recycle to be used while receiving TCP packets.
	 */
	size_t packetRecycleMemorySizeOfPacketsTCP;

	/**
	 * @brief Memory recycle to be used to efficiently manage
	 * memory while receiving UDP packets.
	 */
	MemoryRecyclePacketRestricted * packetRecycleUDP;

	void DefaultVariables();
	void Copy(const NetInstanceProfile & copyMe);

public:
	NetInstanceProfile();
	NetInstanceProfile(const NetInstanceProfile & copyMe);
	NetInstanceProfile & operator=(const NetInstanceProfile & copyMe);
	bool operator==(const NetInstanceProfile & compare) const;
	virtual ~NetInstanceProfile();


	void SetRecvFuncUDP(NetSocket::RecvFunc newRecvFuncUDP);
	void SetHandshakeEnabled(bool newhandshakeEnabled);

	void SetWsaRecvSizeTCP(size_t newWsaRecvSizeTCP);
	void SetRecvSizeTCP(size_t newRecvSizeTCP);
	void SetRecvSizeUDP(size_t newRecvSizeUDP);
	void SetLocalAddrTCP(const NetAddress & newLocalAddrTCP);
	void SetLocalAddrUDP(const NetAddress & newLocalAddrUDP);
	void SetEnabledUDP(bool newEnabledUDP);
	void SetDecryptKeyUDP(const EncryptKey * newDecryptKeyUDP);
	void SetRecvFuncTCP(NetSocket::RecvFunc newRecvFuncTCP);
	void SetModeTCP(NetMode::ProtocolMode newModeTCP);
	void SetModeUDP(NetMode::ProtocolMode newModeUDP);
	void SetAutoResizeTCP(bool newAutoResizeTCP);
	void SetSendTimeout(unsigned int newSendTimeout);
	void SetGracefulDisconnectEnabled(bool newGracefulDisconnect);
	void SetNagleEnabled(bool newNagleEnabled);
	void SetPostFixTCP(const Packet & newPostFixTCP);
	void SetReusableUDP(bool option);
	void SetConnectionToServerTimeout(size_t newConnectionToServerTimeout);
	void SetNumOperations(size_t newNumOperations);
	void SetSendMemoryLimit(size_t memoryLimitTCP, size_t memoryLimitUDP);
	void SetRecvMemoryLimit(size_t memoryLimitTCP, size_t memoryLimitUDP);
	void SetMemoryRecycleTCP(size_t numberOfPackets, size_t memorySizeOfPackets);
	void SetMemoryRecycleUDP(size_t numberOfPackets, size_t memorySizeOfPackets);

	size_t GetWsaRecvSizeTCP() const;
	size_t GetRecvSizeTCP() const;
	size_t GetRecvSizeUDP() const;
	const NetAddress & GetLocalAddrTCP() const;
	const NetAddress & GetLocalAddrUDP() const;
	bool IsEnabledUDP() const;
	bool IsDecryptKeyLoadedUDP();
	const EncryptKey * GetDecryptKeyUDP() const;
	bool IsHandshakeEnabled() const;
	NetMode::ProtocolMode GetModeTCP() const;
	NetMode::ProtocolMode GetModeUDP() const;
	bool GetAutoResizeTCP() const;
	unsigned int GetSendTimeout() const;
	bool IsGracefulDisconnectEnabled() const;
	bool IsNagleEnabled() const;
	Packet GetPostFixTCP() const;
	bool IsReusableUDP() const;
	size_t GetConnectionToServerTimeout() const;
	size_t GetNumOperations() const;
	size_t GetSendMemoryLimitTCP() const;
	size_t GetRecvMemoryLimitTCP() const;
	size_t GetSendMemoryLimitUDP() const;
	size_t GetRecvMemoryLimitUDP() const;
	size_t GetMemoryRecycleNumberOfPacketsTCP() const;
	size_t GetMemoryRecycleMemorySizeOfPacketsTCP() const;
	size_t GetMemoryRecycleNumberOfPacketsUDP() const;
	size_t GetMemoryRecycleMemorySizeOfPacketsUDP() const;

	NetSocket::RecvFunc GetRecvFuncTCP() const;
	NetSocket::RecvFunc GetRecvFuncUDP() const;

	NetModeUdp * GenerateObjectModeUDP(size_t numClients, size_t numOperations) const;
	NetModeTcp * GenerateObjectModeTCP() const;

	NetSocketUDP * GenerateObjectSocketUDP(size_t bufferLength, const NetAddress & localAddr, bool reusable, NetModeUdp * udpMode, NetSocket::RecvFunc recvFunc = NULL) const;
	const MemoryRecyclePacketRestricted & GetMemoryRecyclePacketUDP() const;
	
};