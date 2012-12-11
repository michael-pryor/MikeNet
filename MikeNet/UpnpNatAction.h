#pragma once
#include "UpnpNat.h"

/**
 * @brief	Manages UPnP NAT controls allowing port mappings to be added, read and removed.
 * @remarks	Michael Pryor, 9/16/2010. 
 */
class UpnpNatAction : public UpnpNat
{
	/**
	 * @brief Interacts with NAT settings on a router.
	 */
	IUPnPNAT * nat;

	/**
	 * @brief Interacts with individual port mappings on a router.
	 */
	IStaticPortMappingCollection * portMapCollection;

	/**
	 * @brief Stores information retrieved from UpnpNatAction::portMapCollection.
	 */
	StoreVector<UpnpNatPortMapAction> portMaps;

	void CleanPortMaps();
	void ValidatePortMapCollection();
public:
	UpnpNatAction();
	~UpnpNatAction();

	void UpdateInfo();
	void AddPortMap(long externalPort, const char * protocol, long internalPort, const char * internalIP, bool enabled, const char * description);

	void DeletePortMap(size_t portMapID);
	void DeletePortMap(const char * protocol, long externalPort);

	size_t GetPortMapAmount() const;

	UpnpNatPortMapAction & GetPortMap(size_t portMapID);
	const UpnpNatPortMapAction & GetPortMap(size_t portMapID) const;

	bool FindPortMap(const char * protocol, long externalPort, size_t * position = NULL);	
};