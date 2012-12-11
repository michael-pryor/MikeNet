#include "FullInclude.h"

/**
 * @brief Constructor.
 *
 * @param packet Packet to send. Pointer must remain valid for lifetime of object.
 * @param block If true packet will be sent synchronously, if false packet will be sent asynchronously.
 * @param prefix Prefix to place at start of packet. Data is copied, so reference does not need to remain
 * valid for lifetime of object.
 */
NetSendPrefix::NetSendPrefix(const Packet * packet, bool block, const Packet & prefix) : NetSend(block)
{
	_ErrorException((packet == NULL),"constructing a NetSendPostfix object, packet parameter must not be null",0,__LINE__,__FILE__);
	
	this->prefix = prefix; // Store bytes of prefix
	this->packet = packet;

	/**
	 * this->prefix will remain valid until this object is destroyed.
	 * This object won't be destroyed until send operation is completed.
	 */
	buffers[0].buf = this->prefix.GetDataPtr();
	buffers[0].len = static_cast<DWORD>(this->prefix.GetUsedSize());

	/**
	 * If the send operation blocks until completion then
	 * the send operation does not need its own buffer
	 * because the send operation will be completed before
	 * further usage of the packet buffer takes place.
	 * The destructor MUST NOT deallocate the buffer memory
	 * since this will be done automatically by the destructors
	 * of the objects that the buffer points to (we are not doing
	 * any copying).
	 */
	if(block == true)
	{
		packet->PtrIntoWSABUF(buffers[1]);
	}
	/**
	 * If the send operation does not block until completion
	 * then it needs its own buffer, which is allocated from 
	 * the heap and MUST be deallocated by the destructor.
	 * This is necessary because the packet buffer may be used
	 * by another thread during sending.
	 */
	else
	{
		packet->CopyIntoWSABUF(buffers[1]);
	}
}

/**
 * @brief Destructor.
 */
NetSendPrefix::~NetSendPrefix()
{
	// Memory is only allocated by this object if non blocking
	if(IsBlocking() == false)
	{
		delete[] buffers[1].buf;
	}
}

/** 
 * @brief Retrieves an array of WSABUF structures containing
 * data to send (NetSendPrefix::buffers).
 *
 * @return an array of WSABUF containing data to be sent. The
 * sent packet or data stream will consist of a combination
 * of all elements of the array, starting from element 0.
 */
WSABUF * NetSendPrefix::GetBuffer()
{
	return (WSABUF*)buffers;
}

/** 
 * @brief Retrieves the number of elements in the array returned by GetBuffer().
 *
 * @return number of elements.
 */
size_t NetSendPrefix::GetBufferAmount() const
{
	return NUM_BUFFERS;
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetSendPrefix::TestClass()
{
	cout << "Testing NetSendPrefix class...\n";
	bool problem = false;

	Packet packet("hello world");
	Packet prefix("goodbye and ");
	NetSendPrefix obj(&packet,true,prefix);

	if(obj.GetBufferAmount() != 2)
	{
		cout << "GetBufferAmount or constructor is bad\n";
		problem = true;
	}
	else
	{
		cout << "GetBufferAmount and constructor are good\n";
	}

	if(prefix.compareWSABUF(obj.GetBuffer()[0],obj.GetBuffer()[0].len) == false)
	{
		cout << "Constructor is bad\n";
		problem = true;
	}
	else
	{
		cout << "Constructor is good\n";
	}

	if(packet.compareWSABUF(obj.GetBuffer()[1],obj.GetBuffer()[1].len) == false)
	{
		cout << "Constructor is bad\n";
		problem = true;
	}
	else
	{
		cout << "Constructor is good\n";
	}

	cout << "\n\n";
	return !problem;
}