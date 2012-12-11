#include "FullInclude.h"

/**
 * @brief Constructor.
 *
 * @param localAddr Local address to bind to, if IP or port is set to 0 then winsock will automatically find one.
 * @param [in] clientSocketTemplate Client socket template which describes how clients (who's connections are accepted by this listening socket)
 * should deal with data received and how big their receive buffers should be. \n
 * A copy of this object is passed to each client during server instance setup. \n
 * The object is not directly used by NetSocketListening and so this class will not clean it up.
 */
NetSocketListening::NetSocketListening(const NetAddress & localAddr, NetSocketTCP * clientSocketTemplate) : NetSocketSimple()
{
	try
	{
		this->clientSocketTemplate = clientSocketTemplate;

		Setup(NetSocketSimple::TCP);
		SetReusable(); // If removed, modify constructor in NetSocketTCP so that it does not set reusable to true.

		if(clientSocketTemplate->IsGracefulDisconnectEnabled() == false)
		{
			SetHardShutdown();
		}

		if(clientSocketTemplate->IsNagleEnabled() == false)
		{
			DisableNagle();
		}

		Bind(localAddr);

		SetListening();
	}
	catch(ErrorReport & report)
	{
		delete clientSocketTemplate;
		throw report;
	}
}

/**
 * @brief	Destructor. 
 */
NetSocketListening::~NetSocketListening()
{
	delete clientSocketTemplate;
}

/**
 * @brief Retrieves a constant pointer to the client socket template.
 *
 * @return client socket template. A copy of this object is passed to each client during server instance setup.
 */
const NetSocketTCP * NetSocketListening::GetSocket() const
{
	return clientSocketTemplate;
}

/**
 * @brief Retrieves a mutable deep copy of the client socket template.
 *
 * @return a deep copy of the client socket template.
 */
NetSocketTCP * NetSocketListening::GetCopySocket() const
{
	return Utility::CopyObject(*clientSocketTemplate);
}

/**
 * @brief Called by AcceptConnection() when a TCP connection request is received.
 *
 * @note Method must be defined outside of class exactly as is for winsock to accept it.
 *
 * @param lpCallerID Ignored.
 * @param lpCallerData Ignored.
 * @param lpSQOS Ignored.
 * @param lpGQOS Ignored.
 * @param lpCalleeID Ignored.
 * @param lpCalleeData Ignored.
 * @param g Ignored.
 * @param dwCallbackData 0 if no unused client IDs exist,
 * or the ID that will be assigned to the connecting client if an unused ID does exist.
 *
 * @return @c CF_ACCEPT if the TCP connection request was accepted. This occurs if there is a free client ID.
 * @return @c CF_REJECT if the TCP connection request was rejected. This occurs if there is not a free client ID.
 */
int CALLBACK _AcceptDenyClient(IN LPWSABUF lpCallerID, IN LPWSABUF lpCallerData, IN OUT LPQOS lpSQOS, IN OUT LPQOS lpGQOS,
							   IN LPWSABUF lpCalleeID, OUT LPWSABUF lpCalleeData, OUT GROUP FAR *g, IN DWORD_PTR dwCallbackData)
{
// 	sockaddr_in * addrCaller = (sockaddr_in*)lpCallerID->buf;
// 	sockaddr_in * addrCallee = (sockaddr_in*)lpCalleeID->buf;
// 
// 	NetAddress finalAddrCaller(*addrCaller);
// 	NetAddress finalAddrCallee(*addrCallee);
// 
// 	const char * strCaller = finalAddrCaller.ToString();
// 	const char * strCallee = finalAddrCallee.ToString();



	if(dwCallbackData == 0)
	{
		return (CF_REJECT);
	}
	else
	{
		return (CF_ACCEPT);
	}
}

/**
 * @brief Deals with any pending connection attempts.
 *
 * _AcceptDenyClient is called by @c WSAAccept and its return value determines whether a client is accepted or rejected.
 * 
 * @param testValue If testValue is 0 the connection is rejected, otherwise it is accepted.
 * If non 0 then it represents the client ID that will be assigned to the connecting client.
 * @param addr [out] Address of newly connected client will be copied to here.
 *
 * @throws ErrorReport Iff an error occurs in WSAAccept, but not if a connection attempt fails or 
 * no connection attempts were found on this call.
 *
 * @return new winsock socket object capable of transferring TCP data with newly connected client.
 * @return INVALID_SOCKET if no TCP connection was successfully accepted on this call.
 */
SOCKET NetSocketListening::AcceptConnection(size_t testValue, NetAddress * addr)
{
	// Check for new TCP clients
	// unusedClientID is passed to _AcceptDenyClient, if this is 0, then there are
	// no unused client IDs and the function will reject the client
	// Note: WSAAccept should still be used even if we know the client will be rejected, this is
	// so that the client does not time out, it instead receives indication that it was rejected
	// Note: sockets created via WSAAccept have same properties as the listening socket
	addr->Enter();
	SOCKET newSocket = WSAAccept(winsockSocket, (sockaddr*)addr->GetAddrPtr(),NetUtility::GetSizeSOCKADDR(), &_AcceptDenyClient, (DWORD_PTR)testValue);
	addr->Leave();

	 // Check for errors, ignoring some errors (explained below):
	 // WSAEWOULDBLOCK means no connections were accepted
	 // WSAECONNREFUSED means connection was refused due to _AcceptDenyClient return value
	_ErrorException((newSocket == INVALID_SOCKET && WSAGetLastError() != WSAEWOULDBLOCK && WSAGetLastError() != WSAECONNREFUSED),"whilst attempting to accept a new TCP connection",WSAGetLastError(),__LINE__,__FILE__);

	return newSocket;
}

/**
 * @brief	Helper test class. 
 *
 * @param [in,out]	listeningSocket					Listening socket to use. 
 * @param [in,out]	listeningSocketClient			Listening socket client, where newly connected client information will be stored.
 * @param [in,out]	client							Client to connect to listening socket.
 *
 * @return	true if it succeeds, false if it fails. 
 */
bool NetSocketListening::HelperTestClass(NetSocketListening & listeningSocket, NetSocketTCP & listeningSocketClient, NetSocketTCP & client)
{
	bool problem = false;

	char * convertedToStr = listeningSocket.GetLocalAddress().ToString();
	cout << "Listening socket is bound to address: " << convertedToStr << "..\n";
	delete[] convertedToStr;

	// Connect client to listening socket
	convertedToStr = client.GetLocalAddress().ToString();
	cout << "Connecting a TCP client with local address of " << convertedToStr << " to listening socket..\n";
	delete[] convertedToStr;

	client.Connect(listeningSocket.GetLocalAddress());

	cout << "Waiting for the connection to complete..\n";
	NetAddress localAddrClientListening;
		
	// Once connected, listening socket creates a socket object which is able to communicate with 
	// the newly connected client
	SOCKET newSocket = INVALID_SOCKET;
	do
	{
		newSocket = listeningSocket.AcceptConnection(1,&localAddrClientListening);

		if(newSocket != INVALID_SOCKET)
		{
			listeningSocketClient.LoadSOCKET(newSocket,localAddrClientListening);
			break;
		}
	} while (client.PollConnect() == true && newSocket != INVALID_SOCKET);

	cout << "Client is now connected to listening socket!\n";

	// Check that listeningSocketClient is correct.
	if( localAddrClientListening != client.GetLocalAddress() ||
		listeningSocketClient.GetConnectionStatus() != NetUtility::CONNECTED ||
		listeningSocketClient.IsGracefulDisconnectEnabled() != listeningSocket.GetSocket()->IsGracefulDisconnectEnabled() ||
		listeningSocketClient.GetMode()->GetProtocolMode() != NetMode::TCP_PREFIX_SIZE ||
		listeningSocketClient.GetRecvBufferLength() != 1024 ||
		listeningSocketClient.IsBroadcasting() == true ||
		listeningSocketClient.IsBound() == false ||
		listeningSocketClient.IsHardShutdownEnabled() != listeningSocket.GetSocket()->IsHardShutdownEnabled() ||
		listeningSocketClient.IsSetup() == false ||
		listeningSocketClient.IsListening() == true ||
		listeningSocketClient.IsFullyOperational() == false ||
		listeningSocketClient.IsReusable() == false ||
		listeningSocketClient.IsNagleEnabled() != listeningSocket.IsNagleEnabled())
	{
		cout << "AcceptConnection or LoadSOCKET is bad\n";
		problem = true;
	}
	else
	{
		cout << "AcceptConnection and LoadSOCKET are good\n";
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
bool NetSocketListening::TestClass()
{
	cout << "Testing NetSocketListening class...\n";
	bool problem = false;

	NetUtility::StartWinsock();
	NetUtility::SetupCompletionPort(2);
	
	{
		const char * localHost = NetUtility::ConvertDomainNameToIP("localhost").GetIP();
		NetAddress localAddrListening(localHost,14000);
		NetAddress localAddrClient(localHost,5432);

		bool gracefulDisconnectEnabled = false;
		bool nagleEnabled = true;

		NetSocketListening listeningSocket(localAddrListening,new NetSocketTCP(1024,nagleEnabled,gracefulDisconnectEnabled,new NetModeTcpPrefixSize(2048,false)));
		NetSocketTCP listeningSocketClient(1024,nagleEnabled,gracefulDisconnectEnabled,new NetModeTcpPrefixSize(2048,false));

		NetSocketTCP client(1024,localAddrClient,nagleEnabled,gracefulDisconnectEnabled,new NetModeTcpPrefixSize(2048,false));

		if(HelperTestClass(listeningSocket,listeningSocketClient,client) == false)
		{
			problem = true;
		}
	}

	NetUtility::DestroyCompletionPort();
	NetUtility::FinishWinsock();
	
	cout << "\n\n";
	return !problem;
}