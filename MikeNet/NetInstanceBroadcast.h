#pragma once

/**
 * @brief	Broadcast instance designed to communicate with entities on a common local area network.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * This instance is capable of communicating with entities on a common local area network without first forming
 * a direct connection. Messages sent to the router are broadcasted to all other entities connected to
 * that router using this instance. This is useful for e.g. Detecting a server on a network, and then later making a direct connection to that server.
 * Due to the simplicity of this instance, it is possible to use it with applications not based on this module.
 */
class NetInstanceBroadcast : public NetInstanceUDP
{
	/**
	 * @brief %Counter that keeps track of UDP receive failures.
	 *
	 * WSARecv can sometimes fail for reasons that should not result in
	 * catastrophic failure of the instance e.g. A WSASendTo operation
	 * to a remote address failed. For this reason we retry receive
	 * operations a number of times, when to give up is managed by
	 * this object.
	 */
	Counter recvFailCounterUDP;

	void Initialize(const NetAddress * p_broadcastAddress, bool recvEnabled, size_t recvMemoryLimit = NetInstanceProfile::DEFAULT_RECV_MEMORY_LIMIT, size_t sendMemoryLimit = NetInstanceProfile::DEFAULT_SEND_MEMORY_LIMIT);
protected:
	size_t GetRecvSizeMinUDP() const;

public:	
	/** @brief Minimum UDP buffer size necessary to maintain normal operations. */
	const static size_t recvSizeMinUDP = 0;

	NetInstanceBroadcast(const NetAddress * broadcastAddress, bool recvEnabled, NetSocketUDP * socketUDP, unsigned int sendTimeout = INFINITE, size_t instanceID = 0);
	NetInstanceBroadcast(const NetAddress * broadcastAddress, bool recvEnabled, const NetInstanceProfile & profile, size_t instanceID = 0);
	~NetInstanceBroadcast();

	void ErrorOccurred(size_t clientID=0);

	void DoRecv(NetSocket * socket,size_t clientID=0);
	void CompletionError(NetSocket * completionSocket, size_t clientID=0);

	NetUtility::SendStatus SendUDP(const Packet & packet, bool block, size_t clientID=0);
	NetUtility::SendStatus SendToUDP(const NetAddress & address, const Packet & packet, bool block);

	void DealCompletion(NetSocket * socket, DWORD bytes, size_t clientID=0);

	virtual void CloseSockets();
	
	static bool TestClass();
};