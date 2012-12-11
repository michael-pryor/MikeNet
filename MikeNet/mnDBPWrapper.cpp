// Functions that allow MikeNet to work with DarkBASIC Pro (DBP)
// DBP requires special code for returning strings, floats and bytes
#include "FullInclude.h"
#include <conio.h>
#ifdef DBP
	char * mnGetStringC(INT_PTR Packet, size_t iLength, bool bNullTerminated);
	int mnGetStringC(INT_PTR Packet, char * Destination, size_t iLength, bool bNullTerminated);

	// Receives core data from DBP
	DBP_CPP_DLL void ReceiveCoreDataPtr ( LPVOID pCore )
	{
		// Get Core Data Pointer here
		g_pGlob = (GlobStruct*)pCore;
	}

	// DBP Destructor
	DBP_CPP_DLL void Destructor ( void )
	{
		mn::Finish(-1);
		mnSound::FinishSound();
		mnNAT::FinishNAT();

		// This means that you don't need to delete all packets before terminating a
		// DBP application.
		ThreadSingleMessageKeepLastUser::CleanupThreadsForAllClasses();
	}

	// DBP Constructor
	DBP_CPP_DLL void Constructor ( void )
	{

	}

	// Sets up a string ready to be returned in DBP
	DWORD _ReverseString( DWORD pOldString, DWORD pStringIn, DWORD dwSize )
	{
		if (g_pGlob == NULL)
		{
			MessageBox(NULL,"ERROR","Core not set, commands that return string are unable to function",NULL);
			return(NULL);
		}
		else
		{
			// Delete old string
			if(pOldString != NULL)
			{
				g_pGlob->CreateDeleteString ( (DWORD*)&pOldString, 0 );
			}

			// Return string pointer
			LPSTR pReturnString=NULL;

			// If input string valid
			if(pStringIn != NULL)
			{
				// Create a new string and copy input string to it
				g_pGlob->CreateDeleteString ((DWORD*)&pReturnString, dwSize);
				memcpy(pReturnString, (LPSTR)pStringIn, dwSize);
			}
			// Return new string pointer
			return (DWORD)pReturnString;
		}
	}

	// *Commands used by user

	// Returning bytes fix, so that can be passed directly to function
	// Must return as DWORD
	/*



	// New to v1.1.5
	DBP_CPP_DLL mn::DBP_GetServiceName(DWORD pOldString, size_t ProfileID, size_t ServiceID)
	{
		const const char * cReturn = mnGetServiceName(ProfileID, ServiceID);

		// Ensure compatibility with DBP
		DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

		return(dReturn);
	}

	DBP_CPP_DLL mn::DBP_GetServiceRemoteAddresses(DWORD pOldString, size_t ProfileID, size_t ServiceID)
	{
		const const char * cReturn = mnGetServiceRemoteAddresses(ProfileID, ServiceID);

		// Ensure compatibility with DBP
		DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

		return(dReturn);
	}	
		
	DBP_CPP_DLL mn::DBP_GetRemoteAdminAddresses(DWORD pOldString, size_t ProfileID)
	{
		const const char * cReturn = mnGetRemoteAdminAddresses(ProfileID);

		// Ensure compatibility with DBP
		DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

		return(dReturn);
	}

	DBP_CPP_DLL mn::DBP_GetApplicationName(DWORD pOldString, size_t ProfileID, size_t ApplicationID)
	{
		const const char * cReturn = mnGetApplicationName(ProfileID, ApplicationID);

		// Ensure compatibility with DBP
		DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

		return(dReturn);
	}

	DBP_CPP_DLL mn::DBP_GetApplicationFileName(DWORD pOldString, size_t ProfileID, size_t ApplicationID)
	{
		const const char * cReturn = mnGetApplicationFileName(ProfileID, ApplicationID);

		// Ensure compatibility with DBP
		DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

		return(dReturn);
	}

	DBP_CPP_DLL mn::DBP_GetApplicationRemoteAddresses(DWORD pOldString, size_t ProfileID, size_t ApplicationID)
	{
		const const char * cReturn = mnGetApplicationRemoteAddresses(ProfileID, ApplicationID);

		// Ensure compatibility with DBP
		DWORD dReturn = _ReverseString(pOldString,(DWORD)cReturn,strlen(cReturn)+1);

		return(dReturn);
	}


	*/

#endif
