#pragma once
#include "netinstancetcp.h"

/**
 * @brief	Implements NetInstanceTCP with a standard setup for using a TCP socket (NetSocketTCP).
 * @remarks	Michael Pryor, 8/19/2010. 
 */
class NetInstanceImplementedTCP :
	public NetInstanceTCP
{
protected:
	/** @brief Socket used to communicate via TCP. */
	NetSocketTCP * socketTCP;

public:
	NetInstanceImplementedTCP(NetSocketTCP * socketTCP, bool handshakeEnabled);
	virtual ~NetInstanceImplementedTCP();

	virtual bool IsGracefulDisconnectEnabled() const;

	virtual size_t GetMaxPacketSizeTCP(size_t clientID=0) const;
	virtual size_t GetRecvBufferLengthTCP(size_t clientID=0) const;
	virtual size_t GetPartialPacketCurrentSizeTCP(size_t clientID=0) const;
	virtual double GetPartialPacketPercentageTCP(size_t clientID=0) const;
	
	virtual void SetMaxPacketSizeTCP(size_t newMaxSize=0, size_t clientID=0);

	virtual bool GetAutoResizeTCP(size_t clientID=0) const;
	virtual void SetAutoResizeTCP(bool newAutoResizeTCP=0, size_t clientID=0);

	virtual NetMode::ProtocolMode GetProtocolModeTCP() const;

	virtual bool GetNagleEnabledTCP() const;
	virtual const Packet & GetPostfixTCP() const;
	virtual const NetAddress & GetLocalAddressTCP() const;
	virtual const NetAddress & GetConnectAddressTCP(size_t clientID=0) const;

	virtual void FlushRecvTCP(size_t clientID=0);
	virtual size_t GetPacketAmountTCP(size_t clientID=0) const;

	virtual void ShutdownTCP(size_t clientID=0);

	virtual size_t GetPacketFromStoreTCP(Packet * destination=0, size_t clientID=0);
	virtual NetUtility::SendStatus SendTCP(const Packet & packet, bool block=0, size_t clientID=0);

	virtual NetUtility::ConnectionStatus GetConnectionStateTCP(size_t clientID=0) const;

	NetSocket::RecvFunc GetUserFunctionTCP() const;
	bool IsUserFunctionLoadedTCP() const;

	virtual void CloseSockets();

	void SetSendMemoryLimitTCP(size_t newLimit, size_t clientID=0);
	void SetRecvMemoryLimitTCP(size_t newLimit, size_t clientID=0);
	size_t GetSendMemoryLimitTCP(size_t clientID=0) const;
	size_t GetRecvMemoryLimitTCP(size_t clientID=0) const;
	size_t GetSendMemorySizeTCP(size_t clientID=0) const;
	size_t GetRecvMemorySizeTCP(size_t clientID=0) const;
};

