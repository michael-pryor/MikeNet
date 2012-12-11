#include "FullInclude.h"

UpnpNatCommunication * UpnpNatUtility::control(NULL);

/**
 * @brief	Initializes module.
 */
void UpnpNatUtility::Start()
{
	Finish();
	control = new (nothrow) UpnpNatCommunication();
	Utility::DynamicAllocCheck(control,__LINE__,__FILE__);
}

/**
 * @brief	Cleans up module.
 */
void UpnpNatUtility::Finish()
{
	delete control;
	control = NULL;
}

/**
 * @brief	Determine whether the module is running.
 *
 * @return	true if the module is running, false if not.
 */
bool UpnpNatUtility::IsRunning()
{
	return control != NULL;
}

/**
 * @brief	Retrieves NAT object.
 *
 * @return	NAT object.
 * @throws ErrorReport If NAT is not active.
 */
UpnpNatCommunication * UpnpNatUtility::GetControl()
{
	_ErrorException((!IsRunning()),"performing a NAT operation, NAT is not operational",0,__LINE__,__FILE__);
	return control;
}
