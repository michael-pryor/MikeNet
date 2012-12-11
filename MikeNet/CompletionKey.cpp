// This class is used to give information to the completion port about a completed operation
#include "FullInclude.h"
class NetSocket;

const CompletionKey CompletionKey::shutdownKey(CompletionKey::SHUTDOWN);

/**
 * @brief Constructor where an instance, socket and client are involved.
 *
 * @param type Purpose of completion key.
 * @param [in] socket Pointer to socket that owns this object (optional, default = NULL).
 * @param [in] instance Pointer to instance that owns this object (optional, default = NULL).
 * @param clientID ID of client that owns this object (optional, default = NULL).
 */
CompletionKey::CompletionKey(KeyType type, NetSocket * socket, NetInstance * instance, size_t clientID)
{
	this->socket = socket;
	this->instance = instance;
	this->customObject = NULL;
	this->type = type;
	this->clientID = clientID;
}

/**
 * @brief	Copy constructor / Assignment operator helper method.
 *
 * Nothing is cleaned up by this method.
 *
 * @param	copyMe	Object to copy into this object.
 */
void CompletionKey::Copy(const CompletionKey & copyMe)
{
	this->socket = copyMe.socket;
	this->instance = copyMe.instance;
	this->clientID = copyMe.clientID;
	this->customObject = copyMe.customObject;
	this->type = copyMe.type;
}

/**
 * @brief Copy constructor.
 *
 * Copies @a copyMe into this object.
 * @param copyMe Object to copy.
 */
CompletionKey::CompletionKey(const CompletionKey & copyMe)
{
	Copy(copyMe);
}

/**
 * @brief Assignment operator.
 *
 * Copies @a copyMe into this object.
 *
 * @param copyMe Object to copy.
 *
 * @return reference to this object.
 */
CompletionKey & CompletionKey::operator=(const CompletionKey & copyMe)
{
	Copy(copyMe);

	return *this;
}

/**
 * @brief	Equality operator. 
 *
 * @param	compare	Object to compare with this object.
 *
 * @return	true if @a compare is identical to this object. 
 */
bool CompletionKey::operator==(const CompletionKey & compare) const
{
	return this->socket == compare.socket &&
		   this->instance == compare.instance &&
		   this->clientID == compare.clientID &&
		   this->customObject == compare.customObject &&
		   this->type == compare.type;
}

/**
 * @brief	Inequality operator. 
 *
 * @param	compare	Object to compare with this object.
 *
 * @return	false if @a compare is identical to this object. 
 */
bool CompletionKey::operator!=(const CompletionKey & compare) const
{
	return !(*this == compare);
}

/**
 * @brief Retrieves pointer to socket that owns this object.
 *
 * @return pointer to socket, if NULL then no object was loaded.
 */
NetSocket * CompletionKey::GetSocket()
{
	return socket;
}

/**
 * @brief Retrieves pointer to instance that owns this object.
 *
 * @return pointer to instance, if NULL then no object was loaded.
 */
NetInstance * CompletionKey::GetInstance()
{
	return instance;
}

/**
 * @brief Retrieves custom object that relates to this object.
 *
 * @return pointer to object of any type, if NULL then no object was loaded.
 */
void * CompletionKey::GetCustomObject()
{
	return customObject;
}

/**
 * @brief Retrieves the purpose of this object.
 * 
 * @return purpose of this completion key.
 */
CompletionKey::KeyType CompletionKey::GetType() const
{
	return type;
}

/**
 * @brief Retrieves the client ID of the client that owns this object.
 *
 * @return client ID of client within instance that owns this object.
 */
size_t CompletionKey::GetClientID() const
{
	return clientID;
}

/**
 * @brief Sets instance field, can only be done if the instance field is currently empty.
 *
 * @param [in] instance Instance object that owns this object.
 */
void CompletionKey::SetInstance(NetInstance * instance)
{
	this->instance = instance;
}

/**
 * @brief Sets client ID field, can only be done if the client ID field is currently empty.
 *
 * @param clientID ClientID of client that owns this object.
 */
void CompletionKey::SetClientID(size_t clientID)
{
	_ErrorException((this->clientID != 0),"setting a completion key's client ID field, client ID has already been set",0,__LINE__,__FILE__);
	this->clientID = clientID;
}

/**
 * @brief Determine if this object is owned by an instance.
 *
 * @return true if the completion key contains a pointer to an instance.
 */
bool CompletionKey::IsOwnedByInstance() const
{
	return instance != NULL;
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool CompletionKey::TestClass()
{
	cout << "Testing CompletionKey class...\n";

	bool problem = false;
	CompletionKey key(CompletionKey::SOCKET,(NetSocket*)1000,(NetInstance*)1001,1002);

	// Verify type.
	if(key.GetType() == CompletionKey::SOCKET)
	{
		cout << "Type is good";
	}
	else
	{
		cout << "Type is not good";
		problem = true;
	}
	cout << "\n";

	// Verify socket address.
	if(key.GetSocket() == (NetSocket*)1000)
	{
		cout << "Socket address is good";
	}
	else
	{
		cout << "Socket address is not good";
		problem = true;
	}
	cout << "\n";

	// Verify instance address.
	if(key.GetInstance() == (NetInstance*)1001)
	{
		cout << "Instance address is good";
	}
	else
	{
		cout << "Instance address is not good";
		problem = true;
	}
	cout << "\n";

	// Verify client ID.
	if(key.GetClientID() == 1002)
	{
		cout << "Client ID is good";
	}
	else
	{
		cout << "Client ID is not good";
		problem = true;
	}
	cout << "\n";

	// Copy constructor
	CompletionKey copy(key);
	if( copy != key )
	{
		cout << "Copy constructor or equality operator is bad";
		problem = true;
	}
	else
	{
		cout << "Copy constructor and equality operator are good";
	}
	cout << "\n";

	// Assignment operator
	CompletionKey another(CompletionKey::SHUTDOWN);
	copy = another;
	if( copy != another )
	{
		cout << "Assignment operator or equality operator is bad";
		problem = true;
	}
	else
	{
		cout << "Assignment operator and equality operator are good";
	}
	cout << "\n";

	// Test IsOwnedByInstance
	if(another.IsOwnedByInstance() == false)
	{
		cout << "IsOwnedByInstance is good";
	}
	else
	{
		cout << "IsOwnedByInstance is bad";
		problem = true;
	}
	cout << "\n";
	another.SetInstance((NetInstance*)1000);

	if(another.IsOwnedByInstance() == true)
	{
		cout << "IsOwnedByInstance is good";
	}
	else
	{
		cout << "IsOwnedByInstance is bad";
		problem = true;
	}
	cout << "\n";
	
	cout << "\n\n";
	return !problem;
}
