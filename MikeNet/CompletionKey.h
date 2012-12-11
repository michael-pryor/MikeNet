#pragma once
class NetSocket;
class NetInstance;

/**
 * @brief	Completion key used by the completion port to identify instances, sockets and other objects.
 * @remarks	Michael Pryor, 6/28/2010.
 *
 * When queued events complete they are often passed to the completion port. If they are,
 * they must be identified. To identify them the completion port is given a CompletionKey object
 * and from this it can retrieve the entity that initiated the event and pass it relevant information
 * about the event now that it has completed.
 */
class CompletionKey
{
public:
	/** @brief Types of completion key objects, indicating what their purpose is. */
	enum KeyType
	{
		/** Completed operation was initiated by a socket. */
		SOCKET, 

		/** Completion port threads should shut down. */
		SHUTDOWN
	};
private:

	/** @brief Pointer to socket, not NULL only if this object belongs to a socket. */
	NetSocket * socket;

	/** @brief Pointer to an instance, not NULL only if this object belongs to an instance. */
	NetInstance * instance;

	/** @brief ID of client within instance that this object belongs to. */
	size_t clientID;

	/** @brief Pointer to any object, for additional custom data storage. */
	void * customObject;

	/** @brief Purpose of this completion key. */
	KeyType type;

public:
	/** @brief Completion key to be posted when the completion port is being shut down. */
	static const CompletionKey shutdownKey;

private:
	void Copy(const CompletionKey & copyMe);
public:
	CompletionKey(const CompletionKey & copyMe);
	CompletionKey & operator=(const CompletionKey & copyMe);
	bool operator==(const CompletionKey & compare) const;
	bool operator!=(const CompletionKey & compare) const;

	CompletionKey(KeyType type, NetSocket * socket = NULL, NetInstance * instance = NULL, size_t clientID = NULL);

	NetSocket * GetSocket();
	NetInstance * GetInstance();
	void * GetCustomObject();
	KeyType GetType() const;
	size_t GetClientID() const;

	void SetInstance(NetInstance * instance);
	void SetClientID(size_t clientID);
	bool IsOwnedByInstance() const;

	static bool TestClass();

};