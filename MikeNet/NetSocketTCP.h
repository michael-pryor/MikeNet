#pragma once
class NetModeTcp;

/**
 * @brief	Socket used for TCP data transfer.
 * @remarks	Michael Pryor, 3/28/2010.
 *
 * This class provides functionality specific to the TCP protocol.\n\n
 *
 * This class is not thread safe:
 */
class NetSocketTCP: public NetSocket
{
	/**
	 * @brief Non NULL if @ref gracefulDisconnectPage "graceful disconnection" is enabled. Signaled when an @c FD_CLOSE notification has been received.
	 *
	 * If non NULL Shutdown() / StopSend(), IsRecvPossible() and IsSendPossible() can now be used (with purpose).
	 * When this variable is false these methods should not be used.\n\n
	 *
	 * @c FD_CLOSE is a signal that indicates that the end point that the socket is connected to
	 * has used Shutdown(). All further attempts to start a receive operation will fail and.
	 * IsRecvPossible() will return false. This is only possible when @ref gracefulDisconnectPage "graceful disconnection"
	 * is enabled.
	 */
	ConcurrencyEvent * gracefulShutdown;

	/**
	 * @brief False when Shutdown() or StopSend() have been used.
	 *
	 * All further send operations will fail and IsSendPossible() will return false.
	 * Further receive operations may succeed if IsRecvPossible() returns true.\n\n
	 *
	 * An @c FD_CLOSE notification is sent to the end point that the socket is connected to
	 * when this is first changed to false, see NetSocketTCP::gracefulShutdown for more information.
	 */
	bool sendPossible;

	/**
	 * @brief Describes how received data should be dealt with and how sent data should be modified.
	 */
	NetModeTcp * modeTCP;

	void AssociateGracefulDisconnect();
	void Initialize(bool gracefulDisconnectEnabled, NetModeTcp * modeTCP);

public:
	NetSocketTCP(size_t wsaBufferLength, const NetAddress & localAddr, bool nagleEnabled, bool gracefulDisconnectEnabled, NetModeTcp * modeTCP, NetSocket::RecvFunc recvFunc = NULL);
	NetSocketTCP(size_t wsaBufferLength, bool nagleEnabled, bool gracefulDisconnectEnabled, NetModeTcp * modeTCP, NetSocket::RecvFunc recvFunc = NULL);
	~NetSocketTCP();
private:
	void Copy(const NetSocketTCP & copyMe);
public:
	NetSocketTCP(const NetSocketTCP &);
	NetSocketTCP & operator= (const NetSocketTCP &);

	void LoadSOCKET(SOCKET paraSocket, const NetAddress & addr);

	bool PollConnect() const;

	NetUtility::SendStatus Send(const Packet & packet, bool block, const NetAddress * sendToAddr, unsigned int timeout);

	void Shutdown();
	void StopSend();
	bool IsSendPossible() const;
	bool IsRecvPossible() const;

	bool Recv();

	virtual void Close();

	NetUtility::ConnectionStatus GetConnectionStatus() const;

	const NetModeTcp * GetMode() const;
	NetModeTcp * GetMode();

	bool IsGracefulDisconnectEnabled() const;

	NetSocket::Protocol GetProtocol() const;

	void DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc recvFunc, size_t clientID, size_t instanceID);

	void SetRecvMemoryLimit(size_t newLimit);
	size_t GetRecvMemoryLimit() const;
	size_t GetRecvMemorySize() const;

	static bool TestClass();
};