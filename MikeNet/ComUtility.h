#pragma once

/**
 * @brief	Common COM operations.
 * @remarks	Michael Pryor, 10/16/2010.
 */
namespace ComUtility
{
	bool Initialize();
	void Cleanup();
	

	VARIANT_BOOL ConvertBoolean(bool cBool);
	bool ConvertBoolean(VARIANT_BOOL vBool);
};

