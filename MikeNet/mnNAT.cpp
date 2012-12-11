#include "FullInclude.h"
#include "mnDBPWrapper.h"

/**
 * @brief	Waits for the last action to complete if 'block' is true.
 *
 * @param	block	True to wait for the last action to complete, false
 * to return instantly.
 */
void mnNAT::_Block( bool block )
{
	if(block == true)
	{
		UpnpNatUtility::GetControl()->WaitUntilLastActionFinished();
		_DealWithError();
	}
}

/**
 * @brief	If an error occurred while performing an operation, this method will throw it.
 */
void mnNAT::_DealWithError()
{
	if(UpnpNatUtility::GetControl()->IsErrorStored() == true)
	{
		throw *UpnpNatUtility::GetControl()->GetError();
	}
}

/**
 * @brief	Initializes UPnP NAT module.
 *
 * No NAT commands will work until this has been used. \n\n
 *
 * If NAT is already running, it will be unloaded and reinitialized.
 *
 * @param block	False if the command should return instantly, true if the command should
 * wait for the operation to complete.
 *
 * @return 0 if the operation completed successfully.
 * @return -1 if the operation failed to complete.
 */
DBP_CPP_DLL int mnNAT::StartNAT( bool block )
{
	const char * cCommand = "mnNAT::StartNAT";
	int returnMe = 0;

	try
	{
		UpnpNatUtility::Start();
		UpnpNatUtility::GetControl()->UpdateInfo();
		_Block(block);
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Uninitializes NAT.
 *
 * Does nothing is NAT is not initialized.
 *
 * @return 0 if the operation completed successfully.
 * @return -1 if the operation failed to complete.
 */
DBP_CPP_DLL int mnNAT::FinishNAT()
{
	const char * cCommand = "mnNAT::FinishNAT";
	int returnMe = 0;
	try
	{
		UpnpNatUtility::Finish();
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Updates information stored about port mappings.
 *
 * @param block	False if the command should return instantly, true if the command should
 * wait for the operation to complete.
 *
 * @return 0 if the operation completed successfully.
 * @return -1 if the operation failed to complete.
 */
DBP_CPP_DLL int mnNAT::UpdateNAT( bool block )
{
	const char * cCommand = "mnNAT::UpdateNAT";
	int returnMe = 0;
	try
	{
		UpnpNatUtility::GetControl()->UpdateInfo();
		_Block(block);
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief Creates a port mapping.
 *
 * @param	externalPort	The external port.
 * @param	protocol		The protocol. 
 * @param	internalPort	The internal port. 
 * @param	internalIP		The internal ip. 
 * @param	enabled			True if the entry should be enabled, false if not. 
 * @param	description		A description of the port map.
 * @param block	False if the command should return instantly, true if the command should
 * wait for the operation to complete.
 *
 * @return 0 if the operation completed successfully.
 * @return -1 if the operation failed to complete.
 */
DBP_CPP_DLL int mnNAT::AddPortMap( const char * protocol, long externalPort, long internalPort, const char * internalIP, bool enabled, const char * description, bool block )
{
	const char * cCommand = "mnNAT::AddPortMap";
	int returnMe = 0;
	try
	{
		
		UpnpNatUtility::GetControl()->AddPortMap(externalPort,protocol,internalPort,internalIP,enabled,description);
		_Block(block);
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Retrieves the number of port mappings.
 *
 * @return the number of port mappings loaded.
 */
DBP_CPP_DLL size_t mnNAT::GetPortMapAmount()
{
	const char * cCommand = "mnNAT::GetPortMapAmount";
	size_t returnMe = 0;
	try
	{
		
		returnMe = UpnpNatUtility::GetControl()->GetPortMapAmount();
	}
	STD_CATCH

	return returnMe;
}

/**
 * @brief	Enables a port mapping.
 *
 * @param portMapID ID of the port map to use.
 * @param block	False if the command should return instantly, true if the command should
 * wait for the operation to complete.
 *
 * @return 0 if the operation completed successfully.
 * @return -1 if the operation failed to complete.
 */
DBP_CPP_DLL int mnNAT::EnablePortMap( size_t portMapID, bool block )
{
	const char * cCommand = "mnNAT::EnablePortMap";
	int returnMe = 0;
	try
	{
		
		UpnpNatUtility::GetControl()->SetEnabled(true,portMapID);
		_Block(block);
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Disables a port mapping.
 *
 * @param portMapID ID of the port map to use.
 * @param block	False if the command should return instantly, true if the command should
 * wait for the operation to complete.
 *
 * @return 0 if the operation completed successfully.
 * @return -1 if the operation failed to complete.
 */
DBP_CPP_DLL int mnNAT::DisablePortMap( size_t portMapID, bool block )
{
	const char * cCommand = "mnNAT::DisablePortMap";
	int returnMe = 0;
	try
	{
		
		UpnpNatUtility::GetControl()->SetEnabled(false,portMapID);
		_Block(block);
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Changes the internal IP of a port mapping.
 *
 * @param portMapID ID of the port map to use.
 * @param internalIP The internal IP.
 * @param block	False if the command should return instantly, true if the command should
 * wait for the operation to complete.
 *
 * @return 0 if the operation completed successfully.
 * @return -1 if the operation failed to complete.
 */
DBP_CPP_DLL int mnNAT::SetPortMapInternalIP( size_t portMapID, const char * internalIP, bool block )
{
	const char * cCommand = "mnNAT::SetPortMapInternalIP";
	int returnMe = 0;
	try
	{
		UpnpNatUtility::GetControl()->SetInternalIP(internalIP,block);
		_Block(block);
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Changes the internal port of a port mapping.
 *
 * @param portMapID ID of the port map to use.
 * @param internalPort The internal port.
 * @param block	False if the command should return instantly, true if the command should
 * wait for the operation to complete.
 *
 * @return 0 if the operation completed successfully.
 * @return -1 if the operation failed to complete.
 */
DBP_CPP_DLL int mnNAT::SetPortMapInternalPort( size_t portMapID, long internalPort, bool block )
{
	const char * cCommand = "mnNAT::SetPortMapInternalPort";
	int returnMe = 0;
	try
	{
		
		UpnpNatUtility::GetControl()->SetInternalPort(internalPort,portMapID);
		_Block(block);
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Changes the description of a port mapping.
 *
 * @param portMapID ID of the port map to use.
 * @param description The description.
 * @param block	False if the command should return instantly, true if the command should
 * wait for the operation to complete.
 *
 * @return 0 if the operation completed successfully.
 * @return -1 if the operation failed to complete.
 */
DBP_CPP_DLL int mnNAT::SetPortMapDescription( size_t portMapID, const char * description, bool block )
{
	const char * cCommand = "mnNAT::SetPortMapDescription";
	int returnMe = 0;
	try
	{
		UpnpNatUtility::GetControl()->SetDescription(description,portMapID);
		_Block(block);
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Retrieves the external IP of a port mapping.
 *
 * @param portMapID ID of the port map to use.
 *
 * @return an empty string if an error occurred. 
 * @return the external IP of the port mapping.
 */
CPP_DLL const char * mnNAT::GetPortMapExternalIP( size_t portMapID )
{
	const char * cCommand = "mnNAT::GetPortMapExternalIP";
	const char * returnMe = "";
	try
	{
		
		returnMe = UpnpNatUtility::GetControl()->GetExternalIP(portMapID).GetNullTerminated();
	}
	STD_CATCH

	return returnMe;
}

/**
 * @brief	Retrieves the external port of a port mapping.
 *
 * @param portMapID ID of the port map to use.
 *
 * @return the external port of the port mapping.
 */
DBP_CPP_DLL long mnNAT::GetPortMapExternalPort( size_t portMapID )
{
	const char * cCommand = "mnNAT::GetPortMapExternalPort";
	long returnMe = 0;
	try
	{
		
		returnMe = UpnpNatUtility::GetControl()->GetExternalPort(portMapID);
	}
	STD_CATCH

	return returnMe;
}

/**
 * @brief	Retrieves the internal port of a port mapping.
 *
 * @param portMapID ID of the port map to use.
 *
 * @return the internal port of the port mapping.
 */
DBP_CPP_DLL long mnNAT::GetPortMapInternalPort( size_t portMapID )
{
	const char * cCommand = "mnNAT::GetPortMapInternalPort";
	long returnMe = 0;
	try
	{
		
		returnMe = UpnpNatUtility::GetControl()->GetInternalPort(portMapID);
	}
	STD_CATCH

	return returnMe;
}

/**
 * @brief	Retrieves the protocol of a port mapping.
 *
 * @param portMapID ID of the port map to use.
 *
 * @return the protocol of the port mapping.
 * @return an empty string if an error occurred.
 */
CPP_DLL const char * mnNAT::GetPortMapProtocol( size_t portMapID )
{
	const char * cCommand = "mnNAT::GetPortMapProtocol";
	const char * returnMe = "";
	try
	{
		
		returnMe = UpnpNatUtility::GetControl()->GetProtocol(portMapID).GetNullTerminated();
	}
	STD_CATCH

	return returnMe;
}

/**
 * @brief	Retrieves the internal IP of a port mapping.
 *
 * @param portMapID ID of the port map to use.
 *
 * @return the internal IP of the port mapping.
 * @return an empty string if an error occurred.
 */
CPP_DLL const char * mnNAT::GetPortMapInternalIP( size_t portMapID )
{
	const char * cCommand = "mnNAT::GetPortMapInternalIP";
	const char * returnMe = "";
	try
	{
		
		returnMe = UpnpNatUtility::GetControl()->GetInternalIP(portMapID).GetNullTerminated();
	}
	STD_CATCH

	return returnMe;
}

/**
 * @brief	Determines whether a port mapping is enabled.
 *
 * @param portMapID ID of the port map to use.
 *
 * @return true if the port mapping is enabled, false if not.
 */
DBP_CPP_DLL int mnNAT::GetPortMapEnabled( size_t portMapID )
{
	const char * cCommand = "mnNAT::GetPortMapEnabled";
	bool returnMe = false;
	try
	{
		
		returnMe = UpnpNatUtility::GetControl()->GetEnabled(portMapID);
	}
	STD_CATCH

	return returnMe;
}

/**
 * @brief	Retrieves the description of a port mapping.
 *
 * @param portMapID ID of the port map to use.
 *
 * @return  a description of the port map.
 * @return an empty string if an error occurred.
 */
CPP_DLL const char * mnNAT::GetPortMapDescription( size_t portMapID )
{
	const char * cCommand = "mnNAT::GetPortMapDescription";
	const char * returnMe = "";
	try
	{
		
		returnMe = UpnpNatUtility::GetControl()->GetDescription(portMapID).GetNullTerminated();
	}
	STD_CATCH

	return returnMe;
}

/**
 * @brief	Deletes a port mapping.
 *
 * @param portMapID ID of the port map to use.
 * @param block	False if the command should return instantly, true if the command should
 * wait for the operation to complete.
 *
 * @return 0 if the operation completed successfully.
 * @return -1 if the operation failed to complete.
 */
DBP_CPP_DLL int mnNAT::DeletePortMapA( size_t portMapID, bool block )
{
	const char * cCommand = "mnNAT::DeletePortMapA";
	int returnMe = 0;
	try
	{
		
		UpnpNatUtility::GetControl()->DeletePortMap(portMapID);
		_Block(block);
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Deletes a port mapping.
 *
 * @param protocol The protocol.
 * @param externalPort The external port. 
 * @param block	False if the command should return instantly, true if the command should
 * wait for the operation to complete.
 *
 * @return 0 if the operation completed successfully.
 * @return -1 if the operation failed to complete.
 */
DBP_CPP_DLL int mnNAT::DeletePortMapB( const char * protocol, long externalPort, bool block )
{
	const char * cCommand = "mnNAT::DeletePortMapB";
	int returnMe = 0;
	try
	{
		
		UpnpNatUtility::GetControl()->DeletePortMap(protocol,externalPort);
		_Block(block);
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Determines whether a port mapping exists. 
 *
 * @param	protocol		The protocol. 
 * @param	externalPort	The external port. 
 *
 * @return	0 if the operation completed successfully. 
 * @return	-1 if the operation failed to complete. 
 */
DBP_CPP_DLL int mnNAT::PortMapExist( const char * protocol, long externalPort )
{
	const char * cCommand = "mnNAT::PortMapExist";
	int returnMe = -1;

	try
	{
		
		returnMe = UpnpNatUtility::GetControl()->FindPortMap(protocol,externalPort,NULL);
	}
	STD_CATCH

	return returnMe;
}

/**
 * @brief	Finds the ID of a port mapping.
 * 
 * @param	protocol		The protocol. 
 * @param	externalPort	The external port. 
 *
 * @return	the ID of the found port map.
 */
DBP_CPP_DLL size_t mnNAT::FindPortMap( const char * protocol, long externalPort )
{
	const char * cCommand = "mnNAT::FindPortMap";
	size_t returnMe = 0;

	try
	{
		
		UpnpNatUtility::GetControl()->FindPortMap(protocol,externalPort,&returnMe);
	}
	STD_CATCH

	return returnMe;
}

/**
 * @brief	Does not return until the last 'doing' action has finished.
 *
 * You cannot use this on any method which does not have a 'block' parameter.
 *
 * @return	0 if the operation completed successfully. 
 * @return	-1 if the operation failed to complete. 
 */
DBP_CPP_DLL int mnNAT::WaitUntilLastActionFinished()
{
	const char * cCommand = "mnNAT::WaitUntilLastActionFinished";
	int returnMe = 0;
	try
	{
		UpnpNatUtility::GetControl()->WaitUntilLastActionFinished();
	}
	STD_CATCH_RM

	return returnMe;
}

/**
 * @brief	Determines whether the last 'doing' action has finished.
 *
 * You cannot use this on any method which does not have a 'block' parameter.
 *
 * @return	true if the last action that was initiated has finished, false if not.
 */
DBP_CPP_DLL int mnNAT::PollNAT()
{
	const char * cCommand = "mnNAT::WaitUntilLastActionFinished";
	int returnMe = -1;
	try
	{
		return UpnpNatUtility::GetControl()->IsLastActionFinished();
	}
	STD_CATCH

	return returnMe;	
}

#ifdef DBP
/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mnNAT::DBP_GetPortMapExternalIP(DWORD pOldString, size_t PortMapID)
{
	const char * cReturn = GetPortMapExternalIP(PortMapID);

	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mnNAT::DBP_GetPortMapProtocol(DWORD pOldString, size_t PortMapID)
{
	const char * cReturn = GetPortMapProtocol(PortMapID);

	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mnNAT::DBP_GetPortMapInternalIP(DWORD pOldString, size_t PortMapID)
{
	const char * cReturn = GetPortMapInternalIP(PortMapID);

	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

	return(dReturn);
}

/**
 * @brief	Command modified for use with Dark BASIC Pro.
 *
 * @param	pOldString	String to cleanup.
 *
 * @return	string.
 */
DBP_CPP_DLL DWORD mnNAT::DBP_GetPortMapDescription(DWORD pOldString, size_t PortMapID)
{
	const char * cReturn = GetPortMapDescription(PortMapID);

	// Ensure compatibility with DBP
	DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

	return(dReturn);
}
#endif