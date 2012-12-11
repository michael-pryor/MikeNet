#pragma once

/**
 * @brief	Stores instances, allowing safe shutdown of instance.
 * @remarks	Michael Pryor, 6/15/2010.
 *
 * The purposes of this instance are:
 * - To manage the life of a NetInstance object. An instance object can request it be destroyed.
 * This object will then safely destroy the instance from the main process. Similarly the instance can
 * be manually destroyed using KillInstance. IMPORTANT: Any thread must use ConcurrencyControl methods of
 * this class when accessing any part of the instance e.g. A returned NetInstanceTCP object. The main thread
 * does not need to do this however because it is the only thread that can shutdown the instance. The need
 * to follow this rule is avoided so long as before shutting down the instance only one thread is using the instance
 * (i.e. The thread that shuts down the instance).\n\n
 * - To access different types of instance safely. e.g. If you want to access UDP methods of an instance
 * you can use GetInstanceUDP but if that instance has no UDP methods an exception will be thrown.
 */
class NetInstanceContainer
{
	/** @brief Instance that container owns. */
	NetInstance * instance;

	NetInstance * _AccessInstance();
public:
	NetInstanceContainer(NetInstance * instance);
	NetInstanceContainer();
	~NetInstanceContainer();

	bool IsInstanceLoaded() const;
	bool KillInstance();
	void LoadInstance(NetInstance * instance);

	NetInstanceUDP * GetInstanceUDP();
	NetInstanceTCP * GetInstanceTCP();
	NetInstance * GetInstanceCore();
	NetInstanceServer * GetInstanceServer();
	NetInstanceClient * GetInstanceClient();
	NetInstanceBroadcast * GetInstanceBroadcast();

	/** @warning NetInstanceGroup should only use SetInstanceID and nothing else. */
	friend class NetInstanceGroup;
private:
	void SetInstanceID(size_t newID);

public:
	bool DealWithDestroyRequest();
};