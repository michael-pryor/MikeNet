#include "FullInclude.h"

/**
 * @brief	Constructor. 
 *
 * @param numClients		Number of clients that object should store packets for.
 * @param memoryRecycler	A memory recycler which is copied for each client. Each client
 * has its own separate memory recycler, as a copy of this object. Set to NULL to not recycle memory. (Optional, default = NULL).
 */
NetModeUdpCatchAllNo::NetModeUdpCatchAllNo(size_t numClients, const MemoryRecyclePacketRestricted * memoryRecycler) : NetModeUdpCatchAll(numClients, memoryRecycler)
{
	sendCounter.ResizeAllocate(numClients+1,INITIAL_COUNTER_VALUE);
	recvCounter.ResizeAllocate(numClients+1,INITIAL_COUNTER_VALUE);
}

/**
 * @brief Copy constructor / assignment operator helper method.
 *
 * @param	copyMe	Object to copy.
 */
void NetModeUdpCatchAllNo::Copy(const NetModeUdpCatchAllNo & copyMe)
{
	this->packetStore = copyMe.packetStore;
	this->sendCounter = copyMe.sendCounter;
	this->recvCounter = copyMe.recvCounter;
}

/**
 * @brief Deep copy constructor.
 *
 * @param	copyMe	Object to copy.
 */
NetModeUdpCatchAllNo::NetModeUdpCatchAllNo(const NetModeUdpCatchAllNo & copyMe) : NetModeUdpCatchAll(copyMe)
{
	Copy(copyMe);
}

/**
 * @brief Deep assignment operator.
 *
 * @param	copyMe	Object to copy.
 *
 * @return	reference to this object.
 */
NetModeUdpCatchAllNo & NetModeUdpCatchAllNo::operator= (const NetModeUdpCatchAllNo & copyMe)
{
	NetModeUdpCatchAll::operator=(copyMe);
	Copy(copyMe);
	return *this;
}


/** 
 * @brief Retrieves a deep copy of this object.
 *
 * @return a deep copy of this object.
 */
NetModeUdpCatchAllNo * NetModeUdpCatchAllNo::Clone() const
{
	NetModeUdpCatchAllNo * clone = new (nothrow) NetModeUdpCatchAllNo(*this);
	Utility::DynamicAllocCheck(clone,__LINE__,__FILE__);

	return clone;
}

/**
 * @brief Resets data of specified client.
 *
 * @param clientID ID of client to use.
 */
void NetModeUdpCatchAllNo::Reset(size_t clientID)
{
	NetModeUdpCatchAll::Reset(clientID);
	
	sendCounter[clientID].Set(INITIAL_COUNTER_VALUE);
	recvCounter[clientID].Set(INITIAL_COUNTER_VALUE);
}

/**
 * @brief Reset data of all clients.
 */
void NetModeUdpCatchAllNo::Reset()
{
	for(size_t n = 0;n<sendCounter.Size();n++)
	{
		Reset(n);
	}
}

/**
 * @brief Deals with newly received data.
 *
 * @param buffer Newly received data.
 * @param completionBytes Number of bytes of new data stored in @a buffer.
 * @param [in] udpRecvFunc Method will be executed and data not added to the queue if this is non NULL.
 * @param instanceID Instance that data was received on.
 * @param clientID ID of client that data was received from, set to 0 if not applicable.
 */
void NetModeUdpCatchAllNo::DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc udpRecvFunc, size_t clientID, size_t instanceID)
{
	// Access buffer directly using Packet object
	Packet packetBuffer;
	packetBuffer.SetDataPtr(buffer.buf,buffer.len,completionBytes);

	/**
	 * Deal with packet if it is new.
	 * Counter increases by one every time sender sends.
	 * So, only out of order packets will have a counter lower
	 * or equal to the counter of the last received packet.
	 */
counterJustResetSoTryAgain:
	size_t newPacketCounter = packetBuffer.GetSizeT();
	
	// Ignore connection packets
	if(newPacketCounter != 0)
	{
		if(newPacketCounter >= recvCounter[clientID].Get())
		{
			size_t usedSize = completionBytes-packetBuffer.GetCursor();

			// Copy data into Packet object, excluding the prefix
			Packet * newPacket = packetStoreMemoryRecycle[clientID].GetPacket(usedSize);
			newPacket->LoadFull(buffer,usedSize,packetBuffer.GetCursor(),clientID,0,instanceID,static_cast<clock_t>(newPacketCounter));

			// Set recvCounter
			recvCounter[clientID].Set(newPacketCounter);

			// Add packet to queue
			PacketDone(newPacket,udpRecvFunc);
		}
		else
		{
			/**
			 * If the current counter value is vastly different to the last
			 * counter value, then it is likely that the maximum for
			 * the counter value was reached, and so it looped back round
			 * to 0.
			 */
			bool retryPacket = false;
			recvCounter[clientID].Enter();
			if(recvCounter[clientID].Get() - newPacketCounter >  recvCounter[clientID].Get() / 2)
			{
				recvCounter[clientID].Set(INITIAL_COUNTER_VALUE);
				retryPacket = true;
			}
			recvCounter[clientID].Leave();

			if(retryPacket == true)
			{
				packetBuffer.SetCursor(0);
				goto counterJustResetSoTryAgain;
			}
		}
	}
}

/**
 * @brief Generates a NetSend object.
 *
 * @param packet Packet to send.
 * @param block True if sending should be synchronous, false if sending should be asynchronous.
 *
 * @return a send object.
 */
NetSend * NetModeUdpCatchAllNo::GetSendObject(const Packet * packet, bool block)
{
	Packet aux;
	aux.AddSizeT(sendCounter[packet->GetClientFrom()].Get());

	NetSend * sendObject = new (nothrow) NetSendPrefix(packet,block,aux);
	Utility::DynamicAllocCheck(sendObject,__LINE__,__FILE__);

	sendCounter[packet->GetClientFrom()].Increase(1);
	
	return sendObject;
}

/**
 * @brief Retrieves the protocol mode in use.
 *
 * @return NetMode::UDP_CATCH_ALL_NO.
 */
NetMode::ProtocolMode NetModeUdpCatchAllNo::GetProtocolMode() const
{
	return NetMode::UDP_CATCH_ALL_NO;
}


/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetModeUdpCatchAllNo::TestClass()
{
	cout << "Testing NetModeUdpCatchAllNo class...\n";
	bool problem = false;

	NetModeUdpCatchAllNo obj(10);
	Packet destination;

	// Setup packet
	char * str = "hello, this is a packet! HURRAY!";
	Packet packet;
	packet.AddSizeT(0);
	packet.AddStringC(str,0,false);

	if(_HelperTestClass(obj,packet,str,10,10,0) == false)
	{
		problem = true;	
	}

	cout << "\n\n";
	return !problem;
}
