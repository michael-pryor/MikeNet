#include "FullInclude.h"

const NetAddress NetAddress::EmptyAddress;

/**
 * @brief Sets all variables to default.
 */
void NetAddress::DefaultVariables()
{
	Clear();
}

/**
 * @brief Default constructor.
 */
NetAddress::NetAddress()
{
	DefaultVariables();
}

/**
 * @brief Custom constructor.
 *
 * @param ip IP to load. Must be NULL terminated.
 * @param port Port to load.
 */
NetAddress::NetAddress(const char * ip, unsigned short port)
{
	Load(ip,port);
}

/**
 * @brief Copy constructor.
 *
 * @param copyMe Object to copy.
 */
NetAddress::NetAddress(const NetAddress & copyMe)
{
	this->Enter();
	copyMe.Enter();

		this->addr = copyMe.addr;

	copyMe.Leave();
	this->Leave();
}

/**
 * @brief Loads another NetAddress object into this object.
 *
 * @param copyMe Object to copy.
 *
 * @return reference to this object.
 */
NetAddress & NetAddress::operator=(const NetAddress & copyMe)
{
	return Load(copyMe);
}

/**
 * @brief Loads another NetAddress object into this object.
 *
 * @param copyMe Object to copy.
 *
 * @return reference to this object.
 */
NetAddress & NetAddress::Load(const NetAddress & copyMe)
{
	this->Enter();
	copyMe.Enter();

	this->addr = copyMe.addr;

	copyMe.Leave();
	this->Leave();

	return *this;
}

/**
 * @brief Equality operator.
 *
 * @param testMe Object to compare with this object.
 * @return true If @a testMe and this object are identical.
 * @return false If @a testMe and this object are different in any way.
 */
bool NetAddress::operator==(const NetAddress & testMe) const
{
	size_t readCount = 0;

	this->Enter();
	testMe.Enter();

	bool returnMe = ( this->addr.sin_addr.S_un.S_addr == testMe.addr.sin_addr.S_un.S_addr &&
					  this->addr.sin_family == testMe.addr.sin_family &&
					  this->addr.sin_port == testMe.addr.sin_port );

	testMe.Leave();
	this->Leave();
	return returnMe;
}

/**
 * @brief Equality operator.
 *
 * @param testMe Object to compare with this object.
 * @return false If @a testMe and this object are identical.
 * @return true If @a testMe and this object are different in any way.
 */
bool NetAddress::operator!=( const NetAddress & testMe ) const
{
	return !(*this == testMe);
}


/**
 * @brief Custom constructor.
 *
 * @param copyMe Object to copy.
 */
NetAddress::NetAddress(sockaddr_in copyMe)
{
	this->addr = copyMe;
}

/**
 * @brief Loads a sockaddr_in structure into this object.
 *
 * @param copyMe Object to copy.
 * 
 * @return reference to this object.
 */
NetAddress & NetAddress::Load(sockaddr_in copyMe)
{
	this->Enter();

	this->addr = copyMe;

	this->Leave();

	return *this;
}

/**
 * @brief Clears address, setting everything to default.
 */
void NetAddress::Clear()
{
	this->Enter();

	SecureZeroMemory(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	this->Leave();
}

/**
 * @brief Retrieves winsock address stored.
 *
 * @return winsock address stored by this object.
 */
sockaddr_in NetAddress::GetSockAddr() const
{
	Enter();
	sockaddr_in returnMe = addr;
	Leave();
	return returnMe;
}

/**
 * @brief Retrieves mutable pointer to internal address store.
 *
 * @return mutable pointer to address.
 * @warning Access to this pointer should be controlled using object's read/write control
 * and is not thread safe unless manually controlled.
 */
sockaddr_in * NetAddress::GetAddrPtr()
{
	return &addr;
}

/**
 * @brief Retrieves constant pointer to internal address store.
 *
 * @return constant pointer to address.
 * @warning Access to this pointer should be controlled using object's read/write control
 * and is not thread safe unless manually controlled.
 */
const sockaddr_in * NetAddress::GetAddrPtr() const
{
	return &addr;
}

/**
 * @brief Loads an IP and port into the object.
 *
 * @param ip IP to load, should be NULL terminated.
 * @param port Port to load (optional, default 0).
 *
 * @return reference to this object.
 */
NetAddress & NetAddress::Load(const char * ip, unsigned short port)
{
	Enter();

	try
	{
		addr.sin_family = AF_INET;
		SetIP(ip);
		SetPort(port);
	}
	// Release control of all objects before throwing final exception
	catch(ErrorReport & error){ Leave(); throw(error); }
	catch(...){ Leave(); throw(-1); }

	Leave();

	return *this;
}

/**
 * @brief Retrieves stored IP address.
 *
 * @return IP address as a NULL terminated C string.
 */
const char * NetAddress::GetIP() const
{
	Enter();
		const char * returnMe = inet_ntoa(addr.sin_addr);
	Leave();
	_ErrorException((returnMe == NULL),"converting an IP address from IN_ADDR to C string form",WSAGetLastError(),__LINE__,__FILE__);
	return returnMe;
}

/**
 * @brief Retrieves stored port.
 *
 * @return port in unsigned short form.
 */
unsigned short NetAddress::GetPort() const
{
	Enter();
		unsigned short returnMe = ntohs(addr.sin_port);
	Leave();
	return returnMe;
}

/**
 * @brief Retrieves stored address family.
 *
 * Default is AF_INET.
 *
 * @return address family.
 */
ADDRESS_FAMILY NetAddress::GetFamily() const
{
	Enter();
		ADDRESS_FAMILY returnMe = addr.sin_family;
	Leave();
	return returnMe;
}

/**
 * @brief Sets stored port.
 *
 * @param port New port.
 */
void NetAddress::SetPort(unsigned short port)
{
	Enter();
		addr.sin_port = htons(port);
	Leave();
}

/**
 * @brief Sets stored IP.
 *
 * @param IP New IP.
 */
void NetAddress::SetIP(const char * IP)
{
	Enter();
	try
	{
		if(IP == NULL || strlen(IP) == 0)
		{
			// winsock will automatically assign an IP.
			addr.sin_addr.S_un.S_addr = INADDR_ANY;
		}
		else
		{
			addr.sin_addr.S_un.S_addr = inet_addr(IP);
			// Don't check for errors because error is also signaled when an address of 255.255.255.255 is passed.
			//_ErrorException((addr.sin_addr.S_un.S_addr == INADDR_NONE),"converting an IP address",WSAGetLastError(),__LINE__,__FILE__);
		}
	}
	catch (ErrorReport & error){Leave(); throw(error);}
	catch (...){Leave(); throw(-1);}
	Leave();
}

/**
 * @brief Sets stored family of address.
 *
 * @param family New family (optional, default = AF_INET).
 */
void NetAddress::SetFamily(ADDRESS_FAMILY family)
{
	Enter();
		addr.sin_family = family;
	Leave();
}

/**
 * @brief	Returns the byte representation of the stored IP address.
 *
 * @return	the byte representation of the stored IP address.
 */
unsigned long int NetAddress::GetByteRepresentationIP() const
{
	Enter();
		unsigned long returnMe = addr.sin_addr.S_un.S_addr;
	Leave();
	return returnMe;
}

/**
 * @brief	Retrieve a string representation of this object. 
 *
 * @return	a string representation of this object in the form
 * (excluding ") "IP = 192.168.1.1, port = 6000".
 */
char * NetAddress::ToString() const
{
	Enter();
	
	// Retrieve IP
	const char * IP = GetIP();
	
	// Retrieve port
	unsigned short port = GetPort();
	char * strPort = Utility::ConvertFromIntToString(port);

	// Concatenate into human friendly string
	const char * concatMe[] = {"IP = ", IP, ", port = ", strPort};
	char * stringRepresentation = Utility::ConcatArray(concatMe,4);

	delete[] strPort;

	Leave();

	return stringRepresentation;
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetAddress::TestClass()
{
	bool problem = false;
	cout << "Testing NetAddress class...\n";

	cout << "Making address object\n";
	char * IP = "192.168.1.4";
	unsigned short port = 6423;
	NetAddress addr(IP,port);

	if(strcmp(IP,addr.GetIP()) != 0)
	{
		cout << "Constructor or GetIP is bad\n";
		problem = true;
	}
	else
	{
		cout << "Constructor and GetIP are good\n";
	}

	if(addr.GetPort() != port)
	{
		cout << "Constructor or GetPort is bad\n";
		problem = true;
	}
	else
	{
		cout << "Constructor and GetPort are good\n";
	}

	if(addr.GetFamily() != AF_INET)
	{
		cout << "Constructor or GetFamily is bad\n";
		problem = true;
	}
	else
	{
		cout << "Constructor and GetFamily are good\n";
	}

	char * str = addr.ToString();
	if(strcmp(str,"IP = 192.168.1.4, port = 6423") != 0)
	{
		cout << "ToString is bad\n";
		problem = true;
	}
	else
	{
		cout << "ToString is good\n";
	}
	delete[] str;

	NetAddress different("1.1.1.1",2525);
	NetAddress same;
	same = addr;

	if(addr != same)
	{
		cout << "!= or assignment operator is bad\n";
		problem = true;
	}
	else
	{
		cout << "!= and assignment operator are good\n";
	}

	if(addr == different)
	{
		cout << "== or copy constructor is bad\n";
		problem = true;
	}
	else
	{
		cout << "== and copy constructor are good\n";
	}

	cout << "\n\n";
	return !problem;
}

