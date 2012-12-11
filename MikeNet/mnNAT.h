#pragma once

/**
 * @brief Commands for procedural programming, that wrap around internal classes.
 *
 * These commands exist for those who prefer procedural programming over object orientated.
 * They wrap around the classes of the Universal Plug and Play Network Address Translation module and deal with errors differently 
 * dependent on the error mode we are in.
 */
namespace mnNAT
{
	void _Block(bool block);
	void _DealWithError();

#ifdef C_LINKAGE
	extern "C"
	{
#endif
	DBP_CPP_DLL int StartNAT(bool block);
	DBP_CPP_DLL int FinishNAT();
	DBP_CPP_DLL int UpdateNAT(bool block);
	DBP_CPP_DLL int AddPortMap(const char * protocol, long externalPort, long internalPort, const char * internalIP, bool enabled, const char * description, bool block);
	DBP_CPP_DLL size_t GetPortMapAmount();
	DBP_CPP_DLL int EnablePortMap(size_t portMapID, bool block);
	DBP_CPP_DLL int DisablePortMap(size_t portMapID, bool block);
	DBP_CPP_DLL int SetPortMapInternalIP(size_t portMapID, const char * internalIP, bool block);
	DBP_CPP_DLL int SetPortMapInternalPort(size_t portMapID, long internalPort, bool block);
	DBP_CPP_DLL int SetPortMapDescription(size_t portMapID, const char * description, bool block);
	CPP_DLL const char * GetPortMapExternalIP(size_t portMapID);
	DBP_CPP_DLL long GetPortMapExternalPort(size_t portMapID);
	DBP_CPP_DLL long GetPortMapInternalPort(size_t portMapID);
	CPP_DLL const char * GetPortMapProtocol(size_t portMapID);
	CPP_DLL const char * GetPortMapInternalIP(size_t portMapID);
	DBP_CPP_DLL int GetPortMapEnabled(size_t portMapID);
	CPP_DLL const char * GetPortMapDescription(size_t portMapID);
	DBP_CPP_DLL int DeletePortMapA(size_t portMapID, bool block);
	DBP_CPP_DLL int DeletePortMapB(const char * protocol, long externalPort, bool block);
	DBP_CPP_DLL int PortMapExist(const char * protocol, long externalPort);
	DBP_CPP_DLL size_t FindPortMap(const char * protocol, long externalPort);

	DBP_CPP_DLL int WaitUntilLastActionFinished();
	DBP_CPP_DLL int PollNAT();

#ifdef DBP
	DBP_CPP_DLL DWORD DBP_GetPortMapExternalIP(DWORD pOldString, size_t PortMapID);
	DBP_CPP_DLL DWORD DBP_GetPortMapProtocol(DWORD pOldString, size_t PortMapID);
	DBP_CPP_DLL DWORD DBP_GetPortMapInternalIP(DWORD pOldString, size_t PortMapID);
	DBP_CPP_DLL DWORD DBP_GetPortMapDescription(DWORD pOldString, size_t PortMapID);
#endif
#ifdef C_LINKAGE
	}
#endif
}
