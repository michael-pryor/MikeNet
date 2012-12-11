#pragma once

/**
 * @brief Object stores and manages individual clients.
 * 
 * Class is used by NetInstanceServer and is never used independently.
 */
class NetServerClient: public NetInstanceImplementedTCP
{
	/** @brief UDP remote address that client is connecting from. */
	NetAddress remoteAddrUDP;

	/**
	 * @brief Connection state of client.
	 */
	ConcurrentObject<NetUtility::ConnectionStatus> connectionState;

	/**
	 * @brief clock() that handshaking process began.
	 *
	 * This allows us to determine how long a client has been handshaking for,
	 * and drop clients that take too long.
	 */
	clock_t clockStarted;

	/**
	 * @brief Stores the unique authentication code sent to client during handshaking process.
	 *
	 * This allows the UDP connection to be authenticated, see Authenticate() for more information.
	 */
	int connectCode[NetUtility::authenticationStrength];

	/**
	 * @brief True if currently connected client has been fully connected i.e. connection state is CONNECTED.
	 *
	 * This is necessary when the state changes to DISCONNECTING so that the client is only added to the disconnect list 
	 * if it was fully connected.
	 */
	bool wasFullyConnected;
public:

	NetServerClient(size_t clientID, NetSocketTCP * socketTCP, unsigned int sendTimeout = INFINITE);
	NetServerClient & operator=(NetServerClient & copyMe);
	~NetServerClient();

	size_t GetClientID() const;
	NetUtility::ConnectionStatus GetConnectionState() const;
	void SetConnectionState(NetUtility::ConnectionStatus state);
	clock_t GetClockStarted() const;
	void SetClockStarted();
	void SetConnectCode(size_t element, int code);
	int GetConnectCode(size_t element) const;
	bool Authenticate(vector<int> codes) const;
	void Disconnect();

	NetSocketTCP * GetSocketTCP();
	const NetSocketTCP * GetSocketTCP() const;

	void LoadTCP(SOCKET socket, const NetAddress & addr, bool enabledUDP);
	void LoadUDP(const NetAddress & addr);
	NetUtility::SendStatus SendHandshakingPacket(const Packet & serverInfo, bool enabledUDP);

	const NetAddress & GetConnectedAddressUDP() const;

	size_t GetRecvSizeMinTCP() const;

	void DoRecv(NetSocket * socket, size_t clientID=0);
	void CompletionError(NetSocket * completionSocket, size_t clientID=0);

	void ErrorOccurred(size_t clientID=0);
	void DealCompletion(NetSocket * socket, DWORD bytes, size_t clientID);
	
	void Enter();
	void Leave();
	bool WasFullyConnected() const;
};