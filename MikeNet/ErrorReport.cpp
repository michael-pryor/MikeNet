#include "FullInclude.h"
//#include "CLR_HelperFunctions.h"

ConcurrencyEvent ErrorReport::saveErrorLoaded(false);
ErrorReport ErrorReport::saveError;
CriticalSection ErrorReport::ctSaveError;
ConcurrentObject<int> ErrorReport::errorMode(ErrorReport::DEFAULT_ERROR_MODE);

/**
 * @brief	Set all variables to default.
 */
void ErrorReport::DefaultVariables()
{
	operation = NULL;
	command = NULL;
	fileName = NULL;
	errorCode = 0;
	lineNumber = 0;
}

/**
 * @brief	Default constructor. 
 */
ErrorReport::ErrorReport()
{
	DefaultVariables();
}

/**
 * @brief	Copy constructor / assignment operator helper method. 
 *
 * @param	copyMe	Object to copy. 
 */
void ErrorReport::Copy(const ErrorReport & copyMe)
{
	operation = copyMe.operation;
	command = copyMe.command;
	errorCode = copyMe.errorCode;
	lineNumber = copyMe.lineNumber;
	fileName = copyMe.fileName;
}

/**
 * @brief	Deep copy constructor.
 *
 * @param	copyMe	Object to copy. 
 */
ErrorReport::ErrorReport(const ErrorReport & copyMe)
{
	Copy(copyMe);
}

/**
 * @brief	Deep assignment operator. 
 *
 * @param	copyMe	Object to copy. 
 *
 * @return	a reference to this object.
 */
ErrorReport & ErrorReport::operator= (const ErrorReport & copyMe)
{
	Copy(copyMe);

	return(*this);
}

/**
 * @brief	Destructor. 
 */
ErrorReport::~ErrorReport()
{

}

/**
 * @brief	Loads error information.
 *
 * @param	operation	The operation that was taking place at the time of the error. 
 * @param	errorCode	The error code associated with the error.
 * @param	lineNumber	The line number at which the error occurred.
 * @param	fileName	The name of the file in which the error occurred.
 * @param	command		The command that caused the error (optional, default = NULL).
 */
void ErrorReport::LoadReport(const char * operation, __int64 errorCode, unsigned __int64 lineNumber, const char * fileName, const char * command)
{
	this->operation = operation;
	this->errorCode = errorCode;
	this->lineNumber = lineNumber;
	this->fileName = fileName;
	this->command = command;
}

/**
 * @brief	Sets the command that caused the error.
 *
 * @param	command The command that caused the error.
 */
void ErrorReport::SetCommand(const char * command)
{
	this->command = command;
}

/**
 * @brief	Generates an error message based on stored information.
 *
 * @return	an error message. When done with this string you should deallocate it using delete[].
 */
char * ErrorReport::GetFullMessage() const
{
	char * strErrorCode = Utility::ConvertFromIntToString(errorCode);
	char * strLineNumber = Utility::ConvertFromIntToString((__int64)lineNumber);
	
	const char * outputOperation = operation;
	const char * outputFileName = fileName;
	const char * outputCommand = command;
	if(outputOperation == NULL)
	{
		outputOperation = "performing an unknown operation";
	}
	if(outputFileName == NULL)
	{
		outputFileName = "N/A";
	}
	if(outputCommand == NULL)
	{
		outputCommand = "N/A";
	}

	const size_t iNoParts = 13;
	const char * sPart[iNoParts];
	sPart[0] = "An error occurred in ";
	sPart[1] = outputCommand;
	sPart[2] = " whilst ";
	sPart[3] = outputOperation;
	sPart[4] = " at line ";
	sPart[5] = strLineNumber;
	sPart[6] = " of file \"";
	sPart[7] = outputFileName;
	sPart[8] = "\". Error code ";
	sPart[9] = strErrorCode;
	sPart[10] = ". Version \"";
	sPart[11] = Utility::VERSION;
	sPart[12] = "\"";

	char * fullMessage = Utility::ConcatArray(sPart, iNoParts);

	delete[] strErrorCode;
	delete[] strLineNumber;

	return fullMessage;
}

/**
 * @brief	Displays an error message describing the error.
 */
void ErrorReport::DoMessageBox() const
{
	char * fullMessage = GetFullMessage();
	Utility::DisplayErrorMB(fullMessage);
	delete[] fullMessage;
}

/**
 * @brief	Saves the error message globally.
 */
void ErrorReport::DoSave() const
{
	if(saveErrorLoaded.Get() == false)
	{
		saveError = *this;
		saveErrorLoaded.Set(true);
	}
}

/**
 * @brief	Throws this object as an exception.
 *
 * @throws ErrorReport Always.
 */
void ErrorReport::DoRethrow() const
{
	throw(*this);
}

/**
 * @brief	Take actions specified by global error mode.
 *
 * e.g. if EM_MESSAGE_BOX is enabled DoMessageBox() will be called.\n\n
 *
 * Multiple error modes will be taken into account, so more than one action
 * can occur in one call to this method.
 */
void ErrorReport::DoErrorModeEvent() const
{
	if(IsErrorModeEnabled(EM_MESSAGE_BOX) == true)
	{
		DoMessageBox();
	}

	if(IsErrorModeEnabled(EM_SAVE) == true)
	{
		DoSave();
	}

	if(IsErrorModeEnabled(EM_EXCEPTION) == true)
	{
		DoRethrow();
	}
}

/**
 * @brief	Determines if an error is saved globally.
 *
 * @return	true if an error is saved, false if not.
 */
bool ErrorReport::IsErrorSaved()
{
	return saveErrorLoaded.Get();
}

/**
 * @brief	Retrieves a saved error.
 *
 * @return	the saved error.
 */
ErrorReport ErrorReport::GetSavedError()
{
	ctSaveError.Enter();
		ErrorReport aux = saveError;
	ctSaveError.Leave();
	return aux;
}

/**
 * @brief	Toggles error mode.
 *
 * If the error mode was enabled, it will be disabled and visa versa.
 *
 * @param	errorMode	The error mode to toggle.
 */
void ErrorReport::ToggleErrorMode( ErrorMode errorMode )
{
	ErrorReport::errorMode.BitToggle(errorMode);
}

/**
 * @brief	Enables or disables an error mode.
 *
 * @param	errorMode	The error mode to modify. 
 * @param	enabled		True to enable, false to disable. 
 */
void ErrorReport::SetErrorMode( ErrorMode errorMode, bool enabled )
{
	if(enabled == true)
	{
		ErrorReport::errorMode.BitOn(errorMode);
	}
	else
	{
		ErrorReport::errorMode.BitOff(errorMode);
	}
}

/**
 * @brief	Determines whether an error mode is enabled. 
 *
 * @param	errorMode	The error mode to check. 
 *
 * @return	true if the error mode is enabled, false if not. 
 */
bool ErrorReport::IsErrorModeEnabled( ErrorMode errorMode )
{
	return ErrorReport::errorMode.BitGet(errorMode);
}

/**
 * @brief	Retrieves the operation that was being performed when the error occurred.
 *
 * @return	the operation that was being performed when the error occurred.
 * @return	NULL if none is loaded.
 */
const char * ErrorReport::GetOperation() const
{
	return operation;
}

/**
 * @brief	Retrieves the command that caused the error.
 *
 * @return	the command that caused the error.
 * @return	NULL if none is loaded.
 */
const char * ErrorReport::GetCommand() const
{
	return command;
}

/**
 * @brief	Retrieves the error code associated with the error.
 *
 * @return	the error code.
 * @return	0 if none is loaded.
 */
__int64 ErrorReport::GetErrorCode() const
{
	return errorCode;
}

/**
 * @brief	Retrieves the line number at which the error occurred.
 *
 * @return	the line number at which the error occurred.
 * @return	0 if none is loaded.
 */
unsigned __int64 ErrorReport::GetLineNumber() const
{
	return lineNumber;
}

/**
 * @brief	Retrieves the name of the file that the error occurred in.
 *
 * @return	the name of the file that the error occurred in.
 * @return	NULL if none is loaded.
 */
const char * ErrorReport::GetFileName() const
{
	return fileName;
}

/**
 * @brief Converts integer into enum for ErrorMode.
 *
 * This is useful for compatibility with languages other than C++.
 *
 * @param	mode	Integer to convert.
 *
 * @return	enum equivalent of @a mode.
 */
ErrorReport::ErrorMode ErrorReport::ConvertToErrorMode(int mode)
{
	switch(mode)
	{
	case(EM_MESSAGE_BOX):
	case(EM_SAVE):
	case(EM_EXCEPTION):
		return static_cast<ErrorReport::ErrorMode>(mode);
		break;

	default:
		_ErrorException(true,"converting from integer to error mode, invalid mode received",0,__LINE__,__FILE__);
		return ErrorReport::EM_MESSAGE_BOX;
		break;
	}
}

/**
 * @brief	Specifies whether a new error is currently saved globally.
 *
 * @param	option	Option.
 */
void ErrorReport::SetErrorSaved( bool option )
{
	saveErrorLoaded.Set(option);
}

/**
 * @brief Tests class.
 *
 * @return true if no problems while testing were found, false if not.
 * Note that not all tests automatically check for problems so some tests
 * require manual verification.
 */
bool ErrorReport::TestClass()
{
	cout << "Testing ErrorReport class...\n";
	bool problem = false;

	const char * cCommand = "ErrorReport::TestClass()";

	ErrorReport::ToggleErrorMode(EM_MESSAGE_BOX);
	ErrorReport::SetErrorMode(EM_SAVE,true);

	try
	{
		_ErrorException(true,"simulating an error situation",5000,__LINE__,__FILE__);
	}
	STD_CATCH

	if(ErrorReport::IsErrorSaved() == true)
	{
		char * fullMessage = ErrorReport::GetSavedError().GetFullMessage();
		cout << "Full message: " << fullMessage << '\n';
		delete[] fullMessage;
	}
	else
	{
		problem = true;
	}
	
	ErrorReport::SetErrorSaved(false);

	cCommand = NULL;
	try
	{
		_ErrorException(true,NULL,0,0,NULL);
	}
	STD_CATCH

	if(ErrorReport::IsErrorSaved() == true)
	{
		char * fullMessage = ErrorReport::GetSavedError().GetFullMessage();
		cout << "Full message: " << fullMessage << '\n';
		delete[] fullMessage;
	}
	else
	{
		problem = true;
	}

	cout << "\n\n";
	return !problem;
}