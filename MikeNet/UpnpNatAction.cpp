#include "FullInclude.h"

/**
 * @brief	Default constructor. 
 */
UpnpNatAction::UpnpNatAction()
{
	nat = NULL;
	portMapCollection = NULL;

	HRESULT result = CoCreateInstance(CLSID_UPnPNAT,NULL,CLSCTX_INPROC_SERVER,IID_IUPnPNAT,(void**) &nat);
	_ErrorException((result != S_OK),"creating a NAT instance (!= S_OK)",result, __LINE__,__FILE__);
	_ErrorException((nat == NULL),"creating a NAT instance (NULL)",NULL, __LINE__,__FILE__);
}

/**
 * @brief	Cleans up currently loaded port map information.
 */
void UpnpNatAction::CleanPortMaps()
{
	portMaps.Clear();
	if(portMapCollection != NULL)
	{
		portMapCollection->Release();
	}
}

/**
 * @brief	Destructor. 
 */
UpnpNatAction::~UpnpNatAction()
{
	const char * cCommand = "an internal function (~UpnpNatAction)";
	try
	{
		CleanPortMaps();

		if(nat != NULL)
		{
			nat->Release();
		}
	}
	MSG_CATCH
}

/**
 * @brief	Updates information about port maps on router.
 */
void UpnpNatAction::UpdateInfo()
{
	HRESULT hResult;

	// Cleanup old port map in case this is an update
	CleanPortMaps();

	// Retrieve current port mappings
	hResult = nat->get_StaticPortMappingCollection(&portMapCollection);
	_ErrorException((hResult != S_OK), "retrieving current port mappings (!= S_OK)", hResult, __LINE__, __FILE__);
	_ErrorException((portMapCollection == NULL), "retrieving current port mappings (NULL)", NULL, __LINE__, __FILE__);

	// Scan through list and load port maps into vector
	// Code is based on MSDN sample
	IUnknown * ptrUnk = NULL;
	hResult = portMapCollection->get__NewEnum(&ptrUnk);
	if (SUCCEEDED(hResult))
	{
		IEnumVARIANT * ptrEnumVar = NULL;
		hResult = ptrUnk->QueryInterface(IID_IEnumVARIANT, (void **) &ptrEnumVar);
		if (SUCCEEDED(hResult))
		{
			VARIANT varCurDevice;
			VariantInit(&varCurDevice);
			ptrEnumVar->Reset();
			// Loop through each port map in the collection
			while (S_OK == ptrEnumVar->Next(1, &varCurDevice, NULL))
			{
				IStaticPortMapping * ptrPortMap = NULL;
				IDispatch * pdispDevice = V_DISPATCH(&varCurDevice);
				if (SUCCEEDED(pdispDevice->QueryInterface(IID_IStaticPortMapping, (void **) &ptrPortMap)))
				{
					// Add port map to vector
					UpnpNatPortMapAction * newPortMap = new (nothrow)UpnpNatPortMapAction(ptrPortMap);
					Utility::DynamicAllocCheck(newPortMap,__LINE__,__FILE__);
					portMaps.Add(newPortMap);						
				}
				VariantClear(&varCurDevice);
			}
			ptrEnumVar->Release();
		}
		ptrUnk->Release();
	}
}

/**
 * @brief	Adds a port map to the port forwarding list.
 *
 * @param	externalPort	The external port.
 * @param	protocol		The protocol. 
 * @param	internalPort	The internal port. 
 * @param	internalIP		The internal ip. 
 * @param	enabled			True if the entry should be enabled, false if not. 
 * @param	description		A description of the port map.
 */
void UpnpNatAction::AddPortMap(long externalPort, const char * protocol, long internalPort, const char * internalIP, bool enabled, const char * description)
{
	ValidatePortMapCollection();
	IStaticPortMapping * ptrNewPortMap = NULL;

	// Convert parameters for use with COM
	ComString strProtocol(protocol);
	ComString strInternalIP(internalIP);
	ComString strDescription(description);
	VARIANT_BOOL vbEnabled = ComUtility::ConvertBoolean(enabled);

	// Add to collection
	HRESULT hResult = portMapCollection->Add(externalPort,strProtocol.GetBSTR(),internalPort,strInternalIP.GetBSTR(),vbEnabled,strDescription.GetBSTR(),&ptrNewPortMap);
	_ErrorException((hResult != S_OK), "adding a port map to the collection (!= S_OK)", hResult, __LINE__, __FILE__);
	_ErrorException((ptrNewPortMap == NULL), "adding a port map to the collection (NULL)", NULL, __LINE__, __FILE__);

	// Add to internal store
	UpnpNatPortMapAction * newPortMap = new (nothrow) UpnpNatPortMapAction(ptrNewPortMap);
	Utility::DynamicAllocCheck(newPortMap,__LINE__,__FILE__);
	portMaps.Add(newPortMap);
}

/**
 * @brief	Deletes a port map.
 *
 * @param	portMapID	Identifier for the port map. 
 */
void UpnpNatAction::DeletePortMap(size_t portMapID)
{
	ValidatePortMapCollection();

	// Remove from collection
	ComString strProtocol(portMaps[portMapID].GetProtocol().GetNullTerminated());

	HRESULT hResult = portMapCollection->Remove(portMaps[portMapID].GetExternalPort(),strProtocol.GetBSTR());
	_ErrorException((hResult != S_OK), "removing a port map from the collection", hResult, __LINE__, __FILE__);

	// Remove from internal store
	portMaps.Erase(portMapID);
}

/**
 * @brief	Deletes a port map. 
 *
 * @param   protocol		The protocol of the port map to delete.
 * @param	externalPort	The external port of the port map to delete. 
 */
void UpnpNatAction::DeletePortMap(const char * protocol, long externalPort)
{
	ValidatePortMapCollection();

	// Remote from collection
	ComString strProtocol(protocol);
	
	HRESULT hResult = portMapCollection->Remove(externalPort,strProtocol.GetBSTR());
	_ErrorException((hResult != S_OK), "removing a port map from the collection", hResult, __LINE__, __FILE__);

	// Remove from internal store
	size_t portMapID = 0;
	bool found = false;
	for(size_t n = 0;n<GetPortMapAmount();n++)
	{
		if( (portMaps[n].GetProtocol() == protocol) && (portMaps[n].GetExternalPort() == externalPort) )
		{
			found = true;
			portMapID = n;
			break;
		}
	}

	if(found == true)
	{
		portMaps.Erase(portMapID);
	}
}

/**
 * @brief	Ensures that UpnpNatAction::portMapCollection is not NULL and can be used.
 */
void UpnpNatAction::ValidatePortMapCollection()
{
	if(portMapCollection == NULL)
	{
		UpdateInfo();
	}
}

/**
 * @brief	Retrieves the specified port map. 
 *
 * @param	portMapID	Identifier for the port map. 
 *
 * @return	the port map. 
 */
UpnpNatPortMapAction & UpnpNatAction::GetPortMap(size_t portMapID)
{
	_ErrorException((portMapID >= portMaps.Size()),"retrieving a port map, invalid port map ID",0,__LINE__,__FILE__);
	return portMaps[portMapID];
}

/**
 * @brief	Retrieves the specified port map. 
 *
 * @param	portMapID	Identifier for the port map. 
 *
 * @return	the port map. 
 */
const UpnpNatPortMapAction & UpnpNatAction::GetPortMap(size_t portMapID) const
{
	_ErrorException((portMapID >= portMaps.Size()),"retrieving a port map, invalid port map ID",0,__LINE__,__FILE__);
	return portMaps[portMapID];
}

/**
 * @brief	Retrieves the number of port map loaded.
 */
size_t UpnpNatAction::GetPortMapAmount() const
{
	return portMaps.Size();
}

/**
 * @brief	Searches for a port map with the specified protocol and external port.
 *
 * @param	protocol			The protocol. 
 * @param	externalPort		The external port. 
 * @param [out]	position		Location to store ID of port map. If NULL then ignored.
 *
 * @return	true if a port mapping was found, false if not.
 */
bool UpnpNatAction::FindPortMap( const char * protocol, long externalPort, size_t * position )
{
	bool found = false;

	for(size_t n = 0;n<portMaps.Size();n++)
	{
		if(portMaps[n].GetProtocol() == protocol && portMaps[n].GetExternalPort() == externalPort)
		{
			found = true;
			if(position != NULL)
			{
				*position = n;
			}
			break;
		}
	}

	return found;
}


