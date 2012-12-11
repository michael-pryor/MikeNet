#include "FullInclude.h"

/**
 * @brief Constructor.
 *
 * @param packet Packet to send. Pointer must remain valid for lifetime of object.
 * @param block If true packet will be sent synchronously, if false packet will be sent asynchronously.
 * @param postfix Postfix to attach to the end of the packet. Pointer must remain valid for lifetime of object.
 */
NetSendPostfix::NetSendPostfix(const Packet * packet, bool block, const Packet * postfix) : NetSend(block)
{
	_ErrorException((packet == NULL),"constructing a NetSendPostfix object, packet parameter must not be null",0,__LINE__,__FILE__);
	_ErrorException((postfix == NULL),"constructing a NetSendPostfix object, postfix parameter must not be null",0,__LINE__,__FILE__);
	
	this->postfix = postfix;
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
		postfix->PtrIntoWSABUF(buffers[1]);
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
		postfix->CopyIntoWSABUF(buffers[1]);
	}
}

/**
 * @brief Destructor.
 */
NetSendPostfix::~NetSendPostfix()
{
	// Memory is only allocated by this object if non blocking
	if(IsBlocking() == false)
	{
		delete[] buffers[0].buf;
		delete[] buffers[1].buf;
	}
}

/** 
 * @brief Retrieves an array of WSABUF structures containing
 * data to send (NetSendPostfix::buffers).
 *
 * @return an array of WSABUF containing data to be sent. The
 * sent packet or data stream will consist of a combination
 * of all elements of the array, starting from element 0.
 */
WSABUF * NetSendPostfix::GetBuffer()
{
	return buffers;
}

/** 
 * @brief Retrieves the number of elements in the array returned by GetBuffer().
 *
 * @return number of elements.
 */
size_t NetSendPostfix::GetBufferAmount() const
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
bool NetSendPostfix::TestClass()
{
	cout << "Testing NetSendPostfix class...\n";
	bool problem = false;

	Packet packet("hello world");
	Packet postfix(" and hello universe!");
	NetSendPostfix obj(&packet,true,&postfix);

	if(obj.GetBufferAmount() != 2)
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

	if(postfix.compareWSABUF(obj.GetBuffer()[1],obj.GetBuffer()[1].len) == false)
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