#pragma once

/**
 * @brief	Global UPnP NAT methods and variables.
 * @remarks	Michael Pryor, 9/17/2010. 
 */
class UpnpNatUtility
{
	/**
	 * @brief Controls UPnP NAT.
	 */
	static UpnpNatCommunication * control;
public:
	static void Start();
	static void Finish();
	static bool IsRunning();

	static UpnpNatCommunication * GetControl();
};

