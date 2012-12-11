/**
 * @brief	Manages a group of instances.
 * @remarks	Michael Pryor, 6/5/2010. 
 * 
 * This instance is used for managing a collection of instances.
 * These instances can be identified by ID (unique only within this instance group).\n\n
 *
 * This class is thread safe; however, instances can only be 'finished' by the main process.
 */
class NetInstanceGroup
{
	/**
	 * @brief Thread safe vector of instance containers, these are the members of this instance group.
	 */
	StoreVector<NetInstanceContainer> instance;

	void ValidateInstanceID(size_t instanceID, size_t line, const char * file) const;
public:
	NetInstanceGroup(size_t numInstances);
	void Finish(size_t instanceID);
	void FinishAll();
	bool GetInstanceActive(size_t instanceID) const;

	size_t AddInstance(NetInstance * instance);
	void AddInstance(size_t instanceID, NetInstance * instance);
	NetInstanceContainer & GetInstance(size_t instanceID);
	const NetInstanceContainer & GetInstance(size_t instanceID) const;
	NetInstanceContainer & operator[](size_t instanceID);
	const NetInstanceContainer & operator[](size_t instanceID) const;
	size_t GetNumInstances() const;
};