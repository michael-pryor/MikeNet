#include "FullInclude.h"

/**
 * @brief Sets variables to default.
 */
void NetSocket::DefaultVariables()
{
	recvFunction = NULL;
	recvFunctionAux = NULL;
	completionPortCloseNotification.Set(false);
	notDealingWithData.Set(true);
	ClearSend();
	ClearRecv();
}

/**
 * @brief Sets up NetSocket::recvBuffer, allocating memory to it.
 *
 * @param bufferLength Length of receive buffer in bytes;
 * this is the maximum amount of data that can be received in one receive operation.
 */
void NetSocket::AllocateBuffer(size_t bufferLength)
{
	// If the buffer is too big lets allocate the biggest we can,
	// It hopefully won't impact the protocol.
	if(bufferLength > ULONG_MAX) {
		bufferLength = ULONG_MAX;
	}

	recvBuffer.len = static_cast<ULONG>(bufferLength);
	if(bufferLength > 0)
	{
		recvBuffer.buf =  new (nothrow) char[bufferLength];
		Utility::DynamicAllocCheck(recvBuffer.buf,__LINE__,__FILE__);
	}
	else
	{
		recvBuffer.buf = NULL;
	}
}

/**
 * @brief Initializes objects.
 *
 * @param bufferLength Length of receive buffer in bytes;
 * this is the maximum amount of data that can be received in one receive operation.
 */
void NetSocket::Initialize(size_t bufferLength)
{
	recvOverlapped.hEvent = recvOverlappedEvent.GetEventHandle();
	AllocateBuffer(bufferLength);
}

/**
 * @brief Constructor.
 *
 * @param bufferLength Length of receive buffer in bytes;
 * this is the maximum amount of data that can be received in one receive operation.
 * @param [in] receiveFunction Function to be executed when packets are received.
 * If NULL received packets are put into a queue and retrieved using GetPacketFromStore().
 */
NetSocket::NetSocket(size_t bufferLength, RecvFunc receiveFunction) :
	NetSocketSimple(),
	completionKey(CompletionKey::SOCKET,this),
	recvOverlappedEvent(true),
	completionPortCloseNotification(false),
	notDealingWithData(true)
{
	try
	{
		Initialize(bufferLength);
		DefaultVariables();
		this->recvFunction = receiveFunction;
	}
	catch(ErrorReport & errorReport)
	{
		Close();
		delete[] recvBuffer.buf;
		throw errorReport;
	}
}

/**
 * @brief Constructor.
 *
 * @param bufferLength Length of receive buffer in bytes;
 * this is the maximum amount of data that can be received in one receive operation.
 * @param [in] receiveFunction Function to be executed when packets are received.
 * If NULL received packets are put into a queue and retrieved using GetPacketFromStore().
 * @param [in] instance Pointer to instance that owns this object, use other constructor if no instance does.
 */
NetSocket::NetSocket(size_t bufferLength, RecvFunc receiveFunction, NetInstance * instance) :
	NetSocketSimple(),
	completionKey(CompletionKey::SOCKET,this,instance),
	recvOverlappedEvent(true),
	completionPortCloseNotification(false),
	notDealingWithData(true)
{
	Initialize(bufferLength);
	DefaultVariables();
	this->recvFunction = receiveFunction;
}

/**
 * @brief Deals with completed send operation.
 *
 * In the event of an error that needs further action NetSocket::completionPortCloseNotification is signaled.
 *
 * @param overlapped Pointer to WSAOVERLAPPED object associated with send operation.
 * @param success True if the operation completed successfully.
 * @param shuttingDown True if the operation failed but this was because the socket is shutting down.
 */
void NetSocket::CompletedSendOperation(const WSAOVERLAPPED * overlapped, bool success, bool shuttingDown)
{
	if(success == false)
	{
		// Rarely while stress testing the previous version
		// it was noted that very rarely a bad completion packet
		// is received, if the overlapped pointer is not found then
		// it is assumed to be a bad packet and so we don't call ErrorOccurred.
		if(RemoveSend(overlapped) == true)
		{
			if(shuttingDown == false)
			{
				this->CompletionPortRequestClose();
			}
		}
	}
	else
	{
		RemoveSend(overlapped);
	}
}

/**
 * @brief Deep copy constructor.
 *
 * Socket does not directly copy contents of @a copyMe, but initializes its buffer
 * to be the same size as @a copyMe's.\n\n
 *
 * NetSocketSimple::winsockSocket is not copied.
 *
 * @param	copyMe	Object to copy.
 */
NetSocket::NetSocket(const NetSocket & copyMe) :
	NetSocketSimple(copyMe),
	completionKey(CompletionKey::SOCKET,this),
	recvOverlappedEvent(true),
	completionPortCloseNotification(false),
	notDealingWithData(true)
{
	Initialize(copyMe.recvBuffer.len);
	this->recvFunction = copyMe.recvFunction;
}

/**
 * @brief Deep assignment operator.
 *
 * Socket does not directly copy contents of @a copyMe, but initializes its buffer
 * to be the same size as @a copyMe's.\n\n
 *
 * NetSocketSimple::winsockSocket is not copied.
 *
 * @param	copyMe	Object to copy. 
 * @return	reference to this object.
 */
NetSocket & NetSocket::operator= (const NetSocket & copyMe)
{
	NetSocketSimple::operator=(copyMe);

	if(this->recvBuffer.len != copyMe.recvBuffer.len)
	{
		delete[] recvBuffer.buf;
		this->AllocateBuffer(copyMe.recvBuffer.len);
	}

	this->recvFunction = copyMe.recvFunction;

	return *this;
}

/**
 * @brief Destructor.
 */
NetSocket::~NetSocket()
{
	const char * cCommand = "an internal function (~NetSocket)";
	try
	{
		Close();

		delete[] recvBuffer.buf;
	}
	MSG_CATCH
}

/**
 * @brief Clears NetSocket::recvOverlapped object ready for next receive.
 *
 * This must be done before every receive operation.
 */
void NetSocket::ClearRecv()
{
	recvOverlapped.Internal = 0;
	recvOverlapped.InternalHigh = 0;
	recvOverlapped.Offset = 0;
	recvOverlapped.OffsetHigh = 0;
	recvOverlapped.Pointer = 0;
	flags = 0;
}

/**
 * @brief Closes socket.
 *
 * To ensure that deallocation does not occur when the completion port is using
 * the method:
 * - This method waits for all send operations to be cleaned up.
 * - This method waits for all winsock receive operations to complete.
 * - This method waits for finished receive operations to finish being dealt with.
 *
 * @warning This method must not be used by the completion port, in case only
 * one worker thread exists. In that case the completion port would be waiting
 * for things that only it can do.
 */
void NetSocket::Close()
{
	if(IsSetup() == true)
	{
		// Close winsock socket.
		NetSocketSimple::Close();

		// Wait for receive operations to be canceled, and for the socket
		// to actually be closed. We do this as well as checking notDealingWithData
		// in case another operation is allowed to begin if socket closure takes an unusually
		// long time.
		recvOverlappedEvent.WaitUntilSignaled();

		// If the completion port is not setup and we wait for notDealingWithData,
		// then deadlock will occur because the completion port needs to signal that
		// it is done dealing with data, and it cannot do that if it is not active.
		if(NetUtility::IsCompletionPortSetup() == true)
		{
			// Wait for send operations to be cleaned up.
			while(IsSendEmpty() == false)
			{
				Sleep(1);
			}

			// Wait for all data to be dealt with
			// After a receive operation completes we still need to wait
			// for the data to be dealt with.
			notDealingWithData.WaitUntilSignaled();
		}
		else
		{
			ClearSend();
			notDealingWithData.Set(true);
		}

		// Default variables that should be default after Close.
		DefaultVariables();
	}
}


/**
 * @brief Manually sets the overlapped event.
 *
 * This is necessary when an overlapped recv operation fails before pending as in this case
 * the overlapped event object is not automatically signaled.
 */
void NetSocket::SetRecvOverlappedEvent()
{
	recvOverlappedEvent.Set(true);
}


/**
 * @brief Cleans up the send operation that is using the specified overlapped object.
 *
 * @param operation Pointer to overlapped operation of send operation to cleanup.
 *
 * @return true if an operation was cleaned up, false if not.
 */
bool NetSocket::RemoveSend(const OVERLAPPED * operation)
{
	bool removed = false;

	sendCleanup.Enter();
		// Find send operation to remove
		size_t position = 0;
		bool found = FindSend(operation,position);

		if(found == true)
		{
			RemoveSend(position);
			removed = found;
		}
	sendCleanup.Leave();

	return(removed);
}

/** 
 * @brief Searches for the send operation that is using the specified overlapped object in
 * the send cleanup list.
 *
 * @param operation Pointer to overlapped object.
 * @param [out] position The location of the found send operation will be stored here.
 *
 * @return true if the send operation that is using the specified overlapped object was
 * found in the send cleanup list.
 */
bool NetSocket::FindSend(const OVERLAPPED * operation, size_t & position) const
{
	bool bFound = false;

	sendCleanup.Enter();
	{
		for(size_t n = 0;n<sendCleanup.Size();n++)
		{
			if(&sendCleanup[n].overlapped == operation)
			{
				position = n;
				bFound = true;

				break;
			}
		}
	}
	sendCleanup.Leave();

	return(bFound);
}

/**
 * @brief Cleans up the send operation at the specified element.
 *
 * @param element Element to cleanup.
 */
void NetSocket::RemoveSend(size_t element)
{
	sendCleanup.Enter();
	{
		if(element >= sendCleanup.Size())
		{
			sendCleanup.Leave();
			_ErrorException(true,"cleaning up a send operation by element, element is out of bounds",0,__LINE__,__FILE__);
		}

		/**
		 * Check that object is not in use.
		 * The object will only be in use at this point if it is
		 * still executing the internal function it was doing when
		 * it added itself to the cleanup queue. It may still need to
		 * use some internal variables e.g. If it is blocking for completion
		 * and so we must wait before we clean it up. We must release the
		 * critical section BEFORE cleaning it up.
		 */
		sendCleanup[element].Enter();
		sendCleanup[element].Leave();

		// Send operations are only copied to a separate
		// buffer if asynchronous, otherwise the memory of
		// the packet itself is used.
		if(!sendCleanup.GetElement(element).IsBlocking())
		{
			sendCleanupSize.DecreaseMemorySize(sendCleanup[element].GetTotalBufferLength());
		}

		// Cleanup element
		sendCleanup.Erase(element);
	}
	sendCleanup.Leave();
}

/**
 * @brief Adds a send operation to the cleanup vector.
 *
 * @param send Send operation to add to cleanup vector.
 *
 * @throws ErrorReport If too much memory is used to store send operations in progress.
 */
void NetSocket::AddSend(NetSend * send)
{
	_ErrorException((send == NULL),"adding a send operation to the cleanup queue, NetSend object must not be NULL",0,__LINE__,__FILE__);

	// Send operations are only copied to a separate
	// buffer if asynchronous, otherwise the memory of
	// the packet itself is used.
	if(!send->IsBlocking())
	{
		this->sendCleanupSize.IncreaseMemorySize(send->GetTotalBufferLength()); // Exception is thrown if too much memory is used by send operation.
	}
	sendCleanup.Add(send);
}

/**
 * @brief Empties the send cleanup vector, cleaning up all send operations.
 */
void NetSocket::ClearSend()
{
	sendCleanup.Enter();

	// Cleanup and remove all elements
	while(sendCleanup.Size() > 0)
	{
		RemoveSend((size_t)0);
	}

	sendCleanup.Leave();
}

/**
 * @brief Determines whether the send cleanup vector is empty.
 *
 * @return true if the send cleanup vector is empty.
 */
bool NetSocket::IsSendEmpty() const
{
	bool returnMe = (sendCleanup.Size() == 0);
	return returnMe;
}

/**
 * Associates the socket with a completion port.
 * The completion port takes over the following jobs:
 * - Cleaning up send operations.
 * - Receiving data and dealing with it.
 */
void NetSocket::AssociateCompletionPort()
{
	NetUtility::AssociateWithCompletionPort(reinterpret_cast<HANDLE>(winsockSocket),completionKey);
}

/**
 * @brief Determines the size of the receive buffer.
 *
 * @return the buffer size of the WSABUF. This is the maximum amount of
 * data that can be received with one WSARecv operation.
 */
size_t NetSocket::GetRecvBufferLength() const
{
	return recvBuffer.len;
}

/**
 * @brief Sends a send object to the specified address.
 *
 * @param [in]	sendObject	Object to send. This is consumed by the object and should not be referenced elsewhere.
 * @param	    sendToAddr	Address to send to, if NULL then object is sent to address that socket is connected to.
 * @param		timeout		Length of time in milliseconds to wait before canceling send operation.
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_IN_PROGRESS if the send operation was started, but has not yet completed.
 * @return NetUtility::SEND_FAILED if the send operation failed.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and an entity was killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus NetSocket::Send(NetSend * sendObject, const NetAddress * sendToAddr, unsigned int timeout)
{
	int iResult;
	NetUtility::SendStatus returnMe;

	_ErrorException((sendObject == NULL),"sending a NetSend object, must not be NULL",0,__LINE__,__FILE__);

	/* Prevent completion port from cleaning this object up
	   until we are done using all internal variables.
	   Also note that this makes sending multithread safe, since
	   it is not safe to initiate send operations on the same
	   socket at the same time from different threads. */
	sendObject->Enter();

	try
	{
		/* Queue send operation to be deallocated
		   Note: must be done before WSASend */
		AddSend(sendObject);
	}
	// Exception will be thrown if too much memory
	// is used by this send operation.
	catch(ErrorReport & report)
	{
		// Cleanup send object because it will never be used.
		sendObject->Leave();
		delete sendObject;
		throw report;
	}

	// Send to address previously connected to
	if(sendToAddr == NULL)
	{
		iResult = WSASend(winsockSocket,sendObject->GetBuffer(),static_cast<DWORD>(sendObject->GetBufferAmount()),&sendObject->bytes,NULL,&sendObject->overlapped,NULL);
	}
	// Send to address not connected to
	else
	{
		iResult = WSASendTo(winsockSocket,sendObject->GetBuffer(),static_cast<DWORD>(sendObject->GetBufferAmount()),&sendObject->bytes,NULL,(SOCKADDR*)sendToAddr->GetAddrPtr(),sizeof(SOCKADDR),&sendObject->overlapped,NULL);	
	}

	// Check for errors
	if(iResult == SOCKET_ERROR)
	{
		// If a message will be sent later
		if(WSAGetLastError() == WSA_IO_PENDING)
		{
			// Block until sent
			if(sendObject->IsBlocking() == true)
			{
				returnMe = sendObject->_WaitForCompletion(timeout);

				/* DO NOT use RemoveSend here because operation may still complete later
				 * Operation will be cleaned up when the entity is killed as a result of 
				 * failing to complete in time */
			}
			// Don't block until sent
			else
			{
				returnMe = NetUtility::SEND_IN_PROGRESS;
			}
		}
		else
		{
			returnMe = NetUtility::SEND_FAILED;
		}
	}
	// If no errors occurred, not even pending then the send completed instantly
	else
	{
		returnMe = NetUtility::SEND_COMPLETED;
	}

	/* We are done using this object so it is now okay for object to be cleaned up */
	sendObject->Leave();

	if(returnMe == NetUtility::SEND_FAILED || returnMe == NetUtility::SEND_FAILED_KILL)
	{
		// Operation will never be completed successfully so deallocate manually
		RemoveSend(&sendObject->overlapped);
	}

	return(returnMe);
}

/**
 * @brief Determines whether the specified overlapped object is the overlapped object
 * used by this object to monitor the status of pending receive operations.
 *
 * @return true if @a overlapped points to the overlapped object used by this object.
 * @param overlapped Overlapped pointer to compare.
 */
bool NetSocket::IsOurOverlapped(const WSAOVERLAPPED * overlapped) const
{
	return &this->recvOverlapped == overlapped;
}

/**
 * @brief Determines whether the completion port has signaled this socket for closure.
 *
 * The completion port will only signal a socket for closure if it is stand alone
 * i.e. Not part of an instance.
 *
 * @return true if the completion port wants this socket to be closed.
 */
bool NetSocket::GetCompletionPortCloseRequest() const
{
	bool aux = completionPortCloseNotification.Get();
	completionPortCloseNotification.Set(false);
	return aux;
}

/**
 * @brief Signal that the socket should be closed by the main process.
 *
 * The completion port will only signal a socket for closure if it is stand alone
 * i.e. Not part of an instance.
 *
 * @warning Should only be used by completion port.
 */
void NetSocket::CompletionPortRequestClose()
{
	completionPortCloseNotification.Set(true);
}

/**
 * @brief Signal that the receive operation that was last initiated has completed.
 *
 * @warning Should only be used by completion port.
 */
void NetSocket::SetCompletionPortFinishRecvNotification()
{
	notDealingWithData.Set(true);
}


/**
 * @brief Retrieves the function that is executed when complete packets are received.
 *
 * @return the function that will be executed when data is received.
 * @return NULL if there is no method loaded.
 */
NetSocket::RecvFunc NetSocket::GetRecvFunction() const
{
	return recvFunction.Get();
}

/**
 * @brief Determines whether a receive function has been loaded.
 *
 * If no function has been loaded received packets are put into a queue and
 * retrieved using GetPacketFromStore().
 * 
 * @return true if a method to be executed when packets are received has been set.
 * @return false if no method has been set.
 */
bool NetSocket::IsRecvFunctionLoaded() const
{
	return recvFunction.Get() != NULL;
}

/**
 * @brief	Temporarily disables the receive function.
 *
 * Use UndoRemoveRecvFunction() to undo this action.
 */
void NetSocket::RemoveRecvFunction()
{
	recvFunctionAux = recvFunction;
	recvFunction = NULL;
}

/**
 * @brief	Resets receive function as if RemoveRecvFunction had never been used.
 */
void NetSocket::UndoRemoveRecvFunction()
{
	recvFunction = recvFunctionAux;
	recvFunctionAux = NULL;
}

/**
 * @brief	Sets the completion key instance field.
 *
 * @param [in] instance Pointer to instance to be loaded into completion key. 
 */
void NetSocket::SetInstance( NetInstance * instance )
{
	completionKey.SetInstance(instance);
}
/**
 * @brief	Sets the completion key client ID field.
 *
 * @param clientID Client ID to be loaded into completion key.
 */
void NetSocket::SetClientID( size_t clientID )
{
	completionKey.SetClientID(clientID);
}

/**
 * @brief Class that implements NetSocket so that it can be tested.
 */
class TestNetSocket : public NetSocket
{
public:
	/**
	 * @brief Constructor.
	 *
	 * @param bufferLength Length of receive buffer in bytes;
	 * this is the maximum amount of data that can be received in one receive operation.
	 */
	TestNetSocket(size_t bufferLength) : NetSocket(bufferLength, NULL)
	{

	}

	/**
	 * @brief Implements virtual method.
	 */
	bool Recv()
	{
		return false;
	}

	/** 
	 * @brief Implements virtual method.
	 *
	 * @param packet Ignored.
	 * @param block Ignored.
	 * @param sendToAddr Ignored.
	 * @param timeout Ignored.
	 *
	 * @return NetUtility::SEND_COMPLETED.
	 */
	NetUtility::SendStatus Send(const Packet & packet, bool block, const NetAddress * sendToAddr, unsigned int timeout)
	{
		return NetUtility::SEND_COMPLETED;
	}

	/**
	 * @brief Implements virtual method.
	 *
	 * @return protocol type that socket represents.
	 */
	Protocol GetProtocol() const
	{
		return NetSocket::TCP;
	}

	/**
	 * @brief Implements virtual method.
	 *
	 * @param memoryLimit Ignored.
	 */
	void SetRecvMemoryLimit(size_t memoryLimit)
	{

	}

	/**
	 * @brief Implements virtual method.
	 *
	 * @param buffer Ignored.
	 * @param completionBytes Ignored.
	 * @param [in] recvFunc Ignored.
	 * @param instanceID Ignored.
	 * @param clientID Ignored.
	 */
	void DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc recvFunc, size_t clientID, size_t instanceID)
	{

	}
};

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetSocket::TestClass()
{
	cout << "Testing NetSocket class...\n";
	bool problem = false;

	cout << "Constructing TestNetSocket object..\n";
	TestNetSocket socket(1024);
	
	if(socket.GetRecvBufferLength() != 1024)
	{
		cout << " GetRecvBufferLength or constructor is bad\n";
		problem = true;
	}
	else
	{
		cout << " GetRecvBufferLength and constructor are good\n";
	}

	OVERLAPPED overlapped;

	cout << "Removing send overlapped that is not in vector..\n";
	socket.RemoveSend(&overlapped);

	Packet packet("hello world");
	NetSendRaw * sendObj = new NetSendRaw(&packet,true);
	NetSendRaw * sendObj2 = new NetSendRaw(&packet,true);
	NetSendRaw * sendObj3 = new NetSendRaw(&packet,true);

	cout << "Adding send objects to send cleanup vector..\n";
	socket.AddSend(sendObj);
	socket.AddSend(sendObj2);
	socket.AddSend(sendObj3);

	OVERLAPPED * sendObjOverlapped1 = &sendObj->overlapped;
	OVERLAPPED * sendObjOverlapped2 = &sendObj2->overlapped;
	OVERLAPPED * sendObjOverlapped3 = &sendObj3->overlapped;

	cout << "Finding send overlapped by overlapped pointer..\n";
	size_t position = 0;
	bool found = socket.FindSend(sendObjOverlapped2,position);
	
	if(found == false || position != 1)
	{
		cout << " FindSend is bad\n";
		problem = true;
	}
	else
	{
		cout << " FindSend is good\n";
	}

	cout << "Removing send overlapped by overlapped pointer..\n";
	found = socket.RemoveSend(sendObjOverlapped3);
	
	if(found == false)
	{
		cout << " RemoveSend is bad\n";
		problem = true;
	}
	else
	{
		cout << " RemoveSend is good\n";
	}

	bool isEmpty = socket.IsSendEmpty();
	if(isEmpty == true)
	{
		cout << " IsSendEmpty is bad\n";
		problem = true;
	}
	else
	{
		cout << " IsSendEmpty is good\n";
	}

	cout << "Erasing send cleanup vector..\n";
	socket.ClearSend();
	isEmpty = socket.IsSendEmpty();
	if(isEmpty == false)
	{
		cout << " IsSendEmpty is bad\n";
		problem = true;
	}
	else
	{
		cout << " IsSendEmpty is good\n";
	}

	
	cout << "Associating socket with completion port..\n";
	NetUtility::StartWinsock();
	NetUtility::SetupCompletionPort(2);
	

	socket.Setup(NetSocketSimple::UDP);
	socket.AssociateCompletionPort();

	NetUtility::DestroyCompletionPort();
	NetUtility::FinishWinsock();
	cout << "\n\n";
	return !problem;
}

/**
 * @brief	Changes the maximum amount of memory sending is allowed to consume.
 *
 * Sending is said to 'consume memory' when an asynchronous send call returns
 * before the packet is transmitted. In this case, the API must store the packet
 * until it is sent. During this period of time memory equal to the size of the packet
 * is in use.
 *
 * If too many send operations are in progress, too much memory may be consumed,
 * which ordinarily would cause the server to crash. However, if a send limit
 * is in place then instead the client will be disconnected and any memory
 * in use will be freed.
 *
 * By default there is no limit.
 *
 * @param	memoryLimit	The maximum amount of memory that can be dedicated to sending (in bytes).
 */
void NetSocket::SetSendMemoryLimit( size_t memoryLimit )
{
	sendCleanupSize.SetMemoryLimit(memoryLimit);
}

/**
 * @brief	Retrieves the maximum amount of memory that sending is allowed to consume.
 *
 * See SetSendMemoryLimit for more information.
 *
 * @return	the memory limit in bytes.
 */
size_t NetSocket::GetSendMemoryLimit() const
{
	return sendCleanupSize.GetMemoryLimit();
}

/**
 * @brief	Retrieves the estimated amount of memory that sending is currently consuming.
 *
 * See SetSendMemoryLimit for more information.
 *
 * @return	the estimated amount of memory currently being consumed.
 */
size_t NetSocket::GetSendMemorySize() const
{
	return sendCleanupSize.GetMemorySize();
}