#pragma once
#include "Counter.h"
#include "ComparatorServerClientFindByAddressUDP.h"

/**
 * @brief	Server instance, designed to communicate with clients.
 * @remarks	Michael Pryor, 6/28/2010. 
 *
 * This instance uses TCP and UDP to communicate with clients.
 * Most commonly it will be used to communicate with another entity running a NetInstanceClient instance.
 * However, this instance can also communicate with non DarkNet entities such as web clients.
 */
class NetInstanceServer : public NetInstanceUDP, public NetInstanceTCP
{
public:
	/** @brief Minimum UDP buffer size necessary to maintain normal operations. */
	const static size_t recvSizeMinUDP = 20;

	/** @brief Minimum TCP buffer size necessary to maintain normal operations. */
	const static size_t recvSizeMinTCP = 0;

	size_t GetRecvSizeMinUDP() const;
	size_t GetRecvSizeMinTCP() const;
private:

	/** @brief Socket used to accept TCP connections from clients. */
	NetSocketListening * socketListening;

	/**
	 * @brief %Counter that keeps track of UDP receive failures.
	 *
	 * WSARecv can sometimes fail for reasons that should not result in
	 * catastrophic failure of the instance e.g. A WSASendTo operation
	 * to a remote address failed. For this reason we retry receive
	 * operations a number of times, when to give up is managed by
	 * this object.
	 */
	Counter recvFailCounterUDP;

	/**
	 * @brief Client data.
	 */
	StoreVector<NetServerClient> client;

	/**
	 * @brief Client data sorted by UDP address.
	 *
	 * This is necessary for quick searching of UDP addresses,
	 * done every time a UDP packet is received to determine which
	 * client it belongs to.
	 *
	 * When using multiple critical sections this lock should be
	 * entered first, before client or any client in that vector.
	 * This is important to prevent deadlock.
	 */
	StoreVector<NetServerClient> clientSortedByAddressUDP;

	/**
	 * @brief True when clientSortedByAddressUDP has changed and needs
	 * to be resorted.
	 * 
	 * Protected by clientSortedByAddressUDP's critical section.
	 */
	bool clientSortedByAddressUdpNeedsResort;

	/**
	 * @brief Comparator used to sort clientSortedByAddressUDP, ordering
	 * it by UDP address.
	 */
	ComparatorServerClientFindByAddressUDP comparatorSort;

	/**
	 * @brief Comparator used to search for a client with a specific.
	 * UDP remote NetAddress within clientSOrtedByAddressUDP.
	 */
	ComparatorServerClientFindByAddressUDP comparatorFind;

	/** @brief Maximum number of clients that can be connected to server at any one time. */
	size_t maxClients;

	/** @brief List of recently disconnected clients to be used by ClientLeft. */
	StoreQueue<size_t> disconnected;

	/** @brief Time in milliseconds that a connection attempt will be waited on before giving up. */
	ConcurrentObject<size_t> timeout;

public:
	/** @brief Default time in milliseconds that a connection attempt will be waited on before giving up. */
	static const size_t DEFAULT_CONNECTION_TIMEOUT = 10000;
private:
	/**
	 * @brief %Packet contains data that is sent to clients upon connection.
	 * The data describes key characteristics of the server.
	 */
	Packet serverInfo;

	void ValidateClientID(size_t clientID, size_t line, const char * file) const;

	size_t FindClientByAddressUDP(const NetAddress & addr);
public:

	NetInstanceServer(size_t maxClients, NetSocketListening * listeningSocket, NetSocketUDP * socketUDP, bool handshakeEnabled, unsigned int sendTimeout = INFINITE, size_t connectionTimeout = DEFAULT_CONNECTION_TIMEOUT, size_t instanceID = 0);
	NetInstanceServer(size_t maxClients, const NetInstanceProfile & profile, size_t instanceID = 0);

	void Initialize(size_t maxClients,bool handshakeEnabled, size_t connectionTimeout, NetSocketListening * socketListening, size_t recvMemoryLimitTCP = NetInstanceProfile::DEFAULT_RECV_MEMORY_LIMIT, size_t recvMemoryLimitUDP = NetInstanceProfile::DEFAULT_RECV_MEMORY_LIMIT, size_t sendMemoryLimitTCP = NetInstanceProfile::DEFAULT_SEND_MEMORY_LIMIT, size_t sendMemoryLimitUDP = NetInstanceProfile::DEFAULT_SEND_MEMORY_LIMIT);	
	~NetInstanceServer();

public:
	void AddDisconnect(size_t clientID);
	size_t GetDisconnect();
	size_t GetMaxClients() const;
	size_t GetServerTimeout() const;
	void SetServerTimeout(size_t milliseconds);

	void SetAutoResize(bool autoResize);

	void DoRecv(NetSocket * socket, size_t clientID);
	void CompletionError(NetSocket * completionSocket, size_t clientID=0);

	void ErrorOccurred(size_t clientID);

	NetUtility::ConnectionStatus ClientConnected(size_t clientID) const;
	size_t ClientJoined();
	void DisconnectClient(size_t clientID);

	const Packet & GetServerInfo() const;

	bool IsEnabledUDP() const;
	NetMode::ProtocolMode GetModeUDP() const;

	size_t GetRecvBufferLengthTCP(size_t clientID) const;
	size_t GetPartialPacketCurrentSizeTCP(size_t clientID) const;
	size_t GetMaxPacketSizeTCP(size_t clientID) const;
	void SetMaxPacketSizeTCP(size_t newMaxSize, size_t clientID);

	bool GetAutoResizeTCP(size_t clientID) const;
	void SetAutoResizeTCP(bool newAutoResizeTCP, size_t clientID);

	bool IsGracefulDisconnectEnabled() const;

	NetMode::ProtocolMode GetProtocolModeTCP() const;

	const NetAddress & GetClientLocalAddressTCP(size_t clientID) const;
	const NetAddress & GetLocalAddressTCP() const;
	const NetAddress & GetConnectAddressTCP(size_t clientID) const;
	const NetAddress & GetConnectAddressUDP(size_t clientID) const;

	void FlushRecvTCP(size_t clientID);
	size_t GetPacketAmountTCP(size_t clientID) const;

	void ShutdownTCP(size_t clientID);

	size_t GetPacketFromStoreTCP(Packet * destination, size_t clientID);

	NetUtility::SendStatus SendTCP(const Packet & packet, bool block, size_t clientID);
	void SendAllTCP(const Packet & packet, bool block, size_t clientExclude);

	NetUtility::SendStatus SendUDP(const Packet & packet, bool block, size_t clientID);
	void SendAllUDP(const Packet & packet, bool block, size_t clientExclude);
	NetUtility::SendStatus SendToUDP(const NetAddress & address, const Packet & packet, bool block);

	size_t GetPacketAmountUDP(size_t clientID, size_t operationID=0) const;
	void FlushRecvUDP(size_t clientID);
	size_t GetPacketFromStoreUDP(Packet * destination, size_t clientID, size_t operationID=0);

	double GetPartialPacketPercentageTCP(size_t clientID) const;
	bool GetNagleEnabledTCP() const;
	const Packet & GetPostfixTCP() const;

	void DealCompletion(NetSocket * socket, DWORD bytes, size_t clientID);

	NetUtility::ConnectionStatus GetConnectionStateTCP(size_t clientID) const;

	NetSocket::RecvFunc GetUserFunctionTCP() const;
	bool IsUserFunctionLoadedTCP() const;

	virtual void CloseSockets();

	static bool TestClass();
	void SetSendMemoryLimitTCP(size_t newLimit, size_t clientID);
	void SetRecvMemoryLimitTCP(size_t newLimit, size_t clientID);
	
	size_t GetSendMemoryLimitTCP(size_t clientID) const;
	size_t GetRecvMemoryLimitTCP(size_t clientID) const;

	size_t GetSendMemorySizeTCP(size_t clientID) const;
	size_t GetRecvMemorySizeTCP(size_t clientID) const;

private:
	void RequestResortClientsByAddressUDP();

public:
};
