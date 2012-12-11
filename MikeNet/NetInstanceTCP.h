#pragma once

/**
 * @brief	TCP instance class responsible for managing all TCP operations on at an instance level.
 * @remarks	Michael Pryor, 6/28/2010.
 * 
 * This class should be inherited by any instance class wishing to make use of Transmission Control Protocol.
 */
class NetInstanceTCP : virtual public NetInstance
{
protected:
	/**
	 * @brief True if the handshaking process is enabled.
	 *
	 * False if the handshaking process is disabled, connection will occur with no additional data transfer.
	 */
	bool handshakeEnabled;

	/** @brief Retrieves the smallest acceptable packet size that can be received. */
	virtual size_t GetRecvSizeMinTCP() const=0;

public:
	bool ValidateRecvSizeTCP(size_t size) const;

	NetInstanceTCP(bool handshakeEnabled);
	virtual ~NetInstanceTCP();

   	virtual bool IsHandshakeEnabled() const;

	/**
	 * @brief Retrieves the TCP function that is executed when complete TCP packets are received.
	 *
	 * @return the method set by the user that should
	 * be executed when data is received.
	 * @return NULL if there is no method.
	 */
	virtual NetSocket::RecvFunc GetUserFunctionTCP() const = 0;

	/**
	 * @brief Determines whether a TCP function has been loaded.
	 *
	 * If no TCP function has been loaded received packets are put into a queue and
	 * retrieved using GetPacketFromStoreTCP.
	 * 
	 * @return true if a method to be executed when data is received has been set.
	 * @return false if no method has been set.
	 */
	virtual bool IsUserFunctionLoadedTCP() const = 0;

	/** 
	 * @brief Determines whether @ref gracefulDisconnectPage "graceful disconnection" is enabled.
	 *
	 * @return true if @ref gracefulDisconnectPage "graceful disconnection" is possible, where data is allowed
	 * to finish sending and receiving before the connection is closed.
	 */
	virtual bool IsGracefulDisconnectEnabled() const=0;

	/**
	 * @brief Retrieves the size of the largest packet that can be received without resizing the receive buffer. 
	 *
	 * @param clientID ID of client to use, may be ignored.
	 * @return	the size of the largest packet that can be received, packets larger than this
	 * will require an increase in memory size or an exception will be thrown.
	 */
	virtual size_t GetMaxPacketSizeTCP(size_t clientID) const = 0;

	/**
	 * @brief Retrieves the maximum amount of data that can be received in one socket receive operation.
	 *
	 * @param clientID ID of client to use, may be ignored.
	 * @return the buffer size of the WSABUF used by the TCP socket.
	 * This is the maximum amount of data that can be received with one WSARecv operation.
	 */
	virtual size_t GetRecvBufferLengthTCP(size_t clientID) const = 0;

	/**
	 * @brief Retrieves the amount of the current partial packet that has been received in bytes.
	 *
	 * @param clientID ID of client to use, may be ignored.
	 *
	 * @return the current size of the TCP partial packet being received
	 * i.e. The number of bytes of the packet that have been received.
	 */
	virtual size_t GetPartialPacketCurrentSizeTCP(size_t clientID) const = 0;

	/**
	 * @brief Retrieves the amount of the current partial packet that has been received as a percentage.
     *
	 * @param clientID ID of client to use, may be ignored.
	 *
	 * @return the percentage of the partial packet that has been received
	 * i.e. Between 0 and 100 where 50 indicates that half the packet has
	 * been received and we are now waiting for the second half to be received.
	 */
	virtual double GetPartialPacketPercentageTCP(size_t clientID) const = 0;

	/**
	 * @brief Manually changes the maximum packet size that can be received.
	 *
	 * @param newMaxSize The size of the largest packet that can be received, packets larger than this
	 * will require an increase in memory size or an exception will be thrown.
	 * @param clientID ID of client to use, may be ignored.
	 */
	virtual void SetMaxPacketSizeTCP(size_t newMaxSize, size_t clientID) = 0;

	/**
	 * @brief Determines whether the auto resize TCP option is enabled.
     *
	 * @param clientID ID of client to use, may be ignored.
	 * @return when true, if a packet larger than the max TCP packet
	 * size is received then the max size will be increased silently.
	 * When false, in this case an exception will be thrown.
	 */
	virtual bool GetAutoResizeTCP(size_t clientID) const = 0;

	/**
	 * @brief Enables or disables the auto resize TCP option.
     *
	 * @param newAutoResizeTCP When true, if a packet larger than the max TCP packet
	 * size is received then the max size will be increased silently.
	 * When false, in this case an exception will be thrown.
	 * @param clientID ID of client to use, may be ignored.
	 */
	virtual void SetAutoResizeTCP(bool newAutoResizeTCP, size_t clientID) = 0;

	/**
	 * @brief Retrieves the TCP protocol mode in use.
     *
	 * @return protocol mode in use by instance.
	 */
	virtual NetMode::ProtocolMode GetProtocolModeTCP() const = 0;

	/**
	 * @brief Determines whether the nagle algorithm is enabled.
     *
	 * @return true if the nagle algorithm is enabled for the instance.
	 */
	virtual bool GetNagleEnabledTCP() const = 0;

	/**
	 * @brief Retrieves the TCP postfix in use. 
     *
	 * If not in NetMode::TCP_POSTFIX this command will fail.
	 * @return the postfix in use by the TCP instance.
	 */
	virtual const Packet & GetPostfixTCP() const = 0;

	/**
	 * @brief Retrieves the local TCP address of the instance.
     *
	 * @return TCP local address information. This reference should remain.
	 * valid and unmodified until the next call.
	 */
	virtual const NetAddress & GetLocalAddressTCP() const = 0;

	/** 
	 * @brief Retrieves the TCP address that the instance is connected to.
     *
	 * @param clientID ID of client to use, may be ignored.
	 * @return TCP remote address information of the specified client.
	 * This reference will remain valid and unmodified until the next call.
	 */
	virtual const NetAddress & GetConnectAddressTCP(size_t clientID) const = 0;

	/** 
	 * @brief Empties the received TCP packet queue.
     *
	 * When TCP packets are received and a receive function has not been set (in the constructor),
	 * packets are put into a queue and received using GetPacketFromStoreTCP. If the queue gets too big
	 * newer packets may not be received for some time and so it can be useful to empty the queue.
	 * This command discards everything in the queue and has no impact on the partial packet store.
	 *
	 * @param clientID ID of client to flush.
	 */
	virtual void FlushRecvTCP(size_t clientID) = 0;

	/** 
	 * @brief Retrieves the number of packets in the received TCP packet queue.
     *
	 * @param clientID ID of client to use, may be ignored.
	 *
	 * @return number of fully received TCP packets in the packet store.
	 */
	virtual size_t GetPacketAmountTCP(size_t clientID) const = 0;

	/**
	 * @brief Starts the graceful disconnection process.
	 *
	 * Stops sending on TCP socket so that all further send operations will fail.
	 * All data sent up till now will be received by recipient. \n\n
	 *
	 * This method is part of the @ref gracefulDisconnectPage "graceful disconnect process".
	 *
 	 * @param clientID ID of client to use, may be ignored.
	 */
	virtual void ShutdownTCP(size_t clientID) = 0;

	/**
	 * @brief Retrieves a complete packet from the TCP packet store.
     *
	 * Retrieves a complete packet from the TCP packet store and copies it into @a destination.
	 *
	 * @param [out] destination Destination to copy into.
	 *
	 * @param clientID ID of client to use, may be ignored.
	 *
	 * @return number of packets in packet store before this method call.
	 */
	virtual size_t GetPacketFromStoreTCP(Packet * destination, size_t clientID) = 0;

	/**
	 * @brief Sends a packet via TCP to the specified client.
	 *
	 * @param packet Packet to send.
	 * @param block If true the method will not return until @a packet is completely sent, note that this does not indicate that
	 * the packet has been received by the recipient, instead it simply means the packet is in transit.\n
	 * If false the method will return instantly even if the packet has not been sent.
	 * @param clientID ID of client to send to, may be ignored.
	 *
	 * @return NetUtility::SEND_COMPLETED If the send operation completed successfully instantly.
	 * @return NetUtility::SEND_IN_PROGRESS If the send operation was started, but has not yet completed.
	 * @return NetUtility::SEND_FAILED If the send operation failed.
	 * @return NetUtility::SEND_FAILED_KILL If the send operation failed and an entity was killed as a result (e.g. Client disconnected).
	 */
	virtual NetUtility::SendStatus SendTCP(const Packet & packet, bool block, size_t clientID) = 0;

	/**
	 * @brief Retrieves the state that the TCP connection is in.
	 *
	 * This method is part of the @ref gracefulDisconnectPage "graceful disconnect process".
	 *
	 * @param clientID ID of client to use, may be ignored.
	 *
	 * @return TCP connection state.
	 *
	 * @note GetConnectionState() and this method are unlinked.
	 */
	virtual NetUtility::ConnectionStatus GetConnectionStateTCP(size_t clientID) const = 0;

	/**
	 * @brief	Changes the maximum amount of memory the specified
	 * client is allowed to use for asynchronous TCP send operations.
	 *
	 * See @ref securityPage "security" for more information.
	 *
	 * @param	newLimit	The new limit in bytes. 
	 * @param	clientID	ID of the specified client.
	 */
	virtual void SetSendMemoryLimitTCP(size_t newLimit, size_t clientID) = 0;


	/**
	 * @brief	Changes the maximum amount of memory the specified
	 * client is allowed to use for TCP receive operations.
	 *
	 * See @ref securityPage "security" for more information.
	 *
	 * @param	newLimit	The new limit in bytes. 
	 * @param	clientID	ID of the specified client.
	 */
	virtual void SetRecvMemoryLimitTCP(size_t newLimit, size_t clientID) = 0;

	/**
	 * @brief	Retrieves the maximum amount of memory the
	 * specified client is allowed to use for TCP send operations.
	 *
	 * See @ref securityPage "security" for more information.
	 *
	 * @param	clientID	ID of the specified client.
	 * @return the maximum amount of memory in bytes.
	 */
	virtual size_t GetSendMemoryLimitTCP(size_t clientID) const = 0;

	/**
	 * @brief	Retrieves the maximum amount of memory the
	 * specified client is allowed to use for TCP receive operations.
	 *
	 * See @ref securityPage "security" for more information.
	 *
	 * @param	clientID	ID of the specified client.
	 * @return the maximum amount of memory in bytes.
	 */
	virtual size_t GetRecvMemoryLimitTCP(size_t clientID) const = 0;

	/**
	 * @brief	Retrieves the estimated amount of memory that the
	 * specified client is currently using for TCP send operations.
	 *
	 * See @ref securityPage "security" for more information.
	 *
	 * @param	clientID	ID of the specified client.
	 * @return the maximum amount of memory in bytes.
	 */
	virtual size_t GetSendMemorySizeTCP(size_t clientID) const = 0;

	/**
	 * @brief	Retrieves the estimated amount of memory that the
	 * specified client is currently using for TCP receive operations.
	 *
	 * See @ref securityPage "security" for more information.
	 *
	 * @param	clientID	ID of the specified client.
	 * @return the maximum amount of memory in bytes.
	 */
	virtual size_t GetRecvMemorySizeTCP(size_t clientID) const = 0;
};