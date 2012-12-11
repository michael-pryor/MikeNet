#pragma once
#include "MemoryUsageRestricted.h"
#include "MemoryUsageLog.h"

/**
 * @brief	TCP protocol class, provides a base for extensions to the protocol by TCP mode classes.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * This object describes how received packets should be stored.
 * First any data received is put into a partial buffer. Afterwards
 * if any complete packets exist in the partial buffer, they are moved to
 * a queue or passed to a user supplied function. Classes derived from this
 * object decide how exactly this process is implemented. \n\n
 *
 * This class is thread safe.
 */
class NetModeTcp : public NetMode, public MemoryUsageRestricted
{
	/** @brief Stores complete packets that have been received. */
	StoreQueue<Packet> packetStore;

protected:
	/** @brief Deals with memory allocation and deallocation of packets for this object. */
	MemoryRecyclePacket * packetMemoryRecycle;
private:
	/** @brief Controls access to the packetMemoryRecycle pointer (not the data that it points to). */
	CriticalSection packetMemoryRecyclePtrAccess;

protected:
	/**
	 * @brief Stores partial packets that are not yet completely received.
	 *
	 * These are passed to NetModeTcp::packetStore when completed.
	 */
	Packet partialPacket;

	/**
	 * @brief If true, NetModeTcp::partialPacket will automatically increase memory size as needed.
	 * 
	 * Protected by critical section so that it can be changed during runtime.
	 */
	ConcurrentObject<bool> autoResize;
public:
	void SetAutoResize(bool autoResize);
	bool IsAutoResizeEnabled() const;

	NetModeTcp(size_t partialPacketSize, bool autoResize);
	virtual ~NetModeTcp();
private:
	void Copy(const NetModeTcp & copyMe);
public:
	NetModeTcp(const NetModeTcp &);
	NetModeTcp(size_t partialPacketSize, bool autoResize, MemoryRecyclePacket * memoryRecycle);
	NetModeTcp & operator= (const NetModeTcp &);

	size_t GetPartialPacketUsedSize() const;

	size_t GetPacketAmount(size_t clientID=0, size_t operationID=0) const;

	/**
	 * @brief Determines the amount of the oldest partial packet that
	 * has been received as a percentage.
	 *
	 * @return the percentage of the partial packet that has been received
	 * i.e. Between 0 and 100 where 50 indicates that half the packet has
	 * been received and we are now waiting for the second half to be received.
	 */
	virtual double GetPartialPacketPercentage() const = 0;

	/** 
	 * @brief Retrieves a deep copy of this object.
	 *
	 * @return a deep copy of this object.
	 */
	virtual NetModeTcp * Clone() const = 0;
public:
	size_t GetMaxPacketSize() const;
	size_t GetPartialPacketMemorySize() const;
	void ChangePartialPacketMemorySize(size_t newSize);
	
	size_t GetPacketStoreMemorySize() const;
	
	void ClearPacketStore();
	void ClearData();

	size_t GetPacketFromStore(Packet * destination, size_t clientID=0, size_t operationID=0);
	void PacketDone(Packet * completePacket, NetSocket::RecvFunc tcpRecvFunc);
	
	size_t GetMemorySize() const;

	void LoadMemoryRecycle(MemoryRecyclePacket * memoryRecycle);

protected:
	static size_t GetMemorySize(size_t packetStoreSizeValue, size_t partialPacketSizeValue);

	void EnforceMemoryLimit(size_t desiredPacketStoreSizeValue, size_t desiredPartialPacketSizeValue);
public:
	static bool TestClass();
};