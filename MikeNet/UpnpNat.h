#pragma once

/**
 * @brief	Interface to coordinate communication between UpnpNatAction and UpnpNatCommunication.
 * @remarks	Michael Pryor, 9/16/2010. 
 */
class UpnpNat
{
public:
	virtual ~UpnpNat(){}

	virtual void UpdateInfo()=0;
	virtual void AddPortMap(long externalPort, const char * protocol, long internalPort, const char * internalIP, bool enabled, const char * description)=0;

	virtual void DeletePortMap(size_t portMapID)=0;
	virtual void DeletePortMap(const char * protocol, long externalPort)=0;

	virtual size_t GetPortMapAmount() const=0;

	virtual bool FindPortMap(const char * protocol, long externalPort, size_t * position = NULL)=0;
};