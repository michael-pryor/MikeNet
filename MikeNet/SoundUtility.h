#pragma once
#include "SoundInstanceInput.h"
#include "SoundInstanceOutput.h"
#include "SoundDeviceInput.h"
#include "SoundDeviceOutput.h"

/**
 * @brief	Global sound input/output methods and variables.
 * @remarks	Michael Pryor, 9/17/2010. 
 */
class SoundUtility
{
	/**
	 * @brief Vector of input instances.
	 *
	 * Could not use StoreVector due to possible compiler bug.
	 */
	static vector<SoundInstanceInput*> inputInstances;

	/**
	 * @brief Vector of output instances.
	 *
	 * Could not use StoreVector due to possible compiler bug.
	 */
	static vector<SoundInstanceOutput*> outputInstances;

	static void ValidateInputInstanceID(size_t ID);
	static void ValidateOutputInstanceID(size_t ID);
public:
	static void Start(size_t numInputInstance, size_t numOutputInstances);
	static void Finish();

	static void AddInputInstance(size_t ID, SoundInstanceInput * instance);
	static void AddOutputInstance(size_t ID, SoundInstanceOutput * output);
	static SoundInstanceInput * GetInputInstance(size_t ID);
	static SoundInstanceOutput * GetOutputInstance(size_t ID);
	static void CleanupInputInstance(size_t ID);
	static void CleanupOutputInstance(size_t ID);

	static size_t GetNumInputInstances();
	static size_t GetNumOutputInstances();
};

