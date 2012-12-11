#include "FullInclude.h"

/**
 * @brief Generates NetModeUdp object based on @a protocolMode.
 *
 * @param protocolMode Type of NetModeUdp object to generate.
 * @param numClients  Number of clients that object should support (may be ignored).
 * @param numOperations  Number of operations that object should support (may be ignored).
 * @param recvSize Maximum size of packets that object should be able to deal with (may be ignored).
 * @param decryptKey Decryption key that object should use to decrypt received data (may be ignored).
 * @param memoryRecycle	Used to allocate memory to the protocol. If NULL then no memory recycle is used. See MemoryRecyclePacket for more information. 
 */
NetModeUdp * NetModeUdp::GenerateModeUDP(NetMode::ProtocolMode protocolMode, size_t numClients, size_t numOperations, size_t recvSize, const EncryptKey * decryptKey, const MemoryRecyclePacketRestricted * memoryRecycle)
{
	NetModeUdp * returnMe;

	switch(protocolMode)
	{
	case(NetMode::UDP_CATCH_ALL):
		if(memoryRecycle != NULL)
		{
			returnMe = new (nothrow) NetModeUdpCatchAll(numClients,memoryRecycle);
		}
		else
		{
			returnMe = new (nothrow) NetModeUdpCatchAll(numClients);
		}
		break;

	case(NetMode::UDP_CATCH_ALL_NO):
		if(memoryRecycle != NULL)
		{
			returnMe = new (nothrow) NetModeUdpCatchAllNo (numClients,memoryRecycle);
		}
		else
		{
			returnMe = new (nothrow) NetModeUdpCatchAllNo(numClients);
		}
		break;

	case(NetMode::UDP_PER_CLIENT):
		returnMe = new (nothrow) NetModeUdpPerClient(recvSize,numClients,numOperations,false,decryptKey);
		break;

	case(NetMode::UDP_PER_CLIENT_PER_OPERATION):
		returnMe = new (nothrow) NetModeUdpPerClient(recvSize,numClients,numOperations,true,decryptKey);
		break;

	default:
		_ErrorException(true,"generating a NetModeUdp object, specified protocol is invalid",0,__LINE__,__FILE__);
		break;
	}

	Utility::DynamicAllocCheck(returnMe,__LINE__,__FILE__);
	return returnMe;
}

/**
 * @brief	Helps to test NetModeUdp objects.
 *
 * Client ID used with DealWithData is 10, and operation ID is 5.
 *
 * @param [in,out]	obj				The object. 
 * @param [in,out]	packet			The packet. 
 * @param str						The string stored in @a packet.
 * @param dealWithDataClientID		Client ID to use with DealWithData().
 * @param expectedClientID			Expected result of destination.GetClientFrom().
 * @param operationID				Operation ID to use with DealWithData().
 *
 * @return	false if the test was failed and a problem was found while testing, true if not.
 */
bool NetModeUdp::_HelperTestClass(NetModeUdp & obj, Packet & packet, const char * str, size_t dealWithDataClientID, size_t expectedClientID, size_t operationID)
{
	Packet destination;
	WSABUF buffer;

	bool problem = false;

	// Counter: 500
	packet.SetCursor(0);
	packet.AddSizeT(500);
	packet.PtrIntoWSABUF(buffer);
	obj.DealWithData(buffer,packet.GetUsedSize(),NULL,dealWithDataClientID,1);
	
	if(obj.GetPacketFromStore(&destination,expectedClientID,operationID) != 1 ||
	   destination.GetClientFrom() != expectedClientID ||
	   destination.GetInstance() != 1||
	   destination.GetAge() != 500 ||
	   (destination.GetOperation() != operationID && destination.GetOperation() != 0))
	{
		cout << "DealWithData is bad (packet 1)\n";
		problem = true;
	}
	else
	{
		destination.Erase(0,destination.GetCursor());
		if(destination != str)
		{
			cout << "DealWithData is bad due to contents (packet 1)\n";
			problem = true;
		}
		else
		{
			cout << "DealWithData is good (packet 1)\n";
		}
	}

	// Counter: 499
	packet.SetCursor(0);
	packet.AddSizeT(499);
	packet.PtrIntoWSABUF(buffer);
	obj.DealWithData(buffer,packet.GetUsedSize(),NULL,dealWithDataClientID,1);
	
	if(obj.GetPacketFromStore(&destination,expectedClientID,operationID) != 0)
	{
		cout << "DealWithData is bad (packet 2)\n";
		problem = true;
	}
	else
	{
		cout << "DealWithData is good (packet 2)\n";
	}

	// Counter: 501
	packet.SetCursor(0);
	packet.AddSizeT(501);
	packet.PtrIntoWSABUF(buffer);
	obj.DealWithData(buffer,packet.GetUsedSize(),NULL,dealWithDataClientID,3);
	
	if(obj.GetPacketFromStore(&destination,expectedClientID,operationID) != 1 ||
	   destination.GetClientFrom() != expectedClientID ||
	   destination.GetInstance() != 3 ||
	   destination.GetAge() != 501 ||
	   (destination.GetOperation() != operationID && destination.GetOperation() != 0))
	{
		cout << "DealWithData is bad (packet 3)\n";
		problem = true;
	}
	else
	{
		destination.Erase(0,destination.GetCursor());
		if(destination != str)
		{
			cout << "DealWithData is bad due to contents (packet 3)\n";
			problem = true;
		}
		else
		{
			cout << "DealWithData is good (packet 3)\n";
		}
	}
	
	// Counter: very large (1000 from maximum) 
	packet.SetCursor(0);
	packet.AddSizeT(-1000);
	packet.PtrIntoWSABUF(buffer);
	obj.DealWithData(buffer,packet.GetUsedSize(),NULL,dealWithDataClientID,3);
	
	if(obj.GetPacketFromStore(&destination,expectedClientID,operationID) != 1 ||
	   destination.GetClientFrom() != expectedClientID ||
	   destination.GetInstance() != 3 ||
	   destination.GetAge() != -1000 ||
	   (destination.GetOperation() != operationID && destination.GetOperation() != 0))
	{
		cout << "DealWithData is bad (packet 4)\n";
		problem = true;
	}
	else
	{
		destination.Erase(0,destination.GetCursor());
		if(destination != str)
		{
			cout << "DealWithData is bad due to contents (packet 4)\n";
			problem = true;
		}
		else
		{
			cout << "DealWithData is good (packet 4)\n";
		}
	}
	
	// Counter: 1 (testing code that an entity can continue running
	// forever, when the counter reaches maximum it will loop round to 0).
	packet.SetCursor(0);
	packet.AddSizeT(1);
	packet.PtrIntoWSABUF(buffer);
	obj.DealWithData(buffer,packet.GetUsedSize(),NULL,dealWithDataClientID,3);
	
	if(obj.GetPacketFromStore(&destination,expectedClientID,operationID) != 1 ||
	   destination.GetClientFrom() != expectedClientID ||
	   destination.GetInstance() != 3 ||
	   destination.GetAge() != 1 ||
	   (destination.GetOperation() != operationID && destination.GetOperation() != 0))
	{
		cout << "DealWithData is bad (packet 5)\n";
		problem = true;
	}
	else
	{
		destination.Erase(0,destination.GetCursor());
		if(destination != str)
		{
			cout << "DealWithData is bad due to contents (packet 5)\n";
			problem = true;
		}
		else
		{
			cout << "DealWithData is good (packet 5)\n";
		}
	}

	return !problem;
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetModeUdp::TestClass()
{
	cout << "Testing NetModeUdp class...\n";
	bool problem = false;

	NetModeUdp * mode = GenerateModeUDP(NetMode::UDP_PER_CLIENT_PER_OPERATION,50,5,1024,NULL,NULL);
	
	if(mode->GetProtocolMode() != NetMode::UDP_PER_CLIENT_PER_OPERATION ||
	   mode->GetNumClients() != 50 ||
	   mode->GetNumOperations() != 5)
	{
		cout << "GenerateModeUDP (NetMode::UDP_PER_CLIENT_PER_OPERATION) is bad\n";
		problem = true;
	}
	else
	{
		cout << "GenerateModeUDP (NetMode::UDP_PER_CLIENT_PER_OPERATION) is good\n";
	}
	
	delete mode;

	cout << "\n\n";
	return !problem;
}

/**
 * @brief	Destructor. 
 */
NetModeUdp::~NetModeUdp()
{

}
