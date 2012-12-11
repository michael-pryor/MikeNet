#include "FullInclude.h"

/**
 * @brief Constructor.
 *
 * @param [in] socketUDP Socket to use to send and receive UDP data.  This pointer and its data is now
 * owned by this object and should not be used elsewhere.
 * @note Virtual inheritance means that NetInstance constructor will never be called by this constructor.
 */
NetInstanceUDP::NetInstanceUDP(NetSocketUDP * socketUDP) : NetInstance()
{
	try
	{
		this->socketUDP = socketUDP;

		if(IsEnabledUDP() == true)
		{
			socketUDP->SetInstance(this);
		}
	}
	catch(ErrorReport & report)
	{
		delete socketUDP;
		throw report;
	}
}

/**
 * @brief Destructor.
 */
NetInstanceUDP::~NetInstanceUDP()
{
	const char * cCommand = "an internal function (~NetInstanceUDP)";
	try
	{
		CloseSockets();
		delete socketUDP;
	}
	MSG_CATCH
}

/**
 * @brief	Closes any sockets in use by the instance.
 */
void NetInstanceUDP::CloseSockets()
{
	if(IsEnabledUDP() == true)
	{
		socketUDP->Close();
	}
}


/**
 * @brief Determines whether UDP is enabled.
 *
 * @return true if UDP is enabled, false if not.
 */
bool NetInstanceUDP::IsEnabledUDP() const
{
	return (socketUDP != NULL);
}

/**
 * @brief	Throws an exception if UDP is disabled.
 *
 * @param	line		The line number at which this method is being called.
 * @param	file		The file in which this method is being called.
 */
void NetInstanceUDP::ValidateIsEnabledUDP( size_t line, const char * file ) const
{
	_ErrorException((IsEnabledUDP() == false),"performing a UDP operation, UDP is not operational or is disabled",0,__LINE__,__FILE__);
}

/**
 * @brief Retrieves the UDP mode in use as an enum value.
 *
 * @return the UDP mode in use.
 * @throws ErrorReport If UDP is disabled.
 */
NetMode::ProtocolMode NetInstanceUDP::GetModeUDP() const
{
	ValidateIsEnabledUDP(__LINE__,__FILE__);
	return socketUDP->GetMode()->GetProtocolMode();
}

/**
 * @brief Retrieves the number of UDP operations in use.
 *
 * Operation IDs range from 0 inclusive to 
 * number of operations exclusive. e.g. If maximum
 * number of operations is 5 the following IDs are available:
 * 0, 1, 2, 3, 4.
 *
 * @return the number of UDP operations supported.
 * @throws ErrorReport If UDP is disabled.
 */
size_t NetInstanceUDP::GetNumOperationsUDP() const
{
	ValidateIsEnabledUDP(__LINE__,__FILE__);
	return socketUDP->GetMode()->GetNumOperations();
}

/**
 * @brief Retrieves the UDP receive function in use, or NULL if none is in use.
 *
 * This function is called whenever a UDP packet is received on the instance. If no
 * function exists the packet is put into a queue and retrieved using GetPacketFromStoreUDP().
 *
 * IsUserFunctionLoadedUDP() can be used to determine if a function is loaded.
 *
 * @return the method set by the user that should
 * be executed when data is received. 
 * @return NULL if there is no method.
 *
 * @throws ErrorReport If UDP is disabled.
 */
NetSocket::RecvFunc NetInstanceUDP::GetUserFunctionUDP() const
{
	ValidateIsEnabledUDP(__LINE__,__FILE__);
	return socketUDP->GetRecvFunction();
}

/**
 * @brief Determines whether a UDP receive function is in use.
 *
 * The function itself can be retrieved using GetUserFunctionUDP().
 *
 * @return true if a function has been loaded.
 * @return false if no function has been loaded.
 *
 * @throws ErrorReport If UDP is disabled.
 */
bool NetInstanceUDP::IsUserFunctionLoadedUDP() const
{
	ValidateIsEnabledUDP(__LINE__,__FILE__);
	return socketUDP->IsRecvFunctionLoaded();
}

/**
 * @brief Retrieves the amount of data allocated to the receive buffer (WSABUF).
 *
 * This is the largest packet that can be received successfully. Attempts to receive
 * packets larger than this will fail.
 * 
 * @return the amount of data allocated to the receive buffer.
 *
 * @throws ErrorReport If UDP is disabled.
 */
size_t NetInstanceUDP::GetRecvBufferLengthUDP() const
{
	ValidateIsEnabledUDP(__LINE__,__FILE__);
	return socketUDP->GetRecvBufferLength();
}
/**
 * @brief Calls GetRecvBufferLengthUDP(), included for consistency between TCP and UDP.
 *
 * @return the amount of data allocated to the receive buffer.
 */
size_t NetInstanceUDP::GetMaxPacketSizeUDP() const
{
	return GetRecvBufferLengthUDP();
}

/**
 * @brief Retrieves the protocol mode in use as an enum value.
 *
 * @return protocol mode in use.
 *
 * @throws ErrorReport If UDP is disabled.
 */
NetMode::ProtocolMode NetInstanceUDP::GetProtocolModeUDP() const
{
	ValidateIsEnabledUDP(__LINE__,__FILE__);
	return socketUDP->GetMode()->GetProtocolMode();
}

/**
 * @brief Retrieves a packet from the packet store.
 *
 * When packets are received and a receive function has not been set, they
 * are put into a queue and retrieved using this method.
 *
 * @param clientID ID of client to use, may be ignored.
 * @param operationID Operation ID of operation to use, may be ignored.
 *
 * @return the number of packets in the packet store.
 *
 * @throws ErrorReport If UDP is disabled.
 */
size_t NetInstanceUDP::GetPacketAmountUDP(size_t clientID, size_t operationID) const
{
	ValidateIsEnabledUDP(__LINE__,__FILE__);
	return socketUDP->GetMode()->GetPacketAmount(clientID,operationID);
}

/** 
 * @brief Retrieves the IP and port of the UDP local address that this instance is bound to. 
 *
 * @return UDP local address information. This reference will remain
 * valid and unmodified until the next call.
 *
 * @throws ErrorReport If UDP is disabled.
 */
const NetAddress & NetInstanceUDP::GetLocalAddressUDP() const
{
	ValidateIsEnabledUDP(__LINE__,__FILE__);
	return socketUDP->GetLocalAddress();
}

/** 
 * @brief Retrieves the IP and port of the UDP local address that this instance is connected to. 
 *
 * @return the address that the instance is connected to.
 * @return an empty NetAddress structure if not connected.
 *
 * @throws ErrorReport If UDP is disabled.
 */
const NetAddress & NetInstanceUDP::GetConnectAddressUDP() const
{
	ValidateIsEnabledUDP(__LINE__,__FILE__);
	return socketUDP->GetAddressConnected();
}

/**
 * @brief Empties the UDP packet store.
 *
 * When UDP packets are received and a receive function has not been set,
 * packets are put into a queue and received using GetPacketFromStoreUDP(). If the queue gets too big
 * newer packets may not be received for some time and so it can be useful to empty the queue.
 *
 * @param clientID ID of client to use.
 *
 * @throws ErrorReport If UDP is disabled.
 */
void NetInstanceUDP::FlushRecvUDP(size_t clientID)
{
	ValidateIsEnabledUDP(__LINE__,__FILE__);
	socketUDP->Reset(clientID);
}

/**
 * @brief Retrieves a complete packet from the UDP packet store.
 *
 * @param [out] destination Destination to copy into.
 * @param clientID ID of client to use (default 0).
 * @param operationID ID of operation to use (default 0).
 *
 * @return number of packets in packet store before this method call.
 *
 * @throws ErrorReport If UDP is disabled.
 */
size_t NetInstanceUDP::GetPacketFromStoreUDP(Packet * destination, size_t clientID, size_t operationID)
{
	ValidateIsEnabledUDP(__LINE__,__FILE__);
	return socketUDP->GetPacketFromStore(destination, clientID, operationID);
}

/**
 * @brief Validates a specified receive size, ensuring it is larger than the minimum.
 *
 * @param size Buffer size to validate.
 *
 * @return true if @a size is small enough (i.e. Larger than or equal to GetRecvSizeMinUDP()).
 */
bool NetInstanceUDP::ValidateRecvSizeUDP(size_t size) const
{
	return(size >= GetRecvSizeMinUDP());
}

/**
 * @brief Validates a specified receive size, ensuring it is larger than the minimum.
 *
 * Does nothing if UDP is disabled.
 *
 * @throws ErrorReport If @a size is not small enough (i.e. Larger than or equal to GetRecvSizeMinUDP()).
 */
void NetInstanceUDP::ExceptionValidateRecvSizeUDP() const
{
	if(IsEnabledUDP() == true)
	{
		_ErrorException((ValidateRecvSizeUDP(socketUDP->GetRecvBufferLength()) != true),"initializing a UDP based instance, receive buffer size is too small",0,__LINE__,__FILE__);
	}
}


/**
 * @brief	Changes the maximum amount of memory that the instance
 * is allowed to allocate for asynchronous UDP send operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	newLimit	The new limit in bytes. 
 */
void NetInstanceUDP::SetSendMemoryLimitUDP(size_t newLimit)
{
	NetInstanceUDP::ValidateIsEnabledUDP(__LINE__,__FILE__);
	this->socketUDP->SetSendMemoryLimit(newLimit);
}

/**
 * @brief	Changes the maximum amount of memory the specified
 * client is allowed to use for UDP receive operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	newLimit	The new limit in bytes. 
 * @param	clientID	ID of the specified client.
 */
void NetInstanceUDP::SetRecvMemoryLimitUDP(size_t newLimit, size_t clientID)
{
	NetInstanceUDP::ValidateIsEnabledUDP(__LINE__,__FILE__);
	this->socketUDP->SetRecvMemoryLimit(newLimit, clientID);
}

/**
 * @brief	Retrieves the maximum amount of memory the 
 * instance is allowed to use for UDP send operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @return the maximum amount of memory.
 */
size_t NetInstanceUDP::GetSendMemoryLimitUDP() const
{
	NetInstanceUDP::ValidateIsEnabledUDP(__LINE__,__FILE__);
	return this->socketUDP->GetSendMemoryLimit();	
}

/**
 * @brief	Retrieves the maximum amount of memory the specified
 * client is allowed to use for UDP receive operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	clientID	ID of the specified client.
 *
 * @return the maximum amount of memory.
 */
size_t NetInstanceUDP::GetRecvMemoryLimitUDP( size_t clientID ) const
{
	NetInstanceUDP::ValidateIsEnabledUDP(__LINE__,__FILE__);
	return this->socketUDP->GetRecvMemoryLimit(clientID);	
}

/**
 * @brief	Retrieves the estimated amount of memory that the 
 * instance is currently using for UDP send operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @return the estimated amount of memory.
 */
size_t NetInstanceUDP::GetSendMemorySizeUDP() const
{
	NetInstanceUDP::ValidateIsEnabledUDP(__LINE__,__FILE__);
	return this->socketUDP->GetSendMemorySize();	
}

/**
 * @brief	Retrieves the estimated amount of memory that the specified
 * client is currently using for UDP receive operations.
 *
 * See @ref securityPage "security" for more information.
 *
 * @param	clientID	ID of the specified client.
 *
 * @return the estimated amount of memory.
 */
size_t NetInstanceUDP::GetRecvMemorySizeUDP( size_t clientID ) const
{
	NetInstanceUDP::ValidateIsEnabledUDP(__LINE__,__FILE__);
	return this->socketUDP->GetRecvMemorySize(clientID);	
}
