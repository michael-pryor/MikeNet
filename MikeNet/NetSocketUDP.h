#pragma once

/**
 * @brief	Socket used for UDP data transfer.
 * @remarks	Michael Pryor, 3/28/2010.
 *
 * This class provides functionality specific to the UDP protocol.\n\n
 *
 * This class is not thread safe. Send and RawSend are thread safe.
 *
 */
class NetSocketUDP: public NetSocket
{
	/**
	 * @brief Filled with address that the last received packet has come from.
	 */
	NetAddress recvAddr;

	/**
	 * @brief Describes how received data should be dealt with and how sent data should be modified.
	 *
	 * ConcurretObject protects only the pointer, in case it changes when LoadMode() is used.
	 * The contents of the dereferenced pointer is not protected and does not need to be.
	 */
	ConcurrentObject<NetModeUdp*> modeUDP;

public:
	NetSocketUDP(size_t bufferLength, const NetAddress & localAddr, bool reusable, NetModeUdp * udpMode, NetSocket::RecvFunc recvFunc = NULL);
	NetSocketUDP(size_t bufferLength, const NetAddress & localAddr, NetSocket::RecvFunc recvFunc = NULL);
	~NetSocketUDP();

private:
	void ValidateModeLoaded(size_t line, const char * file) const;
	void Copy(const NetSocketUDP & copyMe);
public:
	NetSocketUDP(const NetSocketUDP &);
	NetSocketUDP & operator= (const NetSocketUDP &);

	void ClearRecv();
	bool Recv();

	NetUtility::SendStatus Send(const Packet & packet, bool block, const NetAddress * sendToAddr, unsigned int timeout);
	NetUtility::SendStatus RawSend(const Packet & packet, bool block, const NetAddress * sendToAddr, unsigned int timeout);

	virtual void Close();
	void Reset(size_t clientID);

	const NetModeUdp * GetMode() const;

	size_t GetPacketFromStore(Packet * destination, size_t clientID, size_t operationID);

	bool IsModeLoaded() const;
	void LoadMode(NetModeUdp * mode);

	NetSocket::Protocol GetProtocol() const;

	const NetAddress & GetRecvAddress() const;

	void DealWithData(const WSABUF & buffer, size_t completionBytes, NetSocket::RecvFunc recvFunc, size_t clientID, size_t instanceID);

	void SetRecvMemoryLimit(size_t newLimit, size_t clientID);
	size_t GetRecvMemoryLimit(size_t clientID) const;
	size_t GetRecvMemorySize(size_t clientID) const;

	static bool TestClass();
};