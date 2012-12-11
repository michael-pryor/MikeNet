#pragma once
#include "NetUtility.h"
#include "Timer.h"
#include "MemoryRecyclePacketRestricted.h"

/**
 * @brief	Client instance, designed to communicate with servers.
 * @remarks	Michael Pryor, 6/28/2010. 
 *
 * This instance uses TCP and UDP to communicate with servers.
 * Most commonly it will be used to communicate with another entity running a NetInstanceServer instance.
 * However, this instance can also communicate with other entities such as web servers.
 */
class NetInstanceClient: public NetInstanceImplementedTCP, public NetInstanceUDP
{
public:
	/** @brief Minimum UDP buffer size necessary to maintain normal operations. */
	const static size_t recvSizeMinUDP = 0;

	/** @brief Minimum TCP buffer size necessary to maintain normal operations. */
	const static size_t recvSizeMinTCP = 33;

private:
	/**
	 * @brief Temporary store to communicate with handshaking thread, which may
     * need to know this when creating modeUDP object.
	 */
	const EncryptKey * decryptKey;

	/**
	 * @brief Temporary store to communicate with handshaking thread, which may
     * need to know this when creating modeUDP object.
	 */
	size_t recvSizeUDP;

	/**
	 * @brief Temporary store to communicate with handshaking thread, which may
	 * need to know this when creating modeUDP object.
	 */
	MemoryRecyclePacketRestricted * memoryRecycle;

	/**
	 * @brief Maximum length of time that client would wait before giving up on connection process.
	 * Exists to pass information to the handshaking thread.
	 */
	size_t timeoutMilliseconds;

	/**
	 * @brief Stores the state of the connection process.
	 */
	ConcurrentObject<NetUtility::ConnectionStatus> connectionStatus; 

	/** @brief ID of client (as allocated by server), filled during the handshaking process. */
	ConcurrentObject<size_t> clientID;

	/**
	 * @brief Maximum number of clients that can be connected to server
	 * (that we are connected to) at any one time, filled during the handshaking process.
	 */
	ConcurrentObject<size_t> maxClients;

	/**
	 * @brief Thread to perform handshaking process with server.
	 */
	ThreadSingle * handshakeThread;

	/** @brief Stores error that occurred in handshake thread so that it can be thrown from PollConnect. */
	ErrorReport handshakeError;

	/** @brief True if an error occurred in handshake thread. */
	bool handshakeErrorOccurred;

	void Initialize(const EncryptKey * decryptKey, const MemoryRecyclePacketRestricted * memoryRecycleUDP = NULL, size_t recvMemoryLimitTCP = NetInstanceProfile::DEFAULT_RECV_MEMORY_LIMIT, size_t recvMemoryLimitUDP = NetInstanceProfile::DEFAULT_RECV_MEMORY_LIMIT, size_t sendMemoryLimitTCP = NetInstanceProfile::DEFAULT_SEND_MEMORY_LIMIT, size_t sendMemoryLimitUDP = NetInstanceProfile::DEFAULT_SEND_MEMORY_LIMIT);

	NetUtility::ConnectionStatus HelperHandshakeStatus(const Timer & timeout);
public:
	size_t GetRecvSizeMinTCP() const;
	size_t GetRecvSizeMinUDP() const;

	size_t GetConnectTimeout() const;

	NetUtility::ConnectionStatus Connect(const NetAddress * addressTCP, const NetAddress * addressUDP, size_t connectionTimeout, bool block);
	NetUtility::ConnectionStatus ThreadHandshakeRoutine();
	NetUtility::ConnectionStatus PollConnect();
	void StopConnect();
	bool IsConnecting() const;
	NetUtility::ConnectionStatus ClientConnected();
	NetUtility::ConnectionStatus GetConnectionState();

	void ErrorOccurred(size_t clientID=0);

	void DoRecv(NetSocket * socket,size_t clientID=0);
	void CompletionError(NetSocket * completionSocket, size_t clientID=0);

	int DoRawSendUDP(const Packet & packet, bool block);

	NetInstanceClient(NetSocketTCP * socketTCP, NetSocketUDP * socketUDP, const MemoryRecyclePacketRestricted * memoryRecycleUDP, bool handshakeEnabled, unsigned int sendTimeout = INFINITE, const EncryptKey * decryptKey = NULL, size_t instanceID = 0);
	NetInstanceClient(const NetInstanceProfile & profile, size_t instanceID=0);
	~NetInstanceClient();

	NetUtility::SendStatus SendUDP(const Packet & packet, bool block, size_t clientID=0);
	NetUtility::SendStatus SendToUDP(const NetAddress & address, const Packet & packet, bool block);

	size_t GetMaxClients();
	size_t GetClientID();


	void DealCompletion(NetSocket * socket, DWORD bytes, size_t clientID=0);

	virtual void CloseSockets();

	static bool TestClass();
};