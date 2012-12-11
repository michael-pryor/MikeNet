#include "FullInclude.h"
#include "ComUtility.h"

/**
 * @brief	Initializes COM.
 *
 * @return	true if initialization occurred, false if COM was already initialized.
 */
bool ComUtility::Initialize()
{
	bool returnMe = false;

	// Note: return value of S_FALSE means that COM has already been initialized on this thread
	HRESULT hResult = CoInitialize(NULL);
	if(hResult != S_FALSE)
	{
		returnMe = true;
		_ErrorException(((hResult != S_OK) && (hResult != RPC_E_CHANGED_MODE)), "initializing COM", hResult, __LINE__,__FILE__);
	}
	return(returnMe);
}

/**
 * @brief	Cleanups up COM.
 */
void ComUtility::Cleanup()
{
	CoUninitialize();
}

/**
 * @brief	Converts from VARIANT_BOOL to bool.
 *
 * @param	vBool	Boolean to convert. 
 *
 * @return	converted boolean.
 */
bool ComUtility::ConvertBoolean(VARIANT_BOOL vBool)
{
	return vBool == VARIANT_TRUE;
}

/**
 * @brief	Converts from bool to VARIANT_BOOL.
 *
 * @param	cBool	Boolean to convert. 
 *
 * @return	converted boolean.
 */
VARIANT_BOOL ComUtility::ConvertBoolean(bool cBool)
{
	VARIANT_BOOL vBool;
	if(cBool == true)
	{
		vBool = VARIANT_TRUE;
	}
	else
	{
		vBool = VARIANT_FALSE;
	}
	return vBool;
}
