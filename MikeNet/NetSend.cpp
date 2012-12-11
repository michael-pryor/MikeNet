#include "FullInclude.h"

/**
 * @brief Constructor.
 *
 * @param	block	True if the send operation should be synchronous, false if it should be asynchronous. 
 */
NetSend::NetSend(bool block) : overlappedEvent(true)
{
	SecureZeroMemory(&overlapped,sizeof(WSAOVERLAPPED));
	overlapped.hEvent = overlappedEvent.GetEventHandle();

	this->block = block;
}

/**
 * @brief Destructor.
 */
NetSend::~NetSend()
{
	const char * cCommand = "an internal function (~NetSend)";
	try
	{

	}
	MSG_CATCH
}

/**
 * @brief Waits for completion of send operation, method does not return until send operation is complete.
 *
 * @param	sendTimeout	The length of time to wait until giving up.
 *
 * @return NetUtility::SEND_COMPLETED if the send operation completed successfully instantly.
 * @return NetUtility::SEND_FAILED_KILL if the send operation failed and the initiating entity should be killed as a result (e.g. Client disconnected).
 */
NetUtility::SendStatus NetSend::_WaitForCompletion(unsigned int sendTimeout)
{
	NetUtility::SendStatus returnMe;

	bool completedInTime = overlappedEvent.WaitUntilSignaled(sendTimeout);

	// If the operation completed within the specified time
	if(completedInTime == false)
	{
		returnMe = NetUtility::SEND_COMPLETED;
	}
	// If the operation did not complete within the specified time
	// then the client should be disconnected, indicated by return value of -1
	else
	{
		returnMe = NetUtility::SEND_FAILED_KILL;
	}
	
	// Return
	return(returnMe);
}

/**
 * @brief Determines whether the send operation is synchronous or asynchronous.
 *
 * @return true if the send operation is synchronous.
 * @return false if the send operation is asynchronous.
 */
bool NetSend::IsBlocking() const
{
	return block;
}


/**
 * @brief Class that implements NetSend so that it can be tested.
 */
class TestNetSend : public NetSend
{
public:

	TestNetSend(bool block) : NetSend(block)
	{

	}
private:
	/** 
	 * @brief Implements virtual method.
	 *
	 * @return NULL.
	 */
	WSABUF * GetBuffer()
	{
		return NULL;
	}

	/** 
	 * @brief Implements virtual method.
	 *
	 * @return 0.
	 */
	size_t GetBufferAmount() const
	{
		return 0;
	}
};


/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetSend::TestClass()
{
	cout << "Testing NetSend class...\n";
	bool problem = false;

	{
		cout << "Creating NetSend object..\n";
		TestNetSend obj(true);

		cout << "Blocking: " << obj.IsBlocking() << '\n';
		if(obj.IsBlocking() == false)
		{
			cout << "IsBlocking is bad\n";
			problem = true;
		}
		else
		{
			cout << "IsBlocking is good\n";
		}

		cout << "Waiting for send operation to complete..\n";
		NetUtility::SendStatus status = obj._WaitForCompletion(1000);

		char * strStatus;
		switch(status)
		{
		case NetUtility::SEND_COMPLETED:
			strStatus = "SEND_COMPLETED";
			break;

		case NetUtility::SEND_FAILED_KILL:
			strStatus = "SEND_FAILED_KILL";
			break;

		default:
			strStatus = "UNKNOWN";
			break;
		}

		cout << "Operation completed with status " << strStatus << "\n";
		if(status != NetUtility::SEND_COMPLETED)
		{
			cout << "_WaitForCompletion is bad\n";
			problem = true;
		}
		else
		{
			cout << "_WaitForCompletion is good\n";
		}
	}
	
	cout << "\n\n";
	return !problem;
}

/**
 * @brief	Determines the total length of all buffers returned by GetBuffer().
 *
 * @return	the total length of all buffers.
 */
size_t NetSend::GetTotalBufferLength()
{
	WSABUF * buffers = GetBuffer();

	size_t total = 0;
	for(size_t n = 0;n<GetBufferAmount();n++)
	{
		total += buffers[n].len;
	}

	return total;
}
