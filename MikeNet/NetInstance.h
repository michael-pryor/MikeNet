#pragma once

/**
 * @brief	Core instance class responsible for the core structure of a networking instance.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * This class should not be directly inherited by instance classes, instead see NetInstanceTCP and NetInstanceUDP.
 * This class makes up the inner workings of any instance whether it be TCP, UDP or any other type.
 */
class NetInstance
{
public:
	/** @brief Possible instance types. */
	enum Type
	{
		/** Instance is in client state i.e. Of NetInstanceClient type. */
		CLIENT,

		/** Instance is in server state i.e. Of NetInstanceServer type. */
		SERVER,

		/** Instance is in broadcast state i.e. Of NetInstanceBroadcast type. */
		BROADCAST,

		/** A client object as part of a server instance i.e. Of NetInstanceClient type. */
		SERVER_CLIENT,

		/** Instance is inactive. */
		INACTIVE,
	};

	static Type ConvertToType(int type);
private:
	/** @brief ID of instance, within NetInstanceGroup. */
	size_t instanceID;

	/**
	 * @brief Type of instance this object is.
	 */
	Type state;

	/** @brief Length of time that send operation will wait before canceling and disconnecting. */
	unsigned int sendTimeout;

protected:
	/** @brief True when this object wants to be destroyed. */
	ConcurrentObject<bool> shouldBeDestroyed;

public:
	NetInstance();
	NetInstance(size_t instanceID, Type state, unsigned int sendTimeout);
	virtual ~NetInstance();
	Type GetState() const;
	size_t GetInstanceID() const;
	unsigned int GetSendTimeout() const;

	// @warning NetInstanceContainer should only use SetInstanceID method and nothing else.
	friend class NetInstanceContainer;
private:
	void SetInstanceID(size_t newID);
public:
	/**
	 * @brief When send and receive operations are completed on this instance, this method is called.
	 * 
	 * @param socket [in,out] Socket that operation was started by.
	 * @param bytes Number of bytes of data transferred in operation.
	 * @param clientID ID of client that owns socket.
	 */
	virtual void DealCompletion(NetSocket * socket, DWORD bytes, size_t clientID) = 0;

	/**
	 * @brief Deals with errors.
	 *
	 * @param clientID ID of client to disconnected.
	 */
	virtual void ErrorOccurred(size_t clientID) = 0;

	virtual void CompletedSendOperation( NetSocket * socket, const WSAOVERLAPPED * overlapped, bool success, bool shuttingDown, size_t clientID);

	/**
	 * @brief	Called by the completion port when an error occurred during an operation.
	 *
	 * @param [in,out]	completionSocket	The socket responsible for the error. 
	 * @param	clientID					The client ID responsible for the error.
	 */
	virtual void CompletionError(NetSocket * completionSocket, size_t clientID)=0;

	/**
	 * @brief Starts a receive operation on the specified socket and deals with errors appropriately.
	 *
	 * @param socket [in,out] Pointer to socket that receive operation should be started on.
	 * @param clientID ID of client using this method.
	 */
	virtual void DoRecv(NetSocket * socket, size_t clientID)=0;

	void RequestDestroy();
	bool GetDestroyRequest() const;

	/**
	 * @brief	Closes any sockets in use by the instance.
	 */
	virtual void CloseSockets() = 0;
};