#include "FullInclude.h"
#include "NetModeTcpRaw.h"

/**
 * @brief	Constructor. 
 *
 * @param [in]	memoryRecycle	The memory recycle object to use. This is consumed by this object
 * and should not be referenced elsewhere. Must not be NULL.
 */
NetModeTcpRaw::NetModeTcpRaw( MemoryRecyclePacket * memoryRecycle ) : NetModeTcp(0,false,memoryRecycle)
{

}

/**
 * @brief	Deep copy constructor.
 *
 * @param	copyMe	The object to copy. 
 */
NetModeTcpRaw::NetModeTcpRaw( const NetModeTcpRaw & copyMe ) : NetModeTcp(copyMe)
{

}

/**
 * @brief	Deep assignment operator.
 *
 * @param	copyMe	The object to copy. 
 *
 * @return	A deep copy of this object. 
 */
NetModeTcpRaw & NetModeTcpRaw::operator=( const NetModeTcpRaw & copyMe )
{
	NetModeTcp::operator=(copyMe);
	return *this;
}

/**
 * @brief Throws an exception as it is not possible to calculate the percentage in this mode.
 *
 * @throws ErrorReport Always because task is impossible.
 * @return never returns.
 */
double NetModeTcpRaw::GetPartialPacketPercentage() const
{
	_ErrorException(true,
	"attempting to determine the percentage of a partial packet that has been received. This operation doesn't make sense for a raw TCP instance"
	,0,__LINE__,__FILE__);

	return 0.0;
}

/** 
 * @brief Retrieves a deep copy of this object.
 *
 * @return a deep copy of this object.
 */
NetModeTcp * NetModeTcpRaw::Clone() const
{
	NetModeTcp * returnMe = new NetModeTcpRaw(*this);
	Utility::DynamicAllocCheck(returnMe,__LINE__,__FILE__);

	return returnMe;
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
void NetModeTcpRaw::DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc tcpRecvFunc, size_t clientID, size_t instanceID)
{
	Packet * completePacket = this->packetMemoryRecycle->GetPacket(completionBytes,this);
	completePacket->LoadFull(buffer,completionBytes,0,clientID,0,instanceID,0);

	// Add packet to packet store or pass it to user function
	PacketDone(completePacket, tcpRecvFunc);
}

/**
 * @brief Generates a NetSend object.
 *
 * @param packet Packet to send.
 * @param block True if sending should be synchronous, false if sending should be asynchronous.
 *
 * @return a send object.
 */
NetSend * NetModeTcpRaw::GetSendObject(const Packet * packet, bool block)
{
	NetSend * sendObject = new (nothrow) NetSendRaw(packet,block);
	Utility::DynamicAllocCheck(sendObject,__LINE__,__FILE__);

	return sendObject;
}

/**
 * @brief Retrieves the protocol mode in use.
 *
 * @return NetMode::TCP_RAW.
 */
NetMode::ProtocolMode NetModeTcpRaw::GetProtocolMode() const
{
	return NetMode::TCP_RAW;
}