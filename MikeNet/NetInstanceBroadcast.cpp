#include "FullInclude.h"
class NetSocketUDP;

/**
 * @brief	Initializes this object. 
 *
 * @param p_broadcastAddress Address to broadcast to, IP should be either local subnet or global broadcast address (255.255.255.255).
 * If NULL then instance will not connect to an address.
 * @param recvEnabled If true then the instance will be able to receive packets on the interface which it is bound to.
 * @param recvMemoryLimit Maximum amount of memory that can be allocated to receive operations (default = NetInstanceProfile::DEFAULT_RECV_MEMORY_LIMIT). See @ref securityPage "security" for more information.
 * @param sendMemoryLimit Maximum amount of memory that can be allocated to broadcast send operations (default = NetInstanceProfile::DEFAULT_SEND_MEMORY_LIMIT). See @ref securityPage "security" for more information.
 */
void NetInstanceBroadcast::Initialize(const NetAddress * p_broadcastAddress, bool recvEnabled, size_t recvMemoryLimit, size_t sendMemoryLimit)
{
	ExceptionValidateRecvSizeUDP();

	socketUDP->SetSendMemoryLimit(sendMemoryLimit);
	socketUDP->SetRecvMemoryLimit(recvMemoryLimit,NULL);

	if(p_broadcastAddress != NULL)
	{
		socketUDP->Connect(*p_broadcastAddress);
	}

	if(recvEnabled)
	{
		DoRecv(socketUDP);
	}
}

/**
 * @brief Constructor.
 *
 * @param p_broadcastAddress Address to broadcast to, IP should be either local subnet or global broadcast address (255.255.255.255).
 * If NULL then instance will not connect to an address.
 * @param recvEnabled If true then the instance will be able to receive packets on the interface which it is bound to.
 * @param [in] p_socketUDP Socket to use to send and receive UDP data, should be constructed for broadcasting. This pointer and its data is now owned by
 * this object and should not be used elsewhere.
 * @param p_sendTimeout Length of time in milliseconds to wait for a send operation to complete before disconnecting client (optional, default INFINITE).
 * @param p_instanceID	ID of instance within instance group.
 */
NetInstanceBroadcast::NetInstanceBroadcast(const NetAddress * p_broadcastAddress, bool recvEnabled, NetSocketUDP * p_socketUDP, unsigned int p_sendTimeout, size_t p_instanceID)  :
		NetInstance(p_instanceID,NetInstance::BROADCAST,p_sendTimeout),
		NetInstanceUDP(p_socketUDP),
		recvFailCounterUDP(Counter::DEFAULT_TIMEOUT, Counter::DEFAULT_LIMIT)
{
	_ErrorException((p_socketUDP == NULL),"initializing a broadcast instance, socketUDP parameter must not be NULL",0,__LINE__,__FILE__);
	this->Initialize(p_broadcastAddress,recvEnabled);
}

/**
 * @brief Constructor, using profile instead of individual parameters.
 *
 * @param p_broadcastAddress Address to broadcast to, IP should be either local subnet or global broadcast address (255.255.255.255).
 * If NULL then instance will not connect to an address.
 * @param recvEnabled If true then the instance will be able to receive packets on the interface which it is bound to.
 * @param p_profile Instance profile containing parameters.
 * @param p_instanceID Unique identifier for instance (optional, default 0).
 */
NetInstanceBroadcast::NetInstanceBroadcast(const NetAddress * p_broadcastAddress, bool recvEnabled, const NetInstanceProfile & p_profile, size_t p_instanceID) : 
		NetInstance(p_instanceID,NetInstance::BROADCAST,p_profile.GetSendTimeout()),
	
		// Do you like my attempt at making this pretty?
		NetInstanceUDP
		(
			static_cast<NetSocketUDP*>
			(
				Utility::DynamicAllocCheck
				(
					new (nothrow) NetSocketUDP
					(
						p_profile.GetRecvSizeUDP(),
						p_profile.GetLocalAddrUDP(),
						p_profile.GetRecvFuncUDP()
					)
				,__LINE__,__FILE__)
			)
		),
		recvFailCounterUDP(Counter::DEFAULT_TIMEOUT, Counter::DEFAULT_LIMIT)
{
	this->Initialize(p_broadcastAddress,recvEnabled);
}

/**
 * @brief Destructor.
 *
 * CloseSockets must be used in the destructor.
 */
NetInstanceBroadcast::~NetInstanceBroadcast()
{
	CloseSockets();
}

/**
 * @brief Retrieves the smallest acceptable packet size that can be received.
 *
 * @return the smallest acceptable packet size that can be received.
 */
size_t NetInstanceBroadcast::GetRecvSizeMinUDP() const
{
	return recvSizeMinUDP;
}

/**
 * @brief	Closes any sockets in use by the instance.
 *
 * Must be called in the destructor so that instance is not cleaned up until completion
 * port is done using it.
 */
void NetInstanceBroadcast::CloseSockets()
{
	NetInstanceUDP::CloseSockets();
}

/**
 * @brief Deals with errors.
 *
 * @param clientID Ignored (optional, default = 0).
 * @exception ErrorReport Always.
 */
void NetInstanceBroadcast::ErrorOccurred(size_t clientID)
{
	_ErrorException(true,"performing a broadcast instance related function",WSAGetLastError(),__LINE__,__FILE__);
}

/**
 * @brief Starts a receive operation via UDP and deals with errors appropriately.
 *
 * Calls NetSocketUDP::Recv. Use this INSTEAD OF socketUDP->Recv, never use socketUDP->Recv directly.
 *
 * @param socket [in,out] Pointer to socket that receive operation should be started on.
 * @param clientID Ignored (optional, default = 0).
 */
void NetInstanceBroadcast::DoRecv(NetSocket * socket, size_t clientID)
{
	_ErrorException((socket != socketUDP),"starting a broadcasting receive operation, invalid socket pointer",0,__LINE__,__FILE__);

RetryRecv:
	bool error = socketUDP->Recv();

	if(error == true)
	{
		if(recvFailCounterUDP.Increment() == false)
		{
			goto RetryRecv;
		}
		else
		{
			ErrorOccurred();
		}
	}
}

/** 
 * @brief Sends a broadcast packet.
 *
 * @param packet %Packet to send.
 * @param block If true the method will not return until @a packet is completely sent, note that this does not indicate that
 * the packet has been received by the recipient, instead it simply means the packet is in transit. \n
 * If false the method will return instantly even if the packet has not been sent.
 * @param clientID Ignored (default 0).
 *
 * @return NetUtility::SEND_COMPLETED If the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS If the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED If the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL If the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus NetInstanceBroadcast::SendUDP(const Packet & packet, bool block, size_t clientID)
{
	return socketUDP->Send(packet,block,NULL,GetSendTimeout());
}

/**
 * @brief Sends a broadcast packet on the specified subnet.
 *
 * @param address Subnet to send to.
 * @param packet %Packet to send.
 * @param block If true the method will not return until @a packet is completely sent to all clients, note that this does not indicate that
 * the packet has been received by all clients, instead it simply means the packet is in transit. \n
 * If false the method will return instantly even if the packet has not been sent.
 *
 * @return NetUtility::SEND_COMPLETED If the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS If the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED If the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL If the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus NetInstanceBroadcast::SendToUDP(const NetAddress & address, const Packet & packet, bool block)
{
	return socketUDP->Send(packet,block,&address,GetSendTimeout());
}

/**
 * @brief When send and receive operations are completed on this instance, this method is called.
 * 
 * @param socket [in,out] Socket that operation was started by.
 * @param bytes Number of bytes of data transferred in operation.
 * @param clientID Ignored (optional, default = 0).
 */
void NetInstanceBroadcast::DealCompletion(NetSocket * socket, DWORD bytes, size_t clientID)
{
	try
	{
		// Deal with received data
		socket->DealWithData(socket->recvBuffer,bytes,socket->GetRecvFunction(),0,this->GetInstanceID());
	}
	// Disconnect client in the event of an error
	catch(ErrorReport & error){	ErrorOccurred(0); }
	catch(...){ ErrorOccurred(0);}
}

/**
 * @brief	Called by the completion port when an error occurred during an operation.
 *
 * @param [in,out]	completionSocket	The socket responsible for the error. 
 * @param	clientID					The client ID responsible for the error (optional, default = 0).
 */
void NetInstanceBroadcast::CompletionError(NetSocket * completionSocket, size_t clientID)
{
	// UDP
	if(completionSocket == socketUDP)
	{
		ErrorOccurred();
	}
	else
	{
		_ErrorException(true,"dealing with a failed completion port operation, invalid socket pointer",0,__LINE__,__FILE__);
	}
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetInstanceBroadcast::TestClass()
{
	cout << "Testing NetInstanceBroadcast class...\n";
	bool problem = false;

	NetUtility::LoadEverything(2,0);

	NetAddress broadcastAddress("255.255.255.255",6000);
	NetAddress localAddrSend;
	NetInstanceBroadcast broadcastSend(&broadcastAddress,true,new NetSocketUDP(1024,localAddrSend));

	NetAddress localAddrRecv(NULL,6000);
	NetInstanceBroadcast broadcastRecv(NULL,true,new NetSocketUDP(1024,localAddrRecv));
	
	Packet recvPacket;
	Timer whileLoopDuration(10000);
	Timer sendFreq(20);
	while(whileLoopDuration.GetState() != true)
	{
		if(sendFreq.GetState() == true)
		{
			Packet sendPacket;
			sendPacket.AddClockT(clock());
			sendPacket.AddStringC("HELLO I AM BROADCASTING!!",0,true);
			broadcastSend.SendUDP(sendPacket,false);
		}

		if(broadcastRecv.GetPacketFromStoreUDP(&recvPacket) > 0)
		{
			clock_t clock = recvPacket.GetClockT();
			char * str = recvPacket.GetStringC(0,true);
			cout << "Broadcast packet received: " << clock << ", " << str << '\n';
			delete[] str;
		}
	}
	
	NetUtility::UnloadEverything();

	cout << "\n\n";
	return !problem;
}
