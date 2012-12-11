#pragma once

/**
 * @brief	TCP mode where the end of a packet is indicated by a postfix. 
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * A postfix which can consist of any data (set in the constructor) is automatically appended to the end of all packets being sent.
 * Packets being received are expected to have this same postfix and problems will occur if they don't.
 * The postfix indicates the end of the packet.\n\n
 *
 * The postfix is not included as part of received packets that are passed to the user. This means that data sent
 * will be received in exactly the same form; the postfix is dealt with behind the scenes.\n\n
 *
 * This class is thread safe.
 */
class NetModeTcpPostfix: public NetModeTcp
{
	/**
	 * @brief Stores postfix used to indicate the end of a packet.
	 */
	Packet postfix;
public:
	NetModeTcpPostfix(size_t partialPacketSize, bool autoResize, const Packet & packetPostfix);
	NetModeTcpPostfix(const NetModeTcpPostfix & copyMe);
	NetModeTcpPostfix(size_t partialPacketSize, bool autoResize, const Packet & packetPostfix, MemoryRecyclePacket * memoryRecycle);
	NetModeTcpPostfix & operator= (const NetModeTcpPostfix & copyMe);
	NetModeTcpPostfix * Clone() const;

	void DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc tcpRecvFunc, size_t clientID, size_t instanceID);

	NetSend * GetSendObject(const Packet * packet, bool block);

	ProtocolMode GetProtocolMode() const;

	double GetPartialPacketPercentage() const;

	const Packet & GetPostfix() const;
	
	static bool TestClass();
};