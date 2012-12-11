#pragma once

/**
 * @brief	UDP instance class responsible for managing all UDP operations on at an instance level.
 * @remarks	Michael Pryor, 6/28/2010.
 * 
 * This class should be inherited by any instance class wishing to make use of User Datagram Protocol.
 */
class NetInstanceUDP : virtual public NetInstance
{
protected:
	/** @brief Socket used to communicate with clients via UDP. */
	NetSocketUDP * socketUDP;

	void ValidateIsEnabledUDP(size_t line, const char * file) const;

	/**
	 * @brief Determines the minimum acceptable size that the UDP receive buffer can be.
	 *
	 * Below this value problems will occur during the connection phase. 
	 * Each instance may have a different limit, depending on what data needs
	 * to be transferred during the connection process.
	 *
	 * @return the minimum acceptable size that the UDP receive buffer can be.
	 */
	virtual size_t GetRecvSizeMinUDP() const=0;

public:
	bool ValidateRecvSizeUDP(size_t size) const;
	void ExceptionValidateRecvSizeUDP() const;

	NetInstanceUDP(NetSocketUDP * socketUDP);
	virtual ~NetInstanceUDP();

	bool IsEnabledUDP() const;
	NetMode::ProtocolMode GetModeUDP() const;
	size_t GetNumOperationsUDP() const;

	NetSocket::RecvFunc GetUserFunctionUDP() const;
	bool IsUserFunctionLoadedUDP() const;

	size_t GetRecvBufferLengthUDP() const;
	size_t GetMaxPacketSizeUDP() const;
	NetMode::ProtocolMode GetProtocolModeUDP() const;
	virtual size_t GetPacketAmountUDP(size_t clientID=0, size_t operationID=0) const;

	const NetAddress & GetLocalAddressUDP() const;
	const NetAddress & GetConnectAddressUDP() const;

	void SetSendMemoryLimitUDP(size_t newLimit);
	void SetRecvMemoryLimitUDP(size_t newLimit, size_t clientID);

	size_t GetSendMemoryLimitUDP() const;
	size_t GetRecvMemoryLimitUDP(size_t clientID) const;

	size_t GetSendMemorySizeUDP() const;
	size_t GetRecvMemorySizeUDP(size_t clientID) const;

	virtual void FlushRecvUDP(size_t clientID);

	virtual size_t GetPacketFromStoreUDP(Packet * destination, size_t clientID=0, size_t operationID=0);

	/** 
	 * @brief Sends a packet via UDP.
	 *
	 * @param packet Packet to send.
	 * @param block If true the method will not return until @a packet is completely sent, note that this does not indicate that
	 * the packet has been received by the recipient, instead it simply means the packet is in transit. \n
	 * If false the method will return instantly even if the packet has not been sent.
	 * @param clientID ID of client to send to, may be ignored (default 0).
	 *
	 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
	 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
	 * @return NetUtility::SEND_FAILED if the send operation failed.
	 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
	 */
	virtual NetUtility::SendStatus SendUDP(const Packet & packet, bool block, size_t clientID=0)=0;

	/**
	 * @brief Sends a packet via UDP to specified address.
	 *
	 * @param address Remote address to send to.
	 * @param packet Packet to send.
	 * @param block If true the method will not return until @a packet is completely sent to all clients, note that this does not indicate that
	 * the packet has been received by all clients, instead it simply means the packet is in transit. \n
	 * If false the method will return instantly even if the packet has not been sent.
	 *
	 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
	 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
	 * @return NetUtility::SEND_FAILED if the send operation failed.
	 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
	 */
	virtual NetUtility::SendStatus SendToUDP(const NetAddress & address, const Packet & packet, bool block)=0;

	virtual void CloseSockets();

};