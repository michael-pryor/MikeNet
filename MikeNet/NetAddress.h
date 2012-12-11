#pragma once
#include "CriticalSection.h"
/**
 * @brief	Stores a single address with IP and port.
 * @remarks	Michael Pryor, 6/28/2010. 
 *
 * Interacts with sockaddr_in winsock structure making
 * use of IP and port easier.\n\n
 *
 * This class is thread safe.
 */
class NetAddress : public CriticalSection
{
	/** @brief Stores address information. */
	sockaddr_in addr;

	void DefaultVariables();
public:
	/**
	 * @brief Empty address set to default, used for comparisons.
	 */
	const static NetAddress EmptyAddress;

	NetAddress();
	
	NetAddress(const char * ip, unsigned short port);
	NetAddress(const NetAddress & copyMe);
	NetAddress(sockaddr_in copyMe);
	NetAddress & operator=(const NetAddress & copyMe);
	bool operator==(const NetAddress & testMe) const;
	bool operator!=(const NetAddress & testMe) const;

	void Clear();
	sockaddr_in GetSockAddr() const;

	sockaddr_in * GetAddrPtr();
	const sockaddr_in * GetAddrPtr() const;

	ADDRESS_FAMILY GetFamily() const;
	const char * GetIP() const;
	unsigned long int GetByteRepresentationIP() const;
	unsigned short GetPort() const;

	void SetFamily(ADDRESS_FAMILY family = AF_INET);
	void SetPort(unsigned short port);
	void SetIP(const char * IP);

	NetAddress & Load(const NetAddress & copyMe);
	NetAddress & Load(sockaddr_in copyMe);
	NetAddress & Load(const char * ip, unsigned short port);

	char * ToString() const;

	static bool TestClass();

};