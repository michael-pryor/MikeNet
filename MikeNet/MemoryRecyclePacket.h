#pragma once
#include "StoreVector.h"
#include "Packet.h"

/**
 * @brief	A method of recycling the memory used by packets.
 *
 * There are many cases where packets need to be created and destroyed frequently.
 * Memory allocation and deallocation is expensive and we can remove this overhead
 * by reusing used up packets. This class aids in that process.
 *
 * Without memory recycling the chain of events when receiving a packet looks roughly like this:
 * @image html recycleWithout.png \n\n\n
 *
 * This operation is repeated frequently, and so we can often reuse memory without deallocating.
 * Memory recyling makes this possible; with it, the chain of events looks rouhgly like this:
 * @image html recycleWith.png \n\n\n
 *
 * When applied to a server, each client has its own unique memory count for its receiving and sending
 * via UDP and TCP. The exception to this is UDP sending, which is not unique to clients (all clients 
 * share the same memory counter).\n\n
 *
 * To ensure efficient recycling and to restrict memory usage accurately, this class
 * should be a single point of memory allocation and deallocation for all packets used
 * for the purpose which the memory recycling and restrictions apply.
 *
 * @author	Michael Pryor
 * @date	25/04/2011.
 */
class MemoryRecyclePacket : public MemoryUsageLog
{
	/**
	 * @brief Contains all packets which are currently ready to be 'recycled'.
	 */
	StoreVector<Packet> recycleBin;

	/**
	 * @brief Maximum number of packets that can be stored in recycle bin.
	 */
	size_t numberOfPackets;
	
	/**
	 * @brief Size of packets stored in recycle bin.
	 */
	size_t packetSize;

	void Copy(const MemoryRecyclePacket & copyMe);
public:
	MemoryRecyclePacket();
	MemoryRecyclePacket(size_t numberOfPackets, size_t packetSize);
	MemoryRecyclePacket(const MemoryRecyclePacket & copyMe);
	MemoryRecyclePacket & operator=(const MemoryRecyclePacket & copyMe);
	virtual ~MemoryRecyclePacket(void);

	Packet * GetPacket(size_t memorySizeOfPacket, const MemoryUsageRestricted * memoryRestrictor = NULL, bool * allocatedFreshMemory = NULL);

	bool RecyclePacket(Packet * packet);

	size_t GetPacketMemorySize() const;
	size_t GetMaxNumberOfPackets() const;
};

