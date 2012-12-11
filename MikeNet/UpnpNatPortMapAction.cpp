#include "FullInclude.h"


/**
 * @brief	Constructor. 
 *
 * Manages a port map and gathers information about it.
 *
 * @param [in]	paraPortMap	Port map to manage.
 */
UpnpNatPortMapAction::UpnpNatPortMapAction(IStaticPortMapping * paraPortMap)
{
	ptrPortMap = paraPortMap;

	ComString comStrResult;
	long lResult;
	VARIANT_BOOL bResult;
	HRESULT hResult;

	hResult = ptrPortMap->get_ExternalIPAddress(comStrResult.GetPtrBSTR());
	if( (hResult != S_FALSE) && (comStrResult.IsLoaded()) )
	{
		_ErrorException((hResult != S_OK), "getting the external IP of a new port map", hResult, __LINE__,__FILE__);
		externalIP = comStrResult;
	}
	comStrResult.Cleanup();

	hResult = ptrPortMap->get_ExternalPort(&lResult);
	if(hResult != S_FALSE)
	{
		_ErrorException((hResult != S_OK), "getting the external port of a new port map", hResult, __LINE__,__FILE__);
		externalPort = lResult;
	}

	hResult = ptrPortMap->get_InternalPort(&lResult);
	if(hResult != S_FALSE)
	{
		_ErrorException((hResult != S_OK), "getting the internal port of a new port map", hResult, __LINE__,__FILE__);
		internalPort = lResult;
	}
	
	hResult = ptrPortMap->get_Protocol(comStrResult.GetPtrBSTR());
	if( (hResult != S_FALSE) && (comStrResult.IsLoaded()) )
	{
		_ErrorException((hResult != S_OK), "getting the protocol of a new port map", hResult, __LINE__,__FILE__);
		protocol = comStrResult;
	}
	comStrResult.Cleanup();

	hResult = ptrPortMap->get_InternalClient(comStrResult.GetPtrBSTR());
	if( (hResult != S_FALSE) && (comStrResult.IsLoaded()) )
	{
		_ErrorException((hResult != S_OK), "getting the internal IP of a new port map", hResult, __LINE__,__FILE__);
		internalIP = comStrResult;
	}
	comStrResult.Cleanup();

	hResult = ptrPortMap->get_Enabled(&bResult);
	_ErrorException((hResult != S_OK), "getting the enabled/disabled status of a new port map", hResult, __LINE__,__FILE__);
	enabled = ComUtility::ConvertBoolean(bResult);

	hResult = ptrPortMap->get_Description(comStrResult.GetPtrBSTR());
	if( (hResult != S_FALSE) && (comStrResult.IsLoaded()) )
	{
		_ErrorException((hResult != S_OK), "getting the description of a new port map", hResult, __LINE__,__FILE__);
		description = comStrResult;
	}
	comStrResult.Cleanup();
}

/**
 * @brief	Destructor. 
 */
UpnpNatPortMapAction::~UpnpNatPortMapAction()
{
	if(ptrPortMap != NULL)
	{
		ptrPortMap->Release();
	}
}


/**
 * @brief	Enables or disables the port map.
 * @param	option True to enable the port map, false to disable the port map.
 * @param	portMapID Ignored.
 */
void UpnpNatPortMapAction::SetEnabled(bool option, size_t portMapID)
{
	HRESULT hResult = ptrPortMap->Enable(ComUtility::ConvertBoolean(option));
	_ErrorException((hResult != S_OK), "enabling or disabling a port map", hResult, __LINE__,__FILE__);

	enabled = option;
}

/**
 * @brief	Changes the description of the port map.
 *
 * @param	description New Description. Must be NULL terminated.
 * @param	portMapID Ignored.
 */
void UpnpNatPortMapAction::SetDescription(const Packet & description, size_t portMapID)
{
	ComString comDescription(description.GetNullTerminated());

	HRESULT hResult = ptrPortMap->EditDescription(comDescription.GetBSTR());
	_ErrorException((hResult != S_OK), "setting the description of a port map", hResult, __LINE__,__FILE__);

	this->description = description;
}

/**
 * @brief	Changes the internal port of the port map.
 *
 * @param	internalPort	New internal port.
 * @param	portMapID Ignored.
 */
void UpnpNatPortMapAction::SetInternalPort(long internalPort, size_t portMapID)
{
	HRESULT hResult = ptrPortMap->EditInternalPort(internalPort);
	_ErrorException((hResult != S_OK), "setting the internal port of a port map", hResult, __LINE__,__FILE__);

	this->internalPort = internalPort;
}

/**
 * @brief	Changes the internal IP of the port map.
 *
 * @param	internalIP	New internal IP.
 * @param	portMapID Ignored.
 */
void UpnpNatPortMapAction::SetInternalIP(const Packet & internalIP, size_t portMapID)
{
	ComString comInternalIP(internalIP.GetNullTerminated());

	HRESULT hResult = ptrPortMap->EditInternalClient(comInternalIP.GetBSTR());
	_ErrorException((hResult != S_OK), "setting the internal IP of a port map", hResult, __LINE__,__FILE__);

	this->internalIP = internalIP;
}

/**
 * @brief Retrieves the external IP of the port map.
 *
 * @param	portMapID Ignored.
 * @return  external IP.
 */
const Packet & UpnpNatPortMapAction::GetExternalIP( size_t portMapID ) const
{
	return externalIP;
}

/**
 * @brief Retrieves the external port of the port map.
 *
 * @param	portMapID Ignored.
 * @return  external port.
 */
long UpnpNatPortMapAction::GetExternalPort( size_t portMapID ) const
{
	return externalPort;
}

/**
 * @brief Retrieves the internal port of the port map.
 *
 * @param	portMapID Ignored.
 * @return  internal port.
 */
long UpnpNatPortMapAction::GetInternalPort( size_t portMapID ) const
{
	return internalPort;
}

/**
 * @brief Retrieves the protocol of the port map.
 *
 * @param	portMapID Ignored.
 * @return  protocol.
 */
const Packet & UpnpNatPortMapAction::GetProtocol( size_t portMapID ) const
{
	return protocol;
}

/**
 * @brief Retrieves the internal IP of the port map.
 *
 * @param	portMapID Ignored.
 * @return  internal IP.
 */
const Packet & UpnpNatPortMapAction::GetInternalIP( size_t portMapID ) const
{
	return internalIP;
}

/**
 * @brief Determines whether the port map is enabled or disabled.
 *
 * @param	portMapID Ignored.
 * @return  true if the port map is enabled, false if it is disabled.
 */
bool UpnpNatPortMapAction::GetEnabled( size_t portMapID ) const
{
	return enabled;
}

/**
 * @brief Retrieves a description of the port map.
 *
 * @param	portMapID Ignored.
 * @return  a description.
 */
const Packet & UpnpNatPortMapAction::GetDescription( size_t portMapID ) const
{
	return description;
}