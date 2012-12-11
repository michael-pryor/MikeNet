#pragma once
#include "UpnpNatPortMap.h"

/**
 * @brief	Port map object managing an individual port mapping on the UPnP NAT.
 * @remarks	Michael Pryor, 9/16/2010. 
 */
class UpnpNatPortMapAction : public UpnpNatPortMap
{
	IStaticPortMapping * ptrPortMap;

	Packet externalIP;
	long externalPort;
	long internalPort;
	Packet protocol;
	Packet internalIP;
	bool enabled;
	Packet description;
public:
	UpnpNatPortMapAction(IStaticPortMapping * portMap);
	~UpnpNatPortMapAction();

	void SetEnabled(bool option, size_t portMapID=0);
	void SetDescription(const Packet & description, size_t portMapID=0);
	void SetInternalPort(long internalPort, size_t portMapID=0);
	void SetInternalIP(const Packet & internalIP, size_t portMapID=0);
	
	const Packet & GetExternalIP(size_t portMapID=0) const;
	long GetExternalPort(size_t portMapID=0) const;
	long GetInternalPort(size_t portMapID=0) const;
	const Packet & GetProtocol(size_t portMapID=0) const;
	const Packet & GetInternalIP(size_t portMapID=0) const;
	bool GetEnabled(size_t portMapID=0) const;
	const Packet & GetDescription(size_t portMapID=0) const;
};