#pragma once
#ifndef CLR
#include "FullInclude.h"
#endif

//DBP_CPP_DLL BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, PVOID lpReserved );
#ifdef DBP
DBP_CPP_DLL void Constructor ( void );
DBP_CPP_DLL void Destructor ( void );
DBP_CPP_DLL void ReceiveCoreDataPtr ( LPVOID pCore );
DWORD _ReverseString( DWORD pOldString, DWORD pStringIn, DWORD dwSize );


extern "C"
{
	/*
	DBP_CPP_DLL DWORD DBP_GetServiceName(DWORD pOldString, size_t ProfileID, size_t ServiceID);
	DBP_CPP_DLL DWORD DBP_GetServiceRemoteAddresses(DWORD pOldString, size_t ProfileID, size_t ServiceID);
	DBP_CPP_DLL DWORD DBP_GetRemoteAdminAddresses(DWORD pOldString, size_t ProfileID);
	DBP_CPP_DLL DWORD DBP_GetApplicationName(DWORD pOldString, size_t ProfileID, size_t ApplicationID);
	DBP_CPP_DLL DWORD DBP_GetApplicationFileName(DWORD pOldString, size_t ProfileID, size_t ApplicationID);
	DBP_CPP_DLL DWORD DBP_GetApplicationRemoteAddresses(DWORD pOldString, size_t ProfileID, size_t ApplicationID);
	*/
}
#endif
