#pragma once

/**
 * @brief	Interface to coordinate communication between UpnpNatPortMapAction and UpnpNatPortMapCommunication.
 * @remarks	Michael Pryor, 9/16/2010. 
 */
class UpnpNatPortMap
{
public:
	virtual ~UpnpNatPortMap(){}

	virtual void SetEnabled(bool option, size_t portMapID)=0;
	virtual void SetDescription(const Packet & description, size_t portMapID)=0;
	virtual void SetInternalPort(long internalPort, size_t portMapID)=0;
	virtual void SetInternalIP(const Packet & internalIP, size_t portMapID)=0;
	
	virtual const Packet & GetExternalIP(size_t portMapID) const=0;
	virtual long GetExternalPort(size_t portMapID) const=0;
	virtual long GetInternalPort(size_t portMapID) const=0;
	virtual const Packet & GetProtocol(size_t portMapID) const=0;
	virtual const Packet & GetInternalIP(size_t portMapID) const=0;
	virtual bool GetEnabled(size_t portMapID) const=0;
	virtual const Packet & GetDescription(size_t portMapID) const=0;
};