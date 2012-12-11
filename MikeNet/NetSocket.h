#pragma once

/**
 * @brief	Socket class providing more advanced functionality than NetSocketSimple that is common to all protocols.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * This class provides functionality for interacting with the completion port and for sending data.\n\n
 *
 * This class is not completely thread safe. Send cleanup is thread safe.
 */
class NetSocket: public NetSocketSimple
{
public:
	/**
	 * @brief Unique completion key used by completion port to identify this object.
	 *
	 * Public access rights so that winsock has access to it.
	 */
	CompletionKey completionKey;


	/** @brief Placeholder method supplied by external source (e.g. Application) that can be called by API. */
	typedef void (*RecvFunc)(Packet & packet);
private:

	/** @brief Function to be called every time a complete packet is received. */
	ConcurrentObject<NetSocket::RecvFunc> recvFunction;

	/**
	 * @brief Temporary store of NetSocket::recvFunction.
	 *
	 * This is used in rare cases where the receive function needs to be
	 * temporarily disabled during an initial connection process, usually
	 * at the instance level. 
	 */
	ConcurrentObject<NetSocket::RecvFunc> recvFunctionAux;

	/**
	 * @brief Event object associated with NetSocket::recvOverlapped. 
	 *
	 * The overlapped event object goes through the following stages: \n
	 * 1. Starts signaled. \n
	 * 2. Begins operation, becomes unsignaled. \n
	 * 3. Finishes operation successfully and becomes signaled. \n
	 * 4. OR fails to complete operation and is manually signaled (to prevent deadlock). \n\n
	 */
	ConcurrencyEvent recvOverlappedEvent;

public:
	/**
	 * @brief Winsock overlapped operation used to identify when a receive operation has completed.
	 *
	 * See NetSocket::recvOverlappedEvent for details on the status of the associated event object.\n\n
	 *
	 * Public access rights so that winsock has access to it.
	 */
	WSAOVERLAPPED recvOverlapped;

	/**
	 * @brief Winsock buffer that is filled with newly received data.
	 *
	 * Public access rights so that winsock has access to it.
	 */
	WSABUF recvBuffer;

	/**
	 * @brief Filled when TCP receive operation completes, is ignored but required by winsock.
	 *
	 * Public access rights so that winsock has access to it.
	 */
	DWORD flags;
private:
	/**
	 * @brief True if completion port wants this socket to be closed by the main process.
     *
	 * @note Completion port does not close socket directly
	 * in order to simplify multithreaded architecture.
	 */
	mutable ConcurrencyEvent completionPortCloseNotification;

protected:
	/**
	 * @brief False after a receive operation has been initiated,  only true when the operation has finished AND
	 * all data has been dealt with.
	 *
	 * This is important during the socket closure process so that resources being used are not cleaned up.
	 */
	ConcurrencyEvent notDealingWithData;

private:
	/**
	 * @brief This vector is filled with send operations that are in progress.
	 *
	 * Upon completion, a completion thread compares the overlapped pointer using.
	 * RemoveSend() to determine which send operation should be cleaned up.
	 * When cleaned up the send operation is removed from this vector.
	 *
	 * @note Vector should not be used if socket is not associated with completion port.
	 * @note Made mutable because vector CPP class is not const correct.
	 */
	mutable StoreVector<NetSend> sendCleanup;

	/**
	 * Keeps track of and restricts how much memory sendCleanup is using.
	 *
	 * By default memory usage is unrestricted.
	 */
	MemoryUsageLogRestricted sendCleanupSize;

	void AllocateBuffer(size_t bufferLength);
	void Initialize(size_t bufferLength);
public:
	NetSocket(size_t bufferLength, RecvFunc receiveFunction);
	NetSocket(size_t bufferLength, RecvFunc receiveFunction, NetInstance * instance);
	virtual ~NetSocket();
	NetSocket(const NetSocket &);
	NetSocket & operator= (const NetSocket &);

	void DefaultVariables();

	void SetSendMemoryLimit(size_t memoryLimit);
	size_t GetSendMemoryLimit() const;
	size_t GetSendMemorySize() const;

	void SetInstance(NetInstance * instance);
	void SetClientID( size_t clientID );

	bool RemoveSend(const OVERLAPPED * operation);
	void RemoveSend(size_t element);
	void AddSend(NetSend * send);
	void ClearSend();
	bool FindSend(const OVERLAPPED * operation, size_t & position) const;
	bool IsSendEmpty() const;

	virtual void Close();

	void AssociateCompletionPort();
protected:
	void SetRecvOverlappedEvent();
public:
	virtual void ClearRecv();

	/**
	 * @brief Starts a receive operation.
	 */
	virtual bool Recv()=0;

 
protected:
	NetUtility::SendStatus Send(NetSend * sendObject, const NetAddress * sendToAddr, unsigned int timeout);
public:

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
	virtual NetUtility::SendStatus Send(const Packet & packet, bool block, const NetAddress * sendToAddr, unsigned int timeout)=0;

	size_t GetRecvBufferLength() const;

	bool IsOurOverlapped(const WSAOVERLAPPED * overlapped) const;

	/**
	 * @brief Retrieves the protocol type that the socket represents as an enum.
	 *
	 * @return protocol type that socket represents.
	 */
	virtual Protocol GetProtocol() const = 0;

	NetSocket::RecvFunc GetRecvFunction() const;
	bool IsRecvFunctionLoaded() const;
	void RemoveRecvFunction();
	void UndoRemoveRecvFunction();

	/**
	 * @brief Deals with newly received data using the socket's protocol object.
	 *
	 * @param buffer Newly received data.
	 * @param completionBytes Number of bytes of new data stored in @a buffer.
	 * @param [in] recvFunc Method will be executed and data not added to the queue if this is non NULL.
	 * @param instanceID Instance that data was received on.
	 * @param clientID ID of client that data was received from, set to 0 if not applicable.
	 */
	virtual void DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc recvFunc, size_t clientID, size_t instanceID)=0;

	virtual void CompletedSendOperation(const WSAOVERLAPPED * overlapped, bool success, bool shuttingDown);

	bool GetCompletionPortCloseRequest() const;
	void CompletionPortRequestClose();
	void SetCompletionPortFinishRecvNotification();

	static bool TestClass();
};