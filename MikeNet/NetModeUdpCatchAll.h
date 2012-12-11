#pragma once
#include "MemoryRecyclePacketRestricted.h"

/**
 * @brief	UDP mode where all packets are received, even out of order ones. 
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * The packet is sent unmodified and packets received are transfered straight to the application.
 * Out of order packets may be received.\n\n
 *
 * In addition to this connection packets may be received in server state, as the client
 * blasts UDP packets to the server which may arrive late, after the connection has completed.
 * These packets always have a prefix of size_t of value 0. You can use this to single them
 * out and discard them. For more information see the @ref handshakePage "server/client handshaking process".\n\n
 *
 * This class is thread safe.
 */
class NetModeUdpCatchAll: public NetModeUdp
{
	void ValidateClientID(size_t clientID) const;

protected:
	/** @brief Stores all recently received packets for every client. */
	StoreVector<StoreQueue<Packet>> packetStore;

	/** @brief Each client has its own memory recycle for its packet queue. */
	StoreVector<MemoryRecyclePacketRestricted> packetStoreMemoryRecycle;

public:
	NetModeUdpCatchAll(size_t numClients, const MemoryRecyclePacketRestricted * memoryRecycler = NULL);
	virtual ~NetModeUdpCatchAll();
private:
	void Copy(const NetModeUdpCatchAll & copyMe);
public:
	NetModeUdpCatchAll(const NetModeUdpCatchAll &);
	NetModeUdpCatchAll & operator= (const NetModeUdpCatchAll &);
	virtual NetModeUdpCatchAll * Clone() const;


	virtual void Reset(size_t clientID);
	virtual void Reset();

	void PacketDone(Packet * completePacket, NetSocket::RecvFunc udpRecvFunc);
	virtual void DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc udpRecvFunc, size_t clientID, size_t instanceID);
	
	size_t GetPacketAmount(size_t clientID, size_t operationID=0) const;

	size_t GetPacketFromStore(Packet * destination, size_t clientID, size_t operationID=0);

	virtual NetSend * GetSendObject(const Packet * packet, bool block);

	virtual ProtocolMode GetProtocolMode() const;
	size_t GetNumOperations() const;
	size_t GetNumClients() const;

	static bool TestClass();
	void SetRecvMemoryLimit(size_t memoryLimit, size_t clientID);
	size_t GetRecvMemoryLimit(size_t clientID) const;
	size_t GetRecvMemorySize(size_t clientID) const;
	bool IsRecvMemorySizeSupported() const;
};