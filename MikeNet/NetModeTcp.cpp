#include "FullInclude.h"
#include "MemoryRecyclePacket.h"

/**
 * @brief Constructor.
 *
 * @param partialPacketSize Maximum amount of partial data (data that does not make up a full packet) that can be stored (in bytes).
 * Packets larger than this size cannot be received without memory reallocation.
 * @param autoResize If true then if a packet larger than @a partialPacketSize is received then more memory will be automatically allocated so that it can be received. \n
 * If false then an error will be thrown if a packet larger than @a partialPacketSize is received.
 *
 * By default memory usage is unrestricted (referring to the use of SizeRestrictable class).
 */
NetModeTcp::NetModeTcp(size_t partialPacketSize, bool autoResize) : autoResize(autoResize)
{
	partialPacket.SetMemorySize(partialPacketSize);

	// Memory recycle stores no packets, so essentially just uses new and delete.
	MemoryRecyclePacket * recycle = new (nothrow) MemoryRecyclePacket;
	Utility::DynamicAllocCheck(recycle,__LINE__,__FILE__);
	LoadMemoryRecycle(recycle);

	this->EnforceMemoryLimit(GetPacketStoreMemorySize(),partialPacketSize); // Needs memory recycle to be constructed first.
}

/**
 * @brief Constructor.
 *
 * @param partialPacketSize Maximum amount of partial data (data that does not make up a full packet) that can be stored (in bytes).
 * Packets larger than this size cannot be received without memory reallocation.
 * @param autoResize If true then if a packet larger than @a partialPacketSize is received then more memory will be automatically allocated so that it can be received. \n
 * If false then an error will be thrown if a packet larger than @a partialPacketSize is received.
 * @param [in]	memoryRecycle	The memory recycle object to use. This is consumed by this object
 * and should not be referenced elsewhere. Must not be NULL.
 *
 * By default memory usage is unrestricted (referring to the use of SizeRestrictable class).
 */
NetModeTcp::NetModeTcp(size_t partialPacketSize, bool autoResize, MemoryRecyclePacket * memoryRecycle) : autoResize(autoResize)
{
	// Must load memory recycle before enforcing limits.
	LoadMemoryRecycle(memoryRecycle);

	this->EnforceMemoryLimit(GetPacketStoreMemorySize(),partialPacketSize);
	partialPacket.SetMemorySize(partialPacketSize);
}

/**
 * @brief Deep copy / assignment operator helper.
 *
 * @param	copyMe	Object to copy.
 */
void NetModeTcp::Copy(const NetModeTcp & copyMe)
{
	this->autoResize = copyMe.autoResize;
	this->partialPacket.SetMemorySize(copyMe.partialPacket.GetMemorySize()); // Memory size is not copied automatically
	this->partialPacket = copyMe.partialPacket;
	this->packetStore = copyMe.packetStore;

	MemoryRecyclePacket * recycle = new (nothrow) MemoryRecyclePacket(*copyMe.packetMemoryRecycle);
	Utility::DynamicAllocCheck(recycle,__LINE__,__FILE__);
	LoadMemoryRecycle(recycle);
}

/**
 * @brief Deep copy constructor.
 *
 * @param	copyMe	Object to copy.
 */
NetModeTcp::NetModeTcp(const NetModeTcp & copyMe) : MemoryUsageRestricted(copyMe)
{
	Copy(copyMe);
}

/**
 * @brief Deep assignment operator.
 *
 * @param	copyMe	Object to copy. 
 * @return	reference to this object.
 */
NetModeTcp & NetModeTcp::operator= (const NetModeTcp & copyMe)
{
	MemoryUsageRestricted::operator=(copyMe);
	Copy(copyMe);
	return *this;
}

/**
 * @brief Destructor.
 */
NetModeTcp::~NetModeTcp()
{
	delete packetMemoryRecycle;
}

/**
 * @brief	Loads a memory recycle object, used to manage memory
 * of packets efficiently.
 *
 * @param [in]	memoryRecycle	The memory recycle object to use. This is consumed by this object
 * and should not be referenced elsewhere. Must not be NULL.
 */
void NetModeTcp::LoadMemoryRecycle(MemoryRecyclePacket * memoryRecycle)
{
	_ErrorException((memoryRecycle == NULL),"loading a packet memory recycle object, must not be NULL",0,__LINE__,__FILE__);
	packetMemoryRecyclePtrAccess._safeWriteValue(packetMemoryRecycle,memoryRecycle);
}

/**
 * @brief Erases all stored TCP data.
 *
 * The object will now be in the same state as if it were newly constructed.
 */
void NetModeTcp::ClearData()
{
	ClearPacketStore();
	partialPacket.Clear();
}

/**
 * @brief Clears only the complete packet store, completely emptying it.
 */
void NetModeTcp::ClearPacketStore()
{
	packetStore.Enter();
	try
	{
		while(!packetStore.IsEmpty())
		{
			packetMemoryRecycle->RecyclePacket(packetStore.ExtractFront());
		}
	}
	catch(const ErrorReport & report){packetStore.Leave(); throw report;}
	packetStore.Leave();
}

/**
 * @brief Retrieves the number of fully received packets in the complete packet store.
 *
 * @param clientID Ignored (optional, default = 0).
 * @param operationID Ignored (optional, default = 0).
 *
 * @return the number of fully received packets in the packet store. 
 */
size_t NetModeTcp::GetPacketAmount( size_t clientID, size_t operationID ) const
{
	return packetStore.Size();
}

/**
 * @brief Changes the the size of the largest packet that can be received.
 *
 * Packets larger than this will require an increase in memory size or
 * an exception will be thrown.
 *
 * @note Attempting to decrease the size may not be effective. If data exists in the buffer
 * this will not be discarded. The buffer will decrease as much as possible without discarding data.
 *
 * @param newSize New size.
 */
void NetModeTcp::ChangePartialPacketMemorySize(size_t newSize)
{
	this->EnforceMemoryLimit(GetPacketStoreMemorySize(),newSize);
	partialPacket.ChangeMemorySize(newSize);
}

/**
 * @brief Enables or disables the 'auto resize' option.
 *
 * @param paraAutoResize Value to set autoResize to. \n
 * When true, if a packet larger than the maximum TCP packet
 * size is received then the max size will be increased silently.\n
 * When false, if a packet larger than the maximum TCP packet
 * size is received then an exception will be thrown.
 */
void NetModeTcp::SetAutoResize(bool paraAutoResize)
{
	autoResize.Set(paraAutoResize);
}

/**
 * @brief Determines whether the 'auto resize' option is enabled.
 *
 * @return when true, if a packet larger than the maximum TCP packet
 * size is received then the max size will be increased silently.\n
 * @return when false, if a packet larger than the maximum TCP packet
 * size is received then an exception will be thrown.
 */
bool NetModeTcp::IsAutoResizeEnabled() const
{
	return autoResize.Get();
}

/**
 * @brief Deals with a complete packet.
 *
 * The class deals with it in one of two ways:
 * - Passes it to a user function specified by @a tcpRecvFunc parameter.
 * - If no user function is defined then it is put into a queue to be retrieved using GetPacketFromStore().
 *
 * A special case exists for an instance in client state and handshaking. In this case the
 * packet is always added to the packet queue. This is necessary because the handshaking thread
 * needs to receive TCP data. The user of this method must ensure @a tcpRecvFunc is NULL in this case.
 *
 * @warning If the packet is passed to a user function this is done synchronously, so this method
 * will not return until the user function returns.
 *
 * @param [in] completePacket The complete packet that should be dealt with. The packet is now owned by this object and should not be referenced elsewhere.
 * @param [in] tcpRecvFunc Method will be executed and data not added to the queue if this is non NULL.
 */
void NetModeTcp::PacketDone(Packet * completePacket, NetSocket::RecvFunc tcpRecvFunc)
{
	if(tcpRecvFunc == NULL)
	{
		// Add the new packet to the TCP packet user buffer.
		packetStore.Add(completePacket);
	}
	else
	{
		// Execute user function to deal with packet.
		(*tcpRecvFunc)(*completePacket);
		this->packetMemoryRecycle->RecyclePacket(completePacket);
	}
}

/**
 * @brief Retrieves the size of the largest packet that can be received without a change in memory size.
 *
 * @return	the size of the largest packet that can be received, packets larger than this
 * will require an increase in memory size or an exception will be thrown.
 */
size_t NetModeTcp::GetMaxPacketSize() const
{
	return partialPacket.GetMemorySize();
}

/**
 * @brief Determines the amount of data currently stored in
 * the partial packet.
 *
 * When used outside of the DealWithData method, this
 * retrieves the size of the packet currently being received.
 *
 * @return the amount of data currently stored in the partial packet.
 * Normally this is the current size of the partial packet being received.
 * i.e. The number of bytes of the packet that have been received.
 */
size_t NetModeTcp::GetPartialPacketUsedSize() const
{
	return partialPacket.GetUsedSize();
}

/**
 * @brief Retrieves a complete packet from the packet store.
 *
 * @param [out] destination Destination to copy into.
 * @param clientID Ignored (optional, default = 0).
 * @param operationID Ignored (optional, default = 0).
 *
 * @return number of packets in packet store before this method call.
 */
size_t NetModeTcp::GetPacketFromStore( Packet * destination, size_t clientID, size_t operationID )
{
	size_t returnMe;
	_ErrorException((destination == NULL),"retrieving a packet from the complete packet store, destination must not be NULL",0,__LINE__,__FILE__);

	// Critical sections used in case packet at front of queue is changed by another thread.
	packetStore.Enter();
	try
	{
		returnMe = packetStore.Size();

		// Nothing changes if no packet was removed.
		if(returnMe > 0)
		{
			// Extract packet from front of queue (removing it but not deallocating it).
			Packet * extractedPacket = packetStore.ExtractFront();

			// Copy packet to destination.
			*destination = *extractedPacket;
			
			// Cleanup memory or recycle memory.
			this->packetMemoryRecycle->RecyclePacket(extractedPacket);
		}
	}
	catch(ErrorReport & errorReport){packetStore.Leave(); throw errorReport;}
	packetStore.Leave();

	return returnMe;
}

/**
 * @brief	Retrieves the amount of memory in use by the packet store.
 *
 * @return	amount of memory in use by the packet store.
 */
size_t NetModeTcp::GetPacketStoreMemorySize() const
{
	return this->packetMemoryRecycle->GetMemorySize();
}

/**
 * @brief	Retrieves the amount of memory in use by the partial packet store.
 *
 * @return	the amount of memory in use by the partial packet store.
 */
size_t NetModeTcp::GetPartialPacketMemorySize() const
{
	return partialPacket.GetMemorySize();
}

/**
 * @brief	Determines the amount of memory in use by this object.
 *
 * @return	the amount of memory in use by this object.
 */
size_t NetModeTcp::GetMemorySize() const
{
	return GetMemorySize(GetPacketStoreMemorySize(), GetPartialPacketMemorySize());
}

/**
 * @brief	Retrieves the total memory size given the packet store and partial packet size.
 *
 * @param	packetStoreSizeValue		The total packet store size value including the size of both packetStore and packetStoreRecycle. 
 * @param	partialPacketSizeValue		The partial packet size value. 
 *
 * @return	the total memory size. 
 */
size_t NetModeTcp::GetMemorySize(size_t packetStoreSizeValue, size_t partialPacketSizeValue)
{
	size_t totalSize = packetStoreSizeValue + partialPacketSizeValue;

	// Integer overflow failsafe.
	if(totalSize < packetStoreSizeValue || totalSize < partialPacketSizeValue)
	{
		// Return maximum integer. 
		return -1;
	}
	else
	{
		return totalSize;
	}
}

/**
 * @brief	Throws an exception if the estimated total memory size is more than
 * the limit.
 *
 * @param	desiredPacketStoreSizeValue				The desired packet store size value including the size of both packetStore and packetStoreRecycle. 
 * @param	desiredPartialPacketSizeValue			The desired partial packet size value. 
 */
void NetModeTcp::EnforceMemoryLimit(size_t desiredPacketStoreSizeValue, size_t desiredPartialPacketSizeValue)
{
	MemoryUsageRestricted::EnforceMemoryLimit(GetMemorySize(desiredPacketStoreSizeValue,desiredPartialPacketSizeValue));
}

/**
 * @brief Class that implements NetModeTcp so that it can be tested.
 */
class TestClassNetModeTcp : public NetModeTcp
{
public:
	/**
	 * @brief Constructor.
	 *
	 * Adds an integer to partial packet for testing purposes.
	 *
	 * @param partialPacketSize Maximum amount of partial data (data that does not make up a full packet) that can be stored (in bytes).
	 * Packets larger than this size cannot be received without memory reallocation.
	 * @param autoResize If true then if a packet larger than @a partialPacketSize is received then more memory will be automatically allocated so that it can be received. \n
	 * If false then an error will be thrown if a packet larger than @a partialPacketSize is received.
	 */
	TestClassNetModeTcp(size_t partialPacketSize, bool autoResize) : NetModeTcp(partialPacketSize, autoResize)
	{
		partialPacket.Add<int>(5000);
	}

	/** 
	 * @brief Implements virtual method.
	 *
	 * @return 0.0.
	 */
	double GetPartialPacketPercentage() const
	{
		return 0.0;
	}

	/** 
	 * @brief Implements virtual method.
	 *
	 * @return NULL.
	 */
	TestClassNetModeTcp * Clone() const
	{
		return NULL;
	}

	/**
	 * @brief Implements virtual method.
	 *
	 * @param packet Packet to send.
	 * @param block True if sending should be synchronous, false if sending should be asynchronous.
	 *
	 * @return NULL.
	 */
	NetSend * GetSendObject(const Packet * packet, bool block)
	{
		return NULL;
	}

	/** 
	 * @brief Implements virtual method.
	 *
	 * @return ProtocolMode::TCP_POSTFIX.
	 */
	NetMode::ProtocolMode GetProtocolMode() const
	{
		return NetMode::TCP_POSTFIX;
	}

	/**
	 * @brief Implements virtual method.
	 *
	 * @param buffer Newly received data.
	 * @param completionBytes Number of bytes of new data stored in @a buffer.
	 * @param [in] recvFunc Method will be executed and data not added to the queue if this is non NULL.
	 * @param instanceID Instance that data was received on.
	 * @param clientID ID of client that data was received from, set to 0 if not applicable.
	 */
	void DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc recvFunc, size_t clientID, size_t instanceID)
	{

	}
};

void NetModeTcpTest(Packet & packet)
{
	cout << "Recv function called";
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetModeTcp::TestClass()
{
	cout << "Testing NetModeTcp class...\n";
	bool problem = false;

	TestClassNetModeTcp obj(1000,false);

	if(obj.GetMaxPacketSize() != 1000)
	{
		cout << "GetMaxPacketSize or Constructor is bad\n";
		problem = true;
	}
	else
	{
		cout << "GetMaxPacketSize and Constructor are good\n";
	}

	obj.ChangePartialPacketMemorySize(2000);
	if(obj.GetMaxPacketSize() != 2000)
	{
		cout << "ChangeMaxPacketSize is bad\n";
		problem = true;
	}
	else
	{
		cout << "ChangeMaxPacketSize is good\n";
	}

	Packet * packet = obj.packetMemoryRecycle->GetPacket(100);
	packet->AddStringC("hello world",0,false);
	obj.PacketDone(packet,NULL);

	if(obj.GetPacketAmount() != 1)
	{
		cout << "PacketDone or GetPacketAmount is bad\n";
		problem = true;
	}
	else
	{
		cout << "PacketDone and GetPacketAmount are good\n";
	}
	
	obj.ClearPacketStore();
	if(obj.GetPacketAmount() != 0)
	{
		cout << "ClearPacketStore is bad\n";
		problem = true;
	}
	else
	{
		cout << "ClearPacketStore is good\n";
	}
	
	packet = obj.packetMemoryRecycle->GetPacket(100);
	packet->AddStringC("hello universe",0,false);
	cout << "Check that below receive function is called!\n";
	obj.PacketDone(packet,NetModeTcpTest);

	if(obj.GetPartialPacketUsedSize() != sizeof(int))
	{
		cout << "GetPartialPacketCurrentSize is bad\n";
		problem = true;
	}
	else
	{
		cout << "GetPartialPacketCurrentSize is good\n";
	}

	obj.ClearData();
	if(obj.GetPartialPacketUsedSize() != 0)
	{
		cout << "ClearData is bad\n";
		problem = true;
	}
	else
	{
		cout << "ClearData is good\n";
	}

	if(obj.IsAutoResizeEnabled() == true)
	{
		cout << "IsAutoResizeEnabled or constructor is bad\n";
		problem = true;
	}
	else
	{
		cout << "IsAutoResizeEnabled and constructor are good\n";
	}

	obj.SetAutoResize(true);
	if(obj.IsAutoResizeEnabled() == false)
	{
		cout << "IsAutoResizeEnabled or constructor is bad\n";
		problem = true;
	}
	else
	{
		cout << "IsAutoResizeEnabled and constructor are good\n";
	}

	cout << "\n\n";
	return !problem;
}