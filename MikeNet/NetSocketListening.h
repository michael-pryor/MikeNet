#pragma once

/**
 * @brief	Listening socket used to listen for and accept new TCP connections.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * Accepts or rejects incoming TCP connection attempts, and
 * stores a template socket object to be used with setting up new clients.
 */
class NetSocketListening: public NetSocketSimple
{
	/**
	 * @brief A client socket template, used when setting up new clients.
	 *
	 * New clients should be loaded into a copy of this template.
	 */
	NetSocketTCP * clientSocketTemplate;

public:
	NetSocketListening(const NetAddress & localAddr, NetSocketTCP * socketTCP);
	virtual ~NetSocketListening();

	const NetSocketTCP * GetSocket() const;
	NetSocketTCP * GetCopySocket() const;

	SOCKET AcceptConnection(size_t testValue, NetAddress * addr);
	
	static bool TestClass();
	static bool HelperTestClass(NetSocketListening & listeningSocket, NetSocketTCP & listeningSocketClient, NetSocketTCP & client);
};