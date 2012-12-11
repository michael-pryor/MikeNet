#pragma once

/**
 * @brief	Simple socket with basic functionality.
 * @remarks	Michael Pryor, 3/28/2010. 
 *
 * The socket wraps around a winsock SOCKET object and is initially unusable.\n\n
 *
 * First the socket needs to be setup, this is done either by WSAAccept when a new TCP connection is made, 
 * or more commonly by the Setup method. Once setup, socket options can be changed and then finally,
 * the socket is bound to a local address using Bind and the socket is ready to be used.\n\n
 *
 * This class is not thread safe.
 */
class NetSocketSimple
{
public:
	/** @brief Protocol types that socket can represent. */
	enum Protocol
	{
		/**
		 * User Datagram Protocol:
		 * - Raw data is received in packet form.
		 * - Raw data can be received out of order.
		 * - Usually quicker than TCP.
		 * - Does not deal well with the transfer of large packets.
		 * - Hard shutdown socket option cannot be used.
		 */
		UDP,

		/**
		 * Transmission Control Protocol:
		 * - Raw data is received as a stream, packets sent are received in chunks.
		 * - Raw data cannot be out of order.
		 * - Usually slower than UDP.
		 * - Has no problem with large amounts of data.
		 * - Small overhead (in terms of total amount of data necessary to transfer)
		 *   when sending small amounts of data intermittently.
		 */
		TCP
	};
private:

	void DefaultVariables();

	/** @brief True when the socket is bound to a local address using Bind(). */
	bool bound;

	/**
	 * @brief True when the socket is fully operational. This means that data can be
	 * sent and received on the socket as it is completely set up.
	 */
	bool fullyOperational;

	/** @brief Stores address that socket is connected to. */
	NetAddress addressConnected;

	/**
	 * @brief True if the socket has broadcasting enabled.
	 *
	 * Broadcasting allows UDP packets to be sent to all entities
	 * on a subnet or an entire network without first making a direct
	 * connection.\n\n
	 *
	 * Example use: You host a server on a network and want to announce
	 * your presence to potential clients. You can broadcast a packet
	 * with details of the address you are hosting on. Clients can then
	 * connect to you without initially knowing of your existence or your
	 * address.
	 */
	bool broadcasting;

	/**
	 * @brief True if socket is in listening mode.
	 *
	 * Listening mode means that TCP connection requests
	 * can be made to the IP and port that the socket is bound to.
	 * This is a core part of any TCP server and allows for WSAAccept
	 * winsock command to be successfully used. NetSocketListening
	 * enables listening mode.
	 */
	bool listening;
protected:
	/**
	 * @brief True if the socket is reusable.
	 *
	 * This means that the IP and port that the socket is bound to
	 * can be bound to by other sockets of the same protocol at the
	 * same time, which would otherwise cause an error on the other
	 * socket. Note that it does not matter if the socket resides
	 * in another application. Data sent to this IP and port will
	 * then be received by both sockets regardless of which socket
	 * it was intended for (excluding the exception mentioned below).\n\n
	 *
	 * For NetSocketSimple::TCP sockets using Connect(), reusable is slightly
	 * different. It means that sockets fulfilling the same criteria
	 * (i.e. NetSocketSimple::TCP and using Connect()) can bind to the same IP
	 * and port and operate as normal without interfering with each other.
	 * If not reusable the socket takes complete control of the IP
	 * and port and other sockets of the same protocol cannot bind to it, 
	 * even when using Connect(). There is no overhead in setting to reusable
	 * and therefore no reason why you shouldn't in this case.
	 */
	bool reusable;
	
	/**
	 * @brief True if hard shutdown is enabled.
	 *
	 * If hard shutdown is enabled disconnection completes instantly without waiting for data transfer to complete.
	 * If hard shutdown is disabled, @ref gracefulDisconnectPage "graceful disconnection" is enabled and disconnection
	 * does not complete instantly, there may be noticeable delay. However, any data transfer is allowed to complete.
	 *
	 * This option can only be true if NetSocketSimple::UDP is in use.
	 */
	bool hardShutdown;

	/**
	 * @brief True if the Nagle algorithm is enabled.
	 *
	 * The nagle algorithm reduces the number of packets that need to be sent, preventing a large overhead when
	 * sending lots of small packets simultaneously. Each packet would needs its own header (which is approximately
	 * 24 bytes) and so the amount of data needed to be sent can grow exponentially. To address this problem, 
	 * the Nagle algorithm does not send data instantly, it waits a small (usually unnoticeable) amount of time 
	 * and combines any further data to the originally queued data. This leads to the sending of one larger packet,
	 * rather than several smaller packets, thus reducing the overhead of multiple headers. Clearly though data transfer
	 * will be slightly slower. When transfer time is critical the Nagle algorithm should usually be disabled.\n\n
	 *
	 * Obviously, since this is a TCP orientated option, it can only be true if NetSocketSimple::TCP is in use.
	 */
	bool nagleEnabled;

	/** @brief Stores the local address that the socket is bound to. */
	mutable NetAddress addressLocal;
protected:
	/**
	 * @brief Winsock socket used to transfer data.
	 *
	 * Set to INVALID_SOCKET when not setup.
	 */
	SOCKET winsockSocket;
public:

	NetSocketSimple();
	virtual ~NetSocketSimple();

private:
	void Copy(const NetSocketSimple & copyMe);
public:
	NetSocketSimple(const NetSocketSimple &);
	NetSocketSimple & operator= (const NetSocketSimple &);
	bool operator==(const NetSocketSimple & compare) const;
	bool operator!=(const NetSocketSimple & compare) const;

	const NetAddress & GetLocalAddress() const;

	bool IsSetup() const;
	virtual void Close();

	void Setup(Protocol protocol);
	virtual void LoadSOCKET(SOCKET operationalSocket, const NetAddress & addressConnected);
	void SetReusable();
	void Bind(const NetAddress & connectAddr);
	void SetHardShutdown();
	void SetListening();
	void SetBroadcasting();
	void DisableNagle();
	void Connect(const NetAddress & connectAddr);

	bool IsFullyOperational() const;
protected:
	void SetFullyOperational(bool operational);

public:
	const NetAddress & GetAddressConnected() const;

	bool IsReusable() const;
	bool IsBound() const;
	bool IsHardShutdownEnabled() const;
	bool IsNagleEnabled() const;
	bool IsBroadcasting() const;
	bool IsListening() const;

	static bool TestClass();

};