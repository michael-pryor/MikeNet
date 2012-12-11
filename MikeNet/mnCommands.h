#pragma once

/**
 * @brief Commands for procedural programming, that wrap around internal classes.
 *
 * These commands exist for those who prefer procedural programming over object orientated.
 * They wrap around the classes of the networking module and deal with errors differently 
 * dependent on the error mode we are in.
 */
namespace mn
{
#ifdef C_LINKAGE
	extern "C"
	{
#endif
	CPP_DLL const char * GetVersion();


	DBP_CPP_DLL int Start(size_t numInstances, size_t numThreads);
	DBP_CPP_DLL int Finish(size_t instanceID);

	DBP_CPP_DLL const char * DNS_B(const char * hostName);
	DBP_CPP_DLL size_t GetLocalInterfaceAmount();

	CPP_DLL const char * GetLocalInterfaceStr(size_t num);
	CPP_DLL const char * GetHostName();

	DBP_CPP_DLL NetUtility::ConnectionStatus Connect(size_t instanceID, const char * connectIP_TCP, unsigned short connectPort_TCP, const char * connectIP_UDP, unsigned short connectPort_UDP, size_t timeoutMilliseconds, bool block, INT_PTR profile);
	DBP_CPP_DLL int StartServer(size_t instanceID, size_t maxClients, INT_PTR profile);

	DBP_CPP_DLL int StartBroadcast(size_t instanceID, const char * connectAddressIP, unsigned short connectAddressPort, bool sendEnabled, bool recvEnabled, INT_PTR profile);

	DBP_CPP_DLL int DisconnectClient(size_t instanceID, size_t clientID);
	DBP_CPP_DLL int SetServerTimeout(size_t instanceID, size_t timeoutMilliseconds);
	DBP_CPP_DLL size_t GetServerTimeout(size_t instanceID);
	DBP_CPP_DLL size_t ClientJoined(size_t instanceID);
	DBP_CPP_DLL size_t ClientLeft(size_t instanceID);
	CPP_DLL const char * GetClientIPTCP(size_t instanceID, size_t clientID);
	DBP_CPP_DLL unsigned short GetClientPortTCP(size_t instanceID, size_t clientID);

	CPP_DLL const char * GetClientIPUDP(size_t instanceID, size_t clientID);
	DBP_CPP_DLL unsigned short GetClientPortUDP(size_t instanceID, size_t clientID);

	DBP_CPP_DLL size_t GetClientFrom(INT_PTR Packet);
	DBP_CPP_DLL int ShutdownClient(size_t instanceID, size_t clientID);
	DBP_CPP_DLL NetUtility::ConnectionStatus ClientConnected(size_t instanceID, size_t clientID);
	DBP_CPP_DLL int FlushRecvTCP(size_t instanceID, size_t clientID);
	DBP_CPP_DLL int FlushRecvUDP(size_t instanceID, size_t clientID);
	DBP_CPP_DLL int ChangeBufferSizeTCP(size_t instanceID, size_t clientID, size_t newSize);
	DBP_CPP_DLL int SetAutoResizeTCP(size_t instanceID, size_t clientID, bool autoResize);

	CPP_DLL const char * GetConnectIPTCP(size_t instanceID);
	DBP_CPP_DLL unsigned short GetConnectPortTCP(size_t instanceID);

	CPP_DLL const char * GetConnectIPUDP(size_t instanceID);
	DBP_CPP_DLL unsigned short GetConnectPortUDP(size_t instanceID);
	DBP_CPP_DLL NetUtility::ConnectionStatus PollConnect(size_t instanceID);
	DBP_CPP_DLL int StopConnect(size_t instanceID);
	CPP_DLL bool GetConnecting(size_t instanceID);
	DBP_CPP_DLL size_t GetClientID(size_t instanceID);
	DBP_CPP_DLL size_t GetMaxClients(size_t instanceID);
	DBP_CPP_DLL size_t GetMaxOperations(size_t instanceID);
	DBP_CPP_DLL size_t GetRecvSizeUDP(size_t instanceID);
	DBP_CPP_DLL size_t GetThreads();
	DBP_CPP_DLL size_t GetNumInstances();
	DBP_CPP_DLL NetInstance::Type GetState(size_t instanceID);
	DBP_CPP_DLL NetMode::ProtocolMode GetModeUDP(size_t instanceID);
	DBP_CPP_DLL int GetEnabledUDP(size_t instanceID);
	DBP_CPP_DLL double GetPercentTCP(size_t instanceID, size_t clientID);
	DBP_CPP_DLL size_t GetRecvSizeTCP(size_t instanceID, size_t clientID);
	DBP_CPP_DLL size_t GetBytesTCP(size_t instanceID, size_t clientID);
	DBP_CPP_DLL int GetHandshakeEnabledTCP(size_t instanceID);
	DBP_CPP_DLL NetMode::ProtocolMode GetModeTCP(size_t instanceID);
	DBP_CPP_DLL int GetGracefulDisconnectEnabledTCP(size_t instanceID);
	DBP_CPP_DLL unsigned int GetSendTimeout(size_t instanceID);
	DBP_CPP_DLL size_t GetStoreAmountTCP(size_t instanceID, size_t clientID);
	DBP_CPP_DLL size_t GetStoreAmountUDP(size_t instanceID, size_t clientID);

	DBP_CPP_DLL int GetPostfixTCP(size_t instanceID, INT_PTR packet);
	DBP_CPP_DLL int GetNagleEnabledTCP(size_t instanceID);
	DBP_CPP_DLL int GetAutoResizeTCP(size_t instanceID, size_t clientID);

	CPP_DLL const char * GetLocalIPTCP(size_t instanceID);
	DBP_CPP_DLL unsigned short GetLocalPortTCP(size_t instanceID);

	CPP_DLL const char * GetLocalIPUDP(size_t instanceID);
	DBP_CPP_DLL unsigned short GetLocalPortUDP(size_t instanceID);

	DBP_CPP_DLL size_t RecvTCP(size_t instanceID, INT_PTR destinationPacket, size_t clientID);
	DBP_CPP_DLL size_t RecvUDP(size_t instanceID, INT_PTR destinationPacket, size_t clientID, size_t operationID);
	DBP_CPP_DLL int SendUDP(size_t instanceID, INT_PTR packet, size_t clientID, bool keep, bool block);
	DBP_CPP_DLL int SendToUDP(size_t instanceID, INT_PTR packet, const char * addrIP, unsigned short addrPort, bool keep, bool block);
	DBP_CPP_DLL int SendTCP(size_t instanceID, INT_PTR packet, size_t clientID, bool keep, bool block);
	DBP_CPP_DLL int SendAllTCP(size_t instanceID, INT_PTR packet, bool keep, bool block, size_t clientExcludeID);
	DBP_CPP_DLL int SendAllUDP(size_t instanceID, INT_PTR packet, bool keep, bool block, size_t clientExcludeID);

	DBP_CPP_DLL int AddUnsignedInt(INT_PTR packet, unsigned int Add);
	DBP_CPP_DLL int AddInt(INT_PTR packet, int Add);
	CPP_DLL int AddLongInt(INT_PTR packet, long int Add);
	DBP_CPP_DLL int AddLongLongInt(INT_PTR packet, long long int Add);
	DBP_CPP_DLL int AddFloat(INT_PTR packet, float Add);
	DBP_CPP_DLL int AddUnsignedByte(INT_PTR packet, unsigned char Add);
	CPP_DLL int AddByte(INT_PTR packet, char Add);
	CPP_DLL int AddSignedByte(INT_PTR packet, signed char Add);
	DBP_CPP_DLL int AddDouble(INT_PTR packet, double Add);
	CPP_DLL int AddLongDouble(INT_PTR packet, long double Add);
	CPP_DLL int AddShortInt(INT_PTR packet, short int Add);
	DBP_CPP_DLL int AddUnsignedShortInt(INT_PTR packet, unsigned short int Add);
	CPP_DLL int AddUnsignedLongInt(INT_PTR packet, unsigned long int Add);
	CPP_DLL int AddUnsignedLongLongInt(INT_PTR packet, unsigned long long int Add);
	DBP_CPP_DLL int AddSizeT(INT_PTR packet, size_t add);
	DBP_CPP_DLL int AddStringC(INT_PTR packet, const char * source, size_t length, bool prefix);
	DBP_CPP_DLL size_t GetPacketRemainder(INT_PTR packet);
	DBP_CPP_DLL unsigned int GetUnsignedInt(INT_PTR packet);
	DBP_CPP_DLL int GetInt(INT_PTR packet);
	CPP_DLL long int GetLongInt(INT_PTR packet);
	DBP_CPP_DLL long long int GetLongLongInt(INT_PTR packet);
	CPP_DLL float GetFloat(INT_PTR packet);
	DBP_CPP_DLL unsigned char GetUnsignedByte(INT_PTR packet);
	CPP_DLL char GetByte(INT_PTR packet);
	CPP_DLL signed char GetSignedByte(INT_PTR packet);
	DBP_CPP_DLL double GetDouble(INT_PTR packet);
	CPP_DLL long double GetLongDouble(INT_PTR packet);
	CPP_DLL short int GetShortInt(INT_PTR packet);
	DBP_CPP_DLL unsigned short int GetUnsignedShortInt(INT_PTR packet);
	CPP_DLL char * GetStringC(INT_PTR packet, size_t length, bool nullTerminated);
	CPP_DLL char * GetStringC_C(INT_PTR packet, size_t length, bool nullTerminated);
	CPP_DLL int GetStringC_B(INT_PTR packet, char * dest, size_t length, bool nullTerminated);
	CPP_DLL unsigned long int GetUnsignedLongInt(INT_PTR packet);
	CPP_DLL unsigned long long int GetUnsignedLongLongInt(INT_PTR packet);
	DBP_CPP_DLL size_t GetSizeT(INT_PTR packet);
	DBP_CPP_DLL size_t GetAge(INT_PTR packet);
	DBP_CPP_DLL size_t GetOperation(INT_PTR packet);
	CPP_DLL const char * GetErrorOperation();
	DBP_CPP_DLL size_t GetInstance(INT_PTR packet);
	DBP_CPP_DLL int ClearPacket(INT_PTR packet);
	DBP_CPP_DLL int AddPacket(INT_PTR dest, INT_PTR source);
	DBP_CPP_DLL int AssignPacket(INT_PTR dest, INT_PTR source);
	DBP_CPP_DLL int ComparePacket(INT_PTR packet1, INT_PTR packet2);
	DBP_CPP_DLL INT_PTR CreatePacket();
	DBP_CPP_DLL int DeletePacket(INT_PTR packet);
	DBP_CPP_DLL int SetMemorySize(INT_PTR packet, size_t size);
	DBP_CPP_DLL size_t GetMemorySize(INT_PTR packet);
	DBP_CPP_DLL int SetUsedSize(INT_PTR packet, size_t size);
	DBP_CPP_DLL size_t GetUsedSize(INT_PTR packet);
	DBP_CPP_DLL int SetCursor(INT_PTR packet, size_t cursor);
	DBP_CPP_DLL size_t GetCursor(INT_PTR packet);
	DBP_CPP_DLL int Erase(INT_PTR packet, size_t startPos, size_t amount);
	DBP_CPP_DLL int Insert(INT_PTR packet, size_t amount);
	DBP_CPP_DLL INT_PTR CreatePacketFromPacket(INT_PTR fromPacket);
	DBP_CPP_DLL INT_PTR CreatePacketFromString(const char * str);
	DBP_CPP_DLL int ComparePacketString(INT_PTR packet, const char * str);
	DBP_CPP_DLL int ChangeMemorySize(INT_PTR packet, size_t size);
	DBP_CPP_DLL int AssignPacketString(INT_PTR destination, const char * from);
	DBP_CPP_DLL size_t GetStringSize(INT_PTR packet);

	DBP_CPP_DLL int Encrypt(INT_PTR packet, INT_PTR key, bool block);
	DBP_CPP_DLL int Decrypt(INT_PTR packet, INT_PTR key, bool block);
	DBP_CPP_DLL int GetLastEncryptionOperationFinished(INT_PTR packet);

	CPP_DLL bool GetErrorFlag();
	DBP_CPP_DLL void SetErrorFlag();
	DBP_CPP_DLL void ClearErrorFlag();
	CPP_DLL const char * GetErrorCommand();
	CPP_DLL const char * GetErrorFile();
	CPP_DLL const char * GetErrorFull();
	DBP_CPP_DLL __int64 GetErrorCode();
	DBP_CPP_DLL unsigned __int64 GetErrorLine();


	DBP_CPP_DLL int SetProfileBufferSizes(INT_PTR profile, size_t recvSizeTCP, size_t recvSizeUDP);
	DBP_CPP_DLL int SetProfileEnabledUDP(INT_PTR profile, bool option);
	DBP_CPP_DLL int SetProfileAutoResizeTCP(INT_PTR profile, bool autoResize);
	DBP_CPP_DLL int SetProfileHandshakeEnabled(INT_PTR profile, bool option);
	DBP_CPP_DLL int SetProfileModeTCP(INT_PTR profile, char modeTCP);
	DBP_CPP_DLL int SetProfileGracefulDisconnectEnabled(INT_PTR profile, bool option);
	DBP_CPP_DLL int SetProfileSendTimeout(INT_PTR profile, unsigned int  timeoutMilliseconds);
	DBP_CPP_DLL int SetProfilePostfixTCP(INT_PTR profile, INT_PTR packet);
	DBP_CPP_DLL int SetProfileNagleEnabledTCP(INT_PTR profile, bool option);
	DBP_CPP_DLL int SetProfileLocalTCP(INT_PTR profile, const char * IP, unsigned short port);
	DBP_CPP_DLL int SetProfileLocalUDP(INT_PTR profile, const char * IP, unsigned short port);
	DBP_CPP_DLL int SetProfileLocal(INT_PTR profile, const char * IP_TCP, unsigned short portTCP, const char * IP_UDP, unsigned short portUDP);
	DBP_CPP_DLL int SetProfileDecryptKeyUDP(INT_PTR profile, INT_PTR key);
	DBP_CPP_DLL int SetProfileServerTimeout(INT_PTR profile, size_t timeoutMilliseconds);
	DBP_CPP_DLL int SetProfileModeUDP(INT_PTR profile, char modeUDP);
	DBP_CPP_DLL int SetProfileReusableUDP(INT_PTR profile, bool option);
	DBP_CPP_DLL int SetProfileNumOperationsUDP(INT_PTR profile, size_t numOperations);

	DBP_CPP_DLL size_t GetProfileBufferSizeTCP(INT_PTR profile);
	DBP_CPP_DLL size_t GetProfileBufferSizeUDP(INT_PTR profile);
	CPP_DLL const char * GetProfileLocalIPTCP(INT_PTR profile);
	DBP_CPP_DLL unsigned short GetProfileLocalPortTCP(INT_PTR profile);
	CPP_DLL const char * GetProfileLocalIPUDP(INT_PTR profile);
	DBP_CPP_DLL unsigned short GetProfileLocalPortUDP(INT_PTR profile);	
	DBP_CPP_DLL int GetProfileEnabledUDP(INT_PTR profile);
	DBP_CPP_DLL int GetProfileDecryptKeyLoadedUDP(INT_PTR profile);
	DBP_CPP_DLL INT_PTR GetProfileDecryptKeyUDP(INT_PTR profile);
	DBP_CPP_DLL	int GetProfileHandshakeEnabled(INT_PTR profile);
	DBP_CPP_DLL	NetMode::ProtocolMode GetProfileModeTCP(INT_PTR profile);
	DBP_CPP_DLL NetMode::ProtocolMode GetProfileModeUDP( INT_PTR profile );
	DBP_CPP_DLL	int GetProfileAutoResizeTCP(INT_PTR profile);
	DBP_CPP_DLL size_t GetProfileSendTimeout(INT_PTR profile);
	DBP_CPP_DLL	int GetProfileGracefulDisconnectEnabled(INT_PTR profile);
	DBP_CPP_DLL	int GetProfileNagleEnabledTCP(INT_PTR profile);
	DBP_CPP_DLL	int GetProfilePostfixTCP(INT_PTR profile, INT_PTR destination);
	DBP_CPP_DLL int GetProfileReusableUDP(INT_PTR profile);
	DBP_CPP_DLL size_t GetProfileServerTimeout(INT_PTR profile);
	DBP_CPP_DLL size_t GetProfileNumOperationsUDP(INT_PTR profile);


	DBP_CPP_DLL INT_PTR CreateInstanceProfile();
	DBP_CPP_DLL INT_PTR CreateInstanceProfileFrom(INT_PTR profile);
	DBP_CPP_DLL int DeleteInstanceProfile(INT_PTR profile);
	DBP_CPP_DLL INT_PTR AssignInstanceProfile(INT_PTR dest, INT_PTR source);
	DBP_CPP_DLL int CompareInstanceProfile(INT_PTR profile1, INT_PTR profile2);

	DBP_CPP_DLL int ToggleErrorMode(int errorMode);
	DBP_CPP_DLL int SetErrorMode(int errorMode, bool enabled);
	DBP_CPP_DLL int GetErrorMode(int errorMode);

	DBP_CPP_DLL INT_PTR CreateKey256(__int64 key1, __int64 key2, __int64 key3, __int64 key4);
	DBP_CPP_DLL INT_PTR CreateKey192(__int64 key1, __int64 key2, __int64 key3);
	DBP_CPP_DLL INT_PTR CreateKey128(__int64 key1, __int64 key2);

	CPP_DLL INT_PTR CreateKeyFine128(int key1, int key2, int key3, int key4);
	CPP_DLL INT_PTR CreateKeyFine192(int key1, int key2, int key3, int key4, int key5, int key6);
	CPP_DLL INT_PTR CreateKeyFine256(int key1, int key2, int key3, int key4, int key5, int key6, int key7, int key8);

	CPP_DLL int GetErrorCodeFine();
	CPP_DLL int GetErrorLineFine();

	DBP_CPP_DLL int DeleteKey(INT_PTR Key);
	
	DBP_CPP_DLL size_t GetLogicalCPU();

	CPP_DLL void FreeString(char * freeMe);


	DBP_CPP_DLL int SetProfileSendMemoryLimit(INT_PTR profile, size_t memoryLimitTCP, size_t memoryLimitUDP);
	DBP_CPP_DLL int SetProfileRecvMemoryLimit(INT_PTR profile, size_t memoryLimitTCP, size_t memoryLimitUDP);
	DBP_CPP_DLL size_t GetProfileSendMemoryLimitTCP(INT_PTR profile);
	DBP_CPP_DLL size_t GetProfileRecvMemoryLimitTCP(INT_PTR profile);
	DBP_CPP_DLL size_t GetProfileSendMemoryLimitUDP(INT_PTR profile);
	DBP_CPP_DLL size_t GetProfileRecvMemoryLimitUDP(INT_PTR profile);

	DBP_CPP_DLL int SetProfileMemoryRecycleTCP(INT_PTR profile, size_t numberOfPackets, size_t memorySizeOfPackets);
	DBP_CPP_DLL int SetProfileMemoryRecycleUDP(INT_PTR profile, size_t numberOfPackets, size_t memorySizeOfPackets);
	DBP_CPP_DLL size_t GetProfileMemoryRecycleNumberOfPacketsTCP(INT_PTR profile);
	DBP_CPP_DLL size_t GetProfileMemoryRecycleMemorySizeOfPacketsTCP(INT_PTR profile);
	DBP_CPP_DLL size_t GetProfileMemoryRecycleNumberOfPacketsUDP(INT_PTR profile);
	DBP_CPP_DLL size_t GetProfileMemoryRecycleMemorySizeOfPacketsUDP(INT_PTR profile);

	DBP_CPP_DLL int SetSendMemoryLimitTCP(size_t instanceID, size_t clientID, size_t newLimit);
	DBP_CPP_DLL int SetRecvMemoryLimitTCP(size_t instanceID, size_t clientID, size_t newLimit);
	DBP_CPP_DLL size_t GetSendMemoryLimitTCP(size_t instanceID, size_t clientID);
	DBP_CPP_DLL size_t GetRecvMemoryLimitTCP(size_t instanceID, size_t clientID);
	DBP_CPP_DLL size_t GetSendMemorySizeTCP(size_t instanceID, size_t clientID);
	DBP_CPP_DLL size_t GetRecvMemorySizeTCP(size_t instanceID, size_t clientID);
	
	DBP_CPP_DLL int SetSendMemoryLimitUDP(size_t instanceID, size_t newLimit);
	DBP_CPP_DLL int SetRecvMemoryLimitUDP(size_t instanceID, size_t clientID, size_t newLimit);
	DBP_CPP_DLL size_t GetSendMemoryLimitUDP(size_t instanceID);
	DBP_CPP_DLL size_t GetRecvMemoryLimitUDP(size_t instanceID, size_t clientID);
	DBP_CPP_DLL size_t GetSendMemorySizeUDP(size_t instanceID);
	DBP_CPP_DLL size_t GetRecvMemorySizeUDP(size_t instanceID, size_t clientID);

#ifdef DBP
	DBP_CPP_DLL DWORD DBP_GetConnecting(size_t instanceID);
	DBP_CPP_DLL DWORD DBP_GetErrorFlag();

	DBP_CPP_DLL DWORD DBP_GetErrorOperation(DWORD pOldString);
	DBP_CPP_DLL DWORD DBP_GetErrorCommand(DWORD pOldString);
	DBP_CPP_DLL DWORD DBP_GetErrorFile(DWORD pOldString);
	DBP_CPP_DLL DWORD DBP_GetErrorFull(DWORD pOldString);
	DBP_CPP_DLL DWORD DBP_GetVersion(DWORD pOldString);

	DBP_CPP_DLL DWORD DBP_GetUnsignedByte(INT_PTR Packet);
	DBP_CPP_DLL DWORD DBP_GetString(DWORD pOldString, INT_PTR Packet, size_t Length, bool NullTerminated);
	DBP_CPP_DLL DWORD DBP_GetLocalIPTCP(DWORD pOldString, size_t Instance);
	DBP_CPP_DLL DWORD DBP_GetLocalIPUDP(DWORD pOldString, size_t Instance);
	DBP_CPP_DLL DWORD DBP_GetConnectIPTCP(DWORD pOldString, size_t Instance);
	DBP_CPP_DLL DWORD DBP_GetConnectIPUDP(DWORD pOldString, size_t Instance);
	DBP_CPP_DLL DWORD DBP_GetClientIPTCP(DWORD pOldString, size_t Instance, size_t clientID);
	DBP_CPP_DLL DWORD DBP_GetClientIPUDP(DWORD pOldString, size_t Instance, size_t clientID);
	DBP_CPP_DLL DWORD DBP_GetFloat(INT_PTR Packet);
	DBP_CPP_DLL DWORD DBP_DNS(DWORD pOldString, char * HostName);
	DBP_CPP_DLL DWORD DBP_GetLocalInterface(DWORD pOldString, size_t Number);
	DBP_CPP_DLL DWORD DBP_GetHostName(DWORD pOldString);
	DBP_CPP_DLL DWORD DBP_GetProfileLocalIPTCP(DWORD pOldString, INT_PTR profile);
	DBP_CPP_DLL DWORD DBP_GetProfileLocalIPUDP(DWORD pOldString, INT_PTR profile);

	DBP_CPP_DLL int DBP_GetMemblock(INT_PTR Packet, size_t DestinationPtr, size_t Length, bool NullTerminated);
	DBP_CPP_DLL int DBP_AddMemblock(INT_PTR Packet, size_t DestinationPtr, size_t Length, bool Prefix); 
#endif

#ifdef C_LINKAGE
	}
#endif

	const NetAddress & DNS_A(const char * hostName);
	const NetAddress & GetLocalInterface(size_t num);

	NetUtility::ConnectionStatus Connect(size_t instanceID, const NetAddress * connectAddrTCP, const NetAddress * connectAddrUDP, size_t timeoutMilliseconds, bool block, const NetInstanceProfile & profile);

	int StartServer(size_t instanceID, size_t maxClients, const NetInstanceProfile & profile);

	int StartBroadcast(size_t instanceID, const NetAddress & connectAddress, bool sendEnabled, bool recvEnabled, const NetInstanceProfile & profile);
	NetAddress GetClientAddressTCP(size_t instanceID, size_t clientID);
	NetAddress GetClientAddressUDP(size_t instanceID, size_t clientID);
	NetAddress GetConnectAddressTCP(size_t instanceID);
	NetAddress GetConnectAddressUDP(size_t instanceID);
	int GetPostfixTCP(size_t instanceID, Packet & packet);

	NetAddress GetLocalAddressTCP(size_t instanceID);
	NetAddress GetLocalAddressUDP(size_t instanceID);
	size_t RecvTCP(size_t instanceID, Packet & destinationPacket, size_t clientID);
	size_t RecvUDP(size_t instanceID, Packet & destinationPacket, size_t clientID, size_t operationID);
	NetUtility::SendStatus SendUDP(size_t instanceID, Packet & packet, size_t clientID, bool keep, bool block);
	NetUtility::SendStatus SendToUDP(size_t instanceID, Packet & packet, const NetAddress & address, bool keep, bool block);
	NetUtility::SendStatus SendTCP(size_t instanceID, Packet & packet, size_t clientID, bool keep, bool block);
	int SendAllTCP(size_t instanceID, Packet & packet, bool keep, bool block, size_t clientExcludeID);
	int SendAllUDP(size_t instanceID, Packet & packet, bool keep, bool block, size_t clientExcludeID);

	int SetProfileModeUDP(NetInstanceProfile & profile, NetMode::ProtocolMode modeUDP);
	NetMode::ProtocolMode GetProfileModeUDP( const NetInstanceProfile & profile );


	int SetProfileBufferSizes(NetInstanceProfile & profile, size_t recvSizeTCP, size_t recvSizeUDP);
	int SetProfileEnabledUDP(NetInstanceProfile & profile, bool option);
	int SetProfileAutoResizeTCP(NetInstanceProfile & profile, bool autoResize);
	int SetProfileHandshakeEnabled(NetInstanceProfile & profile, bool option);
	int SetProfileModeTCP(NetInstanceProfile & profile, NetMode::ProtocolMode modeTCP);
	int SetProfileGracefulDisconnectEnabled(NetInstanceProfile & profile, bool option);
	int SetProfileSendTimeout(NetInstanceProfile & profile, unsigned int timeoutMilliseconds);
	int SetProfilePostfixTCP(NetInstanceProfile & profile, const Packet & packet);
	int SetProfileNagleEnabledTCP(NetInstanceProfile & profile, bool option);
	int SetProfileLocalTCP(NetInstanceProfile & profile, const NetAddress & addressTCP);
	int SetProfileLocalUDP(NetInstanceProfile & profile, const NetAddress & addressUDP);
	int SetProfileLocal(NetInstanceProfile & profile, const NetAddress & addressTCP, const NetAddress & addressUDP);
	int SetProfileServerTimeout(NetInstanceProfile & profile, size_t timeoutMilliseconds);

	NetSocket::RecvFunc GetProfileFunctionTCP(const NetInstanceProfile & profile);
	NetSocket::RecvFunc GetProfileFunctionUDP(const NetInstanceProfile & profile);
	size_t GetProfileBufferSizeTCP(const NetInstanceProfile & profile);
	size_t GetProfileBufferSizeUDP(const NetInstanceProfile & profile);
	int GetProfileEnabledUDP(const NetInstanceProfile & profile);
	int GetProfileAutoResizeTCP(const NetInstanceProfile & profile); 
	int GetProfileHandshakeEnabled(const NetInstanceProfile & profile); 
	NetMode::ProtocolMode GetProfileModeTCP(const NetInstanceProfile & profile); 
	int GetProfileGracefulDisconnectEnabled(const NetInstanceProfile & profile); 
	size_t GetProfileSendTimeout(const NetInstanceProfile & profile);
	Packet GetProfilePostfixTCP(const NetInstanceProfile & profile); 
	int GetProfileNagleEnabledTCP(const NetInstanceProfile & profile); 
	NetAddress GetProfileLocalAddressTCP(const NetInstanceProfile & profile);
	NetAddress GetProfileLocalAddressUDP(const NetInstanceProfile & profile);
	size_t GetProfileServerTimeout(const NetInstanceProfile & profile);


	int ToggleErrorMode(ErrorReport::ErrorMode errorMode);
	int SetErrorMode(ErrorReport::ErrorMode errorMode, bool enabled);
	int GetErrorMode(ErrorReport::ErrorMode errorMode);


	int SetProfileSendMemoryLimit(NetInstanceProfile & profile, size_t memoryLimitTCP, size_t memoryLimitUDP);
	int SetProfileRecvMemoryLimit(NetInstanceProfile & profile, size_t memoryLimitTCP, size_t memoryLimitUDP);
	size_t GetProfileSendMemoryLimitTCP(const NetInstanceProfile & profile);
	size_t GetProfileRecvMemoryLimitTCP(const NetInstanceProfile & profile);
	size_t GetProfileSendMemoryLimitUDP(const NetInstanceProfile & profile);
	size_t GetProfileRecvMemoryLimitUDP(const NetInstanceProfile & profile);

	int SetProfileMemoryRecycleTCP(NetInstanceProfile & profile, size_t numberOfPackets, size_t memorySizeOfPackets);
	int SetProfileMemoryRecycleUDP(NetInstanceProfile & profile, size_t numberOfPackets, size_t memorySizeOfPackets);
	size_t GetProfileMemoryRecycleNumberOfPacketsTCP(const NetInstanceProfile & profile);
	size_t GetProfileMemoryRecycleMemorySizeOfPacketsTCP(const NetInstanceProfile & profile);
	size_t GetProfileMemoryRecycleNumberOfPacketsUDP(const NetInstanceProfile & profile);
	size_t GetProfileMemoryRecycleMemorySizeOfPacketsUDP(const NetInstanceProfile & profile);

	bool TestClass();
}
