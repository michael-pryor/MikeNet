#include "FullInclude.h"

/**
 * @brief Sets all variables to default.
 */
void NetSocketSimple::DefaultVariables()
{
	addressConnected.Clear();
	addressLocal.Clear();
	winsockSocket = INVALID_SOCKET;
	bound = false;
	fullyOperational = false;
	nagleEnabled = true;
	hardShutdown = false;
	reusable = false;
	broadcasting = false;
	listening = false;

}

/**
 * @brief Constructor.
 */
NetSocketSimple::NetSocketSimple()
{
	DefaultVariables();
}

/**
 * @brief Shallow copy / assignment operator helper.
 *
 * @param	copyMe	Object to copy.
 */
void NetSocketSimple::Copy(const NetSocketSimple & copyMe)
{
	this->addressConnected = copyMe.addressConnected;
	this->winsockSocket = copyMe.winsockSocket; // No valid deep copy can be made
	this->bound = copyMe.bound;
	this->fullyOperational = copyMe.fullyOperational;
	this->nagleEnabled = copyMe.nagleEnabled;
	this->hardShutdown = copyMe.hardShutdown;
	this->reusable = copyMe.reusable;
	this->broadcasting = copyMe.broadcasting;
	this->listening = copyMe.listening;
}

/**
 * @brief	Equality operator. 
 *
 * @param	compare	Object to compare with this object.
 *
 * @return	true if @a compare is identical to this object. 
 */
bool NetSocketSimple::operator==(const NetSocketSimple & compare) const
{
	return this->addressConnected == compare.addressConnected &&
		   this->winsockSocket == compare.winsockSocket &&
		   this->bound == compare.bound &&
	       this->fullyOperational == compare.fullyOperational &&
	       this->nagleEnabled == compare.nagleEnabled &&
	       this->hardShutdown == compare.hardShutdown &&
	       this->reusable == compare.reusable &&
	       this->broadcasting == compare.broadcasting &&
	       this->listening == compare.listening;
}

/**
 * @brief	Inequality operator. 
 *
 * @param	compare	Object to compare with this object.
 *
 * @return	false if @a compare is identical to this object. 
 */
bool NetSocketSimple::operator!=(const NetSocketSimple & compare) const
{
	return !(*this == compare);
}

/**
 * @brief Shallow copy constructor.
 *
 * @param	copyMe	Object to copy.
 */
NetSocketSimple::NetSocketSimple(const NetSocketSimple & copyMe)
{
	Copy(copyMe);
}

/**
 * @brief Shallow assignment operator.
 *
 * @param	copyMe	Object to copy. 
 * @return	reference to this object.
 */
NetSocketSimple & NetSocketSimple::operator= (const NetSocketSimple & copyMe)
{
	Copy(copyMe);
	return *this;
}

/**
 * @brief Destructor.
 */
NetSocketSimple::~NetSocketSimple()
{
	if(IsSetup() == true)
	{
		Close();
	}
}

/**
 * @brief Begin closing winsock socket.
 *
 * @warning Cancel notifications are sent out to all pending operations
 * but the socket itself is not closed until all operations have
 * finished which may or may not have occurred by the time this method returns.
 * Do not assume that the socket is fully closed and that all operations have completed
 * once this method returns.
 *
 * @throws ErrorReport If the socket is not setup.
 */
void NetSocketSimple::Close()
{
	if(NetUtility::IsWinsockActive() == true && winsockSocket != INVALID_SOCKET)
	{
		// Start the process of closing the socket
		int iResult = closesocket(winsockSocket);
		_ErrorException((iResult==SOCKET_ERROR),"closing a socket",WSAGetLastError(),__LINE__,__FILE__);
	}

	DefaultVariables();
}

/** 
 * @brief Retrieves local address information about the socket.
 *
 * @return local address information of the socket. This reference will remain
 * valid and unmodified until the next call.
 */
const NetAddress & NetSocketSimple::GetLocalAddress() const
{
	_ErrorException((IsSetup() == false),"retrieving local address information about a socket, the socket is not setup",0,__LINE__,__FILE__);

	addressLocal.Enter();
		sockaddr * address = reinterpret_cast<sockaddr*>(addressLocal.GetAddrPtr());
		int result = getsockname(winsockSocket,address,NetUtility::GetSizeSOCKADDR());
	addressLocal.Leave();
	_ErrorException((result==SOCKET_ERROR),"getting local address information about a socket",WSAGetLastError(),__LINE__,__FILE__);

	return addressLocal;
}

/**
 * @brief Sets up the socket. After being set up socket options can be changed.
 *
 * @param protocol Protocol that socket should support.
 *
 * @throws ErrorReport If the socket is already setup.
 */
void NetSocketSimple::Setup(NetSocketSimple::Protocol protocol)
{
	_ErrorException((IsSetup() == true),"setting up a socket, socket is already setup",0,__LINE__,__FILE__);

	switch(protocol)
	{
		case(NetSocketSimple::UDP):
			winsockSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
		break;

		case(NetSocketSimple::TCP):
			winsockSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		break;

		default:
			_ErrorException(true,"invalid protocol, must be either TCP or UDP",0,__LINE__,__FILE__);
		break;
	}

	_ErrorException((winsockSocket==INVALID_SOCKET),"setting up a socket",WSAGetLastError(),__LINE__,__FILE__);
}

/**
 * @brief Determines whether the socket has been setup.
 *
 * The socket can be setup using Setup().
 *
 * @return true if the socket is setup, false if it is not.
 */
bool NetSocketSimple::IsSetup() const
{
	return !(winsockSocket == INVALID_SOCKET);
}


/** 
 * @brief Enables the reusable option for the socket.
 *
 * See NetSocketSimple::reusable for more information.
 *
 * @throws ErrorReport If the socket is not setup. 
 * @throws ErrorReport If the socket has been bound.
 */
void NetSocketSimple::SetReusable()
{
	_ErrorException((IsSetup() == false),"making a socket reusable, the socket has not been setup",0,__LINE__,__FILE__);
	_ErrorException((IsBound() == true),"making a socket reusable, the socket has been bound",0,__LINE__,__FILE__);

	bool bOption = 1;
	int iResult = setsockopt(winsockSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&bOption,sizeof(bool));
	_ErrorException((iResult==SOCKET_ERROR),"making a socket reusable",WSAGetLastError(),__LINE__,__FILE__);

	reusable = true;
}

/**
 * @brief Retrieves the reusable option for the socket.
 *
 * See NetSocketSimple::reusable for more information.
 *
 * The socket can be set to reusable using SetReusable().
 *
 * @return true if the socket is reusable, false if not.
 */
bool NetSocketSimple::IsReusable() const
{
	return reusable;
}

/**
 * @brief Binds the socket to a local address.
 *
 * @param localAddr Local address to bind to. If IP or port value is 0 then winsock
 * will choose one automatically.
 *
 * @throws ErrorReport If the socket is not setup. 
 * @throws ErrorReport If the socket has already been bound.
 */
void NetSocketSimple::Bind(const NetAddress & localAddr)
{
	_ErrorException((IsBound() == true),"binding a socket, the socket is already bound",0,__LINE__,__FILE__);
	_ErrorException((IsSetup() == false),"binding a socket, the socket has not been setup",0,__LINE__,__FILE__);

	// Manually set local IP so that mnGetLocalIP commands work in all situation
	// (bug fix)
	/*
	NetAddress localAddrCopy = localAddr;
	sockaddr_in & refLocalAddr = *localAddrCopy.GetAddrPtr();
	if(refLocalAddr.sin_addr.S_un.S_addr == 0)
	{
		if(NetUtility::GetNumLocalInterface() > 0)
		{
			refLocalAddr.sin_addr = NetUtility::GetLocalInterface(0).GetAddrPtr()->sin_addr;
		}
	}*/

	int iResult;

	// Bind
	localAddr.Enter();
		iResult = bind(winsockSocket, (SOCKADDR*)localAddr.GetAddrPtr(), *NetUtility::GetSizeSOCKADDR());
	localAddr.Leave();
	_ErrorException((iResult==SOCKET_ERROR),"binding a socket",WSAGetLastError(),__LINE__,__FILE__);

	// Non blocking mode 
	u_long ulOption = 1;
	iResult = ioctlsocket(winsockSocket, FIONBIO, &ulOption);
	_ErrorException((iResult==SOCKET_ERROR),"setting a socket to non blocking mode",WSAGetLastError(),__LINE__,__FILE__);

	bound = true;
}

/**
 * @brief Determines whether the socket has been bound.
 *
 * The socket can be bound using Bind().
 *
 * @return true if the socket has been bound.
 */
bool NetSocketSimple::IsBound() const
{
	return bound;
}

/**
 * @brief Enables the hard shutdown option.
 *
 * See NetSocketSimple::hardShutdown for more information.
 *
 * @throws ErrorReport If the socket is not setup.
 * @throws ErrorReport If the socket has been bound.
 */
void NetSocketSimple::SetHardShutdown()
{
	_ErrorException((IsSetup() == false),"enabling hard shutdown on a socket, the socket has not been setup",0,__LINE__,__FILE__);
	_ErrorException((IsBound() == true),"enabling hard shutdown on a socket, the socket has been bound",0,__LINE__,__FILE__);

	linger structLingerTCP;
	structLingerTCP.l_onoff = 1;
	structLingerTCP.l_linger = 0;

	int iResult = setsockopt(winsockSocket,SOL_SOCKET,SO_LINGER,(char*)&structLingerTCP,sizeof(linger) );
	_ErrorException((iResult==SOCKET_ERROR),"setting a socket to hard shutdown mode",WSAGetLastError(),__LINE__,__FILE__);

	hardShutdown = true;
}

/**
 * @brief Determines whether hard shutdown is enabled.
 *
 * See SetHardShutdown() for more information.
 *
 * @return true if hard shutdown is enabled.
 */
bool NetSocketSimple::IsHardShutdownEnabled() const
{
	return hardShutdown;
}

/** 
 * @brief Sets the socket to listening mode.
 *
 * See NetSocketSimple::listening for more information.
 *
 * @throws ErrorReport If the socket is not setup.
 * @throws ErrorReport If the socket has been bound.
 */
void NetSocketSimple::SetListening()
{
	_ErrorException((IsSetup() == false),"setting a socket to listening mode, the socket has not been setup",0,__LINE__,__FILE__);
	_ErrorException((IsBound() == false),"setting a socket to listening mode, the socket has not been bound",0,__LINE__,__FILE__);

	int iResult = listen(winsockSocket, SOMAXCONN);
	_ErrorException((iResult==SOCKET_ERROR),"setting a socket to listening mode",WSAGetLastError(),__LINE__,__FILE__);

	listening = true;
	fullyOperational = true;
}

/**
 * @brief Determines whether the socket is in listening mode.
 *
 * See NetSocketSimple::listening for more information.
 *
 * @return true if socket is in listening mode, false if not.
 */
bool NetSocketSimple::IsListening() const
{
	return listening;
}

/**
 * @brief Enables broadcasting.
 *
 * See NetSocketSimple::broadcasting for more information.
 *
 * @throws ErrorReport If the socket is not setup.
 * @throws ErrorReport If the socket has been bound.
 */
void NetSocketSimple::SetBroadcasting()
{
	_ErrorException((IsSetup() == false),"setting a socket to broadcast mode, the socket has not been setup",0,__LINE__,__FILE__);
	_ErrorException((IsBound() == true),"setting a socket to broadcasting mode, the socket has been bound",0,__LINE__,__FILE__);

	bool bOption = true;
	int iResult = setsockopt(winsockSocket, SOL_SOCKET, SO_BROADCAST,(char*)&bOption,sizeof(bool));
	_ErrorException((iResult==SOCKET_ERROR),"setting a socket to broadcasting mode",WSAGetLastError(),__LINE__,__FILE__);

	broadcasting = true;
}

/**
 * @brief Determines whether broadcasting is enabled.
 *
 * See NetSocketSimple::broadcasting for more information.
 *
 * @return true if broadcasting is enabled, false if not.
 */
bool NetSocketSimple::IsBroadcasting() const
{
	return broadcasting;
}


/** 
 * @brief Disables the Nagle algorithm.
 *
 * See NetSocketSimple::nagleEnabled for more information.
 *
 * @throws ErrorReport If the socket is not setup.
 * @throws ErrorReport If the socket has been bound.
 */
void NetSocketSimple::DisableNagle()
{
	_ErrorException((IsSetup() == false),"disabling the nagle algorithm on a socket, the socket has not been setup",0,__LINE__,__FILE__);
	_ErrorException((IsBound() == true),"disabling the nagle algorithm on a socket, the socket has been bound",0,__LINE__,__FILE__);

	bool bOption = true;
	int iResult = setsockopt(winsockSocket, SOL_SOCKET, TCP_NODELAY,(char*)&bOption,sizeof(bool));
	_ErrorException((iResult==SOCKET_ERROR),"disabling the nagle algorithm on a TCP socket",WSAGetLastError(),__LINE__,__FILE__);

	nagleEnabled = false;
}

/**
 * @brief Determines whether the Nagle algorithm is enabled.
 */
bool NetSocketSimple::IsNagleEnabled() const
{
	return nagleEnabled;
}

/**
 * @brief Loads a winsock socket object that is already fully operational.
 *
 * @param operationalSocket	Operational winsock socket.
 * @param addressConnected Address that @a operationalSocket is connected to.
 *
 * @throws ErrorReport If this socket has already been setup.
 */
void NetSocketSimple::LoadSOCKET(SOCKET operationalSocket, const NetAddress & addressConnected)
{
	_ErrorException((IsSetup() == true),"loading a socket, this socket is already setup",0,__LINE__,__FILE__);

	winsockSocket = operationalSocket;
	this->addressConnected = addressConnected;

	bound = true;
	fullyOperational = true;
}

/**
 * @brief Connects the socket to a remote address.
 *
 * @param connectAddr Address to connect to.
 *
 * @throws ErrorReport If the socket is not setup.
 */
void NetSocketSimple::Connect(const NetAddress & connectAddr)
{
	_ErrorException((IsSetup() == false),"connecting a socket, the socket is not setup",0,__LINE__,__FILE__);

	int iResult = connect(winsockSocket,(SOCKADDR*)connectAddr.GetAddrPtr(),sizeof(sockaddr));
	_ErrorException(((iResult==SOCKET_ERROR) && (WSAGetLastError() != WSAEWOULDBLOCK)),"attempting to connect a socket", WSAGetLastError(),__LINE__,__FILE__);

	addressConnected = connectAddr;

	fullyOperational = true;
}

/**
 * @brief Determines whether the socket is fully operational.
 *
 * @return true if the socket is fully operational.
 */
bool NetSocketSimple::IsFullyOperational() const
{
	return fullyOperational;
}

/**
 * @brief Manually changes the fully operational setting.
 *
 * @param operational Option.
 */
void NetSocketSimple::SetFullyOperational(bool operational)
{
	fullyOperational = operational;
}

/**
 * @brief Retrieves the address that the socket is connected to.
 *
 * @return reference to address that socket is connected to.
 */
const NetAddress & NetSocketSimple::GetAddressConnected() const
{
	return addressConnected;
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetSocketSimple::TestClass()
{
	bool problem = false;
	cout << "Testing NetSocketSimple class...\n";
	
	NetUtility::StartWinsock();
	NetUtility::LoadLocalInfo();

	// UDP
	{
		NetSocketSimple socket;

		cout << "Setting up UDP socket..\n";
		socket.Setup(NetSocketSimple::UDP);
		cout << "Socket setup: " << socket.IsSetup() << '\n';
	
		cout << "Setting socket to reusable..\n";
		socket.SetReusable();
		cout << "Socket reusable: " << socket.IsReusable() << '\n';

		cout << "Setting socket to broadcasting..\n";
		socket.SetBroadcasting();
		cout << "Socket broadcasting: " << socket.IsBroadcasting() << '\n';

		cout << "Binding to " << NetUtility::GetLocalInterface(0).GetIP() << " local address..\n";
		socket.Bind(NetUtility::GetLocalInterface(0));
		cout << "Socket bound: " << socket.IsBound() << '\n';
		
		cout << "Connecting to " << NetUtility::GetLocalInterface(0).GetIP() << " address..\n";
		NetAddress connectAddr = NetUtility::GetLocalInterface(0);
		connectAddr.SetPort(6000);
		socket.Connect(connectAddr);
		cout << "Socket operational: " << socket.IsFullyOperational() << '\n';
		
		NetAddress connectedAddress;
		connectedAddress = socket.GetAddressConnected();
		char * strConnectedAddress = connectedAddress.ToString();
		cout << "Connected to " << strConnectedAddress << ".\n";
		delete[] strConnectedAddress;

		// Note: will always return 0.0.0.0 because we have not sent data at this point.
		// After that local address retrieved will be correct.
		NetAddress localAddress;
		localAddress = socket.GetLocalAddress();
		char * strLocalAddr = localAddress.ToString();
		cout << "Bound to " << strLocalAddr << ".\n";
		delete[] strLocalAddr;
		
		cout << "Closing socket..\n";
		socket.Close();
	}

	cout << "\n\n";

	// TCP
	{
		NetSocketSimple socket;

		cout << "Setting up TCP socket..\n";
		socket.Setup(NetSocketSimple::TCP);
		cout << "Socket setup: " << socket.IsSetup() << '\n';

		cout << "Setting socket to reusable..\n";
		socket.SetReusable();
		cout << "Socket reusable: " << socket.IsReusable() << '\n';

		cout << "Disabling nagle algorithm..\n";
		socket.DisableNagle();
		cout << "Nagle algorithm: " << socket.IsNagleEnabled() << '\n';

		cout << "Setting socket to hard shutdown mode..\n";
		socket.SetHardShutdown();
		cout << "Hard shutdown: " << socket.IsHardShutdownEnabled() << '\n';

		cout << "Binding to " << NetUtility::GetLocalInterface(0).GetIP() << " local address..\n";
		socket.Bind(NetUtility::GetLocalInterface(0));
		cout << "Socket bound: " << socket.IsBound() << '\n';

		// Connection attempt may not necessarily succeed straight away, or ever.
		// Connect simple starts the process.
		NetAddress connectAddress = NetUtility::ConvertDomainNameToIP("www.google.com");
		connectAddress.SetPort(80);
		char * strConnectAddress = connectAddress.ToString();
		cout << "Connecting to " << strConnectAddress << "..\n";
		delete[] strConnectAddress;

		socket.Connect(connectAddress);

		cout << "Operational: " << socket.IsFullyOperational() << '\n';

		cout << "Closing socket..\n";
		socket.Close();
	}

	cout << "\n\n";

	// TCP listening.
	{
		NetSocketSimple socket;

		cout << "Setting up TCP listening socket..\n";
		socket.Setup(NetSocketSimple::TCP);
		cout << "Socket setup: " << socket.IsSetup() << '\n';

		NetAddress bindTo = NetUtility::GetLocalInterface(0);
		bindTo.SetPort(6543);
		char * strBindTo = bindTo.ToString();
		cout << "Binding to " << strBindTo << "..\n";
		delete[] strBindTo;

		socket.Bind(bindTo);
		cout << "Socket bound: " << socket.IsBound() << '\n';

		cout << "Setting socket to listening mode..\n";
		socket.SetListening();
		cout << "Socket listening: " << socket.IsListening() << '\n';

		cout << "Operational: " << socket.IsFullyOperational() << '\n';

		cout << "Closing socket..\n";
		socket.Close();
	}

	// Copy constructor
	{
		NetSocketSimple socket;
		NetSocketSimple copy(socket);

		if(socket != copy)
		{
			cout << "Copy constructor or equality operator is bad\n";
			problem = true;	
		}
		else
		{
			cout << "Copy constructor and equality operator are good\n";
		}
	}

	// Assignment operator
	{
		NetSocketSimple socket;
		NetSocketSimple copy;
		copy = socket;

		if(socket != copy)
		{
			cout << "Assignment operator or equality operator is bad\n";
			problem = true;
		}
		else
		{
			cout << "Assignment operator and equality operator are good\n";
		}
	}
	NetUtility::UnloadLocalInfo();
	NetUtility::FinishWinsock();

	cout << "\n\n";
	return !problem;
}
