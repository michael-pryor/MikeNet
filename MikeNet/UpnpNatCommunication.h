#pragma once
#include "UpnpNatPortMap.h"
#include "UpnpNat.h"
#include "ThreadMessageItem.h"
#include "ThreadSingleMessageKeepLast.h"

/**
 * @brief	Communicates with UPnP NAT thread, controlling its actions.
 * @remarks	Michael Pryor, 9/16/2010. 
 */
class UpnpNatCommunication :
	public UpnpNat, public UpnpNatPortMap
{
	/**
	 * @brief Thread is responsible for all NAT actions.
	 */
	ThreadSingleMessageKeepLast actionThread;

	/**
	 * @brief Stores and controls UPnP NAT.
	 *
	 * UpnpNatCommunication::actionThread makes changes to NAT whilst
	 * this object can manually read from the object but not write to it.
	 * This object instructs the thread to make any changes necassary.
	 */
	UpnpNatAction * natControls;

public:
	UpnpNatCommunication();
	~UpnpNatCommunication();

	void WaitUntilLastActionFinished() const;
	bool IsLastActionFinished() const;
	const ThreadMessageItem * ExtractLastMessage();

	const ErrorReport * GetError() const;
	bool IsErrorStored() const;
	void ClearError();

	void UpdateInfo();
	void AddPortMap(long externalPort, const char * protocol, long internalPort, const char * internalIP, bool enabled, const char * description);

	void DeletePortMap(size_t portMapID);
	void DeletePortMap(const char * protocol, long externalPort);

	size_t GetPortMapAmount() const;

	void SetEnabled(bool option, size_t portMapID);
	void SetDescription(const Packet & description, size_t portMapID);
	void SetInternalPort(long internalPort, size_t portMapID);
	void SetInternalIP(const Packet & internalIP, size_t portMapID);
	
	const Packet & GetExternalIP(size_t portMapID) const;
	long GetExternalPort(size_t portMapID) const;
	long GetInternalPort(size_t portMapID) const;
	const Packet & GetProtocol(size_t portMapID) const;
	const Packet & GetInternalIP(size_t portMapID) const;
	bool GetEnabled(size_t portMapID) const;
	const Packet & GetDescription(size_t portMapID) const;

	bool FindPortMap(const char * protocol, long externalPort, size_t * position = NULL);


	static bool TestClass();
};

