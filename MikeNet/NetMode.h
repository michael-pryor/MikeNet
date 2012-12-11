#pragma once

#include "NetInstance.h"
#include "NetSocketSimple.h"
#include "NetSocket.h"
/**
 * @brief Base class for net modes such as NetModeUdp and NetModeTcp.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * Makes up the skeleton of a net mode, providing virtual methods that are common to all modes.
 * NetMode objects describe modifications made to data when sent and received, and how they are transferred to the application.
 */
class NetMode
{
public:
	/** @brief Describes the protocol mode in use. */
	enum ProtocolMode
	{
		/**
		 * A prefix of type size_t is automatically added to the start of all packets being sent.
		 * Packets being received are expected to have this prefix and problems will occur if they don't.
		 * The prefix indicates the amount of additional data (not including the prefix) that is in the packet.
		 * The prefix is not included as part of received packets.
		 * e.g. If the prefix of size_t is of value 10, then the actual packet's size is 10 + 8 which is 18. \n\n
		 *
		 * Packets received are put into a queue and none are discarded.\n\n
		 *
		 * Value of 1.
		 */
		TCP_PREFIX_SIZE = 1,
		
		/**
		 * A postfix which can consist of any data is automatically appended to the end of all packets being sent.
		 * Packets being received are expected to have this postfix and problems will occur if they don't.
		 * The postfix indicates the end of the packet and is not included as part of received packets.\n\n
		 *
		 * Packets received are put into a queue and none are discarded.\n\n
		 *
		 * Value of 2.
		 */
		TCP_POSTFIX = 2,

		/**
		 * TCP data is not manipulated at all. Data is received unmodified from the protocol directly.\n\n
		 *
		 * Value of 3.
		 */
		TCP_RAW = 3,

		/**
		 * A prefix of clock() is automatically added to the start of all packets being sent.
		 * Packets being received are expected to have this prefix and problems will occur if they don't.
		 * The prefix is used to determine the age of the packet. clock() increments every millisecond and
		 * therefore the higher the prefix the newer the packet. A record is kept of the newest packet received
		 * and any packets with a prefix lower than that are discarded as they are deemed out of order.
		 * Received packets will not have this prefix. \n\n
		 *
		 * In addition to this, the server (and only the server) must manually add the following prefix to the start of each packet:
		 * - size_t: Indicates the client that the packet is referring to. On the receiving end, the packet will
		 * then appear in that client's packet store without this prefix.\n\n
		 *
		 * On the client and server side there is a packet store for each client which stores the newest
		 * packet received from that client (in the case of server state) or referring to that client (in the case of client state).
		 * Each store contains only one packet, reducing the overhead of a queue.\n\n
		 * 
		 * This UDP mode is good where data can be organized into a per client basis and can be more efficient than. 
		 * UDP_CATCH_ALL or UDP_CATCH_ALL_NO due to there being no queue. Note that some in order packets may be discarded since there is no queue.\n\n
		 *
		 * Value of 1.
		 */
		UDP_PER_CLIENT = 1,

		/**
		 * A prefix of clock() is automatically added to the start of all packets being sent.
		 * Packets being received are expected to have this prefix and problems will occur if they don't.
		 * The prefix is used to determine the age of the packet. clock() increments every millisecond and
		 * therefore the higher the prefix the newer the packet. A record is kept of the newest packet received
		 * and any packets with a prefix lower than that are discarded as they are deemed out of order.
		 * Received packets will not have this prefix. \n\n
		 *
		 * In addition to this, the server (and only the server) must manually add the following prefix to the start of each packet in the below order:
		 * - size_t: Client ID indicating the client that the packet is referring to. On the receiving end, the packet will
		 * then appear in that client's packet store without this part of the prefix.
		 * - size_t: Operation ID indicating the operation that the packet applies to. On the receiving end, the packet will then
		 * appear in the packet store for that operation without this part of the prefix.\n\n
		 *
		 * The client (and only the client) must manually add the following prefix to the start of each packet in the below order:
		 * - size_t: Operation ID indicating the operation that the packet applies to. On the receiving end, the packet will then
		 * appear in the packet store for that operation without this prefix.\n\n
		 *
		 * On the client and server side there is a packet store for each client and each operation which stores the newest
		 * packet received from that client for that operation (in the case of server state) or referring to that client for that operation (in the case of client state).
		 * Each store contains only one packet, reducing the overhead of a queue. The packet store can be thought of as a 2D array which looks like: packetStore[maxClients][numOperations].\n\n
		 * 
		 * This UDP mode is good where data can be organized into a per client, per operation basis and can be more efficient than. 
		 * UDP_CATCH_ALL or UDP_CATCH_ALL_NO due to there being no queue. Note that some in order packets may be discarded since there is no queue.\n\n
		 *
		 * Value of 2.
		 */
		UDP_PER_CLIENT_PER_OPERATION = 2,

		/**
		 * The packet is sent unmodified and packets received are transfered straight to the application.
		 * This is unmodified UDP transfer and so out of order packets may be received.
		 * In addition this connection packets may be received in server state, as the client
		 * blasts UDP packets to the server which may arrive late, after the connection has completed.
		 * These packets always have a prefix of size_t of value 0. You can use this to single them
		 * out and discard them. For more information see the @ref handshakePage "server/client handshaking process".\n\n
		 *
		 * Value of 3.
		 */
		UDP_CATCH_ALL = 3,

		/**
		 * A prefix of size_t is added to the start of all packets being sent.
		 * Packets being received are expected to have this prefix and problems will occur if they don't.
		 * The prefix is a counter which increments by 1 with every send operation. 
		 * Therefore the higher the prefix the newer the packet. A record is kept of the newest packet received
		 * and any packets with a prefix lower than that are discarded as they are deemed out of order.
		 * Received packets will not have this prefix. \n\n
		 *
		 * Received packets are put into a queue and no in order packets are discarded.\n\n
		 *
		 * Value of 4.
		 */
		UDP_CATCH_ALL_NO = 4
	};

	static ProtocolMode ConvertToProtocolModeTCP(int mode);
	static ProtocolMode ConvertToProtocolModeUDP(int mode);
	static bool ValidateProtocolModeTCP(ProtocolMode mode);
	static bool ValidateProtocolModeUDP(ProtocolMode mode);

	/**
	 * @brief Generates a NetSend object based on protocol mode specific criteria. 
	 *
	 * @param packet Packet to send.
	 * @param block True if sending should be synchronous, false if sending should be asynchronous.
	 *
	 * @return a send object formatted for the specific protocol and mode.
	 */
	virtual NetSend * GetSendObject(const Packet * packet, bool block)=0;

	/**
	 * @brief Retrieves the protocol mode in use.
	 *
	 * @return the protocol mode in use, describing how data is manipulated during sending and receiving.
	 */
	virtual ProtocolMode GetProtocolMode() const=0;

	/**
	 * @brief Determines the number of packets in the specified packet store. 
	 *
	 * @param clientID ID of client to use, may be ignored.
	 * @param operationID Operation ID of operation to use, may be ignored.
	 *
	 * @return the number of packets in the packet store.
	 */
	virtual size_t GetPacketAmount(size_t clientID, size_t operationID) const = 0;

	/**
	 * @brief Retrieves a packet from the specified packet store.
	 *
	 * @param [out] destination Destination to copy packet into.
	 * @param clientID ID of client to use, may be ignored.
	 * @param operationID ID of operation to use, may be ignored.
	 *
	 * @return the number of packets in packet queue before this method was called.
	 */
	virtual size_t GetPacketFromStore(Packet * destination, size_t clientID, size_t operationID) = 0;

	/**
	 * @brief Deals with a complete packet.
	 *
	 * The class deals with it in one of two ways:
	 * - Passes it to a user function specified by @a udpRecvFunc parameter.
	 * - If no user function is defined then it is dealt with by the object.
	 *
	 * @param [in] completePacket The complete packet that should be dealt with. The packet is now owned by this object and should not be referenced elsewhere.
	 * @param [in] udpRecvFunc Method will be executed and data not added to the queue if this is non NULL.
	 */
	virtual void PacketDone(Packet * completePacket, NetSocket::RecvFunc udpRecvFunc)=0;

	/**
	 * @brief Deals with newly received data.
	 *
	 * @param buffer Newly received data.
	 * @param completionBytes Number of bytes of new data stored in @a buffer.
	 * @param [in] recvFunc Method will be executed and data not added to the queue if this is non NULL.
	 * @param instanceID Instance that data was received on.
	 * @param clientID ID of client that data was received from, set to 0 if not applicable.
	 */
	virtual void DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc recvFunc, size_t clientID, size_t instanceID)=0;

	static bool TestClass();
};