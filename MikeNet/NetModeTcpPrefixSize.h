#pragma once
#include "MemoryRecyclePacket.h"

/**
 * @brief	TCP mode where each packet has a prefix indicating the size of the remaining data.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * A prefix of type size_t is automatically added to the start of all packets being sent.
 * Packets being received are expected to have this prefix and problems will occur if they don't.
 * The prefix indicates the amount of additional data (not including the prefix) that is in the packet.
 * e.g. If the prefix of size_t is of value 10, then the actual packet's size is 10 + 8 which is 18.\n\n
 *
 * The prefix is not included as part of received packets that are passed to the user. This means that data sent
 * will be received in exactly the same form; the prefix is dealt with behind the scenes.\n\n
 *
 * This class is thread safe.
 */
class NetModeTcpPrefixSize: public NetModeTcp
{
public:
	void DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc tcpRecvFunc, size_t clientID, size_t instanceID);

	NetModeTcpPrefixSize(size_t partialPacketSize, bool autoResize);

	NetSend * GetSendObject(const Packet * packet, bool block);

	NetModeTcpPrefixSize(const NetModeTcpPrefixSize &);
	NetModeTcpPrefixSize(size_t partialPacketSize, bool autoResize, MemoryRecyclePacket * memoryRecycle);
	NetModeTcpPrefixSize & operator= (const NetModeTcpPrefixSize &);
	NetModeTcpPrefixSize * Clone() const;

	ProtocolMode GetProtocolMode() const;

	double GetPartialPacketPercentage() const;

	static bool TestClass();
};