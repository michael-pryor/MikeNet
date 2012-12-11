#include "FullInclude.h"

/**
 * @brief Constructor.
 *
 * @param packet Packet to send.
 * @param block If true packet will be sent synchronously, if false packet will be sent asynchronously.
 */
NetSendRaw::NetSendRaw(const Packet * packet, bool block) : NetSend(block)
{
	_ErrorException((packet == NULL),"constructing a NetSendPostfix object, packet parameter must not be null",0,__LINE__,__FILE__);
	
	this->packet = packet;

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
		packet->PtrIntoWSABUF(buffers[0]);
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
		packet->CopyIntoWSABUF(buffers[0]);
	}
}

/**
 * @brief Destructor.
 */
NetSendRaw::~NetSendRaw()
{
	// Memory is only allocated by this object if non blocking
	if(IsBlocking() == false)
	{
		delete[] buffers[0].buf;
	}
}

/** 
 * @brief Retrieves an array of WSABUF structures containing
 * data to send (NetSendRaw::buffers).
 *
 * @return an array of WSABUF containing data to be sent. The
 * sent packet or data stream will consist of a combination
 * of all elements of the array, starting from element 0.
 */
WSABUF * NetSendRaw::GetBuffer()
{
	return buffers;
}

/** 
 * @brief Retrieves the number of elements in the array returned by GetBuffer().
 *
 * @return number of elements.
 */
size_t NetSendRaw::GetBufferAmount() const
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
bool NetSendRaw::TestClass()
{
	cout << "Testing NetSendRaw class...\n";
	bool problem = false;

	Packet packet("hello world");
	NetSendRaw obj(&packet,true);

	if(obj.GetBufferAmount() != 1)
	{
		cout << "GetBufferAmount or constructor is bad\n";
		problem = true;
	}
	else
	{
		cout << "GetBufferAmount and constructor are good\n";
	}

	if(packet.compareWSABUF(obj.GetBuffer()[0],obj.GetBuffer()[0].len) == false)
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