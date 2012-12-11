#include "FullInclude.h"

/**
 * @brief Constructor.
 *
 * @param partialPacketSize Maximum amount of partial data (data that does not make up a full packet) that can be stored (in bytes). \n
 * Packets larger than this size cannot be received without memory reallocation.
 * @param autoResize If true then if a packet larger than @a partialPacketSize is received then more memory will be allocated so that it can be received. \n
 * If false then an error will be thrown if a packet larger than @a partialPacketSize is received.
 */
NetModeTcpPrefixSize::NetModeTcpPrefixSize(size_t partialPacketSize, bool autoResize) : NetModeTcp(partialPacketSize,autoResize)
{

}

/**
 * @brief Constructor.
 *
 * @param partialPacketSize Maximum amount of partial data (data that does not make up a full packet) that can be stored (in bytes). \n
 * Packets larger than this size cannot be received without memory reallocation.
 * @param autoResize If true then if a packet larger than @a partialPacketSize is received then more memory will be allocated so that it can be received. \n
 * If false then an error will be thrown if a packet larger than @a partialPacketSize is received.
 * @param [in]	memoryRecycle	The memory recycle object to use. This is consumed by this object
 * and should not be referenced elsewhere. Must not be NULL.
 */
NetModeTcpPrefixSize::NetModeTcpPrefixSize(size_t partialPacketSize, bool autoResize, MemoryRecyclePacket * memoryRecycle) : NetModeTcp(partialPacketSize,autoResize, memoryRecycle)
{

}


/**
 * @brief Deep copy constructor.
 *
 * @param	copyMe	Object to copy.
 */
NetModeTcpPrefixSize::NetModeTcpPrefixSize(const NetModeTcpPrefixSize & copyMe) : NetModeTcp(copyMe)
{

}

/**
 * @brief Deep assignment operator.
 *
 * @param	copyMe	Object to copy.
 *
 * @return	reference to this object.
 */
NetModeTcpPrefixSize & NetModeTcpPrefixSize::operator= (const NetModeTcpPrefixSize & copyMe)
{
	NetModeTcp::operator=(copyMe);
	return *this;
}

/** 
 * @brief Retrieves a deep copy of this object.
 *
 * @return a deep copy of this object.
 */
NetModeTcpPrefixSize * NetModeTcpPrefixSize::Clone() const
{
	NetModeTcpPrefixSize * clone = new (nothrow) NetModeTcpPrefixSize(*this);
	Utility::DynamicAllocCheck(clone,__LINE__,__FILE__);

	return clone;
}


/**
 * @brief Deals with newly received data.
 *
 * @param buffer Newly received data.
 * @param completionBytes Number of bytes of new data stored in @a buffer.
 * @param [in] tcpRecvFunc Method will be executed and data not added to the queue if this is non NULL.
 * @param instanceID Instance that data was received on.
 * @param clientID ID of client that data was received from, set to 0 if not applicable.
 */
void NetModeTcpPrefixSize::DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc tcpRecvFunc, size_t clientID, size_t instanceID)
{
	partialPacket.Enter();

	try
	{
		// Ensure that of partial packet buffer after data received is not too large
		size_t newSize = GetPartialPacketUsedSize() + completionBytes;
		if(newSize > GetPartialPacketMemorySize())
		{
			if(!IsAutoResizeEnabled())
			{
				_ErrorException(true,"receiving new TCP data. The size of a newly received packet is larger than the TCP receive buffer",0,__LINE__,__FILE__);
			}
			else
			{
				// Increase memory size to accommodate incoming packet
				this->ChangePartialPacketMemorySize(newSize);
			}
		}

		// Add new bytes to the partial packet buffer
		partialPacket.addEqualWSABUF(buffer,completionBytes);

		// If there are any complete packets in the incomplete packet store then they will be passed to PacketDone
ReCheck:

		// Number of bytes of unread data
		size_t unreadData = partialPacket.GetPacketRemainder();

		// Check to see that remaining data in the packet is not too small
		if(unreadData >= Packet::prefixSizeBytes)
		{
			// Get the size of the most recent packet being received
			size_t packetSize = partialPacket.GetPrefixSizeT(partialPacket.GetCursor());
			size_t packetSizeWithPrefix = packetSize + Packet::prefixSizeBytes; // Data size including prefix
			
			// Warn or adjust partialPacket size if more memory is needed
			if(packetSizeWithPrefix > GetPartialPacketMemorySize())
			{
				if(!IsAutoResizeEnabled())
				{
					_ErrorException(true,"receiving new TCP data. The expected size of a newly received packet is larger than the TCP receive buffer",0,__LINE__,__FILE__);
				}
				else
				{
					// Increase memory size to accommodate incoming packet
					ChangePartialPacketMemorySize(packetSizeWithPrefix);
				}
			}

			// If the most recent packet is now fully received prepare it for PacketDone
			if(packetSizeWithPrefix <= unreadData)
			{
				// A WSABUF is used to tell Packet what data to copy
				WSABUF copyData;
				copyData.buf = partialPacket.GetDataPtr() + partialPacket.GetCursor() + Packet::prefixSizeBytes;
				copyData.len = static_cast<DWORD>(packetSize);

				// Move cursor along, past the data we have dealt with.
				// We do this before creating the final packet in case an exception 
				// is thrown. If the cursor has moved along then future receive
				// operations can succeed, and overwrite the bad data.
				partialPacket.IncCursor(packetSizeWithPrefix);

				// Copy data from partialPacket into a separate packet to be passed to user
				Packet * completePacket = this->packetMemoryRecycle->GetPacket(packetSize,this);
				completePacket->LoadFull(copyData, packetSize, 0, clientID, 0, instanceID, 0);

				// Pass to PacketDone
				PacketDone(completePacket, tcpRecvFunc);

				// Recheck to see if any other completed packets exist in partialPacket
				// (it is possible to receive more than one complete packet at the same time)
				goto ReCheck;
			}
		}

		// Erase packets that we've dealt with, this simply involves decreasing used size
		// Do this after all packets have been dealt with because it is more efficient
		// to erase multiple packets at the same time, rather than each one individually
		partialPacket.Erase(0, partialPacket.GetCursor());
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & error){ partialPacket.Leave(); throw(error); }
	catch(...){	partialPacket.Leave(); throw(-1); }

	partialPacket.Leave();
}

/**
 * @brief Generates a NetSend object.
 *
 * @param packet Packet to send.
 * @param block True if sending should be synchronous, false if sending should be asynchronous.
 *
 * @return a send object.
 */
NetSend * NetModeTcpPrefixSize::GetSendObject(const Packet * packet, bool block)
{
	Packet aux;
	aux.AddSizeT(packet->GetUsedSize());

	NetSend * sendObject = new (nothrow) NetSendPrefix(packet,block,aux);
	Utility::DynamicAllocCheck(sendObject,__LINE__,__FILE__);

	return sendObject;
}

/**
 * @brief Retrieves the protocol mode in use.
 *
 * @return NetMode::TCP_PREFIX_SIZE.
 */
NetMode::ProtocolMode NetModeTcpPrefixSize::GetProtocolMode() const
{
	return NetMode::TCP_PREFIX_SIZE;
}

/**
 * @brief Determines the amount of the oldest partial packet that
 * has been received as a percentage.
 *
 * @return the percentage of the partial packet that has been received
 * i.e. Between 0 and 100 where 50 indicates that half the packet has
 * been received and we are now waiting for the second half to be received.
 */
double NetModeTcpPrefixSize::GetPartialPacketPercentage() const
{
	double percentage = 0.0;

	if(GetPartialPacketUsedSize() >= Packet::prefixSizeBytes)
	{
		double packetFinalSize = static_cast<double>(partialPacket.GetPrefixSizeT());
		double packetCurrentSize = static_cast<double>(GetPartialPacketUsedSize() - Packet::prefixSizeBytes);

		percentage = (packetCurrentSize / packetFinalSize) * 100.0;

		/* Can be > 100 if part of another packet is in partialPacket too
		 * To avoid confusion I set the maximum to 100.0 */
		if(percentage > 100.0)
		{
			percentage = 100.0;
		}
	}
	
	return percentage;
}


/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetModeTcpPrefixSize::TestClass()
{
	cout << "Testing NetModeTcpPrefixSize class...\n";
	bool problem = false;

	NetModeTcpPrefixSize obj(1024,true);

	Packet packet;

	// Make sure that string has an even length (length % 2 = 0)
	char * str = "hello world, this is a packet.";
	packet.AddStringC(str,0,true);
	
	WSABUF buffer;
	buffer.buf = packet.GetDataPtr();
	buffer.len = static_cast<DWORD>(packet.GetUsedSize());

	// Deal with only half of the buffer
	size_t amountToAdd = ((buffer.len-Packet::prefixSizeBytes) / 2)+Packet::prefixSizeBytes;
	obj.DealWithData(buffer,amountToAdd,NULL,1,2);

	// Floats are not precise
	double percentage = obj.GetPartialPacketPercentage();
	if(percentage <= 49.0 || percentage >= 51.0)
	{
		cout << "GetPartialPacketPercentage is bad: " << obj.GetPartialPacketPercentage() << "%\n";
		problem = true;
	}
	else
	{
		cout << "GetPartialPacketPercentage is good: " << percentage << "%\n";
	}

	if(obj.GetPartialPacketUsedSize() != amountToAdd || obj.GetPacketAmount() != 0)
	{
		cout << "DealWithData is bad\n";
		problem = true;
	}
	else
	{
		cout << "DealWithData is good\n";
	}

	// Deal with second half of buffer
	size_t amountAdded = amountToAdd;
	size_t remainingData = buffer.len - amountAdded;

	buffer.buf = buffer.buf + amountAdded;
	buffer.len = static_cast<DWORD>(remainingData);
	obj.DealWithData(buffer,remainingData,NULL,1,2);

	if(obj.GetPartialPacketUsedSize() != 0 || obj.GetPacketAmount() != 1)
	{
		cout << "DealWithData is bad\n";
		problem = true;
	}
	else
	{
		cout << "DealWithData is good\n";
	}

	Packet destination;
	obj.GetPacketFromStore(&destination);

	if(destination != str)
	{
		cout << "DealWithData is bad\n";
		problem = true;
	}
	else
	{
		cout << "DealWithData is good\n";
	}

	// Multiple packets in one DealWithData
	packet.AddStringC("another packet",0,true);
	packet.AddStringC("",0,true); // Empty packet
	
	buffer.buf = packet.GetDataPtr();
	buffer.len = static_cast<DWORD>(packet.GetUsedSize());

	obj.DealWithData(buffer,buffer.len,NULL,1,2);

	if(obj.GetPartialPacketUsedSize() != 0 || obj.GetPacketAmount() != 3)
	{
		cout << "DealWithData is bad\n";
		problem = true;
	}
	else
	{
		cout << "DealWithData is good\n";
	}

	obj.GetPacketFromStore(&destination);
	if(destination != "hello world, this is a packet.")
	{
		cout << "DealWithData is bad (packet 1)\n";
		problem = true;
	}
	else
	{
		cout << "DealWithData is good (packet 1)\n";
	}

	obj.GetPacketFromStore(&destination);
	if(destination != "another packet")
	{
		cout << "DealWithData is bad (packet 2)\n";
		problem = true;
	}
	else
	{
		cout << "DealWithData is good (packet 2)\n";
	}

	obj.GetPacketFromStore(&destination);
	if(destination != "")
	{
		cout << "DealWithData is bad (packet 3)\n";
		problem = true;
	}
	else
	{
		cout << "DealWithData is good (packet 3)\n";
	}

	cout << "\n\n";
	return !problem;
}
