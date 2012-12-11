#include "FullInclude.h"

/**
 * @brief Constructor.
 *
 * @param	instanceID	Unique identifier for instance.
 * @param	state		Type of instance.
 * @param	sendTimeout	Length of time that send operations are allowed to complete before canceling send operation and disconnecting.
 */
NetInstance::NetInstance(size_t instanceID, Type state, unsigned int sendTimeout)
{
	this->instanceID = instanceID;
	this->state = state;
	this->sendTimeout = sendTimeout;
	this->shouldBeDestroyed.Set(false);
}

/**
 * @brief Destructor.
 */
NetInstance::~NetInstance()
{

}

/**
 * @brief Constructor. Should never be called.
 *
 * Included to please compiler regarding virtual inheritance.
 * @throws ErrorReport Always.
 */
NetInstance::NetInstance()
{
	_ErrorException(true,"initializing, NetInstance constructor called, this should never happen",0,__LINE__,__FILE__);
}

/**
 * @brief Retrieves the type of instance that this is.
 *
 * @return instance type.
 */
NetInstance::Type NetInstance::GetState() const
{
	return state;
}

/**
 * @brief ID of instance within NetInstanceGroup object.
 *
 * @return unique instance ID within NetInstanceGroup object.
 */
size_t NetInstance::GetInstanceID() const
{
	return instanceID;
}

/**
 * @brief Retrieve length of time that send operation is allowed to complete before canceling and disconnecting.
 *
 * @return length of time that send operation is allowed to complete before canceling and disconnecting.
 */
unsigned int NetInstance::GetSendTimeout() const
{
	return sendTimeout;
}

/**
 * @brief Deals with completed send operation.
 *
 * @param socket [in,out] Pointer to socket that initiated send operation.
 * @param overlapped Pointer to WSAOVERLAPPED object associated with send operation.
 * @param success True if the operation completed successfully.
 * @param shuttingDown True if the operation failed but this was because the socket is shutting down.
 * @param clientID ID of client that initiated send operation, may be ignored.
 */
void NetInstance::CompletedSendOperation(NetSocket * socket, const WSAOVERLAPPED * overlapped, bool success, bool shuttingDown, size_t clientID)
{
	socket->CompletedSendOperation(overlapped,success,shuttingDown);

	if(socket->GetCompletionPortCloseRequest() == true)
	{
		ErrorOccurred(clientID);
	}
}

/**
 * @brief Determine whether this object wants to be destroyed by its parent NetInstanceContainer.
 *
 * @return true if object wants to be destroyed.
 */
bool NetInstance::GetDestroyRequest() const
{
	return shouldBeDestroyed.Get();
}

/**
 * @brief Requests that object be destroyed by its parent NetInstanceContainer.
 *
 * The container object (NetInstanceContainer) will then cleanup this object
 * when it deems appropriate, so as not to interrupt program flow.
 */
void NetInstance::RequestDestroy()
{
	shouldBeDestroyed.Set(true);
}

/**
 * @brief Change instance ID, for use only by NetInstanceContainer (via friendship) during initialization.
 *
 * @param newID New instance ID.
 */
void NetInstance::SetInstanceID(size_t newID)
{
	instanceID = newID;
}

/**
 * @brief Converts integer into Type.
 *
 * @param	type	Integer to convert.
 * @return	enum equivalent of @a type.
 */
NetInstance::Type NetInstance::ConvertToType(int type)
{
	switch(type)
	{
	case(SERVER):
	case(CLIENT):
	case(BROADCAST):
		return static_cast<NetInstance::Type>(type);
		break;

	default:
		_ErrorException(true,"converting from integer to state, invalid state received",0,__LINE__,__FILE__);
		return NetInstance::CLIENT;
		break;
	}
}