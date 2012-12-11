#include "FullInclude.h"
#include "mnCommands.h"
#include "mnDBPWrapper.h"
#include <iostream>
using namespace std;

/**
 * @brief Starts up the networking module.
 *
 * If the module is already active then the module will be restarted.
 *
 * @param numInstances Number of instances to be created but not setup.
 * @param numThreads Number of threads to be created to manage data being received.
 * @return 0 if no error occurred.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::Start(size_t numInstances, size_t numThreads)
{
	int returnMe = 0;
	const char * cCommand = "mn::Start";

	try
	{
		NetUtility::LoadEverything(numThreads,numInstances);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Cleans up an instance, or the entire networking module.
 *
 * Shuts down the networking module if @a instanceID is -1. \n
 * Shuts down the instance of ID @a instanceID if @a instanceID is > -1.
 *
 * If the module or instance is not active then this command will do nothing.
 * @param instanceID ID of instance to shutdown, or -1 if the entire networking module should be unloaded.
 * @return 0 if no error occurred.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::Finish(size_t instanceID)
{
	int returnMe = 0;
	const char * cCommand = "mn::Finish";

	try
	{
		// Entire module shutdown
		if(instanceID == -1)
		{
			NetUtility::UnloadEverything();
		}
		// Single instance shutdown
		else
		{
			NetUtility::GetInstanceGroup().Finish(instanceID);
		}
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Loads a client instance into the specified instance ID.
 *
 * The client instance attempts to connect to the specified address.
 * Once the connection process is complete the instance is fully active.
 * Any instance already loaded into the specified instance ID will be deactivated and overwritten.
 *
 * @param	instanceID			Instance ID that new instance should be stored in.
 * @param	connectAddrTCP		Address to connect to via TCP.
 * @param	connectAddrUDP		Address to connect to via UDP. Can be NULL if UDP is disabled.
 * @param	timeoutMilliseconds	Length of time in milliseconds to wait before giving up on the connection attempt.
 * @param	block				If true this command will block until @a timeoutMilliseconds has expired or the connection process has completed. \n
 * If false this command will return instantly and mn::PollConnect should be used on the instance to poll on the connection process and determine its success/failure.
 * @param	profile				Instance profile containing additional instance settings.
 *
 * @return	if block is true: \n
 * @return NetUtility::CONNECTED if the handshaking process completed successfully and the client is now fully connected.
 * @return NetUtility::TIMED_OUT if the handshaking process timed out.
 * @return NetUtility::CONNECTION_ERROR if an error occurred.
 * @return NetUtility::REFUSED if the server is full and so the connection attempt failed.\n\n

 * @return if @a block is false:\n
 * @return NetUtility::STILL_CONNECTING indicating that the connection process has begun 
 * and mn::PollConnect should be used to check its status.
 * @return NetUtility::CONNECTION_ERROR if an error occurred.
 */
NetUtility::ConnectionStatus mn::Connect(size_t instanceID, const NetAddress * connectAddrTCP, const NetAddress * connectAddrUDP, size_t timeoutMilliseconds, bool block, const NetInstanceProfile & profile)
{
	NetUtility::ConnectionStatus returnMe = NetUtility::CONNECTION_ERROR;
	const char * cCommand = "mn::Connect";

	try
	{
		// Create instance and add it to group
		NetInstanceClient * newInstance = new (nothrow) NetInstanceClient(profile);
		Utility::DynamicAllocCheck(newInstance,__LINE__,__FILE__);

		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group.AddInstance(instanceID,newInstance);

		// Connect instance
		returnMe = newInstance->Connect(connectAddrTCP,connectAddrUDP,timeoutMilliseconds,block);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Loads a client instance into the specified instance ID.
 *
 * The client instance attempts to connect to the specified address.
 * Once the connection process is complete the instance is fully active.
 * Any instance already loaded into the specified instance ID will be deactivated and overwritten.
 *
 * @param	instanceID			Instance ID that new instance should be stored in.
 * @param	connectIP_TCP		IP address to connect to via TCP.
 * @param	connectPort_TCP		Port to connect to via TCP.
 * @param	connectIP_UDP		IP address to connect to via UDP.
 * @param	connectPort_UDP		Port to connect to via UDP.
 * @param	timeoutMilliseconds	Length of time in milliseconds to wait before giving up on the connection attempt.
 * @param	block				If true this command will block until @a timeoutMilliseconds has expired or the connection process has completed. \n
 * If false this command will return instantly and mn::PollConnect should be used on the instance to poll on the connection process and determine its success/failure.
 * @param	profile				Pointer (cast to INT_PTR) to instance profile containing additional instance settings.
 *
 * @return	if block is true: \n
 * @return NetUtility::CONNECTED if the handshaking process completed successfully and the client is now fully connected.
 * @return NetUtility::TIMED_OUT if the handshaking process timed out.
 * @return NetUtility::CONNECTION_ERROR if an error occurred.
 * @return NetUtility::REFUSED if the server is full and so the connection attempt failed.\n\n

 * @return if @a block is false:\n
 * @return NetUtility::STILL_CONNECTING indicating that the connection process has begun 
 * and mn::PollConnect should be used to check its status.
 * @return NetUtility::CONNECTION_ERROR if an error occurred.
 */
DBP_CPP_DLL NetUtility::ConnectionStatus mn::Connect(size_t instanceID, const char * connectIP_TCP, unsigned short connectPort_TCP, const char * connectIP_UDP, unsigned short connectPort_UDP, size_t timeoutMilliseconds, bool block, INT_PTR profile)
{
	NetUtility::ConnectionStatus returnMe = NetUtility::CONNECTION_ERROR;
	const char * cCommand = "mn::Connect";

	try
	{
		NetAddress connectAddrTCP(connectIP_TCP,connectPort_TCP);
		NetAddress connectAddrUDP(connectIP_UDP,connectPort_UDP);

		returnMe = mn::Connect(instanceID,&connectAddrTCP,&connectAddrUDP,timeoutMilliseconds,block,PointerConverter::GetRefFromInt<NetInstanceProfile>(profile));
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Loads a server instance into the specified instance ID.
 *
 * The instance is fully active and clients can connect after this command has been called.
 * Any instance already loaded into the specified instance ID will be deactivated and overwritten.
 *
 * @param instanceID 	Unique identifier for instance.
 * @param maxClients 	Maximum number of clients that can be connected to server at any one time.
 * @param profile 		Instance profile containing additional instance settings.
 * @return 0 if the server was started successfully.
 * @return -1 if the server failed to start.
 */
int mn::StartServer(size_t instanceID, size_t maxClients, const NetInstanceProfile & profile)
{
	int returnMe = 0;
	const char * cCommand = "mn::StartServer";

	try
	{
		// Create instance and add it to group
		NetInstance * newInstance = new (nothrow) NetInstanceServer(maxClients,profile,instanceID);
		Utility::DynamicAllocCheck(newInstance,__LINE__,__FILE__);

		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group.AddInstance(instanceID,newInstance);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Loads a server instance into the specified instance ID.
 *
 * @copydetails mn::StartServer(size_t, size_t, const NetInstanceProfile &)
 */
DBP_CPP_DLL int mn::StartServer(size_t instanceID, size_t maxClients, INT_PTR profile)
{
	int returnMe = -1;
	const char * cCommand = "mn::StartServer";

	try
	{
		returnMe = mn::StartServer(instanceID,maxClients,PointerConverter::GetRefFromInt<NetInstanceProfile>(profile));
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Loads a broadcasting instance into the specified instance ID. 
 *
 * This instance is capable of communicating with entities on a common local area network without first forming
 * a direct connection. Messages sent to the router are broadcasted to all other entities connected to
 * that router using this instance. This is useful for e.g. Detecting a server on a network, and then later making a direct connection to that server.
 * Due to the simplicity of this instance, it is possible to use it with applications not based on this module.
 *
 * If you are receiving broadcast packets then using the instance profile you should specify the local IP that
 * is on the LAN that you expect to receive broadcast packets from. The local port should be the port that the
 * broadcaster is broadcasting to.\n\n
 *
 * Use mn::RecvUDP to receive data if receiving is enabled.
 * Use mn::SendUDP to send broadcast packets if sending is enabled.\n\n
 *
 * Any instance already loaded into the specified instance ID will be deactivated and overwritten.
 *
 * @param	instanceID		Unique identifier for instance.
 * @param	connectAddress	Address to broadcast to. Set the IP to 255.255.255.255 to broadcast to all entities connected
 * to the local area network, or specify a subnet to broadcast to. Set the port to the port that entities should listen
 * for broadcast packets on.
 * @param	sendEnabled		True if this instance should be capable of sending broadcast packets.
 * @param	recvEnabled		True if this instance should be capable of receiving broadcast packets.
 * @param	profile			Instance profile containing additional instance settings.
 *
 * @return	0 if the instance was started successfully.
 * @return -1 if the instance failed to start.
 */
int mn::StartBroadcast(size_t instanceID, const NetAddress & connectAddress, bool sendEnabled, bool recvEnabled, const NetInstanceProfile & profile)
{
	int returnMe = 0;
	const char * cCommand = "mn::StartBroadcast";

	try
	{
		// Create instance and add it to group
		const NetAddress * ptrConnectAddress;
		if(sendEnabled)
		{
			ptrConnectAddress = &connectAddress;
		}
		else
		{
			ptrConnectAddress = NULL;
		}

		NetInstance * newInstance = new (nothrow) NetInstanceBroadcast(ptrConnectAddress,recvEnabled,profile,instanceID);
		Utility::DynamicAllocCheck(newInstance,__LINE__,__FILE__);

		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group.AddInstance(instanceID,newInstance);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Loads a broadcasting instance into the specified instance ID.
 *
 * This instance is capable of communicating with entities on a common local area network without first forming
 * a direct connection. Messages sent to the router are broadcasted to all other entities connected to
 * that router using this instance. This is useful for e.g. Detecting a server on a network, and then later making a direct connection to that server.
 * Due to the simplicity of this instance, it is possible to use it with applications not based on this module.
 *
 * If you are receiving broadcast packets then using the instance profile you should specify the local IP that
 * is on the LAN that you expect to receive broadcast packets from. The local port should be the port that the
 * broadcaster is broadcasting to.\n\n
 *
 * Use mn::RecvUDP to receive data if receiving is enabled.
 * Use mn::SendUDP to send broadcast packets if sending is enabled.\n\n
 *
 * Any instance already loaded into the specified instance ID will be deactivated and overwritten.
 *
 * @param	instanceID		Unique identifier for instance.
 * @param	connectAddressIP	Address to broadcast to. Set this to 255.255.255.255 to broadcast to all entities connected
 * to the local area network, or specify a subnet to broadcast to.
 * @param	connectAddressPort	Set this to the port that entities should listen.
 * for broadcast packets on.
 * @param	sendEnabled		True if this instance should be capable of sending broadcast packets.
 * @param	recvEnabled		True if this instance should be capable of receiving broadcast packets.
 * @param	profile			Pointer (cast to INT_PTR) to instance profile containing additional instance settings.
 *
 * @return	0 if the instance was started successfully.
 * @return -1 if the instance failed to start.
 */
DBP_CPP_DLL int mn::StartBroadcast(size_t instanceID, const char * connectAddressIP, unsigned short connectAddressPort, bool sendEnabled, bool recvEnabled, INT_PTR profile)
{
	int returnMe = -1;
	const char * cCommand = "mn::StartBroadcast";

	try
	{
		NetAddress connectAddress(connectAddressIP,connectAddressPort);
		returnMe = mn::StartBroadcast(instanceID,connectAddress,sendEnabled,recvEnabled,PointerConverter::GetRefFromInt<NetInstanceProfile>(profile));
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Sets the amount of time that clients are allowed to spend handshaking with the server
 * before being dropped.
 *
 * See @ref handshakeSecurityConnectionTimeout "server/client handshaking process connection timeout" for more information. \n\n
 *
 * From the moment that a client first communicates with the server, it is allowed @a timeoutMilliseconds
 * to complete the handshaking process before the process is aborted and the client is forcefully silently
 * disconnected. Note that mn::ClientLeft will not receive notification since mn::ClientJoined will not have. \n\n
 *
 * The default value is NetInstanceServer::DEFAULT_CONNECTION_TIMEOUT.\n\n
 *
 * This command can be used only on an active server instance.
 *
 * @param	instanceID			Unique identifier for instance.
 * @param	timeoutMilliseconds	Number of milliseconds that connection attempts should be allowed to complete before entity is forcefully disconnected.
 *
 * @return 0 if the command completed successfully.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::SetServerTimeout(size_t instanceID, size_t timeoutMilliseconds)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetServerTimeout";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group[instanceID].GetInstanceServer()->SetServerTimeout(timeoutMilliseconds);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Retrieves the amount of time that clients are allowed to spend handshaking with the server
 * before being dropped.
 * 
 * See @ref handshakeSecurityConnectionTimeout "server/client handshaking process connection timeout" for more information. \n\n
 *
 * From the moment that a client first communicates with the server, it is allowed this amount of time
 * to complete the handshaking process before the process is aborted and the client is forcefully silently
 * disconnected. Note that mn::ClientLeft will not receive notification since mn::ClientJoined will not have. \n\n
 *
 * The default value is NetInstanceServer::DEFAULT_CONNECTION_TIMEOUT.
 *
 * This command can be used only on an active server instance.
 *
 * @param	instanceID			Unique identifier for instance.
 *
 * @return number of milliseconds that connection attempts should be allowed to complete before entity is forcefully disconnected.
 * @return 0 if an error occurred.
 */
DBP_CPP_DLL size_t mn::GetServerTimeout(size_t instanceID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetServerTimeout";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceServer()->GetServerTimeout();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Sets the amount of time that clients are allowed to spend handshaking with the server
 * before being dropped.
 *
 * See @ref handshakeSecurityConnectionTimeout "server/client handshaking process connection timeout" for more information. \n\n
 *
 * From the moment that a client first communicates with the server, it is allowed @a timeoutMilliseconds
 * to complete the handshaking process before the process is aborted and the client is forcefully silently
 * disconnected. Note that mn::ClientLeft will not receive notification since mn::ClientJoined will not have. \n\n
 *
 * The default value is NetInstanceServer::DEFAULT_CONNECTION_TIMEOUT.\n\n
 *
 * @param	profile				Instance profile to use.
 * @param	timeoutMilliseconds	Number of milliseconds that connection attempts should be allowed to complete before entity is forcefully disconnected.
 *
 * @return 0 if the command completed successfully.
 * @return -1 if an error occurred.
 */
int mn::SetProfileServerTimeout(NetInstanceProfile & profile, size_t timeoutMilliseconds)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileServerTimeout";

	try
	{
		profile.SetConnectionToServerTimeout(timeoutMilliseconds);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Sets the amount of time that clients are allowed to spend handshaking with the server
 * before being dropped.
 *
 * @copydetails mn::SetProfileServerTimeout(NetInstanceProfile &, size_t)
 */
DBP_CPP_DLL int mn::SetProfileServerTimeout(INT_PTR profile, size_t timeoutMilliseconds)
{
	int returnMe = -1;
	const char * cCommand = "mn::SetProfileServerTimeout";

	try
	{
		NetInstanceProfile & aux = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::SetProfileServerTimeout(aux,timeoutMilliseconds);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the amount of time that clients are allowed to spend handshaking with the server
 * before being dropped.
 *
 * See @ref handshakeSecurityConnectionTimeout "server/client handshaking process connection timeout" for more information. \n\n
 *
 * From the moment that a client first communicates with the server, it is allowed this amount of time
 * to complete the handshaking process before the process is aborted and the client is forcefully silently
 * disconnected. Note that mn::ClientLeft will not receive notification since mn::ClientJoined will not have. \n\n
 *
 * The default value is NetInstanceServer::DEFAULT_CONNECTION_TIMEOUT.
 *
 * @param	profile				Instance profile to use.
 *
 * @return number of milliseconds that connection attempts should be allowed to complete before entity is forcefully disconnected.
 * @return 0 if an error occurred.
 */
size_t mn::GetProfileServerTimeout(const NetInstanceProfile & profile)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileServerTimeout";

	try
	{
		returnMe = profile.GetConnectionToServerTimeout();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the amount of time that clients are allowed to spend handshaking with the server
 * before being dropped.
 *
 * @copydetails mn::GetProfileServerTimeout(const NetInstanceProfile &)
 */
DBP_CPP_DLL size_t mn::GetProfileServerTimeout(INT_PTR profile)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileServerTimeout";

	try
	{
		NetInstanceProfile & aux = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::GetProfileServerTimeout(aux);
	}
	STD_CATCH

	return(returnMe);
}


/**
 * @brief Manages incoming connections from clients.
 *
 * For a client to connect, this command must have been called several times
 * during the handshaking process. It is best to call this continuously in 
 * a loop that runs this networking module. \n\n
 * 
 * Connection requests will be rejected if the maximum number of clients
 * has been reached.\n\n
 *
 * This command can be used only on an active server instance.\n\n
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @param	instanceID	Unique identifier for instance.
 * @return >0 if a client was accepted, this is the client ID that was assigned to that client.
 * @return  0 if no new clients were accepted in this call.
 */
DBP_CPP_DLL size_t mn::ClientJoined(size_t instanceID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::ClientJoined";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceServer()->ClientJoined();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Keeps track of who has disconnected recently.
 *
 * This command can be used only on an active server instance.
 *
 * @param	instanceID	Unique identifier for instance.
 * @return  >0 if a client has disconnected since the last call to this
 * method, this is the ID of that client.
 * @return  0 if an error occurred.
 */
DBP_CPP_DLL size_t mn::ClientLeft(size_t instanceID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::ClientLeft";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceServer()->GetDisconnect();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the remote TCP IP of a currently connected client.
 *
 * This command can be used only on an active server instance.
 *
 * @param	instanceID	Unique identifier for instance.
 * @param	clientID	ID of client.
 *
 * @return  the remote TCP IP of the specified client.
 * @return	an empty string if an error occurred.
 */
CPP_DLL const char * mn::GetClientIPTCP(size_t instanceID, size_t clientID)
{
	const char * cReturn = "";
	const char * cCommand = "mn::GetClientIPTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		cReturn = group[instanceID].GetInstanceServer()->GetConnectAddressTCP(clientID).GetIP();
	}
	STD_CATCH

	return(cReturn);
}

/**
 * @brief Retrieves the remote TCP port of a currently connected client.
 *
 * This command can be used only on an active server instance.
 *
 * @param	instanceID	Unique identifier for instance.
 * @param	clientID	ID of client.
 *
 * @return  the remote TCP port of the specified client.
 * @return	an empty string if an error occurred.
 */
DBP_CPP_DLL unsigned short mn::GetClientPortTCP(size_t instanceID, size_t clientID)
{
	unsigned short usReturn = 0;
	const char * cCommand = "mn::GetClientPortTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		usReturn = group[instanceID].GetInstanceServer()->GetConnectAddressTCP(clientID).GetPort();
	}
	STD_CATCH

	return(usReturn);
}

/**
 * @brief Retrieves the remote UDP IP of a currently connected client.
 *
 * This command can be used only on an active server instance.
 *
 * @param	instanceID	Unique identifier for instance.
 * @param	clientID	ID of client.
 *
 * @return  the remote UDP IP of the specified client.
 * @return	an empty string if an error occurred.
 */
CPP_DLL const char * mn::GetClientIPUDP(size_t instanceID, size_t clientID)
{
	const char * cReturn = "";
	const char * cCommand = "mn::GetClientIPUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		cReturn = group[instanceID].GetInstanceServer()->GetConnectAddressUDP(clientID).GetIP();
	}
	STD_CATCH

	return(cReturn);
}

/**
 * @brief Retrieves the remote UDP port of a currently connected client.
 *
 * This command can be used only on an active server instance.
 *
 * @param	instanceID	Unique identifier for instance.
 * @param	clientID	ID of client.
 *
 * @return  the remote UDP port of the specified client.
 * @return	an empty string if an error occurred.
 */
DBP_CPP_DLL unsigned short mn::GetClientPortUDP(size_t instanceID, size_t clientID)
{
	unsigned short usReturn = 0;
	const char * cCommand = "mn::GetClientPortUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		usReturn = group[instanceID].GetInstanceServer()->GetConnectAddressUDP(clientID).GetPort();
	}
	STD_CATCH

	return(usReturn);
}

/**
 * @brief Retrieves the client ID of the client that received the specified packet.
 * 
 * @param	packet	Pointer to packet.
 *
 * @return	the client ID of the client that sent us the specified packet.
 * @return  0 if the specified packet was not received from a client.
 */
DBP_CPP_DLL size_t mn::GetClientFrom(INT_PTR packet)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetClientFrom";

	try
	{
		Packet * ptr = PointerConverter::GetPtrFromInt<Packet>(packet);
		returnMe = ptr->GetClientFrom();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the remote TCP address of a currently connected client.
 *
 * This command can be used only on an active server instance.
 *
 * @param	instanceID	Unique identifier for instance.
 * @param	clientID	ID of client.
 *
 * @return  remote TCP address of the specified client.
 * @return	0 if an error occurred.
 */
NetAddress mn::GetClientAddressTCP(size_t instanceID, size_t clientID)
{
	const char * cCommand = "mn::GetClientAddressTCP";
	NetAddress returnMe;

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceServer()->GetConnectAddressTCP(clientID);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the remote UDP address of a currently connected client.
 *
 * This command can be used only on an active server instance.
 *
 * @param	instanceID	Unique identifier for instance.
 * @param	clientID	ID of client.
 *
 * @return  remote UDP address of the specified client.
 * @return	0 if an error occurred.
 */
NetAddress mn::GetClientAddressUDP(size_t instanceID, size_t clientID)
{
	NetAddress returnMe;
	const char * cCommand = "mn::GetClientAddressUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceServer()->GetConnectAddressUDP(clientID);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Begins the graceful disconnection process.
 *
 * After using this command all send commands will fail, receiving will still operate as normal however.
 * All data sent up till now will be received by recipient. \n
 *
 * See the @ref gracefulDisconnectPage "graceful disconnection page" for more information. \n\n
 *
 * Can only be used on an active TCP instance.
 *
 * @param	instanceID	Unique identifier for instance.
 * @param	clientID	ID of client.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 *
 * @throws ErrorReport If graceful disconnect is disabled.
 */
DBP_CPP_DLL int mn::ShutdownClient(size_t instanceID, size_t clientID)
{
	int returnMe = 0;
	const char * cCommand = "mn::ShutdownClient";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group[instanceID].GetInstanceTCP()->ShutdownTCP(clientID);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Disconnects the specified client.
 *
 * In server state the specified client is dropped and the client ID freed. \n
 * In other states the instance is deactivated and freed.
 *
 * @param	instanceID	Unique identifier for instance.
 * @param	clientID	ID of client.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::DisconnectClient(size_t instanceID, size_t clientID)
{
	int returnMe = 0;
	const char * cCommand = "mn::DisconnectClient";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();

		switch(group[instanceID].GetInstanceCore()->GetState())
		{
		case(NetInstance::SERVER):
			group[instanceID].GetInstanceServer()->DisconnectClient(clientID);
			break;

		default:
			group[instanceID].KillInstance();
			break;
		}
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Determines if the specified client is connected, and what stage of the graceful disconnection
 * process it is at.
 *
 * In server state this command is used to check if the specified client ID is in use. \n
 * In client state this command is used to check that the client is still connected to the server (@a clientID is ignored). \n
 * In other states this command returns NetUtility::CONNECTED indicating that the instance is active
 * or NetUtility::NOT_CONNECTED indicating that the instance is inactive.
 *
 * @param	instanceID	Unique identifier for instance.
 * @param	clientID	ID of client.
 *
 * @return NetUtility::CONNECTED: Fully connected/operational.
 * @return NetUtility::NOT_CONNECTED: Not connected at all.
 * @return NetUtility::NO_RECV: Connected but new data cannot be received because @c Shutdown has been used by recipient (only possible if graceful disconnect is enabled).
 * @return NetUtility::NO_SEND: Connected but data cannot be sent because @c Shutdown has been used (only possible if graceful disconnect is enabled).
 * @return NetUtility::NO_SEND_RECV: Connected but data cannot be sent or received because @c Shutdown has been used on both sides,
 * only data that has already been received and is in the packet queue can be received (only possible if graceful disconnect is enabled).
 * @return NetUtility::CONNECTION_ERROR: If an error occurred whilst connecting or whilst executing this command.
 */
DBP_CPP_DLL NetUtility::ConnectionStatus mn::ClientConnected(size_t instanceID, size_t clientID)
{
	NetUtility::ConnectionStatus returnMe = NetUtility::CONNECTION_ERROR;
	const char * cCommand = "mn::ClientConnected";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();

		if(group[instanceID].IsInstanceLoaded() == false)
		{
			return NetUtility::NOT_CONNECTED;
		}

		switch(group[instanceID].GetInstanceCore()->GetState())
		{
		case(NetInstance::SERVER):
			returnMe = group[instanceID].GetInstanceServer()->ClientConnected(clientID);
			break;

		case(NetInstance::CLIENT):
			returnMe = group[instanceID].GetInstanceClient()->ClientConnected();

			if(group[instanceID].DealWithDestroyRequest() == true)
			{
				returnMe = NetUtility::NOT_CONNECTED;
			}
			break;

		case(NetInstance::BROADCAST):
			return NetUtility::CONNECTED;
			break;

		default:
			_ErrorException(true,"determining if client is connected or instance is active, invalid state",0,__LINE__,__FILE__);
			break;
		}
	}
	STD_CATCH

	return(returnMe);
}



/**
 * @brief Modifies the specified instance profile, loading it with buffer sizes.
 *
 * If these buffer sizes are too small, an error will not be thrown until an attempt is made to activate
 * an instance. This is because what is defined as 'too small' differs between instance types.
 *
 * @param [out]	profile			Instance profile to be modified.
 * @param		recvSizeTCP		Maximum size a received TCP packet can be without increasing the buffer size or throwing an error (default is NetInstanceProfile::DEFAULT_BUFFER_SIZE).
 * @param 		recvSizeUDP		Maximum size a received UDP packet can be without throwing an error (default is NetInstanceProfile::DEFAULT_BUFFER_SIZE).
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
int mn::SetProfileBufferSizes(NetInstanceProfile & profile, size_t recvSizeTCP, size_t recvSizeUDP)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileBufferSizes";

	try
	{
		profile.SetRecvSizeUDP(recvSizeUDP);
		profile.SetWsaRecvSizeTCP(recvSizeTCP / 2);
		profile.SetRecvSizeTCP(recvSizeTCP);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Modifies the specified instance profile, loading it with buffer sizes.
 *
 * @copydetails mn::SetProfileBufferSizes(NetInstanceProfile &, size_t, size_t)
 */
DBP_CPP_DLL int mn::SetProfileBufferSizes(INT_PTR profile, size_t recvSizeTCP, size_t recvSizeUDP)
{
	int returnMe = -1;
	const char * cCommand = "mn::SetProfileBufferSizes";

	try
	{
		returnMe = mn::SetProfileBufferSizes(PointerConverter::GetRefFromInt<NetInstanceProfile>(profile),recvSizeUDP,recvSizeTCP);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Disables or enables UDP.
 *
 * When UDP is disabled all UDP commands will fail. The handshaking process will be slightly
 * faster and compatibility with other applications will improve.
 *
 * @param [out]	profile		Instance profile to be modified.
 * @param		option		If true UDP will be enabled, if false UDP will be disabled.
 * Default is NetInstanceProfile::DEFAULT_ENABLED_UDP.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
int mn::SetProfileEnabledUDP(NetInstanceProfile & profile, bool option)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileEnabledUDP";

	try
	{
		profile.SetEnabledUDP(option);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Disables or enables UDP.
 *
 * @copydetails mn::SetProfileEnabledUDP(NetInstanceProfile &, bool)
 */
DBP_CPP_DLL int mn::SetProfileEnabledUDP(INT_PTR profile, bool option)
{
	int returnMe = -1;
	const char * cCommand = "mn::SetProfileEnabledUDP";

	try
	{
		returnMe = mn::SetProfileEnabledUDP(PointerConverter::GetRefFromInt<NetInstanceProfile>(profile),option);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Empties the TCP received packet queue.
 *
 * When TCP packets are received and a receive function has not been set using mn::SetFunction,
 * packets are put into a queue and received using mn::RecvTCP. If the queue gets too big
 * newer packets may not be received for some time and so it can be useful to empty the queue.
 * This command discards everything in the queue.\n\n
 *
 * Can only be used on an active TCP instance.
 *
 * @param	instanceID	Unique identifier for instance.
 * @param	clientID	ID of client.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::FlushRecvTCP(size_t instanceID, size_t clientID)
{
	int returnMe = 0;
	const char * cCommand = "mn::FlushRecvTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group[instanceID].GetInstanceTCP()->FlushRecvTCP(clientID);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Empties the UDP received packet queue.
 *
 * When UDP packets are received and a receive function has not been set using mn::SetFunction,
 * packets are put into a queue and received using mn::RecvUDP. If the queue gets too big
 * newer packets may not be received for some time and so it can be useful to empty the queue.
 * This command discards everything in the queue.\n\n
 *
 * Can only be used on an active UDP instance. Has no impact unless in UDP mode CATCH_ALL or CATCH_ALL_NO.
 *
 * @param	instanceID	Unique identifier for instance.
 * @param	clientID	ID of client.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::FlushRecvUDP(size_t instanceID, size_t clientID)
{
	int returnMe = 0;
	const char * cCommand = "mn::FlushRecvUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group[instanceID].GetInstanceUDP()->FlushRecvUDP(clientID);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Changes the size of the largest TCP packet that can be received;
 * packets larger than this will require an increase in memory size or an error will be thrown.
 *
 * @note Attempting to decrease the size may not be effective. If data exists in the buffer this will not be discarded.
 * The buffer will decrease as much as possible without discarding data. If an attempt is made to decrease the
 * buffer size below the minimum for that instance then an error will occur. \n\n
 *
 * Can only be used on an active TCP instance. 
 *
 * @param	instanceID	Unique identifier for instance.
 * @param	clientID	ID of client.
 * @param   newSize		New buffer size.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::ChangeBufferSizeTCP(size_t instanceID, size_t clientID, size_t newSize)
{
	int returnMe = 0;
	const char * cCommand = "mn::ChangeBufferSizeTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group[instanceID].GetInstanceTCP()->SetMaxPacketSizeTCP(clientID,newSize);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Changes the auto resize TCP option.
 *
 * When auto resize is true, if an incoming TCP packet is too large to be received,
 * instead of throwing an error or forcefully disconnecting that client, the TCP
 * receive buffer automatically resizes to allow for the TCP packet to be
 * received.
 *
 * @param	instanceID	Unique identifier for instance.
 * @param	clientID	ID of client.
 * @param	autoResize	True if the TCP receive buffer should automatically resize, false if not.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::SetAutoResizeTCP(size_t instanceID, size_t clientID, bool autoResize)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetAutoResizeTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group[instanceID].GetInstanceTCP()->SetAutoResizeTCP(autoResize, clientID);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Changes the auto resize TCP option.
 *
 * When auto resize is true, if an incoming TCP packet is too large to be received,
 * instead of throwing an error or forcefully disconnecting that client, the TCP
 * receive buffer automatically resizes to allow for the TCP packet to be
 * received.
 *
 * @param [out]	profile		Instance profile to be modified.
 * @param		autoResize	True if the TCP receive buffer should automatically resize, false if not.
 * Default is NetInstanceProfile::DEFAULT_AUTO_RESIZE_TCP.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
int mn::SetProfileAutoResizeTCP(NetInstanceProfile & profile, bool autoResize)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileAutoResizeTCP";

	try
	{
		profile.SetAutoResizeTCP(autoResize);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Changes the auto resize TCP option.
 *
 * @copydetails mn::SetProfileAutoResizeTCP(NetInstanceProfile&, bool)
 */ 
DBP_CPP_DLL int mn::SetProfileAutoResizeTCP(INT_PTR profile, bool autoResize)
{
	int returnMe = -1;
	const char * cCommand = "mn::SetProfileAutoResizeTCP";

	try
	{
		returnMe = mn::SetProfileAutoResizeTCP(PointerConverter::GetRefFromInt<NetInstanceProfile>(profile), autoResize);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Disables or enables the handshaking option for the specified instance profile.
 *
 * For more information see @ref handshakePage "server/client handshaking process".
 *
 * @param [out]	profile		Instance profile to be modified.
 * @param		option	True if the TCP handshake process should be enabled.
 * Default is NetInstanceProfile::DEFAULT_HANDSHAKE_ENABLED.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred. 
 */
int mn::SetProfileHandshakeEnabled(NetInstanceProfile & profile, bool option)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileHandshakeEnabled";

	try
	{
		profile.SetHandshakeEnabled(option);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Disables or enables the handshaking option for the specified instance profile.
 *
 * @copydetails mn::SetProfileHandshakeEnabled(NetInstanceProfile &, bool)
 */
DBP_CPP_DLL int mn::SetProfileHandshakeEnabled(INT_PTR profile, bool option)
{
	int returnMe = -1;
	const char * cCommand = "mn::SetProfileHandshakeEnabled";

	try
	{
		returnMe = mn::SetProfileHandshakeEnabled(PointerConverter::GetRefFromInt<NetInstanceProfile>(profile), option);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Sets the TCP mode for the specified instance profile.
 *
 * The TCP mode describes changes should be made to TCP packets sent
 * and how data is split into packets when received.
 *
 * @param [out]	profile		Instance profile to be modified.
 * @param		modeTCP		TCP mode option. Default is NetInstanceProfile::DEFAULT_MODE_TCP.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 * @throws ErrorReport If @a modeTCP is not a TCP mode.
 */
int mn::SetProfileModeTCP(NetInstanceProfile & profile, NetMode::ProtocolMode modeTCP)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileModeTCP";

	try
	{
		profile.SetModeTCP(modeTCP);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Sets the TCP mode for the specified instance profile.
 *
 * @copydetails mn::SetProfileModeTCP(NetInstanceProfile &, NetMode::ProtocolMode)
 * @note @a modeTCP is the value of NetMode::ProtocolMode e.g. For TCP_PREFIX_SIZE @a modeTCP should be 1.
 */
DBP_CPP_DLL int mn::SetProfileModeTCP(INT_PTR profile, char modeTCP)
{
	int returnMe = -1;
	const char * cCommand = "mn::SetProfileModeTCP";

	try
	{
		returnMe = mn::SetProfileModeTCP(PointerConverter::GetRefFromInt<NetInstanceProfile>(profile), NetMode::ConvertToProtocolModeTCP(modeTCP));
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Sets the UDP mode for the specified instance profile.
 *
 * The UDP mode describes changes should be made to UDP packets sent
 * and how data is split into packets when received.
 *
 * @param [out]	profile		Instance profile to be modified.
 * @param		modeUDP		UDP mode option. Default is NetInstanceProfile::DEFAULT_MODE_UDP.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 * @throws ErrorReport If @a modeUDP is not a UDP mode.
 */
int mn::SetProfileModeUDP(NetInstanceProfile & profile, NetMode::ProtocolMode modeUDP)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileModeUDP";

	try
	{
		profile.SetModeUDP(modeUDP);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Sets the UDP mode for the specified instance profile.
 *
 * @copydetails mn::SetProfileModeUDP(NetInstanceProfile &, NetMode::ProtocolMode)
 * @note @a modeUDP is the value of NetMode::ProtocolMode e.g. For UDP_CATCH_ALL @a modeUDP should be 4.
 */
DBP_CPP_DLL int mn::SetProfileModeUDP( INT_PTR profile, char modeUDP )
{
	int returnMe = -1;
	const char * cCommand = "mn::SetProfileModeUDP";

	try
	{
		returnMe = mn::SetProfileModeUDP(PointerConverter::GetRefFromInt<NetInstanceProfile>(profile), NetMode::ConvertToProtocolModeUDP(modeUDP));
	}
	STD_CATCH

	return(returnMe);
}


/**
 * @brief Enables or disables the @ref gracefulDisconnectPage "graceful disconnect process"
 * for the specified instance profile.
 *
 * @param [out]	profile		Instance profile to be modified.
 * @param		option		True if graceful disconnect process should be enabled, false if not.
 * Default is NetInstanceProfile::DEFAULT_GRACEFUL_DISCONNECT.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
int mn::SetProfileGracefulDisconnectEnabled(NetInstanceProfile & profile, bool option)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileGracefulDisconnectEnabled";

	try
	{
		profile.SetGracefulDisconnectEnabled(option);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Enables or disables the @ref gracefulDisconnectPage "graceful disconnect process"
 * for the specified instance profile.
 *
 * @copydetails mn::SetProfileGracefulDisconnectEnabled(NetInstanceProfile &, bool)
 */
DBP_CPP_DLL int mn::SetProfileGracefulDisconnectEnabled(INT_PTR profile, bool option)
{
	int returnMe = -1;
	const char * cCommand = "mn::SetProfileGracefulDisconnectEnabled";

	try
	{
		returnMe = mn::SetProfileGracefulDisconnectEnabled(PointerConverter::GetRefFromInt<NetInstanceProfile>(profile), option);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Changes the send timeout option for the specified instance profile. Send operations
 * are allowed this amount of time to complete before the entity that initiated the operation
 * is dropped.
 *
 * @param [out]	profile		Instance profile to be modified.
 * @param		timeoutMilliseconds		Length of time in milliseconds to wait for a send operation to complete before
 * disconnecting client (default = NetInstanceProfile::DEFAULT_SEND_TIMEOUT).
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
int mn::SetProfileSendTimeout(NetInstanceProfile & profile, unsigned int timeoutMilliseconds)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileSendTimeout";

	try
	{
		profile.SetSendTimeout(timeoutMilliseconds);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Changes the send timeout option for the specified instance profile. Send operations
 * are allowed this amount of time to complete before the entity that initiated the operation
 * is dropped.
 *
 * @copydetails mn::SetProfileSendTimeout(NetInstanceProfile &, unsigned int)
 */
DBP_CPP_DLL int mn::SetProfileSendTimeout(INT_PTR profile, unsigned int timeoutMilliseconds)
{
	int returnMe = -1;
	const char * cCommand = "mn::SetProfileSendTimeout";

	try
	{
		returnMe = mn::SetProfileSendTimeout(PointerConverter::GetRefFromInt<NetInstanceProfile>(profile), timeoutMilliseconds);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief This command changes the postfix option of the specified instance profile.
 *
 * This has no impact on instances not in NetMode::TCP_POSTFIX mode.
 *
 * @param [out]	profile		Instance profile to be modified.
 * @param		packet		New postfix, default is NetInstanceProfile::DEFAULT_POSTFIX_TCP.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::SetProfilePostfixTCP(INT_PTR profile, INT_PTR packet)
{
	int returnMe = -1;
	const char * cCommand = "mn::SetProfilePostfixTCP";

	try
	{
		Packet * packetPtr = PointerConverter::GetPtrFromInt<Packet>(packet);
		NetInstanceProfile & profileRef = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::SetProfilePostfixTCP(profileRef,*packetPtr);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief This command changes the postfix option of the specified instance profile.
 *
 * @copydetails mn::SetProfilePostfixTCP(INT_PTR, INT_PTR)
 */
int mn::SetProfilePostfixTCP(NetInstanceProfile & profile, const Packet & packet)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfilePostfixTCP";

	try
	{
		profile.SetPostFixTCP(packet);
	}
	STD_CATCH_RM

	return(returnMe);
}


/**
 * @brief This command changes the nagle algorithm option.
 *
 * @param [out]	profile		Instance profile to be modified.
 * @param		option		If true the nagle algorithm is enabled, if false the nagle algorithm is disabled.
 * Default is NetInstanceProfile::DEFAULT_NAGLE_ENABLED.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
int mn::SetProfileNagleEnabledTCP(NetInstanceProfile & profile, bool option)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileNagleEnabledTCP";

	try
	{
		profile.SetNagleEnabled(option);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief This command changes the nagle algorithm option.
 *
 * @copydetails mn::SetProfileNagleEnabledTCP(NetInstanceProfile &, bool)
 */
int mn::SetProfileNagleEnabledTCP(INT_PTR profile, bool option)
{
	int returnMe = -1;
	const char * cCommand = "mn::SetProfileNagleEnabledTCP";

	try
	{
		returnMe = mn::SetProfileNagleEnabledTCP(PointerConverter::GetRefFromInt<NetInstanceProfile>(profile), option);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the IP address of the specified host name. 
 *
 * @copydetails NetUtility::ConvertDomainNameToIP()
 * @return an empty NetAddress structure if an error occurred.
 */
const NetAddress & mn::DNS_A(const char * hostName)
{
	const char * cCommand = "mn::DNS";
	const NetAddress * returnMe;
	try
	{
		returnMe = &NetUtility::ConvertDomainNameToIP(hostName);
	}
	STD_CATCH

	return(*returnMe);
}

/**
 * @brief Retrieves the IP address of the specified host name. 
 *
 * @copydetails NetUtility::ConvertDomainNameToIP()
 * @return an empty string if an error occurred.
 */
DBP_CPP_DLL const char * mn::DNS_B(const char * hostName)
{
	const char * cCommand = "mn::DNS";
	const char * returnMe = "";
	try
	{
		NetAddress aux = NetUtility::ConvertDomainNameToIP(hostName);
		returnMe = aux.GetIP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves a string containing version information for this API. 
 *
 * @return @copydoc Utility::VERSION
 */
const char * mn::GetVersion()
{
	return Utility::VERSION;
}

/**
 * @brief Retrieves the address that the client is connected to via TCP.
 *
 * Can only be used on instances of type NetInstance::CLIENT.
 *
 * @param	instanceID	Unique identifier for instance.
 * @return	TCP remote address information of the specified client.
 */
NetAddress mn::GetConnectAddressTCP(size_t instanceID)
{
	NetAddress returnMe;
	const char * cCommand = "mn::GetConnectAddressTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceClient()->GetConnectAddressTCP(NULL);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the IP address that the client is connected to via TCP.
 *
 * Can only be used on instances of type NetInstance::CLIENT.
 *
 * @param	instanceID	Unique identifier for instance.
 * @return	TCP remote IP of the specified client.
 * This pointer will remain valid and unmodified until the next call.
 * @return  an empty string if an error occurred.
 */
CPP_DLL const char * mn::GetConnectIPTCP(size_t instanceID)
{
	const char * returnMe = "";
	const char * cCommand = "mn::GetConnectIPTCP";

	try
	{
		returnMe = mn::GetConnectAddressTCP(instanceID).GetIP();
	}
	STD_CATCH

	return(returnMe);
}


/**
 * @brief Retrieves the port that the client is connected to via TCP.
 *
 * Can only be used on instances of type NetInstance::CLIENT.
 *
 * @param	instanceID	Unique identifier for instance.
 * @return	TCP remote port of the specified client.
 * @return  0 if an error occurred.
 */
DBP_CPP_DLL unsigned short mn::GetConnectPortTCP(size_t instanceID)
{
	unsigned short returnMe = 0;
	const char * cCommand = "mn::GetConnectPortTCP";

	try
	{
		returnMe = mn::GetConnectAddressTCP(instanceID).GetPort();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the address that the client is connected to via UDP.
 *
 * Can only be used on instances of type NetInstance::CLIENT.
 *
 * @param	instanceID	Unique identifier for instance.
 * @return	UDP remote address information of the specified client.
 */
NetAddress mn::GetConnectAddressUDP(size_t instanceID)
{
	NetAddress returnMe;
	const char * cCommand = "mn::GetConnectAddressUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceClient()->GetConnectAddressUDP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the IP address that the client is connected to via UDP.
 *
 * Can only be used on instances of type NetInstance::CLIENT.
 *
 * @param	instanceID	Unique identifier for instance.
 * @return	UDP remote IP of the specified client.
 * This pointer will remain valid and unmodified until the next call.
 * @return  an empty string if an error occurred.
 */
CPP_DLL const char * mn::GetConnectIPUDP(size_t instanceID)
{
	const char * returnMe = "";
	const char * cCommand = "mn::GetConnectIPUDP";

	try
	{
		returnMe = mn::GetConnectAddressUDP(instanceID).GetIP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the port that the client is connected to via UDP.
 *
 * Can only be used on instances of type NetInstance::CLIENT.
 *
 * @param	instanceID	Unique identifier for instance.
 * @return	UDP remote port of the specified client.
 * @return  0 if an error occurred.
 */
DBP_CPP_DLL unsigned short mn::GetConnectPortUDP(size_t instanceID)
{
	unsigned short returnMe = 0;
	const char * cCommand = "mn::GetConnectPortUDP";

	try
	{
		returnMe = mn::GetConnectAddressUDP(instanceID).GetPort();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the status of the @ref handshakePage "handshaking process".
 *
 * Can only be used in NetInstance::CLIENT state.
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @param instanceID	Unique identifier for instance.
 *
 * @return NetUtility::STILL_CONNECTING if the handshaking process is in progress still.
 * @return NetUtility::CONNECTED if the handshaking process completed successfully and the client is now fully connected.
 * @return NetUtility::TIMED_OUT if the handshaking process timed out.
 * @return NetUtility::CONNECTION_ERROR if an error occurred.
 * @return NetUtility::REFUSED if the server is full and so the connection attempt failed.
 */
DBP_CPP_DLL NetUtility::ConnectionStatus mn::PollConnect(size_t instanceID)
{
	NetUtility::ConnectionStatus returnMe = NetUtility::CONNECTION_ERROR;
	const char * cCommand = "mn::PollConnect";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceClient()->PollConnect();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Determines whether the client is in the process of connecting to a server.
 *
 * Can only be used in NetInstance::CLIENT state.
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @param instanceID	Unique identifier for instance.
 *
 * @return true if connecting, false if not.
 */
CPP_DLL bool mn::GetConnecting(size_t instanceID)
{
	bool returnMe = false;
	const char * cCommand = "mn::GetConnecting";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceClient()->IsConnecting();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Cancels the @ref handshakePage "handshaking process".
 *
 * Can only be used in NetInstance::CLIENT state.
 *
 * The handshaking must be in progress otherwise this command will fail.
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @warning PollConnect should not be used after this command, it will fail.
 *
 * @param instanceID	Unique identifier for instance.
 * @return 0 if the command completed successfully.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::StopConnect(size_t instanceID)
{
	int returnMe = 0;
	const char * cCommand = "mn::StopConnect";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group[instanceID].GetInstanceClient()->StopConnect();
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Retrieves the client ID that the server has assigned the client.
 *
 * Can only be used in NetInstance::CLIENT state.
 * 
 * @param instanceID	Unique identifier for instance.
 * @return @copydoc NetInstanceClient::clientID
 * @return 0 if the client is not fully connected to the server.
 */
DBP_CPP_DLL size_t mn::GetClientID(size_t instanceID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetClientID";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceClient()->GetClientID();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the maximum number of clients that can be connected to the server at any one time.
 *
 * This command can be used only on an active server or client instance. \n\n
 *
 * Client IDs range from 1 inclusive to this value inclusive, client ID 0 can be used to identify the server, as it
 * will not be assigned to any client.
 *
 * @param instanceID Unique identifier for instance.
 *
 * @return in server state: The maximum number of clients that can be connected to the server at any one time.
 * @return in client state: The maximum number of clients that can be connected to the server at any one time that the client is connected to.
 * @return 0 if an error occurred.
 */
DBP_CPP_DLL size_t mn::GetMaxClients(size_t instanceID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetMaxClients";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();

		switch(group[instanceID].GetInstanceCore()->GetState())
		{
		case(NetInstance::CLIENT):
			returnMe = group[instanceID].GetInstanceClient()->GetMaxClients();
			break;

		case(NetInstance::SERVER):
			returnMe = group[instanceID].GetInstanceServer()->GetMaxClients();
			break;

		default:
			_ErrorException(true,"retrieving the maximum number of clients that can be connected to the server, invalid instance type",0,__LINE__,__FILE__);
			break;
		}
	}
	STD_CATCH

	return(returnMe);
}
/**
 * @brief Retrieves the number of UDP operation IDs available.
 *
 * This command can be used only on an active UDP instance.\n\n
 *
 * In client state this value is retrieved from the server during the handshaking process.\n\n
 *
 * The default value for this option is NetInstanceProfile::DEFAULT_NUM_OPERATIONS.
 *
 * @param instanceID Unique identifier for instance.
 *
 * @return the number of UDP operation IDs available. Operation IDs range from 0 inclusive to 
 * number of operations exclusive.
 * @return 0 if an error occurred.
 */
DBP_CPP_DLL size_t mn::GetMaxOperations(size_t instanceID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetMaxOperations";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceUDP()->GetNumOperationsUDP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the size of the UDP receive buffer. 
 *
 * This command can be used only on an active UDP instance.\n\n
 *
 * The default value for this option is NetInstanceProfile::DEFAULT_BUFFER_SIZE.
 *
 * @param instanceID Unique identifier for instance.
 *
 * @return the size of the UDP receive buffer, UDP packets larger than this size cannot be received.
 * @return 0 if an error occurred.
 */
DBP_CPP_DLL size_t mn::GetRecvSizeUDP(size_t instanceID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetRecvSizeUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceUDP()->GetRecvBufferLengthUDP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the number of completion port threads in operation.
 *
 * @return the number of completion port threads in operation.
 */
DBP_CPP_DLL size_t mn::GetThreads()
{
	return NetUtility::GetNumThreads();
}

/**
 * @brief Retrieves the number of instances available (including inactive ones).
 *
 * @return the number of instances available, included in this number are both inactive and active instances.
 * @return 0 if an error occurred.
 */
DBP_CPP_DLL size_t mn::GetNumInstances()
{
	return NetUtility::GetNumInstances();
}

/**
 * @brief Retrieves the state that the instance is in.
 *
 * This command can be used only on an active instance.\n\n
 *
 * This command is used to determine what type of instance is loaded at an instance ID.
 *
 * @param instanceID Unique identifier for instance.
 * @return NetInstance::Type value, indicating the type of instance loaded at ID @a instanceID.
 * @return NetInstance::INACTIVE if an error occurred.
 */
DBP_CPP_DLL NetInstance::Type mn::GetState(size_t instanceID)
{
	NetInstance::Type returnMe = NetInstance::INACTIVE;
	const char * cCommand = "mn::GetState";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceCore()->GetState();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the UDP mode in use.
 *
 * This command can be used only on an active UDP instance.\n\n
 *
 * The default value for this option is NetInstanceProfile::DEFAULT_MODE_UDP.
 *
 * @param instanceID Unique identifier for instance.
 * @return NetMode::ProtocolMode value indicating the UDP mode in use. Note that the return
 * value will only be a UDP mode, not any other mode type.
 */
DBP_CPP_DLL NetMode::ProtocolMode mn::GetModeUDP(size_t instanceID)
{
	NetMode::ProtocolMode returnMe;
	const char * cCommand = "mn::GetModeUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceUDP()->GetProtocolModeUDP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Determines whether UDP is enabled.
 *
 * This command can be used only on an active UDP instance.\n\n
 *
 * The default value for this option is NetInstanceProfile::DEFAULT_ENABLED_UDP.
 *
 * @param instanceID Unique identifier for instance.
 * @return 1 if UDP is enabled.
 * @return 0 if UDP is disabled.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::GetEnabledUDP(size_t instanceID)
{
	int returnMe = -1;
	const char * cCommand = "mn::IsEnabledUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceUDP()->IsEnabledUDP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Determines how much of the packet currently being receives has been received as a percentage. 
 *
 * This command can be used only on an active TCP instance.\n\n
 *
 * @param instanceID Unique identifier for instance.
 * @param clientID ID of client to use.
 *
 * @return the percentage of the partial packet that has been received
 * i.e. Between 0 and 100 where 50 indicates that half the packet has
 * been received and we are now waiting for the second half to be received.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL double mn::GetPercentTCP(size_t instanceID, size_t clientID)
{
	double returnMe = -1;
	const char * cCommand = "mn::GetPercentTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceTCP()->GetPartialPacketPercentageTCP(clientID);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the TCP buffer size. 
 *
 * This command can be used only on an active TCP instance.\n\n
 *
 * The default value for this option is NetInstanceProfile::DEFAULT_BUFFER_SIZE.
 *
 * @param instanceID Unique identifier for instance.
 * @param clientID ID of client to use.
 * @return the size of the TCP receive buffer. TCP packets larger than this cannot be received
 * unless the auto resize option is true. If the packet cannot be received then an error may occur (this happens in client state)
 * or the problem will be silently dealt with (this happens in server state).
 * @return 0 if an error occurred.
 */
DBP_CPP_DLL size_t mn::GetRecvSizeTCP(size_t instanceID, size_t clientID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetRecvSizeTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceServer()->GetRecvBufferLengthTCP(clientID);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Determines how much of the packet currently being receives has been received in bytes. 
 *
 * This command can be used only on an active TCP instance.
 *
 * @param instanceID Unique identifier for instance.
 * @param clientID ID of client to use.
 *
 * @return the current size of the TCP partial packet being received
 * i.e. The number of bytes of the packet that have been received.
 * @return 0 if an error occurred.
 */
DBP_CPP_DLL size_t mn::GetBytesTCP(size_t instanceID, size_t clientID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetBytesTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceTCP()->GetPartialPacketCurrentSizeTCP(clientID);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Determines if the handshaking process is enabled.
 *
 * For more information see @ref handshakePage "server/client handshaking process".
 *
 * This command can be used only on an active TCP instance.\n\n
 *
 * The default value for this option is NetInstanceProfile::DEFAULT_HANDSHAKE_ENABLED.
 *
 * @param instanceID Unique identifier for instance.
 * @return 1 if the @ref handshakePage "handshake process" is enabled.
 * @return 0 if the @ref handshakePage "handshake process" is disabled.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::GetHandshakeEnabledTCP(size_t instanceID)
{
	int returnMe = -1;
	const char * cCommand = "mn::GetHandshakeEnabledTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceTCP()->IsHandshakeEnabled();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the TCP mode in use.
 *
 * This command can be used only on an active TCP instance.\n\n
 *
 * The default value for this option is NetInstanceProfile::DEFAULT_MODE_TCP.
 *
 * @param instanceID Unique identifier for instance.
 * @return NetMode::ProtocolMode value indicating the TCP mode in use. Note that the return
 * value will only be a TCP mode, not any other mode type.
 */
DBP_CPP_DLL NetMode::ProtocolMode mn::GetModeTCP(size_t instanceID)
{
	NetMode::ProtocolMode returnMe;
	const char * cCommand = "mn::GetModeTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceTCP()->GetProtocolModeTCP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Determines if the graceful disconnect process is enabled.
 *
 * For more information see the  @ref gracefulDisconnectPage "graceful disconnection page".\n\n
 *
 * This command can be used only on an active TCP instance.\n\n
 *
 * The default value for this option is NetInstanceProfile::DEFAULT_GRACEFUL_DISCONNECT.
 *
 * @param instanceID Unique identifier for instance.
 *
 * @return 1 if @ref gracefulDisconnectPage "graceful disconnection" is enabled.
 * @return 0 if @ref gracefulDisconnectPage "graceful disconnection" is disabled.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::GetGracefulDisconnectEnabledTCP(size_t instanceID)
{
	int returnMe = -1;
	const char * cCommand = "mn::GetGracefulDisconnectEnabledTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceTCP()->IsGracefulDisconnectEnabled();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the length of time that send operations are allowed to take
 * before the entity that initiated them is disconnected forcefully.
 *
 * This command can be used only on an active instance.\n\n
 *
 * The default value for this option is NetInstanceProfile::DEFAULT_SEND_TIMEOUT.
 *
 * @param instanceID Unique identifier for instance.
 * @return length of time in milliseconds to wait for a send operation to complete before
 * disconnecting client.
 * @return 0 if an error occurred.
 */
DBP_CPP_DLL unsigned int mn::GetSendTimeout( size_t instanceID )
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetSendTimeout";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceCore()->GetSendTimeout();
	}
	STD_CATCH

	return(static_cast<unsigned int>(returnMe));
}

/**
 * @brief Retrieves the number of packets stored in the TCP received packet queue.
 *
 * This command can be used only on an active TCP instance.
 *
 * @param instanceID Unique identifier for instance.
 * @param clientID ID of client to use.
 *
 * @return the number of packets in the TCP packet store.
 * @return if a TCP receive function has been set using mn::SetFunction, this method will always return 0
 * because the queue system is not in use.
 * @return 0 if an error occurred.
 */
DBP_CPP_DLL size_t mn::GetStoreAmountTCP(size_t instanceID, size_t clientID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetStoreAmountTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceTCP()->GetPacketAmountTCP(clientID);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the number of packets stored in the UDP received packet queue.
 *
 * This command can be used only on an active UDP instance.
 *
 * @param instanceID Unique identifier for instance.
 * @param clientID ID of client to use.
 *
 * @return the number of packets in the UDP packet store.
 * @return if a UDP receive function has been set using mn::SetFunction, this method will always return 0
 * because the queue system is not in use.
 * @return 0 if an error occurred.
 */
DBP_CPP_DLL size_t mn::GetStoreAmountUDP( size_t instanceID, size_t clientID )
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetStoreAmountUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceUDP()->GetPacketAmountUDP(clientID);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the TCP postfix in use.
 *
 * This command copies the TCP postfix of the specified instance into @a packet.\n\n
 *
 * This command can be used only on an active TCP instance in NetMode::TCP_POSTFIX TCP mode.\n\n
 *
 * The default value for this option is NetInstanceProfile::DEFAULT_POSTFIX_TCP.
 *
 * @param instanceID Unique identifier for instance.
 * @param [out] packet %Packet that postfix should be copied into, overwriting any existing data in the packet.
 *
 * @return 0 if no error occurred.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::GetPostfixTCP(size_t instanceID, INT_PTR packet)
{
	int returnMe = 0;
	const char * cCommand = "mn::GetPostfixTCP (long long int)";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		Packet & aux = PointerConverter::GetRefFromInt<Packet>(packet);
		NetInstanceTCP * instanceTCP = group[instanceID].GetInstanceTCP();
		aux = instanceTCP->GetPostfixTCP();
	}
	STD_CATCH_RM

	return(returnMe);
}
/**
 * @brief Retrieves the TCP postfix in use.
 *
 * @copydetails mn::GetPostfixTCP(size_t,INT_PTR)
 */
int mn::GetPostfixTCP(size_t instanceID, Packet & packet)
{
	int returnMe = 0;
	const char * cCommand = "mn::GetPostfixTCP (Packet)";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		packet = group[instanceID].GetInstanceTCP()->GetPostfixTCP();
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Determines whether the nagle algorithm is enabled.
 *
 * This command can be used only on an active TCP instance.\n\n
 *
 * The default value for this option is NetInstanceProfile::DEFAULT_NAGLE_ENABLED.
 *
 * @param instanceID Unique identifier for instance.
 *
 * @return 1 if the nagle algorithm is enabled.
 * @return 0 if the nagle algorithm is disabled.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::GetNagleEnabledTCP(size_t instanceID)
{
	int returnMe = -1;
	const char * cCommand = "mn::GetNagleEnabledTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceTCP()->GetNagleEnabledTCP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Determines whether the TCP buffer size will automatically increase if necessary.
 *
 * This command can be used only on an active TCP instance.\n\n
 *
 * The default value for this option is NetInstanceProfile::DEFAULT_AUTO_RESIZE_TCP.
 *
 * @param instanceID Unique identifier for instance.
 * @param clientID ID of client to use.
 *
 * @return 1 if auto resize is enabled; this means that if a packet larger than mn::GetRecvSizeTCP()
 * size is received then the max size will be increased silently.
 * @return 0 if auto resize is disabled; this means that an exception will be thrown if a packet larger than
 * mn::GetRecvSizeTCP() is received. In server state the client will be silently disconnected and in
 * client state an error will occur.
 */
DBP_CPP_DLL int mn::GetAutoResizeTCP(size_t instanceID, size_t clientID)
{
	int returnMe = -1;
	const char * cCommand = "mn::GetAutoResizeTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceTCP()->GetAutoResizeTCP(clientID);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves a TCP packet from the TCP received packet queue.
 *
 * When TCP packets are received and a TCP receive function has not been set
 * (using mn::SetFunction), they are put into a queue and retrieved
 * using this command.
 *
 * @param instanceID Unique identifier for instance.
 * @param clientID ID of client to receive from.
 * @param [out] destinationPacket Received packet will be copied into this location, overwriting any existing data.
 * @return 0 if no packet was received.
 * @return >0 if a packet was received, this is the number of packets in the receive
 * queue before this command was called.
 */
size_t mn::RecvTCP(size_t instanceID, Packet & destinationPacket, size_t clientID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::RecvTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceTCP()->GetPacketFromStoreTCP(&destinationPacket,clientID);
	}
	STD_CATCH
	
	return(returnMe);
}

/**
 * @brief Retrieves a TCP packet from the TCP received packet queue.
 *
 * @copydetails mn::RecvTCP(size_t, Packet &, size_t)
 */
DBP_CPP_DLL size_t mn::RecvTCP(size_t instanceID, INT_PTR destinationPacket, size_t clientID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::RecvTCP";

	try
	{
		Packet & aux = PointerConverter::GetRefFromInt<Packet>(destinationPacket);
		returnMe = mn::RecvTCP(instanceID,aux,clientID);
	}
	STD_CATCH
	
	return(returnMe);
}

/**
 * @brief Retrieves a UDP packet from the UDP received packet queue.
 *
 * When UDP packets are received and a UDP receive function has not been set
 * (using mn::SetFunction), they are put into a queue or store and retrieved
 * using this command.
 *
 * @param instanceID Unique identifier for instance.
 * @param clientID ID of client to receive from.
 * @param operationID ID of operation to check, only valid in NetMode::UDP_PER_CLIENT_PER_OPERATION UDP mode.
 * @param [out] destinationPacket Received packet will be copied into this location, overwriting any existing data.
 * @return 0 if no packet was received.
 * @return >0 if a packet was received, this is the number of packets in the receive
 * queue before this command was called.
 */
size_t mn::RecvUDP(size_t instanceID, Packet & destinationPacket, size_t clientID, size_t operationID)
{	
	size_t returnMe = 0;
	const char * cCommand = "mn::RecvUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceUDP()->GetPacketFromStoreUDP(&destinationPacket,clientID,operationID);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves a UDP packet from the UDP received packet queue.
 *
 * @copydetails mn::RecvUDP(size_t, Packet &, size_t, size_t)
 */
DBP_CPP_DLL size_t mn::RecvUDP(size_t instanceID, INT_PTR destinationPacket, size_t clientID, size_t operationID)
{	
	size_t returnMe = 0;
	const char * cCommand = "mn::RecvUDP";

	try
	{
		Packet & aux = PointerConverter::GetRefFromInt<Packet>(destinationPacket);
		returnMe = mn::RecvUDP(instanceID,aux,clientID,operationID);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Sends UDP data to a client on the specified instance.
 *
 * @param instanceID Unique identifier for instance.
 * @param clientID ID of client to receive from.
 * @param [in] packet %Packet to send.
 * @param keep If false @a packet's contents will be erased, if true no modifications to @a packet will be made.
 * @param block If false the command will return immediately without waiting for the send operation to complete.
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED if the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus mn::SendUDP(size_t instanceID, Packet & packet, size_t clientID, bool keep, bool block)
{
	NetUtility::SendStatus returnMe = NetUtility::SEND_FAILED; 
	const char * cCommand = "mn::SendUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceUDP()->SendUDP(packet,block,clientID);
		if(keep == false)
		{
			packet.Clear();
		}
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Sends UDP data to a client on the specified instance.
 *
 * @copydetails mn::SendUDP(size_t, Packet &, size_t, bool, bool)
 */
DBP_CPP_DLL int mn::SendUDP(size_t instanceID, INT_PTR packet, size_t clientID, bool keep, bool block)
{
	int returnMe = NetUtility::SEND_FAILED; 
	const char * cCommand = "mn::SendUDP";

	try
	{
		Packet & aux = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = mn::SendUDP(instanceID,aux,clientID,keep,block);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Sends UDP data to an unconnected entity, identified only by its remote address.
 *
 * @param instanceID Unique identifier for instance.
 * @param address Address to send packet to.
 * @param [in] packet %Packet to send.
 * @param keep If false @a packet's contents will be erased, if true no modifications to @a packet will be made.
 * @param block If false the command will return immediately without waiting for the send operation to complete.
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED if the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus mn::SendToUDP(size_t instanceID, Packet & packet, const NetAddress & address, bool keep, bool block)
{
	NetUtility::SendStatus returnMe = NetUtility::SEND_FAILED; 
	const char * cCommand = "mn::SendToUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceUDP()->SendToUDP(address,packet,block);

		if(keep == false)
		{
			packet.Clear();
		}
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Sends UDP data to an unconnected entity, identified only by its remote address.
 *
 * @param instanceID Unique identifier for instance.
 * @param addrIP IP address to send packet to.
 * @param addrPort Port to send packet to.
 * @param [in] packet %Packet to send.
 * @param keep If false @a packet's contents will be erased, if true no modifications to @a packet will be made.
 * @param block If false the command will return immediately without waiting for the send operation to complete.
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED if the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
DBP_CPP_DLL int mn::SendToUDP(size_t instanceID, INT_PTR packet, const char * addrIP, unsigned short addrPort, bool keep, bool block)
{
	int returnMe = -1; 
	const char * cCommand = "mn::SendToUDP";

	try
	{
		NetAddress addr(addrIP,addrPort);
		Packet & auxPacket = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = mn::SendToUDP(instanceID,auxPacket,addr,keep,block);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Sends TCP data to a client on the specified instance.
 *
 * @param instanceID Unique identifier for instance.
 * @param clientID ID of client to receive from.
 * @param [in] packet %Packet to send.
 * @param keep If false @a packet's contents will be erased, if true no modifications to @a packet will be made.
 * @param block If false the command will return immediately without waiting for the send operation to complete.
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED if the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */

NetUtility::SendStatus mn::SendTCP(size_t instanceID, Packet & packet, size_t clientID, bool keep, bool block)
{
	NetUtility::SendStatus returnMe = NetUtility::SEND_FAILED; 
	const char * cCommand = "mn::SendTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceTCP()->SendTCP(packet,block,clientID);
		if(keep == false)
		{
			packet.Clear();
		}
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Sends TCP data to a client on the specified instance.
 *
 * @copydetails mn::SendTCP(size_t, Packet &, size_t, bool, bool)
 */
DBP_CPP_DLL int mn::SendTCP(size_t instanceID, INT_PTR packet, size_t clientID, bool keep, bool block)
{
	int returnMe = -1;
	const char * cCommand = "mn::SendTCP";

	try
	{
		Packet & auxPacket = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = mn::SendTCP(instanceID,auxPacket,clientID,keep,block);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Sends a TCP packet to all clients on the specified instance.
 *
 * @param instanceID Unique identifier for instance.
 * @param [in] packet %Packet to send.
 * @param keep If false @a packet's contents will be erased, if true no modifications to @a packet will be made.
 * @param block If false the command will return immediately without waiting for the send operation to complete.
 * @param clientExcludeID ID of client to exclude, the packet will be sent to all clients except one with this ID.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
int mn::SendAllTCP(size_t instanceID, Packet & packet, bool keep, bool block, size_t clientExcludeID)
{
	int returnMe = 0;
	const char * cCommand = "mn::SendAllTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group[instanceID].GetInstanceServer()->SendAllTCP(packet,block,clientExcludeID);
		if(keep == false)
		{
			packet.Clear();
		}
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Sends a TCP packet to all clients on the specified instance.
 *
 * @copydetails mn::SendAllTCP(size_t, Packet &, bool, bool, size_t)
 */
DBP_CPP_DLL int mn::SendAllTCP(size_t instanceID, INT_PTR packet, bool keep, bool block, size_t clientExcludeID)
{
	int returnMe = 0;
	const char * cCommand = "mn::SendAllTCP";

	try
	{
		Packet & auxPacket = PointerConverter::GetRefFromInt<Packet>(packet);
		mn::SendAllTCP(instanceID,auxPacket,keep,block,clientExcludeID);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Sends a UDP packet to all clients on the specified instance.
 *
 * @param instanceID Unique identifier for instance.
 * @param [in] packet %Packet to send.
 * @param keep If false @a packet's contents will be erased, if true no modifications to @a packet will be made.
 * @param block If false the command will return immediately without waiting for the send operation to complete.
 * @param clientExcludeID ID of client to exclude, the packet will be sent to all clients except one with this ID.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
int mn::SendAllUDP(size_t instanceID, Packet & packet, bool keep, bool block, size_t clientExcludeID)
{
	int returnMe = 0; 
	const char * cCommand = "mn::SendAllUDP";
 
	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group[instanceID].GetInstanceServer()->SendAllUDP(packet,block,clientExcludeID);
		if(keep == false)
		{
			packet.Clear();
		}
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Sends a UDP packet to all clients on the specified instance.
 *
 * @copydoc mn::SendAllUDP(size_t, Packet &, bool, bool, size_t)
 */
DBP_CPP_DLL int mn::SendAllUDP(size_t instanceID, INT_PTR packet, bool keep, bool block, size_t clientExcludeID)
{
	int returnMe = 0;
	const char * cCommand = "mn::SendAllUDP";
 
	try
	{
		Packet & auxPacket = PointerConverter::GetRefFromInt<Packet>(packet);
		mn::SendAllUDP(instanceID,auxPacket,keep,block,clientExcludeID);
	}
	STD_CATCH_RM

	return(returnMe);
}



/**
 * @brief Retrieves the number of local interfaces available.
 *
 * @return the number of local interfaces available,
 * interface IDs range from 0 inclusive to number of local interfaces exclusive.
 */
DBP_CPP_DLL size_t mn::GetLocalInterfaceAmount()
{
	return NetUtility::GetNumLocalInterface();
}

/**
 * @brief Retrieves a local interface.
 *
 * @param num ID of interface, interface IDs range from 0 inclusive to number of local interfaces exclusive.
 * @return local interface.
 */
const NetAddress & mn::GetLocalInterface(size_t num)
{
	const NetAddress * returnMe = NULL; 
	const char * cCommand = "mn::GetLocalInterface";

	try
	{
		returnMe = &NetUtility::GetLocalInterface(num);
	}
	STD_CATCH

	return(*returnMe);
}

/**
 * @brief Retrieves a local interface.
 *
 * @copydetails mn::GetLocalInterface()
 */
CPP_DLL const char * mn::GetLocalInterfaceStr(size_t num)
{
	const char * returnMe = NULL; 
	const char * cCommand = "mn::GetLocalInterface";

	try
	{
		returnMe = NetUtility::GetLocalInterface(num).GetIP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the name of the computer.
 *
 * @return the name of the computer.
 */
CPP_DLL const char * mn::GetHostName()
{
	return NetUtility::GetHostName();
}

/**
 * @brief Retrieves the local TCP address stored in the specified instance profile.
 *
 * @param [in] profile Profile to use.
 * @return the local TCP address currently loaded into the specified profile.
 */
NetAddress mn::GetProfileLocalAddressTCP( const NetInstanceProfile & profile )
{
	NetAddress returnMe; 
	const char * cCommand = "mn::GetProfileLocalAddressTCP";

	try
	{
		returnMe = profile.GetLocalAddrTCP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the local TCP IP address stored in the specified instance profile.
 *
 * @param [in] profile Profile to use.
 * @return the local TCP IP currently loaded into the specified profile.
 */
CPP_DLL const char * mn::GetProfileLocalIPTCP(INT_PTR profile)
{
	const char * returnMe = "";
	const char * cCommand = "mn::GetProfileLocalIPTCP";

	try
	{
		const NetInstanceProfile & aux = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::GetProfileLocalAddressTCP(aux).GetIP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the local TCP port stored in the specified instance profile.
 *
 * @param [in] profile Profile to use.
 * @return the local TCP port currently loaded into the specified profile.
 */
DBP_CPP_DLL unsigned short mn::GetProfileLocalPortTCP(INT_PTR profile)
{
	unsigned short returnMe = 0;
	const char * cCommand = "mn::GetProfileLocalPortTCP";

	try
	{
		const NetInstanceProfile & aux = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::GetProfileLocalAddressTCP(aux).GetPort();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the local UDP address stored in the specified instance profile.
 *
 * @param [in] profile Profile to use.
 * @return the local UDP address currently loaded into the specified profile.
 */
NetAddress mn::GetProfileLocalAddressUDP( const NetInstanceProfile & profile )
{
	NetAddress returnMe; 
	const char * cCommand = "mn::GetProfileLocalAddressUDP";

	try
	{
		returnMe = profile.GetLocalAddrUDP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the local UDP IP address stored in the specified instance profile.
 *
 * @param [in] profile Profile to use.
 * @return the local UDP IP currently loaded into the specified profile.
 */
CPP_DLL const char * mn::GetProfileLocalIPUDP(INT_PTR profile)
{
	const char * returnMe = "";
	const char * cCommand = "mn::GetProfileLocalIPUDP";

	try
	{
		const NetInstanceProfile & aux = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::GetProfileLocalAddressUDP(aux).GetIP();
	}
	STD_CATCH
	
	return(returnMe);
}

/**
 * @brief Retrieves the local UDP port stored in the specified instance profile.
 *
 * @param [in] profile Profile to use.
 * @return the local UDP port currently loaded into the specified profile.
 */
DBP_CPP_DLL unsigned short mn::GetProfileLocalPortUDP(INT_PTR profile)
{
	unsigned short returnMe = 0;
	const char * cCommand = "mn::GetProfileLocalPortUDP";

	try
	{
		const NetInstanceProfile & aux = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::GetProfileLocalAddressUDP(aux).GetPort();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Sets the local TCP address of the specified instance profile.
 *
 * @param [out] profile Profile to use.
 * @param addressTCP The TCP address to load into the profile, overwriting any existing TCP address.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
int mn::SetProfileLocalTCP(NetInstanceProfile & profile, const NetAddress & addressTCP)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileLocalTCP";

	try
	{
		profile.SetLocalAddrTCP(addressTCP);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Sets the local TCP address of the specified instance profile.
 *
 * @param [out] profile Profile to use.
 * @param IP The TCP IP address to load into the profile, overwriting any existing TCP IP.
 * @param port The TCP port to load into the profile, overwriting any existing TCP port.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred. 
 */
DBP_CPP_DLL int mn::SetProfileLocalTCP(INT_PTR profile, const char * IP, unsigned short port)
{
	int returnMe = -1;
	const char * cCommand = "mn::SetProfileLocalTCP";

	try
	{
		NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		NetAddress addr(IP,port);
		
		returnMe = mn::SetProfileLocalTCP(auxProfile,addr);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Sets the local UDP address of the specified instance profile.
 *
 * @param [out] profile Profile to use.
 * @param addressUDP The UDP address to load into the profile, overwriting any existing UDP address.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred. 
 */
int mn::SetProfileLocalUDP(NetInstanceProfile & profile, const NetAddress & addressUDP)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileLocalUDP";

	try
	{
		profile.SetLocalAddrUDP(addressUDP);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Sets the local UDP address of the specified instance profile.
 *
 * @param [out] profile Profile to use.
 * @param IP The UDP IP address to load into the profile, overwriting any existing UDP IP.
 * @param port The UDP port to load into the profile, overwriting any existing UDP port.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred. 
 */
DBP_CPP_DLL int mn::SetProfileLocalUDP(INT_PTR profile, const char * IP, unsigned short port)
{
	int returnMe = -1;
	const char * cCommand = "mn::SetProfileLocalUDP";

	try
	{
		NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		NetAddress addr(IP,port);
		
		returnMe = mn::SetProfileLocalUDP(auxProfile,addr);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Sets the local TCP and UDP address of the specified instance profile.
 *
 * @param [out] profile Profile to use.
 * @param addressTCP The TCP address to load into the profile, overwriting any existing TCP address.
 * @param addressUDP The UDP address to load into the profile, overwriting any existing UDP address.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred. 
 */
int mn::SetProfileLocal(NetInstanceProfile & profile, const NetAddress & addressTCP, const NetAddress & addressUDP)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileLocal";

	try
	{
		mn::SetProfileLocalTCP(profile,addressTCP);
		mn::SetProfileLocalUDP(profile,addressUDP);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Sets the local TCP and UDP address of the specified instance profile.
 *
 * @param [out] profile Profile to use.
 * @param IP_TCP The TCP IP address to load into the profile, overwriting any existing TCP IP.
 * @param portTCP The TCP port to load into the profile, overwriting any existing TCP port.
 * @param IP_UDP The UDP IP address to load into the profile, overwriting any existing UDP IP.
 * @param portUDP The UDP port to load into the profile, overwriting any existing UDP port.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred. 
 */
DBP_CPP_DLL int mn::SetProfileLocal(INT_PTR profile, const char * IP_TCP, unsigned short portTCP, const char * IP_UDP, unsigned short portUDP)
{
	int returnMe = -1;
	const char * cCommand = "mn::SetProfileLocal";

	try
	{
		NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		NetAddress addrTCP(IP_TCP,portTCP);
		NetAddress addrUDP(IP_UDP,portUDP);
		returnMe = mn::SetProfileLocal(auxProfile,addrTCP,addrUDP);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the local TCP address of the specified instance.
 *
 * @param instanceID Unique identifier for instance.
 * @return the local TCP address of the specified instance.
 */
NetAddress mn::GetLocalAddressTCP(size_t instanceID)
{
	NetAddress returnMe; 
	const char * cCommand = "mn::GetLocalAddressTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceTCP()->GetLocalAddressTCP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the local TCP IP of the specified instance.
 *
 * @param instanceID Unique identifier for instance.
 * @return the local TCP IP of the specified instance.
 */
CPP_DLL const char * mn::GetLocalIPTCP(size_t instanceID)
{
	const char * returnMe = "";
	const char * cCommand = "mn::GetLocalIPTCP";

	try
	{
		returnMe = mn::GetLocalAddressTCP(instanceID).GetIP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the local TCP port of the specified instance.
 *
 * @param instanceID Unique identifier for instance.
 * @return the local TCP port of the specified instance.
 */
DBP_CPP_DLL unsigned short mn::GetLocalPortTCP(size_t instanceID)
{
	unsigned short returnMe = 0;
	const char * cCommand = "mn::GetLocalPortTCP";

	try
	{
		returnMe = mn::GetLocalAddressTCP(instanceID).GetPort();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the local UDP address of the specified instance.
 *
 * @param instanceID Unique identifier for instance.
 * @return the local UDP address of the specified instance.
 */
NetAddress mn::GetLocalAddressUDP(size_t instanceID)
{
	NetAddress returnMe; 
	const char * cCommand = "mn::GetLocalAddressUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceUDP()->GetLocalAddressUDP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the local UDP IP of the specified instance.
 *
 * @param instanceID Unique identifier for instance.
 * @return the local UDP IP of the specified instance.
 */
CPP_DLL const char * mn::GetLocalIPUDP(size_t instanceID)
{
	const char * returnMe = "";
	const char * cCommand = "mn::GetLocalIPUDP";

	try
	{
		returnMe = mn::GetLocalAddressUDP(instanceID).GetIP();
	}
	STD_CATCH
	
	return(returnMe);
}

/**
 * @brief Retrieves the local UDP port of the specified instance.
 *
 * @param instanceID Unique identifier for instance.
 * @return the local UDP port of the specified instance.
 */
DBP_CPP_DLL unsigned short mn::GetLocalPortUDP(size_t instanceID)
{
	unsigned short returnMe = 0;
	const char * cCommand = "mn::GetLocalPortUDP";

	try
	{
		returnMe = mn::GetLocalAddressUDP(instanceID).GetPort();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the TCP function set for the specified instance profile.
 *
 * This TCP function is called whenever data is received on the instance.
 * Care must be taken that the function is thread safe.
 * 
 *
 * @param [in]	profile			Instance profile to be used.
 *
 * @return  pointer to function that can be executed when TCP data is received (NULL or 0 if none, default is NetInstanceProfile::DEFAULT_RECV_FUNC).
 */
NetSocket::RecvFunc mn::GetProfileFunctionTCP( const NetInstanceProfile & profile )
{
	NetSocket::RecvFunc returnMe = NULL;
	const char * cCommand = "mn::GetProfileFunctionTCP";

	try
	{
		returnMe = profile.GetRecvFuncTCP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the UDP function set for the specified instance profile.
 *
 * This UDP function is called whenever data is received on the instance.
 * Care must be taken that the function is thread safe.
 * 
 *
 * @param [in]	profile			Instance profile to be used.
 *
 * @return  pointer to function that can be executed when UDP data is received (NULL or 0 if none, default is NetInstanceProfile::DEFAULT_RECV_FUNC).
 */
NetSocket::RecvFunc mn::GetProfileFunctionUDP( const NetInstanceProfile & profile )
{
	NetSocket::RecvFunc returnMe = NULL;
	const char * cCommand = "mn::GetProfileFunctionUDP";

	try
	{
		returnMe = profile.GetRecvFuncUDP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the TCP receive buffer size set for the specified instance profile.
 *
 * @param [in]	profile			Instance profile to use.
 *
 * @return  maximum size a received TCP packet can be without increasing the buffer size or throwing an error (default is NetInstanceProfile::DEFAULT_BUFFER_SIZE).
 * @return	0 if an error occurred. 
 */
size_t mn::GetProfileBufferSizeTCP( const NetInstanceProfile & profile )
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileBufferSizeTCP";

	try
	{
		returnMe = profile.GetRecvSizeTCP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the TCP receive buffer size set for the specified instance profile.
 *
 * @copydetails mn::GetProfileBufferSizeTCP(const NetInstanceProfile &)
 */
size_t mn::GetProfileBufferSizeTCP( INT_PTR profile )
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileBufferSizeTCP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::GetProfileBufferSizeTCP(auxProfile);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the UDP receive buffer size set for the specified instance profile.
 *
 * @param [in]	profile			Instance profile to use.
 *
 * @return  maximum size a received UDP packet can be without increasing the buffer size or throwing an error (default is NetInstanceProfile::DEFAULT_BUFFER_SIZE).
 * @return	0 if an error occurred. 
 */
size_t mn::GetProfileBufferSizeUDP( const NetInstanceProfile & profile )
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileBufferSizeUDP";

	try
	{
		returnMe = profile.GetRecvSizeUDP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the UDP receive buffer size set for the specified instance profile.
 *
 * @copydetails mn::GetProfileBufferSizeUDP(const NetInstanceProfile &)
 */
size_t mn::GetProfileBufferSizeUDP( INT_PTR profile )
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileBufferSizeUDP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::GetProfileBufferSizeUDP(auxProfile);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Determines whether UDP is enabled for the specified instance profile.
 *
 * When UDP is disabled all UDP commands on the specified instance will fail.
 * The handshaking process will be slightly faster and compatibility with
 * other applications will improve.\n\n
 *
 * Default return value is NetInstanceProfile::DEFAULT_ENABLED_UDP.
 *
 * @param [in]	profile		Instance profile to be used.
 *
 * @return  1 if UDP will be enabled.
 * @return  0 if UDP will be disabled.
 * @return	-1 if an error occurred. 
 */
int mn::GetProfileEnabledUDP( const NetInstanceProfile & profile )
{
	int returnMe = -1;
	const char * cCommand = "mn::GetProfileEnabledUDP";

	try
	{
		returnMe = profile.IsEnabledUDP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Determines whether UDP is enabled for the specified instance profile.
 *
 * @copybrief mn::GetProfileEnabledUDP()
 */
DBP_CPP_DLL int mn::GetProfileEnabledUDP( INT_PTR profile )
{
	int returnMe = -1;
	const char * cCommand = "mn::GetProfileEnabledUDP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::GetProfileEnabledUDP(auxProfile);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Determines whether the auto resize TCP option is enabled.
 *
 * When auto resize is true, if an incoming TCP packet is too large to be received,
 * instead of throwing an error or forcefully disconnecting that client, the TCP
 * receive buffer automatically resizes to allow for the TCP packet to be
 * received.
 *
 * @param [in]	profile		Instance profile to be used.
 *
 * @return  1 if auto resize is enabled.
 * @return  0 if auto resize is disabled.
 * @return	-1 if an error occurred. 
 */
int mn::GetProfileAutoResizeTCP( const NetInstanceProfile & profile )
{
	int returnMe = -1;
	const char * cCommand = "mn::GetProfileAutoResizeTCP";

	try
	{
		returnMe = profile.GetAutoResizeTCP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Determines whether the auto resize TCP option is enabled.
 *
 * @copydetails mn::GetProfileAutoResizeTCP(const NetInstanceProfile &)
 */
DBP_CPP_DLL	int mn::GetProfileAutoResizeTCP( INT_PTR profile )
{
	int returnMe = -1;
	const char * cCommand = "mn::GetProfileAutoResizeTCP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::GetProfileAutoResizeTCP(auxProfile);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Determines whether the TCP handshake process is enabled for the specified instance profile.
 *
 * For more information see @ref handshakePage "server/client handshaking process".\n\n
 *
 * Default return value is NetInstanceProfile::DEFAULT_HANDSHAKE_ENABLED.
 *
 * @param [in]	profile		Instance profile to be used.
 *
 * @return  1 if the handshake process should be enabled.
 * @return  0 if the handshake process should be disabled.
 * @return	-1 if an error occurred. 
 */
int mn::GetProfileHandshakeEnabled( const NetInstanceProfile & profile )
{
	int returnMe = -1;
	const char * cCommand = "mn::GetProfileHandshakeEnabled";

	try
	{
		returnMe = profile.IsHandshakeEnabled();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Determines whether the TCP handshake process is enabled for the specified instance profile.
 * @copydetails mn::GetProfileHandshakeEnabled(const NetInstanceProfile &)
 */
DBP_CPP_DLL	int mn::GetProfileHandshakeEnabled( INT_PTR profile )
{
	int returnMe = -1;
	const char * cCommand = "mn::GetProfileFunctionTCP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::GetProfileHandshakeEnabled(auxProfile);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the TCP mode for the specified instance profile.
 *
 * The TCP mode describes changes should be made to TCP packets
 * sent and how data is split into packets when received.
 *
 * @param [in]	profile		Instance profile to be used.
 *
 * @return  TCP mode option. Default is NetInstanceProfile::DEFAULT_MODE_TCP.
 */
NetMode::ProtocolMode mn::GetProfileModeTCP( const NetInstanceProfile & profile )
{
	NetMode::ProtocolMode returnMe;
	const char * cCommand = "mn::GetProfileFunctionTCP";

	try
	{
		returnMe = profile.GetModeTCP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the TCP mode for the specified instance profile.
 *
 * @copydoc mn::GetProfileModeTCP(const NetInstanceProfile &)
 */
DBP_CPP_DLL	NetMode::ProtocolMode mn::GetProfileModeTCP( INT_PTR profile )
{
	NetMode::ProtocolMode returnMe;
	const char * cCommand = "mn::GetProfileFunctionTCP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::GetProfileModeTCP(auxProfile);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the UDP mode for the specified instance profile.
 *
 * The UDP mode describes changes should be made to UDP packets
 * sent and how data is split into packets when received.
 *
 * @param [in]	profile		Instance profile to be used.
 *
 * @return  UDP mode option. Default is NetInstanceProfile::DEFAULT_MODE_UDP.
 */
NetMode::ProtocolMode mn::GetProfileModeUDP( const NetInstanceProfile & profile )
{
	NetMode::ProtocolMode returnMe;
	const char * cCommand = "mn::GetProfileFunctionUDP";

	try
	{
		returnMe = profile.GetModeUDP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the UDP mode for the specified instance profile.
 *
 * @copydoc mn::GetProfileModeUDP(const NetInstanceProfile &)
 */
DBP_CPP_DLL	NetMode::ProtocolMode mn::GetProfileModeUDP( INT_PTR profile )
{
	NetMode::ProtocolMode returnMe;
	const char * cCommand = "mn::GetProfileFunctionUDP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::GetProfileModeUDP(auxProfile);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Enables or disables the @ref gracefulDisconnectPage "graceful disconnect process"
 * for the specified instance profile.
 *
 * Default return value is NetInstanceProfile::DEFAULT_GRACEFUL_DISCONNECT.
 *
 * @param [in]	profile		Instance profile to be used.
 *
 * @return  1 if graceful disconnect process will be enabled.
 * @return  0 if graceful disconnect process will be disabled.
 * @return	-1 if an error occurred.
 */
int mn::GetProfileGracefulDisconnectEnabled( const NetInstanceProfile & profile )
{
	int returnMe = -1;
	const char * cCommand = "mn::GetProfileFunctionTCP";

	try
	{
		returnMe = profile.IsGracefulDisconnectEnabled();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Enables or disables the @ref gracefulDisconnectPage "graceful disconnect process"
 * for the specified instance profile.
 *
 * @copydetails mn::GetProfileGracefulDisconnectEnabled(const NetInstanceProfile &)
 */
DBP_CPP_DLL	int mn::GetProfileGracefulDisconnectEnabled( INT_PTR profile )
{
	int returnMe = -1;
	const char * cCommand = "mn::GetProfileFunctionTCP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::GetProfileGracefulDisconnectEnabled(auxProfile);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the send timeout option for the specified instance profile. Send operations
 * are allowed this length of time to complete before the entity that initiated them
 * is forcefully disconnected.
 *
 * @param [in]	profile		Instance profile to be used.
 *
 * @return  length of time in milliseconds to wait for a send operation to complete before
 * disconnecting client (default = NetInstanceProfile::DEFAULT_SEND_TIMEOUT).
 */
size_t mn::GetProfileSendTimeout( const NetInstanceProfile & profile )
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileFunctionTCP";

	try
	{
		returnMe = profile.GetSendTimeout();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the send timeout option for the specified instance profile. Send operations
 * are allowed this length of time to complete before the entity that initiated them
 * is forcefully disconnected.
 *
 * @copydetails mn::GetProfileSendTimeout(const NetInstanceProfile &)
 */
DBP_CPP_DLL size_t mn::GetProfileSendTimeout( INT_PTR profile )
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileFunctionTCP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::GetProfileSendTimeout(auxProfile);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the postfix option of the specified instance profile.
 *
 * This option only apples to Instances in NetMode::TCP_POSTFIX mode.
 * @param [in]	profile		Instance profile to be used.
 *
 * @return  postfix, default is NetInstanceProfile::DEFAULT_POSTFIX_TCP.
 */
Packet mn::GetProfilePostfixTCP( const NetInstanceProfile & profile )
{
	Packet returnMe;
	const char * cCommand = "mn::GetProfileFunctionTCP";

	try
	{
		returnMe = profile.GetPostFixTCP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the postfix option of the specified instance profile.
 * This option only apples to instances in NetMode::TCP_POSTFIX mode.
 * @param [in]	profile		Instance profile to be used.
 * @param [out] destination Destination packet to store postfix, any existing contents
 * will be overwritten. Default is NetInstanceProfile::DEFAULT_POSTFIX_TCP.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL	int mn::GetProfilePostfixTCP( INT_PTR profile, INT_PTR destination )
{
	int returnMe = 0;
	const char * cCommand = "mn::GetProfileFunctionTCP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		Packet & auxDestination = PointerConverter::GetRefFromInt<Packet>(destination);
		auxDestination = mn::GetProfilePostfixTCP(auxProfile);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Retrieves the nagle algorithm option from the specified instance profile.
 *
 * The default return value for this command is NetInstanceProfile::DEFAULT_NAGLE_ENABLED.
 * 
 * @param [in] profile	Instance profile to be used.
 *
 * @return 1 if the nagle algorithm is enabled.
 * @return 0 if the nagle algorithm is disabled.
 * @return -1 if an error occurred.
 */
int mn::GetProfileNagleEnabledTCP( const NetInstanceProfile & profile )
{
	int returnMe = -1;
	const char * cCommand = "mn::GetProfileFunctionTCP";

	try
	{
		returnMe = profile.IsNagleEnabled();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the nagle algorithm option from the specified instance profile.
 *
 * @copydetails mn::GetProfileNagleEnabledTCP(const NetInstanceProfile &)
 */
DBP_CPP_DLL	int mn::GetProfileNagleEnabledTCP( INT_PTR profile )
{
	int returnMe = -1;
	const char * cCommand = "mn::GetProfileFunctionTCP";

	try
	{
		const NetInstanceProfile & aux = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = mn::GetProfileNagleEnabledTCP(aux);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Creates a new instance profile, filled with default options.
 *
 * An instance profile is an object that contains a variety of options
 * specific to instances. Any instance can have an instance profile loaded into
 * it during initialization. Each option has a default value, which often does not
 * need to be changed. By using instance profiles lengthy constructors are avoided.
 * As well as this, similar profiles with similar settings can use the same profile;
 * this is a neat way of sharing options between instances.
 *
 * @return instance profile with default settings.
 */
INT_PTR mn::CreateInstanceProfile()
{
	const char * cCommand = "mn::CreateInstanceProfile";
	NetInstanceProfile * profile = NULL;
	try
	{
		profile = new (nothrow) NetInstanceProfile();
		Utility::DynamicAllocCheck(profile,__LINE__,__FILE__);
	}
	STD_CATCH
	return (INT_PTR)profile;
}

/**
 * @brief Deletes the specified instance profile.
 *
 * @param profile Profile to delete.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
int mn::DeleteInstanceProfile(INT_PTR profile)
{
	const char * cCommand = "mn::DeleteInstanceProfile";
	int returnMe = 0;

	try
	{
		NetInstanceProfile * ptr = PointerConverter::GetPtrFromInt<NetInstanceProfile>(profile);
		delete ptr;
	}
	STD_CATCH_RM
	return returnMe;
}


/**
 * @brief Creates a new instance profile, copying the data of a pre-existing instance profile.
 *
 * @param profile Instance profile to copy.
 * @return new Instance profile.
 */
INT_PTR mn::CreateInstanceProfileFrom(INT_PTR profile)
{
	const char * cCommand = "mn::CreateInstanceProfileFrom";

	NetInstanceProfile * returnMe;
	try
	{
		NetInstanceProfile & refProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = new (nothrow) NetInstanceProfile(refProfile);
		Utility::DynamicAllocCheck(returnMe,__LINE__,__FILE__);
	}
	STD_CATCH
	return (INT_PTR)returnMe;
}

/**
 * @brief Assigns the contents of a source profile into a destination profile.
 *
 * @param dest Location to copy into.
 * @param source Location to copy from.
 * @return @a dest.
 */
INT_PTR mn::AssignInstanceProfile(INT_PTR dest, INT_PTR source)
{
	const char * cCommand = "mn::AssignInstanceProfile";

	try
	{
		NetInstanceProfile & refDest = PointerConverter::GetRefFromInt<NetInstanceProfile>(dest);
		NetInstanceProfile & refSource = PointerConverter::GetRefFromInt<NetInstanceProfile>(source);
		
		refDest = refSource;
	}
	STD_CATCH
	return (INT_PTR)dest;
}

/**
 * @brief Compares @a profile1 and @a profile2.
 *
 * @param profile1 An instance profile to compare with @a profile2.
 * @param profile2 An instance profile to compare with @a profile1.
 *
 * @return 1 if @a profile1 and @a profile2 are identical.
 * @return 0 if @a profile1 and @a profile2 are different in any way.
 * @return -1 if an error occurred.
 */
int mn::CompareInstanceProfile(INT_PTR profile1, INT_PTR profile2)
{
	const char * cCommand = "mn::CompareInstanceProfile";

	int returnMe = -1;
	try
	{
		NetInstanceProfile & refProfile1 = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile1);
		NetInstanceProfile & refProfile2 = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile2);

		returnMe = (refProfile1 == refProfile2);
	}
	STD_CATCH
	return returnMe;
}





















/**
 * @brief Adds an unsigned integer to the specified packet.
 *
 * The data is is copied into the packet's data buffer and is stores as a series of bytes.
 * Data is added at the current cursor position (mn::GetCursor) and the cursor position
 * is increased by the size of the data added. Used size is increased by the size of the data
 * added assuming the data is added at the end of the packet; if not, used size is only
 * increased if more memory is used. Memory size is increased if necessary.
 *
 * @param packet %Packet that data should be added to.
 * @param add Data to add to the packet.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::AddUnsignedInt(INT_PTR packet, unsigned int add)
{
	int returnMe = 0; 
	const char * cCommand = "mn::AddUnsignedInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Add(add);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Adds an integer to the specified packet.
 *
 * @copydoc mn::AddUnsignedInt()
 */
DBP_CPP_DLL int mn::AddInt(INT_PTR packet, int add)
{
	int returnMe = 0;
	const char * cCommand = "mn::AddInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Add(add);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Adds a long integer to the specified packet.
 *
 * @copydoc mn::AddUnsignedInt()
 */
CPP_DLL int mn::AddLongInt(INT_PTR packet, long int add)
{
	int returnMe = 0; 
	const char * cCommand = "mn::AddLongInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Add(add);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Adds an long long integer to the specified packet.
 *
 * @copydoc mn::AddUnsignedInt()
 */
DBP_CPP_DLL int mn::AddLongLongInt(INT_PTR packet, long long int add)
{
	int returnMe = 0; 
	const char * cCommand = "mn::AddLongLongInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Add(add);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Adds a float to the specified packet.
 *
 * @copydoc mn::AddUnsignedInt()
 */
DBP_CPP_DLL int mn::AddFloat(INT_PTR packet, float add)
{
	int returnMe = 0; 
	const char * cCommand = "mn::AddFloat";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Add(add);
	}
	STD_CATCH_RM

	return(returnMe);
	
}

/**
 * @brief Adds an unsigned byte to the specified packet.
 *
 * @copydoc mn::AddUnsignedInt()
 */
DBP_CPP_DLL int mn::AddUnsignedByte(INT_PTR packet, unsigned char add)
{
	int returnMe = 0; 
	const char * cCommand = "mn::AddUnsignedByte";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Add(add);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Adds a byte to the specified packet.
 *
 * @copydoc mn::AddUnsignedInt()
 */
CPP_DLL int mn::AddByte(INT_PTR packet, char add)
{
	int returnMe = 0; 
	const char * cCommand = "mn::AddByte";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Add(add);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Adds a signed byte to the specified packet.
 *
 * @copydoc mn::AddUnsignedInt()
 */
CPP_DLL int mn::AddSignedByte(INT_PTR packet, signed char add)
{
	int returnMe = 0; 
	const char * cCommand = "mn::AddSignedByte";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Add(add);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Adds a double to the specified packet.
 *
 * @copydoc mn::AddUnsignedInt()
 */
DBP_CPP_DLL int mn::AddDouble(INT_PTR packet, double add)
{
	int returnMe = 0; 
	const char * cCommand = "mn::AddDouble";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Add(add);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Adds a long double to the specified packet.
 *
 * @copydoc mn::AddUnsignedInt()
 */
CPP_DLL int mn::AddLongDouble(INT_PTR packet, long double add)
{
	int returnMe = 0; 
	const char * cCommand = "mn::AddLongDouble";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Add(add);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Adds a short integer to the specified packet.
 *
 * @copydoc mn::AddUnsignedInt()
 */
CPP_DLL int mn::AddShortInt(INT_PTR packet, short int add)
{
	int returnMe = 0; 
	const char * cCommand = "mn::AddShortInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Add(add);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Adds an unsigned short integer to the specified packet.
 *
 * @copydoc mn::AddUnsignedInt()
 */
DBP_CPP_DLL int mn::AddUnsignedShortInt(INT_PTR packet, unsigned short int add)
{
	int returnMe = 0; 
	const char * cCommand = "mn::AddUnsignedShortInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Add(add);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Adds an unsigned short integer to the specified packet.
 *
 * @copydoc mn::AddUnsignedInt()
 *
 * Use this command when adding client IDs and operation IDs in NetMode::UDP_PER_CLIENT and NetMode::UDP_PER_CLIENT_PER_OPERATION.
 */
DBP_CPP_DLL int mn::AddSizeT(INT_PTR packet, size_t add)
{
	int returnMe = 0; 
	const char * cCommand = "mn::AddSizeT";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.AddSizeT(add);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Adds an unsigned long int to the specified packet.
 *
 * @copydetails mn::AddUnsignedInt()
 */
CPP_DLL int mn::AddUnsignedLongInt(INT_PTR packet, unsigned long int add)
{
	int returnMe = 0; 
	const char * cCommand = "mn::AddUnsignedLongInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Add(add);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Adds an unsigned long long integer to the specified packet.
 *
 * @copydoc mn::AddUnsignedInt()
 */
CPP_DLL int mn::AddUnsignedLongLongInt(INT_PTR packet, unsigned long long int add)
{
	int returnMe = 0; 
	const char * cCommand = "mn::AddUnsignedLongLongInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Add(add);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Adds a C string to the specified packet.
 *
 * The data is is copied into the packet's data buffer and is stores as a series of bytes.
 * Data is added at the current cursor position (mn::GetCursor) and the cursor position
 * is increased by the size of the data added. Used size is increased by the size of the data
 * added assuming the data is added at the end of the packet; if not, used size is only
 * increased if more memory is used. Memory size is increased if necessary.
 *
 * @param [out] packet %Packet to add data to.
 * @param [in] source String to add.
 * @param length If 0 @a source must be a null terminated string, otherwise length
 * must be the number of bytes of @a source that should be added. Note that if the string
 * is null terminated, the null terminator is not added to the packet.
 * @param prefix If true a prefix will be added to the packet, indicating the size
 * of the remaining string. On the receiving end this prefix can be used by specifying
 * a length of 0 using mn::GetStringC. This removes the need to know the size of the string
 * before extracting it.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::AddStringC(INT_PTR packet, const char * source, size_t length, bool prefix)
{
	int returnMe = 0; 
	const char * cCommand = "mn::AddStringC";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.AddStringC(source,length,prefix);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Calculates the number of unread bytes in the specified packet.
 *
 * @return the number of unread bytes in the specified packet.
 * The calculation used is mn::GetUsedSize() - mn::GetCursor().
 */
DBP_CPP_DLL size_t mn::GetPacketRemainder(INT_PTR packet)
{
	size_t returnMe = 0; 
	const char * cCommand = "mn::GetPacketRemainder";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.GetPacketRemainder();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves an unsigned integer from the specified packet.
 *
 * Data is retrieved from the packet's data buffer starting at the cursor position (mn::GetCursor).
 * The cursor position is moved along by the amount of data extracted.
 * Used size and memory size are not changed.
 *
 * @param packet %Packet to extract data from.
 * @return extracted data.
 * @exception ErrorReport If retrieving data would cause the cursor position to be more than the used size.
 */
DBP_CPP_DLL unsigned int mn::GetUnsignedInt(INT_PTR packet)
{
	unsigned int returnMe = 0; 
	const char * cCommand = "mn::GetUnsignedInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.Get<unsigned int>();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves an integer from the specified packet.
 *
 * @copydetails mn::GetUnsignedInt()
 */
DBP_CPP_DLL int mn::GetInt(INT_PTR packet)
{
	int returnMe = -1; 
	const char * cCommand = "mn::GetInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.Get<int>();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves a long integer from the specified packet.
 *
 * @copydetails mn::GetUnsignedInt()
 */
CPP_DLL long int mn::GetLongInt(INT_PTR packet)
{
	long int returnMe = -1; 
	const char * cCommand = "mn::GetLongInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.Get<long int>();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves a long long integer from the specified packet.
 *
 * @copydetails mn::GetUnsignedInt()
 */
DBP_CPP_DLL long long int mn::GetLongLongInt(INT_PTR packet)
{
	long long int returnMe = -1; 
	const char * cCommand = "mn::GetLongLongInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.Get<long long int>();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves a float from the specified packet.
 *
 * @copydetails mn::GetUnsignedInt()
 */
CPP_DLL float mn::GetFloat(INT_PTR packet)
{
	float returnMe = -1; 
	const char * cCommand = "mn::GetFloat";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.Get<float>();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves an unsigned byte from the specified packet.
 *
 * @copydetails mn::GetUnsignedInt()
 * @note RC table ensures mn::GetByte = mn::GetUnsignedByte in DarkBASIC Pro.
 */
DBP_CPP_DLL unsigned char mn::GetUnsignedByte(INT_PTR packet)
{
	unsigned char returnMe = 0;
	const char * cCommand = "mn::GetUnsignedByte";
 
	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.Get<unsigned char>();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves a byte from the specified packet.
 *
 * @copydetails mn::GetUnsignedInt()
 */
CPP_DLL char mn::GetByte(INT_PTR packet)
{
	char returnMe = 0; 
	const char * cCommand = "mn::GetByte";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.Get<char>();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves a signed byte from the specified packet.
 *
 * @copydetails mn::GetUnsignedInt()
 */
CPP_DLL signed char mn::GetSignedByte(INT_PTR packet)
{
	signed char returnMe = 0;
	const char * cCommand = "mn::GetSignedByte";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.Get<signed char>();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves a double from the specified packet.
 *
 * @copydetails mn::GetUnsignedInt()
 */
DBP_CPP_DLL double mn::GetDouble(INT_PTR packet)
{
	double returnMe = -1; 
	const char * cCommand = "mn::GetDouble";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.Get<double>();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves a long double from the specified packet.
 *
 * @copydetails mn::GetUnsignedInt()
 */
CPP_DLL long double mn::GetLongDouble(INT_PTR packet)
{
	long double returnMe = -1; 
	const char * cCommand = "mn::GetLongDouble";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.Get<long double>();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves a short integer from the specified packet.
 *
 * @copydetails mn::GetUnsignedInt()
 */
CPP_DLL short int mn::GetShortInt(INT_PTR packet)
{
	short int returnMe = -1; 
	const char * cCommand = "mn::GetShortInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.Get<short int>();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves an unsigned short integer from the specified packet.
 *
 * @copydetails mn::GetUnsignedInt()
 */
DBP_CPP_DLL unsigned short int mn::GetUnsignedShortInt(INT_PTR packet)
{
	unsigned short int returnMe = 0; 
	const char * cCommand = "mn::GetUnsignedShortInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.Get<unsigned short int>();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the size of a string by reading its prefix.
 *
 * The string must have a prefix or this command will return meaningless values.\n\n
 *
 * The prefix is read from the packet's data buffer starting at the cursor position (mn::GetCursor).
 * The cursor position is not changed by this command;
 * this means that you can use this command and straight away use mn::GetStringC().
 *
 * @param packet %Packet to extract data from.
 *
 * @return the size of the string.
 */
DBP_CPP_DLL size_t mn::GetStringSize(INT_PTR packet)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetStringSize";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.GetStringSize();
	}
	STD_CATCH

	return returnMe;
}

/**
 * @brief	Encrypts the specified packet.
 *
 * Note that you must not read or write data to the packet while the operation is in progress.
 *
 * @param	packet	The packet to use.
 * @param	key		The key to use.
 * @param	block	If true the command will not return until the packet is fully encrypted.
 * If false the command will return straight away and mn::GetLastEncryptionOperationFinished
 * should be used to determine when it has finished its operation.
 *
 * @return	0 if no error occurred.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::Encrypt( INT_PTR packet, INT_PTR key, bool block )
{
	int returnMe = 0;
	const char * cCommand = "mn::Encrypt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		EncryptKey & refKey = PointerConverter::GetRefFromInt<EncryptKey>(key);
		ref.Encrypt(refKey,block);
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Decrypts the specified packet.
 *
 * Note that you must not read or write data to the packet while the operation is in progress.
 *
 * @param	packet	The packet to use.
 * @param	key		The key to use.
 * @param	block	If true the command will not return until the packet is fully decrypted.
 * If false the command will return straight away and mn::GetLastEncryptionOperationFinished
 * should be used to determine when it has finished its operation.
 *
 * @return	0 if no error occurred.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::Decrypt( INT_PTR packet, INT_PTR key, bool block )
{
	int returnMe = 0;
	const char * cCommand = "mn::Decrypt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		EncryptKey & refKey = PointerConverter::GetRefFromInt<EncryptKey>(key);
		ref.Decrypt(refKey,block);
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Determines whether the last encryption or decryption operation
 * that was started on the specified packet has finished.
 *
 * @param	packet	The packet to use. 
 *
 * @return	1 if the operation has finished.
 * @return	0 if the operation is still in progress.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::GetLastEncryptionOperationFinished(INT_PTR packet)
{
	int returnMe = -1;
	const char * cCommand = "mn::IsLastEncryptionOperationFinished";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.IsLastEncryptionOperationFinished();
	}
	STD_CATCH

	return returnMe;
}

/**
 * @brief Retrieves a C string from the specified packet, allocating new memory to that string.
 *
 * Data is retrieved from the packet's data buffer starting at the cursor position (mn::GetCursor).
 * The cursor position is moved along by the amount of data extracted.
 * Used size and memory size are not changed.
 *
 * @param packet %Packet to extract data from.
 * @param length If 0 it is assumed that @a packet has a prefix
 * indicating the size of the remaining data in the packet. This prefix
 * is added by mn::AddStringC when prefix is set to true. If >0
 * then this is the number of bytes that will be read from the packet.
 * @param nullTerminated If true a null terminator is appended to the returned string.
 *
 * @return extracted data. This must be deallocated using delete[] or mn::FreeString. NULL if the data could not be extracted.
 * @exception ErrorReport If retrieving data would cause the cursor position to be more than the used size.
  */
CPP_DLL char * mn::GetStringC(INT_PTR packet, size_t length, bool nullTerminated)
{
	char * returnMe = NULL; 
	const char * cCommand = "mn::GetStringC";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.GetStringC(length,nullTerminated);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves a C string from the specified packet without the need to deallocate the returned memory.
 *
 * Data is retrieved from the packet's data buffer starting at the cursor position (mn::GetCursor).
 * The cursor position is moved along by the amount of data extracted.
 * Used size and memory size are not changed.
 *
 * @param packet %Packet to extract data from.
 * @param length If 0 it is assumed that @a packet has a prefix
 * indicating the size of the remaining data in the packet. This prefix
 * is added by mn::AddStringC when prefix is set to true. If >0
 * then this is the number of bytes that will be read from the packet.
 * @param nullTerminated If true a null terminator is appended to the returned string.
 *
 * @return extracted data. Unlike other variations of GetStringC, the returned memory is managed by the API and should
 * not be deallocated. The memory pointed to by the returned pointer is guarenteed to remain valid only until the next
 * GetStringC_C call. Note that this function is not thread safe.
 * @exception ErrorReport If retrieving data would cause the cursor position to be more than the used size.
  */
CPP_DLL char * mn::GetStringC_C(INT_PTR packet, size_t length, bool nullTerminated)
{
	static char * lastCall = NULL;

	if(lastCall != NULL)
	{
		delete[] lastCall;
	}

	lastCall = GetStringC(packet,length,nullTerminated);
	return lastCall;
}

/**
 * @brief Retrieves a C string from the specified packet, copying into previously allocated memory.
 *
 * Data is retrieved from the packet's data buffer starting at the cursor position (mn::GetCursor).
 * The cursor position is moved along by the amount of data extracted.
 * Used size and memory size are not changed.
 *
 * @param packet %Packet to extract data from.
 * @param dest String to copy extracted data to.
 * @param length If 0 it is assumed that @a packet has a prefix
 * indicating the size of the remaining data in the packet. This prefix
 * is add by mn::AddStringC when prefix is set to true. If >0
 * then this is the number of bytes that will be read from the packet.
 * @param nullTerminated If true a null terminator is appended to the returned string.
 * Note that mn::AddStringC does not ever add a null terminator.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 *
 * @exception ErrorReport If retrieving data would cause the cursor position to be more than the used size.
 */
CPP_DLL int mn::GetStringC_B(INT_PTR packet, char * dest, size_t length, bool nullTerminated)
{
	int returnMe = 0; 
	const char * cCommand = "mn::GetStringC";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.GetStringC(dest,length,nullTerminated);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Retrieves an unsigned long integer from the specified packet.
 *
 * @copydetails mn::GetUnsignedInt()
 */
CPP_DLL unsigned long int mn::GetUnsignedLongInt(INT_PTR packet)
{
	unsigned long int returnMe = 0; 
	const char * cCommand = "mn::GetUnsignedLongInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.Get<unsigned long int>();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves an unsigned long long integer from the specified packet.
 *
 * @copydetails mn::GetUnsignedInt()
 */
CPP_DLL unsigned long long int mn::GetUnsignedLongLongInt(INT_PTR packet)
{
	unsigned long long int returnMe = 0; 
	const char * cCommand = "mn::GetUnsignedLongLongInt";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.Get<unsigned long long int>();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves an unsigned long long integer from the specified packet.
 *
 * @copydetails mn::GetUnsignedInt()
 */
DBP_CPP_DLL size_t mn::GetSizeT(INT_PTR packet)
{
	size_t returnMe = 0; 
	const char * cCommand = "mn::GetSizeT";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.GetSizeT();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the clock value of the specified packet, indicating the packet's age.
 *
 * @copydoc Packet::GetAge()
 *
 * @param packet %Packet to use.
 */
DBP_CPP_DLL size_t mn::GetAge(INT_PTR packet)
{
	size_t returnMe = 0; 
	const char * cCommand = "mn::GetClock";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.GetAge();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the operation ID of the specified packet, indicating what operation the packet was received on.
 *
 * @param packet %Packet to use.
 * @return operation that packet was received via. This is set when receiving in NetMode::UDP_PER_CLIENT_PER_OPERATION.
 */
DBP_CPP_DLL size_t mn::GetOperation(INT_PTR packet)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetOperation";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.GetOperation();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the instance ID of the specified packet, indicating the ID of the instance that the packet was received on.
 *
 * @param packet %Packet to use.
 * @copydoc Packet::GetInstance()
 */
DBP_CPP_DLL size_t mn::GetInstance(INT_PTR packet)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetInstance";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.GetInstance();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Empties the specified packet, erasing all data stored.
 *
 * Memory size is unchanged.
 * The main actions taken are to reset cursor position and used size,
 * meaning that this operation is very efficient.
 *
 * @param packet %Packet to use.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 *
 */
DBP_CPP_DLL int mn::ClearPacket(INT_PTR packet)
{
	int returnMe = 0; 
	const char * cCommand = "mn::ClearPacket";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Clear();
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Combines two packets together.
 *
 * @param dest %Packet that @a source should be added to.
 * @param source %Packet that should be added to @a dest.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::AddPacket(INT_PTR dest, INT_PTR source)
{
	int returnMe = 0;
	const char * cCommand = "mn::AddPacket";

	try
	{
		Packet & refDest = PointerConverter::GetRefFromInt<Packet>(dest);
		Packet & refSource = PointerConverter::GetRefFromInt<Packet>(source);
		
		refDest += refSource;
	}
	STD_CATCH_RM
	return(returnMe);
}

/**
 * @brief Assigns a source packet to another destination packet, overwriting anything in the destination packet.
 *
 * @param dest %Packet that @a source should be copied into.
 * @param source %Packet that should be copied into @a dest.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::AssignPacket(INT_PTR dest, INT_PTR source)
{
	int returnMe = 0;
	const char * cCommand = "mn::AssignPacket";

	try
	{
		Packet & refDest = PointerConverter::GetRefFromInt<Packet>(dest);
		Packet & refSource = PointerConverter::GetRefFromInt<Packet>(source);
		
		refDest = refSource;
	}
	STD_CATCH_RM
	return(returnMe);
}

/**
 * @brief Compares two packets.
 *
 * @param packet1 %Packet to compare with @a packet2.
 * @param packet2 %Packet to compare with @a packet1.
 * 
 * @return 1 if @a packet1 is identical to @a packet2.
 * @return 0 if @a packet1 is different in any way to @a packet2.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::ComparePacket(INT_PTR packet1, INT_PTR packet2)
{
	int returnMe = -1;
	const char * cCommand = "mn::ComparePacket";

	try
	{
		Packet & refPacket1 = PointerConverter::GetRefFromInt<Packet>(packet1);
		Packet & refPacket2 = PointerConverter::GetRefFromInt<Packet>(packet2);

		returnMe = (refPacket1 == refPacket2);
	}
	STD_CATCH
	return(returnMe);
}

/**
 * @brief Creates a new empty packet.
 *
 * @return new empty packet.
 */
DBP_CPP_DLL INT_PTR mn::CreatePacket()
{
	INT_PTR returnMe = -1;
	const char * cCommand = "mn::CreatePacket";

	try
	{
		// Create packet
		Packet * ptr = new (nothrow) Packet;
		Utility::DynamicAllocCheck(ptr,__LINE__,__FILE__);

		// Store address in integer
		returnMe = (INT_PTR)ptr;
	}
	STD_CATCH

	// returnMe address
	return(returnMe);
}

/**
 * @brief Deletes the specified packet.
 *
 * @param packet %Packet to delete.
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::DeletePacket(INT_PTR packet)
{
	int returnMe = 0;
	const char * cCommand = "mn::DeletePacket";

	try
	{
		Packet * ptr = PointerConverter::GetPtrFromInt<Packet>(packet);
		delete ptr;
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Changes the memory size of the packet, overwriting any packet data in the process.
 *
 * %Packet options such as clock value or instanceID are not lost after using this command.
 * Only memory size, cursor position and used size are changes.\n\n
 *
 * The memory size determines how much data can be stored in the packet.
 * Although the packet automatically increases in memory size as data is added,
 * it is best to set the memory size initially to avoid unnecessary reallocation.
 *
 * @param packet %Packet to use.
 * @param size New memory size.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::SetMemorySize(INT_PTR packet, size_t size)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetMemorySize";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.SetMemorySize(size);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Retrieves the memory size of the packet.
 *
 * The memory size determines how much data can be stored in the packet.
 * Although the packet automatically increases in memory size as data is added,
 * it is best to set the memory size initially to avoid unnecessary reallocation.
 *
 * @param packet %Packet to use.
 *
 * @return  the memory size of the packet.
 */
DBP_CPP_DLL size_t mn::GetMemorySize(INT_PTR packet)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetMemorySize";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.GetMemorySize();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Changes the used size of the packet.
 *
 * The used size indicates how much of the memory size is in use.
 * If an attempt is made to change the used size to be more than 
 * the memory size, then the memory size is increased as necessary.
 *
 * @param packet %Packet to use.
 * @param size New used size.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::SetUsedSize(INT_PTR packet, size_t size)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetUsedSize";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.SetUsedSize(size);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Retrieves the used size of the packet.
 *
 * The used size indicates how much of the memory size is in use.
 *
 * @param packet %Packet to use.
 *
 * @return  the used size.
 */
DBP_CPP_DLL size_t mn::GetUsedSize(INT_PTR packet)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetUsedSize";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.GetUsedSize();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Changes the cursor position of the specified packet.
 *
 * If the new cursor position is more than
 * the used size then an error will occur.\n\n
 *
 * The cursor indicates where Get commands that extract data should
 * begin reading from and where add commands that add data should
 * begin adding data.
 *
 * @param packet %Packet to use.
 * @param cursor New cursor position.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::SetCursor(INT_PTR packet, size_t cursor)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetCursor";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.SetCursor(cursor);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Retrieves the cursor position of the specified packet.
 *
 * The cursor indicates where Get commands that extract data should
 * begin reading from and where add commands that add data should
 * begin adding data.
 *
 * @param packet %Packet to use.
 *
 * @return the cursor position.
 */
DBP_CPP_DLL size_t mn::GetCursor(INT_PTR packet)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetCursor";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = ref.GetCursor();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Erases part of the packet, decreasing used size and cursor position by the amount erased.
 * 
 * @param packet %Packet to use.
 * @param startPos Position to start erasing data.
 * @param amount Number of bytes to erase.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::Erase(INT_PTR packet, size_t startPos, size_t amount)
{
	int returnMe = 0;
	const char * cCommand = "mn::Erase";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Erase(startPos, amount);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Inserts an empty space in the packet at the cursor position, increasing used size by the amount inserted.
 *
 * Cursor position is not changed, which means that data can be written to the empty space straight away
 * without worrying about the cursor.
 *
 * @param packet %Packet to use.
 * @param amount Number of bytes to insert at cursor position.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::Insert(INT_PTR packet, size_t amount)
{
	int returnMe = 0;
	const char * cCommand = "mn::Insert";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.Insert(amount);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Creates a new packet, copying the contents of another packet.
 *
 * @param fromPacket %Packet to copy.
 * @return a new packet created based on @a fromPacket, this packet is identical to @a fromPacket.
 */
DBP_CPP_DLL INT_PTR mn::CreatePacketFromPacket(INT_PTR fromPacket)
{
	INT_PTR returnMe = NULL;
	const char * cCommand = "mn::CreatePacketFromPacket";

	try
	{
		// Get reference
		Packet & refFromPacket = PointerConverter::GetRefFromInt<Packet>(fromPacket);

		// Create packet using refFromPacket
		Packet * ptr = new (nothrow) Packet(refFromPacket);
		Utility::DynamicAllocCheck(ptr,__LINE__,__FILE__);

		// Store address in integer
		returnMe = (INT_PTR)ptr;
	}
	STD_CATCH

	// returnMe address
	return(returnMe);
}

/**
 * @brief Creates a new packet, copying the contents of a C string.
 *
 * @param str String to copy.
 * @return a new packet created based on @a str, this packet contains @a str.
 */
DBP_CPP_DLL INT_PTR mn::CreatePacketFromString(const char * str)
{
	INT_PTR returnMe = NULL;
	const char * cCommand = "mn::CreatePacketFromString";

	try
	{
		// Create packet using ptrFromPacket
		Packet * ptr = new (nothrow) Packet(str);
		Utility::DynamicAllocCheck(ptr,__LINE__,__FILE__);

		// Store address in integer
		returnMe = (INT_PTR)ptr;
	}
	STD_CATCH

	// returnMe address
	return(returnMe);
}

/**
 * @brief Compares a packet with a C string.
 * 
 * @param packet %Packet to use.
 * @param str NULL terminated string to use.
 * 
 * @return 1 if @a packet contains @a str and if the used size of @a packet is equal to the length of @a str.
 * @return 0 if @a packet doesn't contain @a str or if @a str length is not equal to @a packet used size.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::ComparePacketString(INT_PTR packet, const char * str)
{
	int returnMe = -1;
	const char * cCommand = "mn::ComparePacketString";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		returnMe = (ref == str);
	}
	STD_CATCH
	return(returnMe);
}

/**
 * @brief Changes the memory size of the specified packet, without overwriting any packet data in the process.
 *
 * Only memory size, cursor position and used size are changed.\n\n
 *
 * The memory size determines how much data can be stored in the packet.
 * Although the packet automatically increases in memory size as data is added,
 * it is best to set the memory size initially to avoid unnecessary reallocation.
 *
 * @param packet %Packet to use.
 * @param size New memory size.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::ChangeMemorySize(INT_PTR packet, size_t size)
{
	int returnMe = 0;
	const char * cCommand = "mn::ChangeMemorySize";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(packet);
		ref.ChangeMemorySize(size);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Assigns a C string to the specified packet.
 *
 * @a destination's used size will equal @a from's string length.
 * The contents of @a destination will equal the contents of @a from.
 *
 * @param destination %Packet to use.
 * @param from NULL terminated string to use.
 *
 * @return  0 if the command completed successfully.
 * @return	-1 if an error occurred.
 */
DBP_CPP_DLL int mn::AssignPacketString(INT_PTR destination, const char * from)
{
	int returnMe = 0;
	const char * cCommand = "mn::AssignPacketString";

	try
	{
		Packet & ref = PointerConverter::GetRefFromInt<Packet>(destination);
		ref = from;
	}
	STD_CATCH_RM
	return(returnMe);
}

/**
 * @brief	Determines if an error is saved globally.
 *
 * Errors are only saved globally when ErrorReport::EM_SAVE is enabled.
 *
 * @return	true if an error is saved, false if not.
 */
CPP_DLL bool mn::GetErrorFlag()
{
	return ErrorReport::IsErrorSaved();
}

/**
 * @brief	Retrieves the operation that was being performed when the error that is saved globally occurred.
 *
 * @return	the operation that was being performed when the error occurred.
 * @return	NULL if none is loaded.
 */
CPP_DLL const char * mn::GetErrorOperation()
{
	return ErrorReport::GetSavedError().GetOperation();
}

/**
 * @brief	Retrieves the command that caused the error that is saved globally.
 *
 * @return	the command that caused the error.
 * @return	NULL if none is loaded.
 */
CPP_DLL const char * mn::GetErrorCommand()
{
	return ErrorReport::GetSavedError().GetCommand();
}

/**
 * @brief	Retrieves the name of the file that the error that is saved globally occurred in.
 *
 * @return	the name of the file that the error occurred in.
 * @return	NULL if none is loaded.
 */
CPP_DLL const char * mn::GetErrorFile()
{
	return ErrorReport::GetSavedError().GetFileName();
}

/**
 * @brief	Generates an error message based on the global error report.
 *
 * @return	an error message. When done with this string you should deallocate it using delete[].
 */
CPP_DLL const char * mn::GetErrorFull()
{
	return ErrorReport::GetSavedError().GetFullMessage();
}

/**
 * @brief	Retrieves the error code associated with the error that is saved globally.
 *
 * @return	the error code.
 * @return	0 if none is loaded.
 */
DBP_CPP_DLL __int64 mn::GetErrorCode()
{
	return ErrorReport::GetSavedError().GetErrorCode();
}

/**
 * @brief	Retrieves the error code associated with the error that is saved globally.
 *
 * This method is only needed for languages which don't support DLL calls with 64 bit integer
 * parameters or return values.
 *
 * @return	the error code.
 * @return	0 if none is loaded.
 */
CPP_DLL int mn::GetErrorCodeFine()
{
	return static_cast<int>(ErrorReport::GetSavedError().GetErrorCode());
}

/**
 * @brief	Retrieves the line number at which the error that is saved globally occurred.
 *
 * @return	the line number at which the error occurred.
 * @return	0 if none is loaded.
 */
DBP_CPP_DLL unsigned __int64 mn::GetErrorLine()
{
	return ErrorReport::GetSavedError().GetLineNumber();
}

/**
 * @brief	Retrieves the line number at which the error that is saved globally occurred.
 *
 * This method is only needed for languages which don't support DLL calls with 64 bit integer
 * parameters or return values.
 *
 * @return	the line number at which the error occurred.
 * @return	0 if none is loaded.
 */
CPP_DLL int mn::GetErrorLineFine()
{
	return static_cast<int>(ErrorReport::GetSavedError().GetLineNumber());
}

/**
 * @brief	Toggles error mode.
 *
 * If the error mode was enabled, it will be disabled and visa versa.
 *
 * @param	errorMode	The error mode to toggle.
 */
int mn::ToggleErrorMode(ErrorReport::ErrorMode errorMode)
{
	int returnMe = 0;
	const char * cCommand = "mn::ToggleErrorMode";
	try
	{
		ErrorReport::ToggleErrorMode(errorMode);
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Toggles error mode.
 *
 * If the error mode was enabled, it will be disabled and visa versa.
 *
 * @param	errorMode	The error mode to toggle. See ErrorReport::ErrorMode values.
 */
DBP_CPP_DLL int mn::ToggleErrorMode(int errorMode)
{
	int returnMe = 0;
	const char * cCommand = "mn::ToggleErrorMode";
	try
	{
		ToggleErrorMode(ErrorReport::ConvertToErrorMode(errorMode));
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Enables or disables an error mode.
 *
 * @param	errorMode	The error mode to modify. 
 * @param	enabled		True to enable, false to disable. 
 */
int mn::SetErrorMode(ErrorReport::ErrorMode errorMode, bool enabled)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetErrorMode";
	try
	{
		ErrorReport::SetErrorMode(errorMode,enabled);
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Enables or disables an error mode.
 *
 * @param	errorMode	The error mode to modify. See ErrorReport::ErrorMode values.
 * @param	enabled		True to enable, false to disable. 
 */
DBP_CPP_DLL int mn::SetErrorMode(int errorMode, bool enabled)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetErrorMode";
	try
	{
		SetErrorMode(ErrorReport::ConvertToErrorMode(errorMode),enabled);
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Determines whether an error mode is enabled. 
 *
 * @param	errorMode	The error mode to check. 
 *
 * @return	true if the error mode is enabled, false if not. 
 */
int mn::GetErrorMode(ErrorReport::ErrorMode errorMode)
{
	int returnMe = -1;
	const char * cCommand = "mn::IsErrorModeEnabled";
	try
	{
		returnMe = ErrorReport::IsErrorModeEnabled(errorMode);
	}
	STD_CATCH

	return returnMe;
}

/**
 * @brief	Determines whether an error mode is enabled. 
 *
 * @param	errorMode	The error mode to check. See ErrorReport::ErrorMode values.
 *
 * @return	true if the error mode is enabled, false if not. 
 */
DBP_CPP_DLL int mn::GetErrorMode(int errorMode)
{
	int returnMe = -1;
	const char * cCommand = "mn::GetErrorMode";
	try
	{
		returnMe = GetErrorMode(ErrorReport::ConvertToErrorMode(errorMode));
	}
	STD_CATCH

	return returnMe;
}

/**
 * @brief	Creates an encryption key of bit strength 256.
 *
 * The same key must be used to encrypt as is used to decrypt if the
 * decrypted data is to make sense.
 *
 * @param	key1	The first key. 
 * @param	key2	The second key. 
 * @param	key3	The third key. 
 * @param	key4	The fourth key. 
 *
 * @return encryption key.
 */
DBP_CPP_DLL INT_PTR mn::CreateKey256(__int64 key1, __int64 key2, __int64 key3, __int64 key4)
{
	INT_PTR returnMe = 0;
	const char * cCommand = "mn::CreateKey256";

	try
	{
		EncryptKey * keyData = new (nothrow) EncryptKey(key1,key2,key3,key4);
		Utility::DynamicAllocCheck(keyData,__LINE__,__FILE__);
		returnMe = reinterpret_cast<INT_PTR>(keyData);
	}
	STD_CATCH_RM

	// Return address
	return(returnMe);
}

/**
 * @brief	Creates an encryption key of bit strength 192.
 *
 * The same key must be used to encrypt as is used to decrypt if the
 * decrypted data is to make sense.
 *
 * @param	key1	The first key. 
 * @param	key2	The second key. 
 * @param	key3	The third key. 
 *
 * @return encryption key.
 */
DBP_CPP_DLL INT_PTR mn::CreateKey192(__int64 key1, __int64 key2, __int64 key3)
{
	INT_PTR returnMe = 0;
	const char * cCommand = "mn::CreateKey192";

	try
	{
		EncryptKey * keyData = new (nothrow) EncryptKey(key1,key2,key3);
		Utility::DynamicAllocCheck(keyData,__LINE__,__FILE__);
		returnMe = reinterpret_cast<INT_PTR>(keyData);
	}
	STD_CATCH_RM

	// Return address
	return(returnMe);
}

/**
 * @brief	Creates an encryption key of bit strength 128.
 *
 * The same key must be used to encrypt as is used to decrypt if the
 * decrypted data is to make sense.
 *
 * @param	key1	The first key. 
 * @param	key2	The second key. 
 *
 * @return encryption key.
 */
DBP_CPP_DLL INT_PTR mn::CreateKey128(__int64 key1, __int64 key2)
{
	INT_PTR returnMe = 0;
	const char * cCommand = "mn::CreateKey128";

	try
	{
		EncryptKey * keyData = new (nothrow) EncryptKey(key1,key2);
		Utility::DynamicAllocCheck(keyData,__LINE__,__FILE__);
		returnMe = reinterpret_cast<INT_PTR>(keyData);
	}
	STD_CATCH_RM

	// Return address
	return(returnMe);
}

/**
 * @brief	Creates an encryption key of bit strength 128 using 32 bit keys.
 *
 * The same key must be used to encrypt as is used to decrypt if the
 * decrypted data is to make sense.
 *
 * @param	key1	The first key. 
 * @param	key2	The second key. 
 * @param   key3    The third key.
 * @param   key4    The fourth key.
 *
 * @return encryption key.
 */
CPP_DLL INT_PTR mn::CreateKeyFine128(int key1, int key2, int key3, int key4)
{
	INT_PTR returnMe = 0;
	const char * cCommand = "mn::CreateKeyFine128";

	try
	{
		EncryptKey * keyData = new (nothrow) EncryptKey(key1,key2,key3,key4,true);
		Utility::DynamicAllocCheck(keyData,__LINE__,__FILE__);
		returnMe = reinterpret_cast<INT_PTR>(keyData);
	}
	STD_CATCH_RM

	// Return address
	return(returnMe);
}

/**
 * @brief	Creates an encryption key of bit strength 192 using 32 bit keys.
 *
 * The same key must be used to encrypt as is used to decrypt if the
 * decrypted data is to make sense.
 *
 * @param	key1	The first key. 
 * @param	key2	The second key. 
 * @param	key3	The third key. 
 * @param	key4	The fourth key. 
 * @param	key5	The fifth key. 
 * @param	key6	The sixth key. 
 *
 * @return encryption key.
 */
CPP_DLL INT_PTR mn::CreateKeyFine192(int key1, int key2, int key3, int key4, int key5, int key6)
{
	INT_PTR returnMe = 0;
	const char * cCommand = "mn::CreateKeyFine192";

	try
	{
		EncryptKey * keyData = new (nothrow) EncryptKey(key1,key2,key3,key4,key5,key6,true);
		Utility::DynamicAllocCheck(keyData,__LINE__,__FILE__);
		returnMe = reinterpret_cast<INT_PTR>(keyData);
	}
	STD_CATCH_RM

	// Return address
	return(returnMe);
}

/**
 * @brief	Creates an encryption key of bit strength 256 using 32 bit keys.
 *
 * The same key must be used to encrypt as is used to decrypt if the
 * decrypted data is to make sense.
 *
 * @param	key1	The first key. 
 * @param	key2	The second key. 
 * @param	key3	The third key. 
 * @param	key4	The fourth key. 
 * @param	key5	The fifth key. 
 * @param	key6	The sixth key. 
 * @param	key7	The seventh key. 
 * @param	key8	The eighth key. 
 *
 * @return encryption key.
 */
CPP_DLL INT_PTR mn::CreateKeyFine256(int key1, int key2, int key3, int key4, int key5, int key6, int key7, int key8)
{
	INT_PTR returnMe = 0;
	const char * cCommand = "mn::CreateKeyFine256";

	try
	{
		EncryptKey * keyData = new (nothrow) EncryptKey(key1,key2,key3,key4,key5,key6,key7,key8,true);
		Utility::DynamicAllocCheck(keyData,__LINE__,__FILE__);
		returnMe = reinterpret_cast<INT_PTR>(keyData);
	}
	STD_CATCH_RM

	// Return address
	return(returnMe);
}

/**
 * @brief	Deletes the specified encryption key.
 *
 * @param	Key	The key to delete.
 *
 * @return	0 if no error occurred.
 * @return  -1 if an error occurred.
 */
DBP_CPP_DLL int mn::DeleteKey(INT_PTR Key)
{
	int returnMe = 0; 
	const char * cCommand = "mn::DeleteKey";

	try
	{
		EncryptKey * ptr = (EncryptKey*)Key;
		delete ptr;
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Retrieves the number of logical cores on the system.
 *
 * @return	the number of logical cores on the system.
 */
DBP_CPP_DLL size_t mn::GetLogicalCPU()
{
	size_t returnMe = 0; 
	const char * cCommand = "mn::GetLogicalCPU";

	try
	{
		returnMe = ThreadSingle::GetNumLogicalCores();
	}
	STD_CATCH

	return(returnMe);
}


/**
 * @brief	Specifies the maximum amount of memory that send operations of
 * a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   profile			Instance profile to modify.
 * @param	memoryLimitTCP	The memory limit in bytes. Set to 0 if no change is needed.
 * @param	memoryLimitUDP	The memory limit in bytes. Set to 0 if no change is needed.
 *
 * @return	0 if no error occurred.
 * @return  -1 if an error occurred.
 */
int mn::SetProfileSendMemoryLimit( NetInstanceProfile & profile, size_t memoryLimitTCP , size_t memoryLimitUDP )
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileSendMemoryLimit";

	try
	{
		profile.SetSendMemoryLimit(memoryLimitTCP,memoryLimitUDP);
	}
	STD_CATCH_RM
	return returnMe;
}

/**
 * @brief	Specifies the maximum amount of memory that receive operations of
 * a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   profile			Instance profile to modify.
 * @param	memoryLimitTCP	The memory limit in bytes. Set to 0 if no change is needed.
 * @param	memoryLimitUDP	The memory limit in bytes. Set to 0 if no change is needed.
 *
 * @return	0 if no error occurred.
 * @return  -1 if an error occurred.
 */
int mn::SetProfileRecvMemoryLimit( NetInstanceProfile & profile, size_t memoryLimitTCP , size_t memoryLimitUDP )
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileRecvMemoryLimit";

	try
	{
		profile.SetRecvMemoryLimit(memoryLimitTCP,memoryLimitUDP);
	}
	STD_CATCH_RM
	return returnMe;
}

/**
 * @brief Retrieves the maximum amount of memory that TCP send operations 
 * of a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   profile			Instance profile to use.
 *
 * @return TCP send memory limit.
 */
size_t mn::GetProfileSendMemoryLimitTCP(const NetInstanceProfile & profile)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileSendMemoryLimitTCP";

	try
	{
		returnMe = profile.GetSendMemoryLimitTCP();
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief Retrieves the maximum amount of memory that TCP receive operations 
 * of a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   profile			Instance profile to use.
 *
 * @return TCP receive memory limit.
 */
size_t mn::GetProfileRecvMemoryLimitTCP(const NetInstanceProfile & profile)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileRecvMemoryLimitTCP";

	try
	{
		returnMe = profile.GetRecvMemoryLimitTCP();
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief Retrieves the maximum amount of memory that UDP send operations 
 * of a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   profile			Instance profile to use.
 *
 * @return UDP send memory limit.
 */
size_t mn::GetProfileSendMemoryLimitUDP(const NetInstanceProfile & profile)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileSendMemoryLimitUDP";

	try
	{
		returnMe = profile.GetSendMemoryLimitUDP();
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief Retrieves the maximum amount of memory that TCP receive operations 
 * of a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   profile			Instance profile to use.
 *
 * @return UDP receive memory limit.
 */
size_t mn::GetProfileRecvMemoryLimitUDP(const NetInstanceProfile & profile)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileRecvMemoryLimitUDP";

	try
	{
		returnMe = profile.GetRecvMemoryLimitUDP();
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief	Sets up the memory recycle to be used when receiving TCP packets.
 *
 * A separate memory recycle is allocated to each client, each individually created
 * with the parameters specified here.\n\n
 *
 * See MemoryRecyclePacket for more information.
 *
 * @param	profile				Instance profile to modify.
 * @param	numberOfPackets		Maximum number of packets which can be stored by memory recycle. 
 * @param	memorySizeOfPackets	Memory size of packets in memory recycle.
 *
 * @return	0 if no error occurred.
 * @return  -1 if an error occurred.
 */
int mn::SetProfileMemoryRecycleTCP( NetInstanceProfile & profile, size_t numberOfPackets, size_t memorySizeOfPackets )
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileMemoryRecycleTCP";

	try
	{
		profile.SetMemoryRecycleTCP(numberOfPackets,memorySizeOfPackets);
	}
	STD_CATCH_RM
	return returnMe;
}

/**
 * @brief	Sets up the memory recycle to be used when receiving UDP packets.
 *
 * A separate memory recycle is allocated to each client, each individually created
 * with the parameters specified here.\n\n
 *
 * See MemoryRecyclePacket for more information.
 *
 * @param	profile				Instance profile to modify.
 * @param	numberOfPackets		Maximum number of packets which can be stored by memory recycle. 
 * @param	memorySizeOfPackets	Memory size of packets in memory recycle.
 *
 * @return	0 if no error occurred.
 * @return  -1 if an error occurred.
 */
int mn::SetProfileMemoryRecycleUDP( NetInstanceProfile & profile, size_t numberOfPackets, size_t memorySizeOfPackets )
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileMemoryRecycleUDP";

	try
	{
		profile.SetMemoryRecycleUDP(numberOfPackets,memorySizeOfPackets);
	}
	STD_CATCH_RM
	return returnMe;
}

/**
 * @brief	Retrieves the number of packets that the TCP receiving memory recycle can store.
 *
 * See MemoryRecyclePacket for more information.
 *
 * @param	profile	Instance profile to modify.
 *
 * @return	number of packets.
 */
size_t mn::GetProfileMemoryRecycleNumberOfPacketsTCP( const NetInstanceProfile & profile )
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileMemoryRecycleNumberOfPacketsTCP";

	try
	{
		returnMe = profile.GetMemoryRecycleNumberOfPacketsTCP();
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief	Retrieves the amount of memory that each packet in the TCP recycle has allocated to it.
 *
 * See MemoryRecyclePacket for more information.
 *
 * @param profile Instance profile to modify.
 *
 * @return	amount of memory (in bytes).
 */
size_t mn::GetProfileMemoryRecycleMemorySizeOfPacketsTCP( const NetInstanceProfile & profile )
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileMemoryRecycleMemorySizeOfPacketsTCP";

	try
	{
		returnMe = profile.GetMemoryRecycleMemorySizeOfPacketsTCP();
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief	Retrieves the number of packets that the UDP receiving memory recycle can store.
 *
 * See MemoryRecyclePacket for more information.
 *
 * @param	profile	Instance profile to modify.
 *
 * @return	number of packets.
 */
size_t mn::GetProfileMemoryRecycleNumberOfPacketsUDP( const NetInstanceProfile & profile )
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileMemoryRecycleNumberOfPacketsUDP";

	try
	{
		returnMe = profile.GetMemoryRecycleNumberOfPacketsUDP();
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief	Retrieves the amount of memory that each packet in the UDP recycle has allocated to it.
 *
 * See MemoryRecyclePacket for more information.
 *
 * @param profile Instance profile to modify.
 *
 * @return	amount of memory (in bytes).
 */
size_t mn::GetProfileMemoryRecycleMemorySizeOfPacketsUDP( const NetInstanceProfile & profile )
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileMemoryRecycleMemorySizeOfPacketsUDP";

	try
	{
		returnMe = profile.GetMemoryRecycleMemorySizeOfPacketsUDP();
	}
	STD_CATCH
	return returnMe;
}







/**
 * @brief	Specifies the maximum amount of memory that send operations of
 * a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   profile			Instance profile to modify.
 * @param	memoryLimitTCP	The memory limit in bytes. Set to 0 if no change is needed.
 * @param	memoryLimitUDP	The memory limit in bytes. Set to 0 if no change is needed.
 *
 * @return 0 if no error occurred, -1 if an error occurred.
 */
DBP_CPP_DLL int mn::SetProfileSendMemoryLimit( INT_PTR profile, size_t memoryLimitTCP , size_t memoryLimitUDP )
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileSendMemoryLimit";

	try
	{
		NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		SetProfileSendMemoryLimit(auxProfile,memoryLimitTCP,memoryLimitUDP);
	}
	STD_CATCH_RM
	return returnMe;
}

/**
 * @brief	Specifies the maximum amount of memory that receive operations of
 * a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   profile			Instance profile to modify.
 * @param	memoryLimitTCP	The memory limit in bytes. Set to 0 if no change is needed.
 * @param	memoryLimitUDP	The memory limit in bytes. Set to 0 if no change is needed.
 *
 * @return 0 if no error occurred, -1 if an error occurred.
 */
DBP_CPP_DLL int mn::SetProfileRecvMemoryLimit( INT_PTR profile, size_t memoryLimitTCP , size_t memoryLimitUDP )
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileRecvMemoryLimit";

	try
	{
		NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		SetProfileRecvMemoryLimit(auxProfile,memoryLimitTCP,memoryLimitUDP);
	}
	STD_CATCH_RM
	return returnMe;
}

/**
 * @brief Retrieves the maximum amount of memory that TCP send operations 
 * of a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   profile			Instance profile to use.
 *
 * @return TCP send memory limit.
 */
DBP_CPP_DLL size_t mn::GetProfileSendMemoryLimitTCP(INT_PTR profile)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileSendMemoryLimitTCP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<const NetInstanceProfile>(profile);
		returnMe = GetProfileSendMemoryLimitTCP(auxProfile);
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief Retrieves the maximum amount of memory that TCP receive operations 
 * of a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   profile			Instance profile to use.
 *
 * @return TCP receive memory limit.
 */
DBP_CPP_DLL size_t mn::GetProfileRecvMemoryLimitTCP(INT_PTR profile)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileRecvMemoryLimitTCP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<const NetInstanceProfile>(profile);
		returnMe = GetProfileRecvMemoryLimitTCP(auxProfile);
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief Retrieves the maximum amount of memory that UDP send operations 
 * of a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   profile			Instance profile to use.
 *
 * @return UDP send memory limit.
 */
DBP_CPP_DLL size_t mn::GetProfileSendMemoryLimitUDP(INT_PTR profile)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetSendMemoryLimitUDP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<const NetInstanceProfile>(profile);
		returnMe = GetProfileSendMemoryLimitUDP(auxProfile);
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief Retrieves the maximum amount of memory that TCP receive operations 
 * of a single client can consume.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   profile			Instance profile to use.
 *
 * @return UDP receive memory limit.
 */
DBP_CPP_DLL size_t mn::GetProfileRecvMemoryLimitUDP(INT_PTR profile)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileRecvMemoryLimitUDP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<const NetInstanceProfile>(profile);
		returnMe = GetProfileRecvMemoryLimitUDP(auxProfile);
	}
	STD_CATCH
	return returnMe;
}




/**
 * @brief	Sets up the memory recycle to be used when receiving TCP packets.
 *
 * A separate memory recycle is allocated to each client, each individually created
 * with the parameters specified here.\n\n
 *
 * See MemoryRecyclePacket for more information.
 *
 * @param	profile				Instance profile to modify.
 * @param	numberOfPackets		Maximum number of packets which can be stored by memory recycle. 
 * @param	memorySizeOfPackets	Memory size of packets in memory recycle.
 *
 * @return	0 if no error occurred.
 * @return  -1 if an error occurred.
 */
DBP_CPP_DLL int mn::SetProfileMemoryRecycleTCP(INT_PTR profile, size_t numberOfPackets, size_t memorySizeOfPackets)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileMemoryRecycleTCP";

	try
	{
		NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		SetProfileMemoryRecycleTCP(auxProfile,numberOfPackets,memorySizeOfPackets);
	}
	STD_CATCH_RM
	return returnMe;
}

/**
 * @brief	Sets up the memory recycle to be used when receiving UDP packets.
 *
 * A separate memory recycle is allocated to each client, each individually created
 * with the parameters specified here.\n\n
 *
 * See MemoryRecyclePacket for more information.
 *
 * @param	profile				Instance profile to modify.
 * @param	numberOfPackets		Maximum number of packets which can be stored by memory recycle. 
 * @param	memorySizeOfPackets	Memory size of packets in memory recycle.
 *
 * @return	0 if no error occurred.
 * @return  -1 if an error occurred.
 */
DBP_CPP_DLL int mn::SetProfileMemoryRecycleUDP(INT_PTR profile, size_t numberOfPackets, size_t memorySizeOfPackets)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileMemoryRecycleUDP";

	try
	{
		NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		SetProfileMemoryRecycleUDP(auxProfile,numberOfPackets,memorySizeOfPackets);
	}
	STD_CATCH_RM
	return returnMe;
}

/**
 * @brief	Retrieves the number of packets that the TCP receiving memory recycle can store.
 *
 * See MemoryRecyclePacket for more information.
 *
 * @param	profile	Instance profile to modify.
 *
 * @return	number of packets.
 */
DBP_CPP_DLL size_t mn::GetProfileMemoryRecycleNumberOfPacketsTCP(INT_PTR profile)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileMemoryRecycleNumberOfPacketsTCP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<const NetInstanceProfile>(profile);
		returnMe = GetProfileMemoryRecycleNumberOfPacketsTCP(auxProfile);
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief	Retrieves the amount of memory that each packet in the TCP recycle has allocated to it.
 *
 * See MemoryRecyclePacket for more information.
 *
 * @param profile Instance profile to modify.
 *
 * @return	amount of memory (in bytes).
 */
DBP_CPP_DLL size_t mn::GetProfileMemoryRecycleMemorySizeOfPacketsTCP(INT_PTR profile)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileMemoryRecycleMemorySizeOfPacketsTCP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<const NetInstanceProfile>(profile);
		returnMe = GetProfileMemoryRecycleMemorySizeOfPacketsTCP(auxProfile);
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief	Retrieves the number of packets that the UDP receiving memory recycle can store.
 *
 * See MemoryRecyclePacket for more information.
 *
 * @param	profile	Instance profile to modify.
 *
 * @return	number of packets.
 */
DBP_CPP_DLL size_t mn::GetProfileMemoryRecycleNumberOfPacketsUDP(INT_PTR profile)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileMemoryRecycleNumberOfPacketsUDP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<const NetInstanceProfile>(profile);
		returnMe = GetProfileMemoryRecycleNumberOfPacketsUDP(auxProfile);
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief	Retrieves the amount of memory that each packet in the UDP recycle has allocated to it.
 *
 * See MemoryRecyclePacket for more information.
 *
 * @param profile Instance profile to modify.
 *
 * @return	amount of memory (in bytes).
 */
DBP_CPP_DLL size_t mn::GetProfileMemoryRecycleMemorySizeOfPacketsUDP(INT_PTR profile)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileMemoryRecycleMemorySizeOfPacketsUDP";

	try
	{
		const NetInstanceProfile & auxProfile = PointerConverter::GetRefFromInt<const NetInstanceProfile>(profile);
		returnMe = GetProfileMemoryRecycleMemorySizeOfPacketsUDP(auxProfile);
	}
	STD_CATCH
	return returnMe;
}













/**
 * @brief	Changes the maximum amount of memory that the instance
 * is allowed to allocate for asynchronous UDP send operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   instanceID	ID of instance to use.
 * @param	newLimit	The new limit in bytes. 
 */
int mn::SetSendMemoryLimitUDP(size_t instanceID, size_t newLimit)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetSendMemoryLimitUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group[instanceID].GetInstanceUDP()->SetSendMemoryLimitUDP(newLimit);
	}
	STD_CATCH_RM
	return returnMe;	
}

/**
 * @brief	Changes the maximum amount of memory the specified
 * client is allowed to use for UDP receive operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   instanceID	ID of instance to use.
 * @param	clientID	ID of the specified client.
 * @param	newLimit	The new limit in bytes. 
 */
int mn::SetRecvMemoryLimitUDP(size_t instanceID, size_t clientID, size_t newLimit)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetRecvMemoryLimitUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group[instanceID].GetInstanceUDP()->SetRecvMemoryLimitUDP(newLimit,clientID);
	}
	STD_CATCH_RM
	return returnMe;	
}

/**
 * @brief	Retrieves the maximum amount of memory the 
 * instance is allowed to use for UDP send operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   instanceID	ID of instance to use.
 *
 * @return the maximum amount of memory.
 */
size_t mn::GetSendMemoryLimitUDP(size_t instanceID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetSendMemoryLimitUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceUDP()->GetSendMemoryLimitUDP();
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief	Retrieves the maximum amount of memory the specified
 * client is allowed to use for UDP receive operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   instanceID	ID of instance to use.
 * @param	clientID	ID of the specified client.
 *
 * @return the maximum amount of memory.
 */
size_t mn::GetRecvMemoryLimitUDP( size_t instanceID, size_t clientID )
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetRecvMemoryLimitUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceUDP()->GetRecvMemoryLimitUDP(clientID);
	}
	STD_CATCH
	return returnMe;	
}

/**
 * @brief	Retrieves the estimated amount of memory that the 
 * instance is currently using for UDP send operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   instanceID	ID of instance to use.
 *
 * @return the estimated amount of memory.
 */
size_t mn::GetSendMemorySizeUDP(size_t instanceID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetSendMemorySizeUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceUDP()->GetSendMemorySizeUDP();
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief	Retrieves the estimated amount of memory that the specified
 * client is currently using for UDP receive operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   instanceID	ID of instance to use.
 * @param	clientID	ID of the specified client.
 *
 * @return the estimated amount of memory.
 */
size_t mn::GetRecvMemorySizeUDP( size_t instanceID, size_t clientID )
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetRecvMemorySizeUDP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceUDP()->GetRecvMemorySizeUDP(clientID);
	}
	STD_CATCH
	return returnMe;
}








/**
 * @brief	Changes the maximum amount of memory that the instance
 * is allowed to use for asynchronous TCP send operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   instanceID  ID of instance to use.
 * @param	clientID	ID of client to use.
 *
 * @param	newLimit	The new limit in bytes.
 *
 * @return 0 if no error occurred, -1 if an error occurred.
 */
int mn::SetSendMemoryLimitTCP(size_t instanceID, size_t clientID, size_t newLimit)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetSendMemoryLimitTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group[instanceID].GetInstanceTCP()->SetSendMemoryLimitTCP(newLimit,clientID);
	}
	STD_CATCH_RM
	return returnMe;
}


/**
 * @brief	Changes the maximum amount of memory that the instance is
 * allowed to use for TCP receive operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   instanceID  ID of instance to use.
 * @param	clientID	ID of client to use.
 *
 * @param	newLimit	The new limit in bytes.
 *
 * @return 0 if no error occurred, -1 if an error occurred.
 */
int mn::SetRecvMemoryLimitTCP(size_t instanceID, size_t clientID, size_t newLimit)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetRecvMemoryLimitTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		group[instanceID].GetInstanceTCP()->SetRecvMemoryLimitTCP(newLimit,clientID);
	}
	STD_CATCH_RM
	return returnMe;	
}

/**
 * @brief	Retrieves the maximum amount of memory that the instance
 * is allowed to use for TCP send operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   instanceID  ID of instance to use.
 * @param	clientID	ID of client to use.
 *
 * @return the maximum amount of memory in bytes.
 */
size_t mn::GetSendMemoryLimitTCP(size_t instanceID, size_t clientID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetSendMemoryLimitTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceTCP()->GetSendMemoryLimitTCP(clientID);
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief	Retrieves the maximum amount of memory the
 * instance is allowed to use for TCP receive operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   instanceID  ID of instance to use.
 * @param	clientID	ID of client to use.
 *
 * @return the maximum amount of memory in bytes.
 */
size_t mn::GetRecvMemoryLimitTCP(size_t instanceID, size_t clientID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetRecvMemoryLimitTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceTCP()->GetRecvMemoryLimitTCP(clientID);
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief	Retrieves the estimated amount of memory that the
 * instance is currently using for TCP send operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   instanceID  ID of instance to use.
 * @param	clientID	ID of client to use.
 *
 * @return the maximum amount of memory in bytes.
 */
size_t mn::GetSendMemorySizeTCP(size_t instanceID, size_t clientID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetSendMemorySizeTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceTCP()->GetSendMemorySizeTCP(clientID);
	}
	STD_CATCH
	return returnMe;
}

/**
 * @brief	Retrieves the estimated amount of memory that the instance
 * is currently using for TCP receive operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param   instanceID  ID of instance to use.
 * @param	clientID	ID of client to use.
 *
 * @return the maximum amount of memory in bytes.
 */
size_t mn::GetRecvMemorySizeTCP(size_t instanceID, size_t clientID)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetRecvMemorySizeTCP";

	try
	{
		NetInstanceGroup & group = NetUtility::GetInstanceGroup();
		returnMe = group[instanceID].GetInstanceTCP()->GetRecvMemorySizeTCP(clientID);
	}
	STD_CATCH
	return returnMe;
}






#ifdef DBP
/**
 * @brief Determines whether the client is in the process of connecting to a server.
 *
 * Can only be used in NetInstance::CLIENT state.
 *
 * This method is part of the @ref handshakePage "server/client handshaking process".
 *
 * @param instanceID	Unique identifier for instance.
 *
 * @return true if connecting, false if not. DWORD return value required by DBP for booleans.
 */
DBP_CPP_DLL DWORD mn::DBP_GetConnecting(size_t instanceID)
{
	DWORD returnMe = 0;
	const char * cCommand = "mn::GetConnecting";

	try
	{
		returnMe = GetConnecting(instanceID);
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief	Determines if an error is saved globally.
 *
 * Errors are only saved globally when ErrorReport::EM_SAVE is enabled.
 *
 * @return	true if an error is saved, false if not. DWORD return value required by DBP for booleans.
 */
DBP_CPP_DLL DWORD mn::DBP_GetErrorFlag()
{
	return ErrorReport::IsErrorSaved();
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	byte.
 */
DBP_CPP_DLL DWORD mn::DBP_GetUnsignedByte(INT_PTR Packet)
{
	return(GetUnsignedByte(Packet));
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetString(DWORD pOldString, INT_PTR Packet, size_t Length, bool NullTerminated)
{
	const char * cCommand = "GetString (DBP)";

	char * cReturn = NULL;

	try
	{
		cReturn = GetStringC(Packet,Length,NullTerminated);
	}
	STD_CATCH

	// Ensure compatibility with DBP
	if(cReturn == NULL)
	{
		cReturn = new (nothrow) char[1];
		Utility::DynamicAllocCheck(cReturn,__LINE__,__FILE__);
		cReturn[0] = '\0';
	}

	if(Length == 0){Length = strlen(cReturn);}
	if(NullTerminated == true){Length++;}

	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,Length);
	delete[] cReturn;
	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetLocalIPTCP(DWORD pOldString, size_t Instance)
{
	const char * cReturn = GetLocalIPTCP(Instance);

	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);
	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetLocalIPUDP(DWORD pOldString, size_t Instance)
{
	const char * cReturn = GetLocalIPUDP(Instance);
		
	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);
	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetConnectIPTCP(DWORD pOldString, size_t Instance)
{
	const char * cReturn = GetConnectIPTCP(Instance);
		
	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);
	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetConnectIPUDP(DWORD pOldString, size_t Instance)
{
	const char * cReturn = GetConnectIPUDP(Instance);
		
	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn, strlen(cReturn)+1);
	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetClientIPTCP(DWORD pOldString, size_t Instance, size_t clientID)
{
	const char * cReturn = GetClientIPTCP(Instance, clientID);
		
	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);
	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetClientIPUDP(DWORD pOldString, size_t Instance, size_t clientID)
{
	const char * cReturn = GetClientIPUDP(Instance, clientID);
		
	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);
	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetFloat(INT_PTR Packet)
{
	// Get return as std string
	float fReturn = GetFloat(Packet);
		
	// Ensure compatibility with DBP
	return(*(DWORD*)&fReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 * @param	HostName	Host name to convert.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_DNS(DWORD pOldString, char * HostName)
{
	const char * cReturn = DNS_B(HostName);
		
	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetLocalInterface(DWORD pOldString, size_t Number)
{
	const char * cReturn = GetLocalInterfaceStr(Number);

	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetHostName(DWORD pOldString)
{
	const char * cReturn = GetHostName();

	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetProfileLocalIPTCP(DWORD pOldString, INT_PTR profile)
{
	const char * cReturn = GetProfileLocalIPTCP(profile);

	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetProfileLocalIPUDP(DWORD pOldString, INT_PTR profile)
{
	const char * cReturn = GetProfileLocalIPUDP(profile);

	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL int mn::DBP_GetMemblock(INT_PTR Packet, size_t DestinationPtr, size_t Length, bool NullTerminated)
{
	int returnMe = GetStringC_B(Packet, (char*)DestinationPtr,Length,NullTerminated);
	return(returnMe);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL int mn::DBP_AddMemblock(INT_PTR Packet, size_t DestinationPtr, size_t Length, bool Prefix)
{
	int returnMe = AddStringC(Packet,(char*)DestinationPtr,Length,Prefix);
	return(returnMe);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetErrorOperation(DWORD pOldString)
{
	const char * sReturn = GetErrorOperation();
		
	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)sReturn,strlen(sReturn)+1);
	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetErrorCommand(DWORD pOldString)
{
	const char * sReturn = GetErrorCommand();
		
	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)sReturn,strlen(sReturn)+1);
	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetErrorFile(DWORD pOldString)
{
	const char * sReturn = GetErrorFile();
		
	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)sReturn,strlen(sReturn)+1);
	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mn::DBP_GetErrorFull(DWORD pOldString)
{
	const char * sReturn = GetErrorFull();
		
	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)sReturn,strlen(sReturn)+1);
	delete[] sReturn;
	return(dReturn);
}

/**
 * @brief Retrieves a string containing version information for this API. Special version for DarkBASIC Pro.
 *
 * @param pOldString If not NULL then cleaned up.
 *
 * @return @copydoc Utility::VERSION
 */
DBP_CPP_DLL DWORD mn::DBP_GetVersion(DWORD pOldString)
{
	const char * returnMe = GetVersion();

	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)returnMe,strlen(returnMe)+1);

	return(dReturn);
}
#endif

/**
 * @brief	Sets the error flag to true so that mn::GetErrorFlag returns true. 
 */
DBP_CPP_DLL void mn::SetErrorFlag()
{
	ErrorReport::SetErrorSaved(true);
}

/**
 * @brief	Clears the error flag so that mn::GetErrorFlag returns false.
 */
DBP_CPP_DLL void mn::ClearErrorFlag()
{
	ErrorReport::SetErrorSaved(false);
}


/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool mn::TestClass()
{
	cout << "Testing mn namespace...\n";
	bool problem = false;



	// ***********************************************************
	INT_PTR key = mn::CreateKey128(2515,25125);

	size_t n = 0;
	while(n < 1000)
	{
		INT_PTR packet = mn::CreatePacket();
		mn::AddInt(packet,1);

		mn::Encrypt(packet,key,1);
		mn::DeletePacket(packet);

		n++;
		cout << "Iteration " << n << "\n";

	}

	mn::DeleteKey(key);

	return problem;

	// *****************************************************

	{
		mn::Start(1,0);

		const char * localInterface = NULL;
		for(size_t n = 0;n<mn::GetLocalInterfaceAmount();n++)
		{
			if(n == 0)
			{
				localInterface = mn::GetLocalInterfaceStr(n);
			}
		}

		mn::Finish(-1);

		INT_PTR dnKey = mn::CreateKey256(11111111,11111111,11111111,11111111);

		INT_PTR sendPacket = mn::CreatePacket();
		INT_PTR recvPacket = mn::CreatePacket();
		INT_PTR soundInput = mn::CreatePacket();

		mn::Start(1,0);

		size_t maxPlayers = 35;

		bool voiceOn = true;

		INT_PTR hostNIP = mn::CreateInstanceProfile();

		mn::SetProfileModeUDP(hostNIP,NetMode::UDP_CATCH_ALL_NO);
		mn::SetProfileLocal(hostNIP,localInterface,3888,localInterface,3889);
		mn::SetProfileBufferSizes(hostNIP,50000,50000);

		mn::StartServer(0,maxPlayers,hostNIP);

		mnSound::StartSound(1,1);

		INT_PTR sip = mnSound::CreateSoundProfile();

		mnSound::SetSoundProfile(sip,7000,8,1);

		mnSound::StartInput(0,-1,2,100,sip);

		clock_t lastClock = 0;

		clock_t runtimeClock = clock();

		while(clock() - runtimeClock < 30000)
		{
			size_t result = mn::ClientJoined(0);
			if(result > 0)
			{
				cout << "New client joined: \n";
				cout << "\t" << mn::GetClientIPTCP(0,result) << '\n';
				cout << "\t" << mn::GetClientPortTCP(0,result) << '\n';
				cout << "\t" << mn::GetClientIPUDP(0,result) << '\n';
				cout << "\t" << mn::GetClientPortUDP(0,result) << '\n';
			}

			result = mn::ClientLeft(0);
			if(result > 0)
			{
				cout << "Client left: " << result << '\n';
			}

			size_t clientID = 1;

			result = mn::RecvUDP(0,recvPacket,clientID,0);

			if(result > 0)
			{
				mn::Decrypt(recvPacket,dnKey,1);

				for(int i = 0;i<=10;i++)
				{
					char * str = mn::GetStringC(recvPacket,0,1);
					delete[] str;
				}

				if(clock() > lastClock + 5)
				{
					for(int i = 0;i<=10;i++)
					{
						mn::AddStringC(sendPacket,"test",0,1);
					}
					mn::Encrypt(sendPacket,dnKey,1);
					mn::SendAllUDP(0,sendPacket,0,0,0);
					lastClock = clock();
				}
			}
		}

		mnSound::FinishSound();
		mn::Finish(-1);

		mn::DeletePacket(recvPacket);
		mn::DeletePacket(sendPacket);
		mn::DeletePacket(soundInput);
		mn::DeleteInstanceProfile(hostNIP);
	}
	
	cout << "\n\n";
	return !problem;
}


/**
 * @brief Determines whether a decryption key has been set.
 *
 * @param profile Instance profile to use.
 * 
 * @return 1 if decrypt key UDP is loaded.
 * @return 0 if decrypt key is not loaded. This is the default.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::GetProfileDecryptKeyLoadedUDP( INT_PTR profile )
{
	int returnMe = 0;
	const char * cCommand = "mn::GetProfileDecryptKeyLoaded";

	try
	{
		NetInstanceProfile & ref = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = ref.IsDecryptKeyLoadedUDP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Retrieves the encryption key used to decrypt incoming UDP packets.
 *
 * @param profile Instance profile to use.
 * 
 * @return encryption key. Note that the key is not necessarily used by.
 * UDP instances, only if the mode in use specifically specifies in its documentation
 * will the key ever be used.
 */
DBP_CPP_DLL INT_PTR mn::GetProfileDecryptKeyUDP(INT_PTR profile)
{
	INT_PTR returnMe = -1;
	const char * cCommand = "mn::GetProfileDecryptKeyUDP";

	try
	{
		_ErrorException((GetProfileDecryptKeyLoadedUDP(profile) == false),"retrieving a profile's decryption key, none is loaded",0,__LINE__,__FILE__);
		NetInstanceProfile & ref = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = reinterpret_cast<INT_PTR>(ref.GetDecryptKeyUDP());
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Changes the encryption key used to decrypt incoming UDP packets.
 *
 * Note that the key is not necessarily used by UDP
 * instances, only if the mode in use specifically specifies in its documentation
 * will the key ever be used.
 *
 * @param profile Instance profile to use.
 * @param key Key to use. 0 to unload the currently loaded encryption key.
 * 
 * @return 0 if no error occurred.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::SetProfileDecryptKeyUDP(INT_PTR profile, INT_PTR key)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileDecryptKeyUDP";

	try
	{
		NetInstanceProfile & ref = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		EncryptKey * keyPtr = PointerConverter::GetPtrFromInt<EncryptKey>(key);
		ref.SetDecryptKeyUDP(keyPtr);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Enables or disables the reusable UDP option. If reusable other applications, instances
 * and sockets can bind to the same UDP address.
 *
 * @param profile Instance profile to use.
 * @param reusable True to enable, false to disable the option.
 * 
 * @return 0 if no error occurred.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::SetProfileReusableUDP(INT_PTR profile, bool reusable)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileReusableUDP";

	try
	{
		NetInstanceProfile & ref = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		ref.SetReusableUDP(reusable);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief Determines whether UDP should be reusable. If reusable other applications, instances
 * and sockets can bind to the same UDP address.
 *
 * @param profile Instance profile to use.
 * 
 * @return 1 if UDP is set to reusable.
 * @return 0 if UDP is not set to reusable. This is default.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::GetProfileReusableUDP(INT_PTR profile)
{
	int returnMe = -1;
	const char * cCommand = "mn::GetProfileReusableUDP";

	try
	{
		NetInstanceProfile & ref = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = ref.IsReusableUDP();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief  Retrieves the number of UDP operations in NetModeUdp::UDP_PER_CLIENT_PER_OPERATION.
 *
 * @param profile Instance profile to use.
 * 
 * @return the number of UDP operations set.
 */
DBP_CPP_DLL size_t mn::GetProfileNumOperationsUDP(INT_PTR profile)
{
	size_t returnMe = 0;
	const char * cCommand = "mn::GetProfileNumOperations";

	try
	{
		NetInstanceProfile & ref = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		returnMe = ref.GetNumOperations();
	}
	STD_CATCH

	return(returnMe);
}

/**
 * @brief Specifies the number of UDP operations in NetModeUdp::UDP_PER_CLIENT_PER_OPERATION.
 *
 * @param profile Instance profile to use.
 * @param numOperations Number of operations.
 * 
 * @return 0 if no error occurred.
 * @return -1 if an error occurred.
 */
DBP_CPP_DLL int mn::SetProfileNumOperationsUDP(INT_PTR profile, size_t numOperations)
{
	int returnMe = 0;
	const char * cCommand = "mn::SetProfileNumOperations";

	try
	{
		NetInstanceProfile & ref = PointerConverter::GetRefFromInt<NetInstanceProfile>(profile);
		ref.SetNumOperations(numOperations);
	}
	STD_CATCH_RM

	return(returnMe);
}

/**
 * @brief	Deallocates specified string.
 * 
 * @warning The memory of the specified string will be deallocated and so the
 * string may no longer be used after this call. If the string is used then
 * memory corruption may occur.
 *
 * @param [in]	freeMe	String to deallocate. If NULL then no action is taken. 
 */
CPP_DLL void mn::FreeString(char * freeMe)
{
	delete[] freeMe;
}