#include "FullInclude.h"
class SoundInstanceInput;
class SoundInstanceOutput;

vector<SoundInstanceInput*> SoundUtility::inputInstances;
vector<SoundInstanceOutput*> SoundUtility::outputInstances;

/**
 * @brief	Starts up the sound input / output module.
 *
 * @param	numInputInstances	Number of input instances. 
 * @param	numOutputInstances	Number of output instances. 
 */
void SoundUtility::Start( size_t numInputInstances, size_t numOutputInstances )
{
	Finish();

	inputInstances.resize(numInputInstances,NULL);
	outputInstances.resize(numOutputInstances,NULL);
}

/**
 * @brief	Shuts down the sound input / output module.
 */
void SoundUtility::Finish()
{
	for(size_t n = 0;n<inputInstances.size();n++)
	{
		delete inputInstances[n];
		inputInstances[n] = NULL;
	}

	for(size_t n = 0;n<outputInstances.size();n++)
	{
		delete outputInstances[n];
		outputInstances[n] = NULL;
	}
}

/**
 * @brief	Validates the specified input instance ID.
 *
 * @param	ID	The identifier to check.
 *
 * @throws clError If the ID is invalid.
 */
void SoundUtility::ValidateInputInstanceID( size_t ID )
{
	_ErrorException((ID >= inputInstances.size()),"performing an input instance related operation, invalid instance ID",0,__LINE__,__FILE__);
}

/**
 * @brief	Validates the specified output instance ID.
 *
 * @param	ID	The identifier to check.
 *
 * @throws clError If the ID is invalid.
 */
void SoundUtility::ValidateOutputInstanceID( size_t ID )
{
	_ErrorException((ID >= outputInstances.size()),"performing an output instance related operation, invalid instance ID",0,__LINE__,__FILE__);
}

/**
 * @brief	Adds an input instance.
 *
 * @param	ID					The identifier to assign to the instance. 
 * @param [in]		instance	The instance to use. This object is now owned by this class and should not be referenced elsewhere.
 */
void SoundUtility::AddInputInstance( size_t ID, SoundInstanceInput * instance )
{
	_ErrorException((instance == NULL),"adding an input instance, instance parameter is NULL",0,__LINE__,__FILE__);
	ValidateInputInstanceID(ID);
	CleanupInputInstance(ID);
	inputInstances[ID] = instance;
}

/**
 * @brief	Adds an output instance.
 *
 * @param	ID					The identifier to assign to the instance. 
 * @param [in]		instance	The instance to use. This object is now owned by this class and should not be referenced elsewhere.
 */
void SoundUtility::AddOutputInstance( size_t ID, SoundInstanceOutput * instance )
{
	_ErrorException((instance == NULL),"adding an output instance, instance parameter is NULL",0,__LINE__,__FILE__);
	ValidateOutputInstanceID(ID);
	CleanupOutputInstance(ID);
	outputInstances[ID] = instance;
}

/**
 * @brief	Retrieves an input instance. 
 *
 * @param	ID	The identifier. 
 *
 * @return	input instance.
 */
SoundInstanceInput * SoundUtility::GetInputInstance( size_t ID )
{
	ValidateInputInstanceID(ID);
	_ErrorException(inputInstances[ID] == NULL,"performing an input instance related operation, instance is not active",0,__LINE__,__FILE__);
	return inputInstances[ID];
}

/**
 * @brief	Retrieves an output instance.
 *
 * @param	ID	The identifier. 
 *
 * @return	output instance.
 */
SoundInstanceOutput * SoundUtility::GetOutputInstance( size_t ID )
{
	ValidateOutputInstanceID(ID);
	_ErrorException(outputInstances[ID] == NULL,"performing an output instance related operation, instance is not active",0,__LINE__,__FILE__);
	return outputInstances[ID];
}

/**
 * @brief	Retrieves the number of input instances created using SoundUtility::Start.
 *
 * @return	the number of input instances created. 
 */
size_t SoundUtility::GetNumInputInstances()
{
	return inputInstances.size();
}

/**
 * @brief	Retrieves the number of input instances created using SoundUtility::Start.
 *
 * @return	the number of output instances created. 
 */
size_t SoundUtility::GetNumOutputInstances()
{
	return outputInstances.size();
}

/**
 * @brief	Cleans up an input instance. 
 *
 * @param	ID	The identifier. 
 */
void SoundUtility::CleanupInputInstance( size_t ID )
{
	ValidateInputInstanceID(ID);
	delete inputInstances[ID];
	inputInstances[ID] = NULL;
}

/**
 * @brief	Cleans up an output instance. 
 *
 * @param	ID	The identifier. 
 */
void SoundUtility::CleanupOutputInstance( size_t ID )
{
	ValidateOutputInstanceID(ID);
	delete outputInstances[ID];
	outputInstances[ID] = NULL;
}

