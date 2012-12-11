#pragma once

/**
 * @brief	UDP mode where only in order packets are received, some in order packets may be discarded however.
 * @remarks	Michael Pryor, 6/28/2010. 
 *
 * A prefix of clock() is automatically added to the start of all packets being sent.
 * Packets being received are expected to have this prefix and problems will occur if they don't.
 * The prefix is used to determine the age of the packet. clock() increments every millisecond and
 * therefore the higher the prefix the newer the packet. A record is kept of the newest packet received
 * and any packets with a prefix lower than that are discarded as they are deemed out of order.\n\n
 *
 * The prefix is not included as part of received packets that are passed to the user; the prefix is dealt with behind the scenes.\n\n
 *
 * In addition to this, the server (and only the server) must manually add the following prefix to the start of each packet:
 * - size_t: Indicates the client that the packet is referring to. On the receiving end, the packet will
 * then appear in that client's packet store without this prefix.\n\n
 *
 * If perOperation is set to true in the constructor, then on both the server and client side an additional
 * prefix should be added (after the client prefix):
 * - size_t: Indicates the operation that the packet is referring to. On the receiving end, the packet 
 * will then appear in that operation IDs store without this prefix.\n\n
 *
 * For details on how to implement this system in a custom instance see DealWithData().\n\n
 *
 * On the client and server side there is a packet store for each client which stores the newest
 * packet received from that client (in the case of server state) or referring to that client (in the case of client state).
 * Each store contains only one packet, reducing the overhead of a queue.\n\n
 * 
 * This UDP mode is good where data can be organized into a per client basis and can be more efficient than NetMode::UDP_CATCH_ALL
 * and NetMode::UDP_CATCH_ALL_NO due to there being no queue. Note that some in order packets may be discarded since there is no queue.\n\n
 *
 * This class is thread safe.
 */

class NetModeUdpPerClient: public NetModeUdp
{
	
	/** @brief 2D vector that stores received packets by client and operation. */
	StoreVector<StoreVector<Packet>> packetStore;

	/** @brief True if object is used in NetMode::UDP_PER_CLIENT_PER_OPERATION UDP mode, false if object is used in NetMode::UDP_PER_CLIENT UDP mode. */
	bool perOperation;

	/** @brief Pointer to decryption key used to decrypt incoming packets before reading them. */
	const EncryptKey * decryptKey;
public:
	NetModeUdpPerClient(size_t recvSize, size_t numClients, size_t numOperations, bool perOperation, const EncryptKey * decryptKey);
	~NetModeUdpPerClient();
private:
	void Copy(const NetModeUdpPerClient & copyMe);

	void ValidateClientID(size_t clientID) const;
	void ValidateOperationID(size_t operationID) const;
public:
	NetModeUdpPerClient(const NetModeUdpPerClient &);
	NetModeUdpPerClient & operator= (const NetModeUdpPerClient &);
	NetModeUdpPerClient * Clone() const;


	void Reset(size_t clientID);
	void Reset();

	void DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc udpRecvFunc, size_t clientID, size_t instanceID);
	
	size_t GetPacketAmount(size_t clientID, size_t operationID) const;

	size_t GetPacketFromStore(Packet * destination, size_t clientID, size_t operationID);

	clock_t GetRecvCounter(size_t clientID, size_t operationID);
	void SetRecvCounter(size_t clientID, size_t operationID, clock_t newCounter);

	NetSend * GetSendObject(const Packet * packet, bool block);

	ProtocolMode GetProtocolMode() const;
	size_t GetNumOperations() const;
	size_t GetNumClients() const;
	void PacketDone(Packet * completePacket, NetSocket::RecvFunc udpRecvFunc);

	static bool TestClass();
	void SetRecvMemoryLimit(size_t memoryLimit, size_t clientID);
	size_t GetRecvMemoryLimit(size_t clientID) const;
	size_t GetRecvMemorySize(size_t clientID) const;
	bool IsRecvMemorySizeSupported() const;
};