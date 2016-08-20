#include "FullInclude.h"
/** 
 * @page gracefulDisconnectPage Graceful Disconnection Process
 * Graceful disconnection allows clients to finish sending and receiving data before disconnecting.
 * @section howItWorks How it works
 * Below exact method/command names have been avoided because there are a number of ways of performing
 * each operation depending on which part of the module you are interacting with. It should be clear however
 * which method or command you should use for each operation. \n\n
 * 
 * The below example shows a client disconnecting from a server, if the server wants to drop a client
 * the process remains the same but with the roles reversed. \n\n
 *
 * Initially both the client and server are fully connected and the connection status is NetUtility::CONNECTED.\n
 * @image html FullyConnected.png \n\n\n
 *
 *
 * If a client wants to disconnect, it should first initiate the graceful disconnection process
 * by shutting the connection down using a Shutdown method/command. After this any attempt to initiate TCP send operations will fail.
 * However, the client can still receive data as normal from the server. The TCP connection status will now return. 
 * NetUtility::NO_SEND on the client side and NetUtility::NO_RECV on the server side. \n
 * @image html ServerCanSend.png \n\n\n
 *
 *
 * Once the server has finished sending any remaining data it should shut the connection down 
 * using a Shutdown method/command. After this any further attempt to send to the client will fail.
 * The TCP connection status will now return NetUtility::NO_SEND_RECV on both the client and server side.
 * Now any data left in the TCP packet queue on the client and server side can be extracted. Once
 * the queue is empty the connection can be completely closed and the clean-up process begun. \n
 * @image html FullyDisconnected.png
 */

/**
 * @page securityPage Security
 * The API is secure in the sense that malicious clients cannot cause the server to crash. The primary way that
 * a client could do this is by forcing the server to allocate too much memory. To this end, there are two types
 * of restrictions imposable on any instance or socket:
 * - Sending memory limit: when a client sends data asynchronously it is copied to a temporary buffer which
 * is deallocated only when the send operation completes. If too many send operations are in progress then too
 * much memory may be used. The limit is specified in bytes.
 * - Receiving memory limit: when a client receives data and no receive function is set, then the data
 * is temporarily stored until the application retrieves it. If too many packets are received too quickly
 * then too much memory may be used. The limit is specified in bytes.\n\n
 *
 * Default behavior is to disconnect the client or socket silently. \n\n
 *
 * In server state, where more than one client the limits apply as follows:
 * - TCP asynchronous send operations have a separate limit for each client.
 * - UDP asynchronous send operations have a shared limit which all clients contribute to.
 * The client responsible for going over this limit is disconnected, but in general care should be
 * taken by the server not to send too much data via UDP.
 * - TCP and UDP receiving have separate limits for each client.
 * - In UDP_PER_CLIENT_PER_OPERATION different operations do not have separate limits.
 *
 */

/**
 * @page handshakePage	Server/Client Handshake
 * This page explains exactly what happens when a client attempts to connect to the server, and the impact
 * that disabling the TCP handshake will have.\n\n
 *
 * @section handshakeProcess The Process
 * The below process is implemented by NetInstanceClient and NetInstanceServer. \n\n
 *
 * size_t is either a 32 bit unsigned integer or a 64 bit unsigned integer depending on which build of the API
 * you are using. If you are running the 32 bit version, 4 bytes of padding are added after each 32 bit size_t
 * meaning that there are always 64 bits in a size_t. This ensures that all builds of the API can operate
 * together.\n\n
 *
 * @subsection handshakeProcessActual Process
 * - Client attempts to connect to server via TCP.\n
 *
 * - Client is rejected if the server is full.
 * - If the client is accepted, it is sent a TCP packet from the server which contains:
 *		- size_t: Maximum number of clients that can be connected to server.
 *		- size_t: Number of UDP operations (only if UDP is enabled).
 *		- signed char: UDP mode (only if UDP is enabled).
 *		- size_t: Client ID of newly connected client.
 *		- int: Authentication code (only if UDP is enabled).
 *		- int: Authentication code (only if UDP is enabled).
 *		- int: Authentication code (only if UDP is enabled).
 *		- int: Authentication code (only if UDP is enabled).\n
 *
 * - Client receives packet.
 * - If UDP is disabled is the client is now fully connected and the connection process is over.
 * - If UDP is enabled the process continues as follows.\n
 *
 * - Client sends UDP packet to server. The purpose of this packet is to traverse the client's NAT
 *   and validate the UDP connection. The client repeatedly sends this packet to avoid problems
 *   with packet loss. It contains:
 *		- size_t: Prefix of 0 indicating the packet's purpose.
 *		- size_t: Client's client ID.
 *		- int: Authentication code.
 *		- int: Authentication code.
 *		- int: Authentication code.
 *		- int: Authentication code.\n\n
 *
 * - Server receives UDP packet.
 * - If the client was not validated successfully the server forcefully disconnects the client.
 * - If the client is validated successfully then in order to signal that the connection process is over and tell the client that it 
 *   can stop sending the UDP packet the server sends a TCP packet to the client containing no data (except for the prefix indicating a length of 0).
 *
 * - If at any point in this process either the server's connection timeout expires or the client's timeout expires
 *   then the connection process is aborted.\n\n
 *
 * @section handshakeSecurity Security
 * @subsection handshakeSecurityAuthentication Authentication
 * Authentication codes are random integers generated by the server and are used to prevent malicious activity in the following example:
 * - Normal client begins connecting and finalizes TCP connection.
 * - Server is now waiting for UDP packet which may come from a different IP or port to the TCP connection.
 * - Malicious client sends UDP packet to server attempting to hijack normal client's connection.\n
 *
 * In this example malicious client only needs to guess the client ID if there are no authentication codes.
 * With authentication codes however, it is near impossible
 * for a malicious client to hijack a connection in this way.\n\n
 *
 * @subsection handshakeSecurityConnectionTimeout Connection Timeout
 * If the server is spammed with connection attempts that never complete it would eventually throw an error due to running out of memory.
 *
 * From the moment that a client first communicates with the server, it is allowed a set amount of time
 * to complete the handshaking process before the process is aborted and the client is forcefully silently
 * disconnected.\n\n
 *
 * @section handshakeDisable Impact of disabling handshake
 * Disabling the handshake process has the following impact:
 * - All UDP commands will fail.
 * - Clients will be unable to retrieve the following using in built methods/commands:
 *		- Their client ID.
 *		- The maximum number of clients that can be connected to the server.
 */

#ifdef WEBSITE_MODE
/**
 * @mainpage MikeNet v2.0.2
 *
 * @section intro_sec Introduction
 * MikeNet is an <a href="https://github.com/watfordxp/MikeNet"><b>open source</b></a> well documented windows networking API for C++, .NET based languages (e.g. C#, VB.NET), DarkBASIC Pro and any language that has the ability to call functions of a DLL.
 * MikeNet is lightweight, flexible and efficient and took approximately one year to develop. It includes the following core functionality:
 * - TCP data transfer using packets.
 * - UDP data transfer.
 * - Broadcasting (optionally to a specific subnet).
 * - VoIP module.
 * - Multithreaded encryption and decryption using Advanced Encryption Standard.
 * - Universal Plug and Play NAT traversal.
 *
 * @section level_sec From low to high
 * MikeNet consists of various different layers, which helps to limits the trade off between performance and ease of use.\n\n
 *
 * The lower levels are only available to C++ users, who can use the classes provided to create more complex, high performing systems. Users can 
 * extend classes and plug them into other subsystems of MikeNet. e.g. You can create a class inheriting NetModeUdp and create your own UDP mode, which can then
 * be plugged into other classes such as NetSocketUDP or NetInstanceServer. This makes MikeNet highly flexible, and means that it can be easily modified to fulfil
 * any networking requirement.
 *
 * At higher levels you have a set of easy to use commands built for usage in a procedural manner. These commands can be found in @link mn networking@endlink, @link mnSound sound@endlink and @link mnNAT UPnP NAT@endlink sections and are
 * available to all languages. Using these commands you can quickly set up a scalable server that allows clients to connect via TCP and UDP; click
 * @ref page_sample_server_code "here" for some sample C++ code.
 *
 * @section demo_sec Demo Code
 * You can download a visual studio 2010 and 2012 solution containing a number of sample projects using MikeNet from <a href="demo_code.rar"><b>here</b></a>.
 *
 * @section contact_sec Contact
 * If you have any questions about MikeNet feel free to email <a href="mailto:pryor.michael@gmail.com" target="_blank"><b>pryor.michael@gmail.com</b></a>. I will usually reply within 24 hours.
 * 
 * @section Download
 * I have made a precompiled version with an installer which can quickly be integrated into your projects. The package includes:
 * - C++ 32 bit and 64 bit debug and release libraries and pdb files for visual studio 2010 and 2012
 * - C++ 32 bit release library and pdb file for visual studio 2008
 * - DarkBASIC Pro DLL
 * - .NET/CLR 32 bit and 64 bit DLL
 * - Normal 32 bit and 64 bit DLL
 * - Documentation
 * - Demo code
 *
 * You can get it from <a href="DarkNet v2.0.2 - Release_TGC.rar"><b>here</b></a>.
 *
 * @endhtmlonly
 */

/**
 * @page page_sample_server_code Sample high level server code
@code
// Demonstrates how to use 'catch all'(3) and 'catch all, no out of order'(4) UDP mode on the server side
// Both modes are used in exactly the same way, but no out of order packets can be received in UDP mode 4


#include <MikeNet/FullInclude.h>
#include <windows.h>
#include <iostream>
using namespace std;

// Operations
const size_t OP_FLOAT = 0;
const size_t OP_STRING = 1;
const size_t OP_INT = 2;

// Entry point
void main()
{
	// Setup server
	size_t returnVal = 0;
	cout << "MikeNet version: " << mn::GetVersion() << "\n";

	INT_PTR recvPacket = mn::CreatePacket();
	INT_PTR sendPacket = mn::CreatePacket();
	mn::SetMemorySize(sendPacket,1024);

	mn::Start(1,0);

	// Determine IP of localhost
	const char * localHost = mn::DNS_B("localhost");

	INT_PTR profile = mn::CreateInstanceProfile();
	mn::SetProfileLocal(profile,localHost,65000,localHost,65000);
	mn::SetProfileModeUDP(profile,NetMode::UDP_CATCH_ALL_NO);
	
	mn::StartServer(0,50,profile);
	cout << "Server started on TCP port " << mn::GetLocalPortTCP(0) << " and UDP port " << mn::GetLocalPortUDP(0) << '\n';

	// Main loop
	while(true)
	{
		// Use less CPU
		Sleep(1);

		// New clients
		returnVal = mn::ClientJoined(0);
		if(returnVal > 0)
		{
			cout << "New client joined, ID: " << returnVal << "\n";
		}

		// Leaving clients
		returnVal = mn::ClientLeft(0);
		if(returnVal > 0)
		{
			cout << "Client left, ID: " << returnVal << "\n";
		}

		for(size_t cl = 1;cl<=mn::GetMaxClients(0);cl++)
		{
			// TCP packets
			returnVal = mn::RecvTCP(0,recvPacket,cl);
		
			if(returnVal > 0)
			{
				cout << "New TCP packet received from client " << returnVal << "\n";
			}
		
			// UDP packets
			returnVal = mn::RecvUDP(0,recvPacket,cl,0);
				
			if(returnVal > 0)
			{
				// Determine operation
				size_t op = mn::GetSizeT(recvPacket);

				// Begin formulating packet to send to clients
				mn::AddSizeT(sendPacket,op);
				mn::AddSizeT(sendPacket,cl);

				// Decide what to do depending on operation
				switch(op)
				{
					case(OP_FLOAT):
					{
						float fl = mn::GetFloat(recvPacket);
						cout << "Float received from client " << cl << ": " << fl << '\n';
						
						// Finish formulating packet to send to clients
						mn::AddFloat(sendPacket,fl);
					}
					break;

					case(OP_STRING):
					{
						char * str = mn::GetStringC(recvPacket,0,1);
						cout << "String received from client " << cl << ": " << str;

						// Finish formulating packet to send to clients
						mn::AddStringC(sendPacket,str,0,true);

						// Deallocate memory allocated by mn::GetStringC
						delete[] str;
					}
					break;

					case(OP_INT):
					{
						int i = mn::GetInt(recvPacket);
						cout << "Integer received from client " << cl << ": " << i << '\n';

						// Finish formulating packet to send to clients
						mn::AddInt(sendPacket,i);
					}
					break;

					default:
						cout << "Invalid operation\n";
						system("PAUSE");
					break;
				}

				// Send packet to all clients (excluding the client we received it from)
				mn::SendAllUDP(0,sendPacket,0,1,cl);
			}
		}
	}
}
@endcode
 */
#endif