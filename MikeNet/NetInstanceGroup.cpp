#include "FullInclude.h"

/**
 * @brief Constructor.
 *
 * Creates @a numInstances number of empty instance containers,
 * containing no instances. These are then filled with 
 * instance objects later on.
 *
 * @param numInstances Number of instances to store.
 */
NetInstanceGroup::NetInstanceGroup(size_t numInstances) : instance()
{
	instance.ResizeAllocate(numInstances);
}

/**
 * @brief Validates instance ID and throws an exception if the ID is out of range.
 *
 * @param instanceID Instance number to check.
 * @param line Line number of method call.
 * @param [in] file File of method call.
 *
 * @throws ErrorReport If @a instanceID is invalid, must be between 0 (inclusive) and the number of instances (exclusive).
 */
void NetInstanceGroup::ValidateInstanceID(size_t instanceID, size_t line, const char * file) const
{
	_ErrorException((instanceID >= instance.Size()),"performing an instance related function. Invalid instance specified",0,line,file);
}

/**
 * @brief Cleans up all instances in group.
 *
 * Instance container remains in vector, but has the instance it contains unloaded.\n\n
 *
 * No impact unless calling thread is main process.
 */
void NetInstanceGroup::FinishAll()
{
	// Will return NULL if main process is calling.
	if(ThreadSingle::GetCallingThread() == NULL)
	{
		// Take control here to stop instance.Size() changing
		// mid way through method execution.
		//
		// It is okay to pass a readCount of 0 to EnterWrite
		// because all control is released before any method
		// returns in this class.
		instance.Enter();

		try
		{
			for(size_t n = 0;n<instance.Size();n++)
			{
				instance[n].KillInstance();
			}
		}
		// Release control of objects before throwing final exception
		catch (ErrorReport & error){this->instance.Leave(); throw(error);}
		catch(...){this->instance.Leave(); throw(-1);}

		instance.Leave();
	}
}

/**
 * @brief Cleans up specified instance.
 *
 * Instance container remains in vector, but has the instance it contains unloaded.
 *
 * @param instanceID ID of instance to cleanup.
 */
void NetInstanceGroup::Finish(size_t instanceID)
{
	ValidateInstanceID(instanceID,__LINE__,__FILE__);
	instance[instanceID].KillInstance();
}

/**
 * @brief Determines whether an instance is active.
 *
 * @param instanceID ID of instance to check.
 * @return true if instance is active, false if instance is inactive.
 */
bool NetInstanceGroup::GetInstanceActive(size_t instanceID) const
{
	ValidateInstanceID(instanceID,__LINE__,__FILE__);
	return instance[instanceID].IsInstanceLoaded();
}

/**
 * @brief Adds an instance to the group, increasing the group size by 1.
 *
 * @param [in] newInstance Instance to add. This pointer and its data is now owned by
 * this object and should not be used elsewhere.
 * @return new instance ID.
 */
size_t NetInstanceGroup::AddInstance(NetInstance * newInstance)
{
	// Take control here rather than relying
	// on in built controls in methods we call
	// in case after instance.Add() instance.Size
	// changes further due to calls by another
	// thread.
	this->instance.Enter();
	size_t id;
	try
	{
		NetInstanceContainer * container = new (nothrow) NetInstanceContainer(newInstance);
		Utility::DynamicAllocCheck(container,__LINE__,__FILE__);

		this->instance.Add(container);

		id = this->instance.Size()-1;
		container->SetInstanceID(id);
	}
	// Release control of objects before throwing final exception
	catch (ErrorReport & error){this->instance.Leave(); throw(error);}
	catch(...){this->instance.Leave(); throw(-1);}

	this->instance.Leave();

	return id;
}

/**
 * @brief Add an instance to the group at the specified ID.
 * 
 * @param instanceID Instance ID that @a instance should occupy.
 * @param [in] instance Pointer to instance that should fill @a instanceID, this pointer and its data is now
 * owned by this object and should not be used elsewhere.
 *
 * @throws ErrorReport If instance is already active at specified instance ID.
 */
void NetInstanceGroup::AddInstance(size_t instanceID, NetInstance * instance)
{
	this->instance[instanceID].LoadInstance(instance);
	this->instance[instanceID].SetInstanceID(instanceID);
}

/**
 * @brief Retrieves a mutable reference to the specified instance container.
 *
 * @return mutable reference to specified instance container.
 * @param instanceID ID of instance container to retrieve.
 */
NetInstanceContainer & NetInstanceGroup::GetInstance(size_t instanceID)
{
	_ErrorException((GetInstanceActive(instanceID) == false),"retrieving an instance object from a group, instance is no longer active",0,__LINE__,__FILE__);
	return instance[instanceID];
}

/**
 * @brief Retrieves a constant reference to the specified instance container.
 *
 * @return constant reference to specified instance container.
 * @param instanceID ID of instance container to retrieve.
 */
const NetInstanceContainer & NetInstanceGroup::GetInstance(size_t instanceID) const
{
	_ErrorException((GetInstanceActive(instanceID) == false),"retrieving an instance object from a group, instance is no longer active",0,__LINE__,__FILE__);
	return instance[instanceID];
}

/**
 * @brief Retrieves a mutable reference to the specified instance container.
 *
 * @return mutable reference to specified instance container.
 * @param instanceID ID of instance container to retrieve.
 */
NetInstanceContainer & NetInstanceGroup::operator[](size_t instanceID)
{
	return GetInstance(instanceID);
}

/**
 * @brief Retrieves a constant reference to the specified instance container.
 *
 * @return constant reference to specified instance container.
 * @param instanceID ID of instance container to retrieve.
 */
const NetInstanceContainer & NetInstanceGroup::operator[](size_t instanceID) const
{
	return GetInstance(instanceID);
}

/**
 * @brief Retrieves the number of instances stored in this group.
 *
 * @return the number of instances in instance group,
 * these instances can be inactive or active, this method does not differentiate.
 */
size_t NetInstanceGroup::GetNumInstances() const
{
	return instance.Size();
}
