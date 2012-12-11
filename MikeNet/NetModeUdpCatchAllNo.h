#pragma once

/**
 * @brief	UDP mode where all ordered packets are received, out of order packets are discarded.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * A prefix of size_t is added to the start of all packets being sent.
 * The prefix is a counter which increments by 1 with every send operation. 
 * Therefore the higher the prefix the newer the packet. A record is kept of the newest packet received
 * and any packets with a prefix lower than that are discarded as they are deemed out of order.
 * Packets being received are expected to have this prefix and problems will occur if they don't.\n\n
 *
 * The prefix is not included as part of received packets that are passed to the user. This means that data sent
 * will be received in exactly the same form; the prefix is dealt with behind the scenes.\n\n
 *
 * This class is thread safe.
 */
class NetModeUdpCatchAllNo: public NetModeUdpCatchAll
{
	/**
	 * @brief Stores a counter for each client which is incremented by 1 with every send operation (by that client).
	 *
	 * The counter is inserted at the start of each packet and is used by the recipient to determine
	 * the age of the packet, where newer packets have a higher counter (see class description: NetModeUdpCatchAllNo).
	 */
	StoreVector<ConcurrentObject<size_t>> sendCounter;
	
	/**
	 * @brief Stores the largest counter received from each client.
	 *
	 * Packets received with lower counters are discarded as they are deemed out of order.
	 */
	StoreVector<ConcurrentObject<size_t>> recvCounter;

public:
	/**
	 * @brief Initial value that counters should be set to.
	 *
	 * A counter value of 0 indicates a connection packet so
	 * we cannot use this value.
	 */
	static const size_t INITIAL_COUNTER_VALUE = 1;

	NetModeUdpCatchAllNo(size_t numClients, const MemoryRecyclePacketRestricted * memoryRecycler = NULL);
private:
	void Copy(const NetModeUdpCatchAllNo & copyMe);
public:
	NetModeUdpCatchAllNo(const NetModeUdpCatchAllNo &);
	NetModeUdpCatchAllNo & operator= (const NetModeUdpCatchAllNo &);
	NetModeUdpCatchAllNo * Clone() const;


	void Reset(size_t clientID);
	void Reset();

	void DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc udpRecvFunc, size_t clientID, size_t instanceID);
	
	NetSend * GetSendObject(const Packet * packet, bool block);

	ProtocolMode GetProtocolMode() const;

	static bool TestClass();
};