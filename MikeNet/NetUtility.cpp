#include "FullInclude.h"


DWORD_PTR NetUtility::threadAffinity = 0;
char * NetUtility::hostName = NULL;
StoreVector<NetAddress> NetUtility::localInterface;
CompletionPort * NetUtility::completionPort = NULL;
NetInstanceGroup * NetUtility::instanceGroup = NULL;
bool NetUtility::winsockActive = false;
bool NetUtility::localInfoLoaded = false;
NetAddress NetUtility::convertedDomainName;
int NetUtility::sizeSOCKADDR = sizeof(SOCKADDR);

const unsigned char NetUtility::EncryptionBox[256] =
	{
			0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
			0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
			0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
			0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
			0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
			0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
			0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
			0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
			0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
			0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
			0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
			0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
			0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
			0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
			0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
			0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
	};

const unsigned char NetUtility::InverseEncryptionBox[256] =
	{
		0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
		0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
		0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
		0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
		0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
		0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
		0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
		0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
		0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
		0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
		0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
		0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
		0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
		0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
		0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
		0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
	};

const unsigned char NetUtility::EncryptionRoundConstant[255] =
	{
		0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 
		0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 
		0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 
		0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 
		0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 
		0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 
		0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 
		0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 
		0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 
		0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 
		0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 
		0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 
		0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 
		0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 
		0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 
		0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb
	};

/**
 * @brief Retrieves the number of completion port threads in operation.
 *
 * @return number of completion port threads in operation.
 */
size_t NetUtility::GetNumThreads()
{
	if(IsCompletionPortSetup() == true)
	{
		return completionPort->Size();
	}
	else
	{
		return 0;
	}
}

/**
 * @brief Retrieves the thread ID associated with the main process.
 *
 * @return the main process' thread ID.
 */
size_t NetUtility::GetMainProcessThreadID()
{
	return GetNumThreads();
}

/**
 * @brief Retrieves the number of multithreaded participants.
 *
 * @return the number of multithreaded participants i.e.
 * number of threads + one main process.
 */
size_t NetUtility::GetNumThreadedParticipants()
{
	return GetNumThreads()+1;
}

/**
 * @brief Retrieves the name of the computer.
 *
 * @return name of computer.
 */
const char * NetUtility::GetHostName()
{
	return hostName;
}

/**
 * @brief Retrieves a local interface.
 *
 * @return a local interface IP address.
 */
const NetAddress & NetUtility::GetLocalInterface(size_t ele)
{
	return localInterface[ele];
}

/**
 * @brief Retrieves the number of local interface available.
 *
 * @return get number of available local interfaces. \n
 * interface IDs range from 0 inclusive to number of local interfaces exclusive.
 */
size_t NetUtility::GetNumLocalInterface()
{
	return localInterface.Size();
}

/**
 * @brief Validates the specified thread ID, ensuring it is not out of bounds.
 *
 * @throws ErrorReport If threadID is invalid, must be between 0 (inclusive) and the number of threads (exclusive).
 * @param threadID Thread ID to check.
 * @param line Line number of method call.
 * @param file Of method call.
 */
void NetUtility::ValidateThreadID(size_t threadID, size_t line, const char * file)
{
	_ErrorException((threadID >= completionPort->Size()),"performing a thread related function. Invalid thread specified",0,line,file);
}

/**
 * @brief Retrieves a pointer to sizeof(SOCKADDR).
 *
 * @return non constant pointer to sizeSOCKADDR as some winsock methods require this.
 * @warning DO NOT MODIFY RETURNED POINTER! The only reason it is non constant is to please winsock!
 */
int * NetUtility::GetSizeSOCKADDR()
{
	return &sizeSOCKADDR;	
}

/**
 * @brief Sets up completion port and creates threads to manage completion port.
 *
 * @param numThreads Number of threads to be created to manage completion port. Set to 0 and the return value
 * of ThreadSingle::GetNumCores() will be used instead.
 */
void NetUtility::SetupCompletionPort(size_t numThreads)
{
	ThreadSingle::AllocateThreadLocalStorage();
	ThreadSingle::ThreadSetCallingThread(NULL);

	if(numThreads == 0)
	{
		numThreads = ThreadSingle::GetNumLogicalCores();
	}

	completionPort = new (nothrow) CompletionPort(numThreads,&NetManageCompletionPort);
	Utility::DynamicAllocCheck(completionPort,__LINE__,__FILE__);
}

/**
 * @brief Shuts down completion port and its threads.
 */
void NetUtility::DestroyCompletionPort()
{
	delete completionPort;
	completionPort = NULL;

	ThreadSingle::DeallocateThreadLocalStorage();
}


/**
 * @brief Determines whether the completion port is setup.
 *
 * @return true if the completion port is setup using SetupCompletionPort().
 */
bool NetUtility::IsCompletionPortSetup()
{
	return (completionPort != NULL);
}

/**
 * @brief Associates an object with the completion port, so that status indicators
 * can be received by the completion port about that object.
 *
 * @param object Object to associate with completion port.
 * @param key Key associated with object, to uniquely identify it.
 */
void NetUtility::AssociateWithCompletionPort(HANDLE object, const CompletionKey & key)
{
	completionPort->Associate(object,key);
}

/**
 * @brief Retrieves and stores information about local interface and the computer name.
 */
void NetUtility::LoadLocalInfo()
{
	UnloadLocalInfo();

	// Get host name
	hostName = new (nothrow) char[maxComputerNameSize];
	Utility::DynamicAllocCheck(hostName,__LINE__,__FILE__);

	int result = gethostname(hostName, maxComputerNameSize);
	_ErrorException((result==SOCKET_ERROR),"finding the host name",WSAGetLastError(),__LINE__,__FILE__);

	// Generate list of available IPs
	hostent * phe = gethostbyname(hostName);
	_ErrorException((phe==NULL),"listing available local IPs",WSAGetLastError(),__LINE__,__FILE__);

	// Determine number of IPs in list
	size_t ipCount = 0;
	while(phe->h_addr_list[ipCount] != 0)
	{
		ipCount++;
	}

	// Store IPs
	localInterface.ResizeAllocate(ipCount);

	for (size_t n = 0;n<ipCount;n++)
	{
		sockaddr_in loadMe;
		loadMe.sin_family = AF_INET;
		loadMe.sin_addr = *reinterpret_cast<IN_ADDR*>(phe->h_addr_list[n]);
		loadMe.sin_port = 0;
		localInterface[n].Load(loadMe);
	}

	localInfoLoaded = true;
}

/**
 * @brief Unloads local information stored about local interface and the computer name.
 *
 * If no information is loaded this method does nothing.
 *
 * @return unload local information previously loaded with LoadInfo.
 */
void NetUtility::UnloadLocalInfo()
{
	delete[] hostName;
	hostName = NULL;

	localInterface.Clear();

	localInfoLoaded = false;
}

/**
 * @brief Determines whether information has been loaded and stored about local interface and the computer name.
 *
 * @return true if local information is currently loaded, local info is loaded using LoadLocalInfo().
 */
bool NetUtility::IsLocalInfoLoaded()
{
	return localInfoLoaded;
}

/**
 * @brief Initializes winsock, winsock is at the heart of the networking module.
 */
void NetUtility::StartWinsock()
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData); // Returns error code if there is an error, 0 means no error
	_ErrorException((iResult != NULL),"initializing winsock",iResult,__LINE__,__FILE__);

	winsockActive = true;
}

/**
 * @brief Unloads winsock, this method will fail if winsock was not initialized using StartWinsock previously.
 */
void NetUtility::FinishWinsock()
{
	if(winsockActive == true)
	{
		int iResult = WSACleanup();
		_ErrorException((iResult != NULL),"shutting down winsock",WSAGetLastError(),__LINE__,__FILE__);

		winsockActive = false;
	}
}

/**
 * @brief Determines whether winsock is loaded.
 *
 * @return true if winsock is active, i.e. Has been started using StartWinsock().
 */
bool NetUtility::IsWinsockActive()
{
	return winsockActive;
}

/**
 * @brief Create instance group which contains instances that are controlled by global mn commands.
 *
 * Any existing group will be overwritten.
 *
 * @param numInstances Number of instances to be created but not setup.
 */
void NetUtility::CreateInstanceGroup( size_t numInstances )
{
	DestroyInstanceGroup();
	instanceGroup = new (nothrow) NetInstanceGroup(numInstances);
	Utility::DynamicAllocCheck(instanceGroup,__LINE__,__FILE__);
}

/**
 * @brief Shuts down instance group.
 *
 * If no group is loaded this method will do nothing.
 */
void NetUtility::DestroyInstanceGroup()
{
	delete instanceGroup;
	instanceGroup = NULL;
}

/**
 * @brief Retrieves reference to instance group.
 *
 * @return reference to instance group. 
 *
 * @throws ErrorReport If no instance group is loaded.
 */
NetInstanceGroup & NetUtility::GetInstanceGroup()
{
	_ErrorException((IsInstanceGroupLoaded() == false),"accessing the global instance group, it is not loaded",0,__LINE__,__FILE__);
	return *instanceGroup;
}

/**
 * @brief Determines whether an instance group is loaded and stored by NetUtility.
 *
 * @return true if a winsock group is currently loaded, a group is loaded using CreateInstanceGroup().
 */
bool NetUtility::IsInstanceGroupLoaded()
{
	return (instanceGroup != NULL);
}

/**
 * @brief Performs all possible setup operations, making the networking module fully operational.
 *
 * @param numThreads Number of threads to be created to manage completion port.
 * @param numInstances Number of instances to be created but not setup.
 */
void NetUtility::LoadEverything(size_t numThreads, size_t numInstances)
{
	StartWinsock();
	LoadLocalInfo();
	SetupCompletionPort(numThreads);
	CreateInstanceGroup(numInstances);
}

/**
 * @brief Performs all possible shutdown operations, fully unloading the networking module.
 *
 * Anything not loaded will be skipped.
 */
void NetUtility::UnloadEverything()
{
	UnloadLocalInfo();
	DestroyInstanceGroup();
	FinishWinsock();
	DestroyCompletionPort(); // Must be done after FinishWinsock()
}

/**
 * @brief Determines whether the networking module is fully operational.
 *
 * @return true if module is networking module is fully loaded and ready for use.
 */
bool NetUtility::IsEverythingLoaded()
{
	return IsLocalInfoLoaded() && IsInstanceGroupLoaded() && IsWinsockActive() && IsCompletionPortSetup();
}

/**
 * @brief Converts a domain name (e.g. www.google.com) to an IP address.
 *
 * If @a str contains an IP address already then the return value will be that IP address unchanged.\n\n
 *
 * This method is thread safe. The return value will remain valid and unmodified until the next call.
 *
 * @param hostName Domain name to convert.
 * @return IP address representation of @a addr.
 */
const NetAddress & NetUtility::ConvertDomainNameToIP(const char * hostName)
{
	DWORD dwResult;
	struct addrinfo * aiResult;

	convertedDomainName.Enter();
	try {
		// Get address information
		dwResult = getaddrinfo(hostName, NULL, NULL, &aiResult);
		_ErrorException((dwResult!=0),"converting host name",WSAGetLastError(),__LINE__,__FILE__);

		// Store address
		convertedDomainName.Load(*(sockaddr_in*)aiResult->ai_addr);

		NetAddress test1;
		NetAddress test2;
		test1 = test2;

		// Free memory allocated by getaddrinfo
		freeaddrinfo(aiResult);
	}
	catch(ErrorReport & error){	convertedDomainName.Leave(); throw; }
	catch(...){ convertedDomainName.Leave(); throw; }
	convertedDomainName.Leave();

	return convertedDomainName;
}

/**
 * @brief Determines the number of instances in the instance group stored by NetUtility.
 *
 * @return number of instances in instance group.
 */
size_t NetUtility::GetNumInstances()
{
	if(IsInstanceGroupLoaded() == false)
	{
		return 0;
	}
	else
	{
		return instanceGroup->GetNumInstances();
	}
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool NetUtility::TestClass()
{
	cout << "Testing NetUtility class...\n";
	// Note: not testing basic things tested by other classes e.g. SetupCompletionPort()

	cout << "Winsock active: " << IsWinsockActive() << '\n';

	cout << "Starting winsock..\n";
	StartWinsock();
	cout << "Winsock active: " << IsWinsockActive() << '\n';


	cout << "Loading local address information..\n";
	LoadLocalInfo();

	cout << "Host name: " << GetHostName() << '\n';

	cout << "Local address information: \n";
	for(size_t n = 0;n<GetNumLocalInterface();n++)
	{
		cout << n << ": " << GetLocalInterface(n).GetIP() << '\n';
	}

	cout << "Local information loaded: " << IsLocalInfoLoaded() << '\n';

	cout << "Google.com IP: " << ConvertDomainNameToIP("www.google.com").GetIP() << '\n';

	cout << "Unloading local address information..\n";
	UnloadLocalInfo();


	cout << "Ending winsock..\n";
	FinishWinsock();
	cout << "Winsock active: " << IsWinsockActive() << '\n';

	cout << "\n\n";
	return true;
}






