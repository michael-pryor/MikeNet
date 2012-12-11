#include "FullInclude.h"
#include "MemoryRecyclePacketRestricted.h"

/**
 * @brief	Default constructor. 
 */
MemoryRecyclePacketRestricted::MemoryRecyclePacketRestricted(void)
{
}

/**
 * @brief	Constructor. 
 *
 * @param	numberOfPackets	Number of packets that should be in the recycle bin. Initially there
 * will be this many packets in the recycle bin. Thereafter, although the number of packets stored in
 * the recycle bin may decrease, it will never exceed this number.
 * @param	memorySizeOfPackets		The size of packets stored in the recycle bin. No packet less than or greater
 * than this value will ever be stored in the recycle bin.
 * @param	memoryLimit	Maximum amount of memory which this object can allocate at any one time.
 */
MemoryRecyclePacketRestricted::MemoryRecyclePacketRestricted( size_t numberOfPackets, size_t memorySizeOfPackets, size_t memoryLimit ) :
	MemoryRecyclePacket(numberOfPackets,memorySizeOfPackets),
	MemoryUsageRestricted(memoryLimit)
{

}

/**
 * @brief	Deep copy constructor. 
 *
 * @param	copyMe	Object to copy. 
 */
MemoryRecyclePacketRestricted::MemoryRecyclePacketRestricted( const MemoryRecyclePacketRestricted & copyMe ) :
	MemoryRecyclePacket(copyMe),
	MemoryUsageRestricted(copyMe)
{

}

/**
 * @brief	Deep assignment operator.
 *
 * @param   copyMe	Object to copy. 
 *
 * @return	a deep copy of this object.
 */
MemoryRecyclePacketRestricted & MemoryRecyclePacketRestricted::operator=( const MemoryRecyclePacketRestricted & copyMe )
{
	MemoryRecyclePacket::operator=(copyMe);
	MemoryUsageRestricted::operator=(copyMe);
	return *this;
}

/**
 * @brief	Destructor. 
 */
MemoryRecyclePacketRestricted::~MemoryRecyclePacketRestricted(void)
{
}

/**
 * @brief	Retrieves a packet from the recycle bin if possible.
 *
 * A packet of memory size GetPacketMemorySize will be retrieved if the requested memory size
 * is less than or equal to GetPacketMemorySize. If possible
 * this packet will be retrieved from the recycle bin, but if the recycle bin is
 * empty then a fresh packet will be created using non recycled memory. Non recycled
 * memory is also used if the requested packet size is more than GetPacketMemorySize. \n\n
 *
 * This object will be used as the memory restrictor.
 *
 * @param	memorySizeOfPacket		The requested memory size of the packet.
 * @param	allocatedFreshMemory	If non NULL, is set to true if fresh memory was allocated to the returned packet,
 * and set to false if recycled memory was used. (Optional, default = NULL).
 *
 * @return	the retrieved packet using recycled memory if possible, and non recycled memory otherwise.
 * Note that its memory size will not necessarily equal @a memorySizeOfPacket, it may be more but will
 * always be at least this amount.
 */
Packet * MemoryRecyclePacketRestricted::GetPacket( size_t memorySizeOfPacket, bool * allocatedFreshMemory )
{
	return MemoryRecyclePacket::GetPacket(memorySizeOfPacket,this,allocatedFreshMemory);
}
