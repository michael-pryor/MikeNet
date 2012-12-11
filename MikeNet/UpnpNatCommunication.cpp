#include "FullInclude.h"

/**
 * @brief	Default constructor. 
 */
UpnpNatCommunication::UpnpNatCommunication() :
	natControls(NULL),
	actionThread(&UpnpNatActionThread,&natControls,false)
{
	actionThread.WaitForThreadToBeReady();
}

/**
 * @brief	Destructor. 
 */
UpnpNatCommunication::~UpnpNatCommunication()
{
	const char * cCommand = "~UpnpNatCommunication";
	try
	{
		actionThread.TerminateFriendly(true);
		_ErrorException((natControls != NULL),"terminating UPnP NAT, thread did not destroy NAT object",0,__LINE__,__FILE__);
	}
	MSG_CATCH
}

/**
 * @brief	Adds a port map to the port forwarding list by sending an instruction to the controlling thread.
 *
 * @param	externalPort	The external port.
 * @param	protocol		The protocol. 
 * @param	internalPort	The internal port. 
 * @param	internalIP		The internal ip. 
 * @param	enabled			True if the entry should be enabled, false if not. 
 * @param	description		A description of the port map.
 */
void UpnpNatCommunication::AddPortMap( long externalPort, const char * protocol, long internalPort, const char * internalIP, bool enabled, const char * description )
{
	ThreadMessageItem * message = new (nothrow) ThreadMessageItemAddPortMap(natControls,externalPort,protocol,internalPort,internalIP,enabled,description);
	Utility::DynamicAllocCheck(message,__LINE__,__FILE__);
	actionThread.PostMessageItem(message);
}

/**
 * @brief	Deletes a port map.
 *
 * @param	portMapID	Identifier for the port map. 
 */
void UpnpNatCommunication::DeletePortMap( size_t portMapID )
{
	ThreadMessageItem * message = new (nothrow) ThreadMessageItemDeletePortMap(natControls,portMapID);
	Utility::DynamicAllocCheck(message,__LINE__,__FILE__);
	actionThread.PostMessageItem(message);
}

/**
 * @brief	Deletes a port map. 
 *
 * @param   protocol		The protocol of the port map to delete.
 * @param	externalPort	The external port of the port map to delete. 
 */
void UpnpNatCommunication::DeletePortMap( const char * protocol, long externalPort )
{
	ThreadMessageItem * message = new (nothrow) ThreadMessageItemDeletePortMap(natControls,protocol,externalPort);
	Utility::DynamicAllocCheck(message,__LINE__,__FILE__);
	actionThread.PostMessageItem(message);
}

/**
 * @brief	Retrieves the number of port map loaded.
 *
 * @return the number of port mappings loaded.
 */
size_t UpnpNatCommunication::GetPortMapAmount() const
{
	return natControls->GetPortMapAmount();
}

/**
 * @brief	Enables or disables the port map.
 * @param	option True to enable the port map, false to disable the port map.
 * @param	portMapID ID of the port map to use.
 */
void UpnpNatCommunication::SetEnabled( bool option, size_t portMapID )
{
	ThreadMessageItem * message = new (nothrow) ThreadMessageItemSetPortMapEnabled(&natControls->GetPortMap(portMapID),option);
	Utility::DynamicAllocCheck(message,__LINE__,__FILE__);
	actionThread.PostMessageItem(message);
}

/**
 * @brief	Changes the description of the port map.
 *
 * @param   description New description. Must be NULL terminated.
 * @param	portMapID ID of the port map to use.
 */
void UpnpNatCommunication::SetDescription( const Packet & description, size_t portMapID )
{
	ThreadMessageItem * message = new (nothrow) ThreadMessageItemSetPortMapDescription(&natControls->GetPortMap(portMapID),description);
	Utility::DynamicAllocCheck(message,__LINE__,__FILE__);
	actionThread.PostMessageItem(message);
}

/**
 * @brief	Changes the internal port of the port map.
 *
 * @param	internalPort	New internal port.
 * @param	portMapID ID of the port map to use.
 */
void UpnpNatCommunication::SetInternalPort( long internalPort, size_t portMapID )
{
	ThreadMessageItem * message = new (nothrow) ThreadMessageItemSetPortMapInternalPort(&natControls->GetPortMap(portMapID),internalPort);
	Utility::DynamicAllocCheck(message,__LINE__,__FILE__);
	actionThread.PostMessageItem(message);
}

/**
 * @brief	Changes the internal IP of the port map.
 *
 * @param	internalIP	New internal IP.
 * @param	portMapID ID of the port map to use.
 */
void UpnpNatCommunication::SetInternalIP( const Packet & internalIP, size_t portMapID )
{
	ThreadMessageItem * message = new (nothrow) ThreadMessageItemSetPortMapInternalIP(&natControls->GetPortMap(portMapID),internalIP);
	Utility::DynamicAllocCheck(message,__LINE__,__FILE__);
	actionThread.PostMessageItem(message);
}

/**
 * @brief	Updates NAT, reloading all information. 
 */
void UpnpNatCommunication::UpdateInfo()
{
	ThreadMessageItem * message = new (nothrow) ThreadMessageItemUpdateNat(natControls);
	Utility::DynamicAllocCheck(message,__LINE__,__FILE__);
	actionThread.PostMessageItem(message);
}


/**
 * @brief Retrieves the external IP of the port map.
 *
 * @param	portMapID ID of the port map to use.
 * @return  external IP.
 */
const Packet & UpnpNatCommunication::GetExternalIP( size_t portMapID ) const
{
	return natControls->GetPortMap(portMapID).GetExternalIP();
}

/**
 * @brief Retrieves the external port of the port map.
 *
 * @param	portMapID ID of the port map to use.
 * @return  external port.
 */
long UpnpNatCommunication::GetExternalPort( size_t portMapID ) const
{
	return natControls->GetPortMap(portMapID).GetExternalPort();
}

/**
 * @brief Retrieves the internal port of the port map.
 *
 * @param	portMapID ID of the port map to use.
 * @return  internal port.
 */
long UpnpNatCommunication::GetInternalPort( size_t portMapID ) const
{
	return natControls->GetPortMap(portMapID).GetInternalPort();
}

/**
 * @brief Retrieves the protocol of the port map.
 *
 * @param	portMapID ID of the port map to use.
 * @return  protocol.
 */
const Packet & UpnpNatCommunication::GetProtocol( size_t portMapID ) const
{
	return natControls->GetPortMap(portMapID).GetProtocol();
}

/**
 * @brief Retrieves the internal IP of the port map.
 *
 * @param	portMapID ID of the port map to use.
 * @return  internal IP.
 */
const Packet & UpnpNatCommunication::GetInternalIP( size_t portMapID ) const
{
	return natControls->GetPortMap(portMapID).GetInternalIP();
}

/**
 * @brief Determines whether the port map is enabled or disabled.
 *
 * @param	portMapID ID of the port map to use.
 * @return  true if the port map is enabled, false if it is disabled.
 */
bool UpnpNatCommunication::GetEnabled( size_t portMapID ) const
{
	return natControls->GetPortMap(portMapID).GetEnabled();
}

/**
 * @brief Retrieves a description of the port map.
 *
 * @param	portMapID ID of the port map to use.
 * @return  a description of the port map.
 */
const Packet & UpnpNatCommunication::GetDescription( size_t portMapID ) const
{
	return natControls->GetPortMap(portMapID).GetDescription();
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
bool UpnpNatCommunication::FindPortMap( const char * protocol, long externalPort, size_t * position )
{
	return natControls->FindPortMap(protocol,externalPort,position);
}

/**
 * @brief	Does not return until the last action has finished.
 */
void UpnpNatCommunication::WaitUntilLastActionFinished() const
{
	actionThread.GetLastMessage()->WaitUntilNotInUseByThread();
	if(actionThread.GetError() != NULL)
	{
		actionThread.GetError()->DoRethrow();
	}
}

/**
 * @brief	Determines whether the last action has finished.
 *
 * @return true if the last action is finished.
 */
bool UpnpNatCommunication::IsLastActionFinished() const
{
	bool returnMe = actionThread.GetLastMessage()->IsMessageInUseByThread();
	if(actionThread.GetError() != NULL)
	{
		actionThread.GetError()->DoRethrow();
	}
	return returnMe;
}

/**
 * @brief	Extracts the last message that was sent to the controller thread,
 * so that it will not be automatically cleaned up.
 *
 * @return	the last message sent. This pointer should be cleaned up using delete. Make sure
 * to set in use by sender to false before attempting to cleanup.
 * @return  NULL if no message has been sent yet.
 */
const ThreadMessageItem * UpnpNatCommunication::ExtractLastMessage()
{
	return actionThread.ExtractLastMessage();
}

/**
 * @brief	Retrieves a stored thread error report.
 *
 * @return	NULL if no report is stored.
 * @return  an error report describing the error.
 */
const ErrorReport * UpnpNatCommunication::GetError() const
{
	return actionThread.GetError();
}

/**
 * @brief	Determines whether an error report is stored by the thread.
 *
 * @return	true if a report is stored, false if not.
 */
bool UpnpNatCommunication::IsErrorStored() const
{
	return actionThread.IsErrorStored();
}

/**
 * @brief	Erases any currently stored error report stored by the thread.
 */
void UpnpNatCommunication::ClearError()
{
	actionThread.ClearError();
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool UpnpNatCommunication::TestClass()
{
	cout << "Testing UpnpNatCommunication class...\n";
	bool problem = false;

	UpnpNatCommunication nat;
	nat.UpdateInfo();
	nat.WaitUntilLastActionFinished();

	nat.AddPortMap(5000,"UDP",4000,"192.168.1.4",false,"hello world");
	nat.WaitUntilLastActionFinished();

	if(nat.FindPortMap("UDP",5000,NULL) == true)
	{
		cout << "Port map added successfully\n";
	}
	else
	{
		problem = true;
		cout << "Port map not added successfully\n";
	}
	cout << "\n\n";

	for(size_t n = 0;n<nat.GetPortMapAmount();n++)
	{
		cout << "Port map entry " << n+1 << '\n';
		cout << nat.GetDescription(n).GetNullTerminated() << '\n';
		cout << nat.GetInternalIP(n).GetNullTerminated() << ":" << nat.GetInternalPort(n) << '\n';
		cout << nat.GetExternalIP(n).GetNullTerminated() << ":" << nat.GetExternalPort(n) << '\n';
		cout << "Protocol: " << nat.GetProtocol(n).GetNullTerminated() << ", enabled: " << nat.GetEnabled(n) << '\n';
		cout << "\n\n";
	}
	
	nat.DeletePortMap("UDP",5000);
	nat.WaitUntilLastActionFinished();

	if(nat.FindPortMap("UDP",5000,NULL) == false)
	{
		cout << "Port map removed successfully\n";
	}
	else
	{
		problem = true;
		cout << "Port map not removed successfully\n";
	}

	if(nat.IsErrorStored() == true)
	{
		char * fullMessage = nat.GetError()->GetFullMessage();
		cout << fullMessage << '\n';
		delete[] fullMessage;
	}

	cout << "\n\n";
	return !problem;
}

