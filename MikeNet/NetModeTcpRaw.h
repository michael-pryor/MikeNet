#pragma once
#include "FullInclude.h"

class NetModeTcpRaw : public NetModeTcp
{
public:
	NetModeTcpRaw(MemoryRecyclePacket * memoryRecycle);
	NetModeTcpRaw(const NetModeTcpRaw & copyMe);
	NetModeTcpRaw & operator=(const NetModeTcpRaw & copyMe);
	double GetPartialPacketPercentage() const;
	NetModeTcp * Clone() const;

	void DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc tcpRecvFunc, size_t clientID, size_t instanceID);

	NetSend * GetSendObject(const Packet * packet, bool block);

	ProtocolMode GetProtocolMode() const;
};