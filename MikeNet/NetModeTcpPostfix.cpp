#include "FullInclude.h"
class NetSocketTCP;

/**
 * @brief Constructor.
 *
 * @param partialPacketSize Maximum amount of partial data (data that does not make up a full packet) that can be stored (in bytes). \n
 * Packets larger than this size cannot be received without memory reallocation.
 * @param autoResize If true then if a packet larger than @a partialPacketSize is received then more memory will be allocated so that it can be received. \n
 * If false then an error will be thrown if a packet larger than @a partialPacketSize is received.
 * @param packetPostfix Packet being sent will have this inserted at the end of the packet. Incoming packets are expected to have this postfix at the end.
 */
NetModeTcpPostfix::NetModeTcpPostfix(size_t partialPacketSize, bool autoResize, const Packet & packetPostfix) : NetModeTcp(partialPacketSize, autoResize)
{
	postfix = packetPostfix;
}

/**
 * @brief Constructor.
 *
 * @param partialPacketSize Maximum amount of partial data (data that does not make up a full packet) that can be stored (in bytes). \n
 * Packets larger than this size cannot be received without memory reallocation.
 * @param autoResize If true then if a packet larger than @a partialPacketSize is received then more memory will be allocated so that it can be received. \n
 * If false then an error will be thrown if a packet larger than @a partialPacketSize is received.
 * @param packetPostfix Packet being sent will have this inserted at the end of the packet. Incoming packets are expected to have this postfix at the end.
 * @param [in]	memoryRecycle	The memory recycle object to use. This is consumed by this object
 * and should not be referenced elsewhere. Must not be NULL.
 */
NetModeTcpPostfix::NetModeTcpPostfix(size_t partialPacketSize, bool autoResize, const Packet & packetPostfix, MemoryRecyclePacket * memoryRecycle) : NetModeTcp(partialPacketSize, autoResize, memoryRecycle)
{
	postfix = packetPostfix;
}

/**
 * @brief Deep copy constructor.
 *
 * @param	copyMe	Object to copy.
 */
NetModeTcpPostfix::NetModeTcpPostfix(const NetModeTcpPostfix & copyMe) : NetModeTcp(copyMe)
{
	postfix = copyMe.postfix;
}

/**
 * @brief Deep assignment operator.
 *
 * @param	copyMe	Object to copy.
 *
 * @return	reference to this object.
 */
NetModeTcpPostfix & NetModeTcpPostfix::operator= (const NetModeTcpPostfix & copyMe)
{
	NetModeTcp::operator=(copyMe);
	this->postfix = copyMe.postfix;
	return *this;
}

/**
 * @brief Generates a deep copy of this object.
 *
 * @return a deep copy of this object.
 */
NetModeTcpPostfix * NetModeTcpPostfix::Clone() const
{
	NetModeTcpPostfix * clone = new (nothrow) NetModeTcpPostfix(*this);
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
void NetModeTcpPostfix::DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc tcpRecvFunc, size_t clientID, size_t instanceID)
{
	partialPacket.Enter();
	try
	{
		// Size after data received
		size_t newSize = GetPartialPacketUsedSize() + completionBytes;

		// Ensure that size is not too large
		if(newSize > GetPartialPacketMemorySize())
		{
			if(!IsAutoResizeEnabled())
			{
				_ErrorException(true,"receiving new TCP data. The size of a newly received packet is larger than the TCP receive buffer",0,__LINE__,__FILE__);
			}
			else
			{
				// Increase memory size to accommodate incoming packet
				ChangePartialPacketMemorySize(newSize);
			}
		}

		// Add new bytes to the incomplete packet store
		// Added onto the end, denoted by used size
		partialPacket.addEqualWSABUF(buffer,completionBytes);

		// If there are any complete packets in the incomplete packet store then move them into the user buffer
ReCheck:

		// Search for postfix because this indicates the end of a packet
		// Returned will be pointer to the START of postfix within sData
		size_t endPos = 0;
		bool found = false;
		if(partialPacket.GetCursor() != GetPartialPacketUsedSize())
		{
			found = partialPacket.Find(partialPacket.GetCursor(),0,postfix,&endPos);
		}
		
		if(found == true)
		{
			// Determine length of packet BEFORE \r\n
			size_t packetSize = endPos - partialPacket.GetCursor();

			// packetData is used to tell Packet what data to copy
			// Postfix is not copied
			WSABUF packetData;
			packetData.buf = partialPacket.GetDataPtr();
			packetData.len = static_cast<DWORD>(endPos);

			// Copy packet, storing it in separate packet
			size_t offset = partialPacket.GetCursor();
			size_t used = endPos - offset;

			// Move cursor along, past the data we have dealt with.
			// We do this before creating the final packet in case an exception 
			// is thrown. If the cursor has moved along then future receive
			// operations can succeed, and overwrite the bad data.
			partialPacket.IncCursor(packetSize + postfix.GetUsedSize());

			Packet * completePacket = this->packetMemoryRecycle->GetPacket(used,this);
			completePacket->LoadFull(packetData,used,offset,clientID,0,instanceID,0);

			// Add packet to packet store or pass it to user function
			PacketDone(completePacket, tcpRecvFunc);

			// Recheck to see if any other completed packets exist in the TCP buffer
			goto ReCheck;
		}

		// Erase packets that we've dealt (without changing memory size)
		// Do this after all packets have been dealt with because it is more efficient
		// to erase multiple packets at the same time, rather than each one individually
		partialPacket.Erase(0, partialPacket.GetCursor());
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & Error){ partialPacket.Leave(); throw(Error); }
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
NetSend * NetModeTcpPostfix::GetSendObject(const Packet * packet, bool block)
{
	NetSend * sendObject = new (nothrow) NetSendPostfix(packet,block,&postfix);
	Utility::DynamicAllocCheck(sendObject,__LINE__,__FILE__);

	return sendObject;
}

/**
 * @brief Retrieves the protocol mode in use.
 *
 * @return NetMode::TCP_POSTFIX.
 */
NetMode::ProtocolMode NetModeTcpPostfix::GetProtocolMode() const
{
	return NetMode::TCP_POSTFIX;
}

/**
 * @brief Throws an exception as it is not possible to calculate the percentage in this mode.
 *
 * @throws ErrorReport Always because task is impossible.
 * @return never returns.
 */
double NetModeTcpPostfix::GetPartialPacketPercentage() const
{
	_ErrorException(true,
	"attempting to determine the percentage of a partial packet that has been received. This is impossible on a postfix instance since there is no way of determining when the postfix will be retrieved"
	,0,__LINE__,__FILE__);

	return 0.0;
}

/**
 * @brief Retrieves the postfix in use, which indicates the end of a packet.
 *
 * @return the postfix in use.
 */
const Packet & NetModeTcpPostfix::GetPostfix() const
{
	return postfix;
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetModeTcpPostfix::TestClass()
{
	cout << "Testing NetModeTcpPostfix class...\n";
	bool problem = false;

	Packet postfix("\r\n");
	NetModeTcpPostfix obj(1024,true,postfix,new MemoryRecyclePacket(10,12));

	WSABUF buf;
	buf.buf = "hello world";
	buf.len = static_cast<DWORD>(strlen(buf.buf));

	WSABUF buf2;
	buf2.buf = "\r\n";
	buf2.len = 2;

	WSABUF buf3;
	buf3.buf = "here we add multiple packets in one go\r\nThis is another packet!\r\nAnd another\r\n\r\n";
	buf3.len = static_cast<DWORD>(strlen(buf3.buf));

	obj.DealWithData(buf,buf.len,NULL,1,2);

	if(obj.GetPartialPacketUsedSize() != buf.len ||
	   obj.GetPacketAmount() != 0)
	{
		cout << "DealWithData is bad\n";
		problem = true;
	}
	else
	{
		cout << "DealWithData is good\n";
	}

	if(obj.GetMemorySize() == 0)
	{
		cout << "GetMemorySize is bad\n";
		problem = true;
	}
	else
	{
		cout << "GetMemorySize is good: " << obj.GetMemorySize() << "\n";
	}
	
	
	obj.DealWithData(buf2,buf2.len,NULL,1,2);

	if(obj.GetPartialPacketUsedSize() != 0 ||
	   obj.GetPacketAmount() != 1)
	{
		cout << "DealWithData is bad\n";
		problem = true;
	}
	else
	{
		cout << "DealWithData is good\n";
	}

	if(obj.GetMemorySize() == 0)
	{
		cout << "GetMemorySize is bad\n";
		problem = true;
	}
	else
	{
		cout << "GetMemorySize is good: " << obj.GetMemorySize() << "\n";
	}
	
	Packet retrieved;
	obj.GetPacketFromStore(&retrieved);

	if(retrieved.compareWSABUF(buf,buf.len) == false ||
	   retrieved.GetClientFrom() != 1 ||
	   retrieved.GetInstance() != 2)
	{
		cout << "GetPacketFromStore is bad\n";
		problem = true;
	}
	else
	{
		cout << "GetPacketFromStore is good\n";
	}

	if(obj.GetMemorySize() == 0)
	{
		cout << "GetMemorySize is bad\n";
		problem = true;
	}
	else
	{
		cout << "GetMemorySize is good: " << obj.GetMemorySize() << "\n";
	}
	
	obj.DealWithData(buf3,buf3.len,NULL,3,2);
	if(obj.GetPartialPacketUsedSize() != 0 ||
	   obj.GetPacketAmount() != 4)
	{
		cout << "DealWithData is bad\n";
		problem = true;
	}
	else
	{
		cout << "DealWithData is good\n";
	}

	if(obj.GetMemorySize() == 0)
	{
		cout << "GetMemorySize is bad\n";
		problem = true;
	}
	else
	{
		cout << "GetMemorySize is good: " << obj.GetMemorySize() << "\n";
	}

	obj.GetPacketFromStore(&retrieved);
	if(retrieved.GetUsedSize() != 38)
	{
		cout << "DealWithData is bad (packet 1)\n";
		problem = true;
	}
	else
	{
		cout << "DealWithData is good (packet 1)\n";
	}

	if(obj.GetMemorySize() == 0)
	{
		cout << "GetMemorySize is bad\n";
		problem = true;
	}
	else
	{
		cout << "GetMemorySize is good: " << obj.GetMemorySize() << "\n";
	}

	obj.GetPacketFromStore(&retrieved);
	if(retrieved.GetUsedSize() != 23)
	{
		cout << "DealWithData is bad (packet 2)\n";
		problem = true;
	}
	else
	{
		cout << "DealWithData is good (packet 2)\n";
	}

	if(obj.GetMemorySize() == 0)
	{
		cout << "GetMemorySize is bad\n";
		problem = true;
	}
	else
	{
		cout << "GetMemorySize is good: " << obj.GetMemorySize() << "\n";
	}

	obj.GetPacketFromStore(&retrieved);
	if(retrieved.GetUsedSize() != 11)
	{
		cout << "DealWithData is bad (packet 3)\n";
		problem = true;
	}
	else
	{
		cout << "DealWithData is good (packet 3)\n";
	}

	if(obj.GetMemorySize() == 0)
	{
		cout << "GetMemorySize is bad\n";
		problem = true;
	}
	else
	{
		cout << "GetMemorySize is good: " << obj.GetMemorySize() << "\n";
	}

	obj.GetPacketFromStore(&retrieved);
	if(retrieved.GetUsedSize() != 0)
	{
		cout << "DealWithData is bad (packet 4)\n";
		problem = true;
	}
	else
	{
		cout << "DealWithData is good (packet 4)\n";
	}

	if(obj.GetMemorySize() == 0)
	{
		cout << "GetMemorySize is bad\n";
		problem = true;
	}
	else
	{
		cout << "GetMemorySize is good: " << obj.GetMemorySize() << "\n";
	}

	// Adding of this packet succeeds.
	obj.SetMemoryLimit(obj.GetMemorySize() + (buf.len*2));
	obj.DealWithData(buf,buf.len,NULL,0,0);
	obj.DealWithData(buf,buf.len,NULL,0,0);
	obj.DealWithData(buf2,buf2.len,NULL,0,0);

	size_t originalMemorySize = obj.GetMemorySize();

	bool exception = false;
	try
	{
		// Adding of this packet fails due to memory limit.
		obj.DealWithData(buf,buf.len,NULL,0,0);
		obj.DealWithData(buf,buf.len,NULL,0,0);
		obj.DealWithData(buf2,buf2.len,NULL,0,0);
	}
	catch(ErrorReport & report)
	{
		exception = true;
	}

	if(exception && obj.GetMemorySize() == originalMemorySize)
	{
		cout << "DealWithData memory restriction success.\n";
	}
	else
	{
		cout << "DealWithData memory restriction failure " << obj.GetMemorySize() << "\n";
	}

	cout << "\n\n";
	return !problem;
}
