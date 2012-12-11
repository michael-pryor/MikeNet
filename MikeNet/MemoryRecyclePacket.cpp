#include "FullInclude.h"
#include "MemoryRecyclePacket.h"

/**
 * @brief	Constructor. 
 *
 * @param	numberOfPackets	Number of packets that should be in the recycle bin. Initially there
 * will be this many packets in the recycle bin. Thereafter, although the number of packets stored in
 * the recycle bin may decrease, it will never exceed this number.
 * @param	packetSize		The size of packets stored in the recycle bin. No packet less than or greater
 * than this value will ever be stored in the recycle bin.
 */
MemoryRecyclePacket::MemoryRecyclePacket(size_t numberOfPackets, size_t packetSize)
{
	this->numberOfPackets = numberOfPackets;
	this->packetSize = packetSize;

	recycleBin.ResizeAllocate(numberOfPackets);

	for(size_t n = 0;n<recycleBin.Size();n++)
	{
		recycleBin[n].SetMemorySize(packetSize);
		this->IncreaseMemorySize(packetSize);
	}
}

/**
 * @brief Default constructor.
 *
 * Memory recycle simply allocates and deallocates memory without recycling any
 * memory if this constructor is used.
 */
MemoryRecyclePacket::MemoryRecyclePacket()
{
	this->numberOfPackets = 0;
	this->packetSize = 0;
}

/**
 * @brief	Copies the contents of @a copyMe into this object (deep).
 *
 * @param	copyMe	The object to copy.
 */
void MemoryRecyclePacket::Copy(const MemoryRecyclePacket & copyMe)
{
	this->recycleBin = copyMe.recycleBin;
	this->numberOfPackets = copyMe.numberOfPackets;
	this->packetSize = copyMe.packetSize;
}

/**
 * @brief	Deep copy constructor. 
 *
 * @param	copyMe	Object to copy. 
 */
MemoryRecyclePacket::MemoryRecyclePacket( const MemoryRecyclePacket & copyMe ) :
	MemoryUsageLog(copyMe)
{
	Copy(copyMe);
}

/**
 * @brief	Deep assignment operator. 
 *
 * @param	copyMe	Object to copy. 
 *
 * @return	a deep copy of this object. 
 */
MemoryRecyclePacket & MemoryRecyclePacket::operator=(const MemoryRecyclePacket & copyMe)
{
	MemoryUsageLog::operator=(copyMe);
	Copy(copyMe);
	return *this;
}

/**
 * @brief	Destructor. 
 */
MemoryRecyclePacket::~MemoryRecyclePacket(void)
{

}

/**
 * @brief	Retrieves the amount of memory assigned to each packet stored
 * by this object.
 *
 * @return	the amount of memory assigned to each packet stored by this object.
 */
size_t MemoryRecyclePacket::GetPacketMemorySize() const
{
	return this->packetSize;
}

/**
 * @brief	Retrieves the maximum number of packets this
 * object can store, ready to be recycled.
 *
 * @return	the maximum number of packets that this object can store.
 */
size_t MemoryRecyclePacket::GetMaxNumberOfPackets() const
{
	return this->numberOfPackets;
}

/**
 * @brief	Retrieves a packet from the recycle bin if possible.
 *
 * A packet of memory size GetPacketMemorySize will be retrieved if the requested memory size
 * is less than or equal to GetPacketMemorySize. If possible
 * this packet will be retrieved from the recycle bin, but if the recycle bin is
 * empty then a fresh packet will be created using non recycled memory. Non recycled
 * memory is also used if the requested packet size is more than GetPacketMemorySize.
 *
 * @param	memorySizeOfPacket		The requested memory size of the packet.
 * @param   memoryRestrictor		Object which keeps track of this object's memory usage and restricts it. Object
 * may keep track of other objects too. Set to NULL if memory should not be restricted. (Optional, default = NULL). 
 * @param	allocatedFreshMemory	If non NULL, is set to true if fresh memory was allocated to the returned packet,
 * and set to false if recycled memory was used. (Optional, default = NULL).
 *
 * @return	the retrieved packet using recycled memory if possible, and non recycled memory otherwise.
 * Note that its memory size will not necessarily equal @a memorySizeOfPacket, it may be more but will
 * always be at least this amount.
 */
Packet * MemoryRecyclePacket::GetPacket( size_t memorySizeOfPacket, const MemoryUsageRestricted * memoryRestrictor, bool * allocatedFreshMemory )
{
	Packet * packet = NULL;

	recycleBin.Enter();
	try
	{
		if(memorySizeOfPacket <= this->GetPacketMemorySize() )
		{
			if(recycleBin.Size() > 0)
			{
				packet = recycleBin.Extract(0);

				if(allocatedFreshMemory != NULL)
				{
					*allocatedFreshMemory = false;
				}
			}
			else
			{
				if(memoryRestrictor != NULL)
				{
					memoryRestrictor->EnforceMemoryLimitIncrease(this->GetPacketMemorySize());
				}

				packet = new (nothrow) Packet();
				Utility::DynamicAllocCheck(packet,__LINE__,__FILE__);

				packet->SetMemorySize(this->GetPacketMemorySize());

				this->IncreaseMemorySize(this->GetPacketMemorySize());

				if(allocatedFreshMemory != NULL)
				{
					*allocatedFreshMemory = true;
				}
			}
		}
		else
		{
			if(memoryRestrictor != NULL)
			{
				memoryRestrictor->EnforceMemoryLimitIncrease(memorySizeOfPacket);
			}

			packet = new (nothrow) Packet();
			Utility::DynamicAllocCheck(packet,__LINE__,__FILE__);

			packet->SetMemorySize(memorySizeOfPacket);
			
			this->IncreaseMemorySize(memorySizeOfPacket);

			if(allocatedFreshMemory != NULL)
			{
				*allocatedFreshMemory = true;
			}
		}
	}
	catch(ErrorReport & report){recycleBin.Leave(); throw report;}
	recycleBin.Leave();

	return packet;
}

/**
 * @brief	Recycles the specified packet if possible.
 *
 * The specified packet is consumed by this object, whether it is recycled
 * or not. For the packet to be recycled, the recycle bin must not have reached
 * its limit of GetNumberOfPackets packets, and the packet must be of size. 
 * GetPacketMemorySize. If the packet cannot be recycled it will be deallocated
 * with no recycling.
 *
 * @param [in]	packet	The packet to recycle. This is always consumed and should
 * not be referenced elsewhere after this call.
 *
 * @return	true if the packet has been recycled, false if the packet has been deallocated.
 */
bool MemoryRecyclePacket::RecyclePacket( Packet * packet )
{
	bool addedToRecyleBin;

	recycleBin.Enter();
	try
	{
		if(packet != NULL &&
		   recycleBin.Size() < numberOfPackets &&
		   packet->GetMemorySize() == this->GetPacketMemorySize())
		{
			packet->Clear();
			recycleBin.Add(packet);
			addedToRecyleBin = true;
		}
		else
		{
			size_t sizeOfPacketToDeallocate = 0;
			if(packet != NULL)
			{
				sizeOfPacketToDeallocate = packet->GetMemorySize();
			}

			delete packet;
			this->DecreaseMemorySize(sizeOfPacketToDeallocate);

			addedToRecyleBin = false;
		}
	}
	catch(ErrorReport & report){recycleBin.Leave(); throw report;}
	recycleBin.Leave();	

	return addedToRecyleBin;
}
