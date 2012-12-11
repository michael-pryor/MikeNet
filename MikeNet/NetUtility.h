#pragma once
#include "CompletionPort.h"
#include "StoreVector.h"
class NetInstanceGroup;
/**
 * @brief	Global networking methods and variables.
 * @author	Michael Pryor, 4/19/2010.
 *
 * Used throughout the networking module.
 */
class NetUtility
{
public:
	/**
	 * @brief Values used to indicate error.
	 */
	enum Error
	{
		/**
		 * Attempted a networking operation but failed, used where the return value indicates error and is signed.
		 * This is the most common error return value. \n
		 * Value of -1.
		 */
		NET_ERROR = -1,

		/**
		 * Attempted a networking operation but failed, used where the return value indicates error and is unsigned.\n
		 * Value of 0.
		 */
		UNSIGNED_NET_ERROR = 0,
	};

	/**
	 * @brief Describes the stage that a connection is in.
	 */
	enum ConnectionStatus
	{
		/**
		 * Connection was refused. \n
		 * Value of -2.
		 */
		REFUSED = -2,

		/**
		 * Error occurred whilst connecting. \n
		 * Value of -1.
		 */
		CONNECTION_ERROR = -1,
		
		/**
		 * Connection timed out, i.e. Did not complete within required amount of time. \n
		 * Value of 0.
		 */
		TIMED_OUT = 0,

		/**
		 * Not connected at all i.e. inactive.\n
		 * Value of 0.
		 */
		NOT_CONNECTED = 0,

		/**
		 * Fully connected i.e. Active. \n
		 * Value of 1.
		 */
		CONNECTED = 1,

		/**
		 * Connection attempt is still in progress. \n
		 * Value of 2.
		 */
		STILL_CONNECTING = 2,

		/**
		 * The graceful disconnection process is in progress and data may no longer be sent via TCP
		 * but may still be received. Other protocols are unaffected. This status is received when
		 * the connection has been shut down; see @ref gracefulDisconnectPage "graceful disconnecting"
		 * for more information. \n
		 * Value of 2. 
		 */
		NO_SEND = 2,

		/**
		 * The graceful disconnection process is in progress and data may no longer be received via TCP
		 * but may still be sent. Other protocols are unaffected. This status is received when
		 * the connection has been shut down by the other participant; see @ref gracefulDisconnectPage "graceful disconnecting"
		 * for more information. \n
		 * Value of 3. 
		 */
		NO_RECV = 3,

		/**
		 * The graceful disconnection process is in progress and data may no longer be sent or received via TCP.
		 * Other protocols are unaffected. This status is received when the connection has been shut down by both participants
		 * but there are packets left in the receive queue; see @ref gracefulDisconnectPage "graceful disconnecting"
		 * for more information. \n
		 * Value of 4. 
		 */
		NO_SEND_RECV = 4,

		/**
		 * The connection process is in progress.
		 * Value of 5.
		 */
		CONNECTING = 5,

		/**
		 * The connection has completed but confirmation is needed before the connection becomes fully active. \n
		 * Value of 6.
		 */
		CONNECTED_AC = 6,

		/**
		 * The connection is still active but a request has been made for it to be terminated. \n
		 * Value of 7.
		 */
		DISCONNECTING = 7,
	};

	/**
	 * @brief Describes the result of a send operation.
	 */
	enum SendStatus
	{
		/**
		 * The send operation completed successfully and the data has now been sent. \n
		 * Value of 0.
		 */
		SEND_COMPLETED = 0,

		/**
		 * The send operation was started, but has not yet completed.\n
		 * Value of 1.
		 */
		SEND_IN_PROGRESS = 1,

		/**
		 * The send operation failed, but not catastrophically, so the entity does not need to be killed.\n
		 * Value of -1.
		 */
		SEND_FAILED = -1,

		/**
		 * The send operation failed catastrophically, so the entity should be killed.
		 * Value of -2.
		 */
		SEND_FAILED_KILL = -2,
	};
	
private:
	/** @brief Completion port object to manage the completion port and associated threads. */
	static CompletionPort * completionPort;

	/** @brief Each bit represents the affinity of a thread where bit 0 is thread 0. */
	static DWORD_PTR threadAffinity;

	/** @brief Name of computer. */
	static char * hostName;

	/** @brief Available local interfaces. */
	static StoreVector<NetAddress> localInterface;

	/** @brief Group of instances controlled by mn namespace. */
	static NetInstanceGroup * instanceGroup;


	/** @brief True if winsock has been started using StartWinsock(). */
	static bool winsockActive;

	/** @brief True if local information has been loaded using LoadLocalInfo(). */
	static bool localInfoLoaded;

	/** @brief Sizeof(SOCKADDR) some winsock methods require a pointer to this. */
	static int sizeSOCKADDR;
public:
	/** @brief Max length that computer name can be, longer computer names will probably be truncated. */
	const static unsigned int maxComputerNameSize = 1024;

	/** @brief Level of UDP authentication used by instances. */
	const static unsigned int authenticationStrength = 4;

	/**
	 * @brief Used during encryption/decryption.
	 *
	 * Used with Packet::_SubBytes which replaces each byte with one from this table.
	 */
	const static unsigned char EncryptionBox[256];

	/**
	 * @brief Used during encryption/decryption. 
	 *
	 * Used with Packet::_InvSubBytes which replaces each byte with one from this table.
	 */
	const static unsigned char InverseEncryptionBox[256];

	/**
	 * @brief Used during encryption/decryption.
	 *
	 * The round constant word array, EncryptionRoundConstant[i], contains the values given by 
	 * x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(28). \n
	 * Note that i starts at 1, not 0.
	 */
	const static unsigned char EncryptionRoundConstant[255];

private:
	/**
	 * @brief Stores the result of ConvertDomainNameToIP().
	 */
	static NetAddress convertedDomainName;


	static void ValidateThreadID(size_t threadID, size_t line, const char * file);
public:
	static int * GetSizeSOCKADDR();
	
	static void StartWinsock();
	static void FinishWinsock();
	static bool IsWinsockActive();

	static NetInstanceGroup & GetInstanceGroup();
	static void CreateInstanceGroup(size_t numInstances);
	static void DestroyInstanceGroup();
	static bool IsInstanceGroupLoaded();
	static size_t GetNumInstances();

	static size_t GetMainProcessThreadID();
	static size_t GetNumThreads();
	static size_t GetNumThreadedParticipants();

	static void SetupCompletionPort(size_t numThreads);
	static void DestroyCompletionPort();
	static bool IsCompletionPortSetup();
	static void AssociateWithCompletionPort(HANDLE object, const CompletionKey & key);

	static const char * GetHostName();
	static const NetAddress & GetLocalInterface(size_t ele);
	static size_t GetNumLocalInterface();
	static void LoadLocalInfo();
	static void UnloadLocalInfo();
	static bool IsLocalInfoLoaded();

	static void LoadEverything(size_t numThreads, size_t numInstances);
	static void UnloadEverything();
	static bool IsEverythingLoaded();

	static const NetAddress & ConvertDomainNameToIP(const char * str);

	static bool TestClass();
};
