#pragma once
#include "memoryrecyclepacket.h"

/**
 * @brief	A method of recycling and restricting the memory used by packets.
 *
 * There are many cases where packets need to be created and destroyed frequently.
 * Memory allocation and deallocation is expensive and we can remove this overhead
 * by reusing used up packets. This class aids in that process.
 *
 * @author	Michael Pryor
 * @date	01/05/2011.
 */
class MemoryRecyclePacketRestricted :
	public MemoryRecyclePacket, public MemoryUsageRestricted
{
public:
	MemoryRecyclePacketRestricted();
	MemoryRecyclePacketRestricted(size_t numberOfPackets, size_t memorySizeOfPackets, size_t memoryLimit = 0);
	MemoryRecyclePacketRestricted(const MemoryRecyclePacketRestricted & copyMe);
	MemoryRecyclePacketRestricted & operator=(const MemoryRecyclePacketRestricted & copyMe);
	
	virtual ~MemoryRecyclePacketRestricted(void);

	Packet * GetPacket( size_t memorySizeOfPacket, bool * allocatedFreshMemory = NULL );

};

