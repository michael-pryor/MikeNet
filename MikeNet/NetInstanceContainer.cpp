#include "FullInclude.h"

/**
 * @brief Constructor, container is full with a loaded instance.
 *
 * @param [in] instance Pointer to instance that this object should manage.
 * This pointer and its data is now owned by this object and should not be used elsewhere.
 */
NetInstanceContainer::NetInstanceContainer(NetInstance * instance)
{
	this->instance = instance;
}

/**
 * @brief Constructor, container is empty with no loaded instance.
 */
NetInstanceContainer::NetInstanceContainer()
{
	this->instance = NULL;
}

/**
 * @brief Destructor.
 */
NetInstanceContainer::~NetInstanceContainer()
{
	KillInstance();
}

/**
 * @brief Determines whether an instance is loaded. Not thread safe.
 *
 * @warning This method is not thread safe, calling thread must have read
 * or write control while using this method unless that thread is the main process.
 *
 * @return true if an instance is loaded.
 */
bool NetInstanceContainer::IsInstanceLoaded() const
{
	return instance != NULL;
}

/**
 * @brief Destroy currently loaded instance.
 *
 * Method has no impact unless called by main process.
 *
 * @return true if instance was destroyed.
 * @return false if instance was not destroyed.
 */
bool NetInstanceContainer::KillInstance()
{
	// Will return NULL if calling thread is main process.
	if(ThreadSingle::IsThreadLocalStorageAllocated() == false || ThreadSingle::GetCallingThread() == NULL)
	{
		delete instance;
		instance = NULL;
		return true;
	}
	else
	{
		return false;
	}
}


/**
 * @brief Access currently loaded instance. Not thread safe.
 *
 * @warning This method is not thread safe, calling thread must have read.
 * or write control while using this method unless that thread is the main process.
 *
 * @return pointer to currently loaded instance.
 */
NetInstance * NetInstanceContainer::_AccessInstance()
{
	_ErrorException((IsInstanceLoaded() == false),"accessing an instance, no active instance is loaded",0,__LINE__,__FILE__);
	return instance;
}

/**
 * @brief Retrieves a NetInstanceUDP version of the stored instance. Not thread safe.
 *
 * @return instance as NetInstanceUDP object.
 *
 * @warning This method is not thread safe, calling thread must have read.
 * or write control while using this method unless that thread is the main process.
 *
 * @throws ErrorReport If instance is not of this type.
 */
NetInstanceUDP * NetInstanceContainer::GetInstanceUDP()
{
	NetInstanceUDP * instance = dynamic_cast<NetInstanceUDP*>(_AccessInstance());
	_ErrorException((instance == NULL),"retrieving a NetInstanceUDP object, instance is not of this type",0,__LINE__,__FILE__);
	return instance;
}

/**
 * @brief Retrieves a NetInstanceTCP version of the stored instance. Not thread safe.
 *
 * @return instance as NetInstanceTCP object.
 *
 * @warning This method is not thread safe, calling thread must have read.
 * or write control while using this method unless that thread is the main process.
 *
 * @throws ErrorReport If instance is not of this type.
 */
NetInstanceTCP * NetInstanceContainer::GetInstanceTCP()
{
	NetInstanceTCP * instance = dynamic_cast<NetInstanceTCP*>(_AccessInstance());
	_ErrorException((instance == NULL),"retrieving a NetInstanceTCP object, instance is not of this type",0,__LINE__,__FILE__);
	return instance;
}

/**
 * @brief Retrieves a NetInstanceServer version of the stored instance. Not thread safe.
 *
 * @return instance as NetInstanceServer object.
 *
 * @warning This method is not thread safe, calling thread must have read.
 * or write control while using this method unless that thread is the main process.
 *
 * @throws ErrorReport If instance is not of this type.
 */
NetInstanceServer * NetInstanceContainer::GetInstanceServer()
{
	NetInstanceServer * instance = dynamic_cast<NetInstanceServer*>(_AccessInstance());
	_ErrorException((instance == NULL),"retrieving a NetInstanceServer object, instance is not of this type",0,__LINE__,__FILE__);
	return instance;
}

/**
 * @brief Retrieves a NetInstanceClient version of the stored instance. Not thread safe.
 *
 * @return instance as NetInstanceClient object.
 *
 * @warning This method is not thread safe, calling thread must have read.
 * or write control while using this method unless that thread is the main process.
 *
 * @throws ErrorReport If instance is not of this type.
 */
NetInstanceClient * NetInstanceContainer::GetInstanceClient()
{
	NetInstanceClient * instance = dynamic_cast<NetInstanceClient*>(_AccessInstance());
	_ErrorException((instance == NULL),"retrieving a NetInstanceClient object, instance is not of this type",0,__LINE__,__FILE__);
	return instance;
}

/**
 * @brief Retrieves a NetInstanceBroadcast version of the stored instance. Not thread safe.
 *
 * @return instance as NetInstanceBroadcast object.
 *
 * @warning This method is not thread safe, calling thread must have read.
 * or write control while using this method unless that thread is the main process.
 *
 * @throws ErrorReport If instance is not of this type.
 */
NetInstanceBroadcast * NetInstanceContainer::GetInstanceBroadcast()
{
	NetInstanceBroadcast * instance = dynamic_cast<NetInstanceBroadcast*>(_AccessInstance());
	_ErrorException((instance == NULL),"retrieving a NetInstanceClient object, instance is not of this type",0,__LINE__,__FILE__);
	return instance;
}

/**
 * @brief Retrieves a the stored NetInstance object. Not thread safe.
 *
 * @return stored instance.
 *
 * @warning This method is not thread safe, calling thread must have read.
 * or write control while using this method unless that thread is the main process.
 *
 * @throws ErrorReport If instance is not of this type.
 */
NetInstance * NetInstanceContainer::GetInstanceCore()
{
	return _AccessInstance();
}

/**
 * @brief Loads an instance into this container.
 *
 * @param [in] instance Instance to load. This pointer and its data is now owned
 * by this object and should not be used elsewhere.
 *
 * @throws ErrorReport If an instance is already loaded.
 */
void NetInstanceContainer::LoadInstance(NetInstance * instance)
{
	_ErrorException((IsInstanceLoaded() == true),"loading an instance, an instance is already loaded",0,__LINE__,__FILE__);
	this->instance = instance;
}

/**
 * @brief For use only by NetInstanceGroup (via friendship) to set instanceID during initialization.
 *
 * @param newID New instance ID.
 */
void NetInstanceContainer::SetInstanceID(size_t newID)
{
	instance->SetInstanceID(newID);
}


/**
 * @brief Cleans up instance if instance requests this using NetInstance::RequestDestroy.
 *
 * @return true if instance was cleaned up, false if not.
 */
bool NetInstanceContainer::DealWithDestroyRequest()
{
	if(instance->GetDestroyRequest() == true)
	{
		return KillInstance();
	}
	else
	{
		return false;
	}
}