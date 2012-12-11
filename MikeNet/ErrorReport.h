#pragma once
#include "ConcurrencyEvent.h"
#include "ConcurrentObject.h"
#include "StoreQueue.h"

/**
 * @brief	Stores an error report.
 * @remarks	Michael Pryor, 9/17/2010. 
 */
class ErrorReport
{
public:
	/**
	 * @brief Error mode describes how errors should be dealt with.
	 */
	enum ErrorMode
	{
		/**
		 * Errors should be saved globally to be retrieved later using. 
		 * ErrorReport::GetSavedError and ErrorReport::IsErrorSaved.\n
		 * Value = 1.
		 */
		EM_SAVE = 1,

		/**
		 * Errors should be displayed in a message box.\n
		 * Value = 2.
		 */
		EM_MESSAGE_BOX = 2,

		/**
		 * Errors should be thrown as an exception. \n
		 * Value = 3.
		 */
		EM_EXCEPTION = 3
	};

	/**
	 * @brief Default value for ErrorReport::errorMode.
	 *
	 * By default EM_MESSAGE_BOX is enabled and all other modes are disabled.
	 */
	static const size_t DEFAULT_ERROR_MODE = 6;

private:
	/**
	 * @brief Signaled when ErrorReport::saveError is loaded with an error.
	 */
	static ConcurrencyEvent saveErrorLoaded;

	/**
	 * @brief Stores an error that can be accessed globally.
	 */
	static ErrorReport saveError;

	/**
	 * @brief Controls access to ErrorReport::saveError.
	 *
	 * Could not use ConcurrentObject here.
	 */
	static CriticalSection ctSaveError;

	/**
	 * @brief Stores the error mode('s) that the API is in.
	 *
	 * Error mode only applies to mn namespace.
	 * Throughout the rest of the API exceptions are thrown always.
	 */
	static ConcurrentObject<int> errorMode;

public:
	static bool IsErrorSaved();
	static void SetErrorSaved(bool option);
	static ErrorReport GetSavedError();

	static void ToggleErrorMode(ErrorMode errorMode);
	static void SetErrorMode(ErrorMode errorMode, bool enabled);
	static bool IsErrorModeEnabled(ErrorMode errorMode);

	static ErrorReport::ErrorMode ConvertToErrorMode(int mode);
private:
	/**
	 * @brief Operation that caused an error e.g. dynamically allocating memory.
	 */
	const char * operation;

	/**
	 * @brief Command that was responsible for causing error.
	 */
	const char * command;

	/**
	 * @brief Error code associated with error.
	 *
	 * Large data type to ensure compatibility with all possible return types of API.
	 */
	__int64 errorCode;
	
	/**
	 * @brief Line number at which error occurred.
	 */
	unsigned __int64 lineNumber;

	/**
	 * @brief Name of file in which error occurred.
	 */
	const char * fileName;

	void Copy(const ErrorReport & CopyMe);
public:

	void DefaultVariables();

	ErrorReport();
	ErrorReport(const ErrorReport & CopyMe);
	ErrorReport & operator= (const ErrorReport & CopyMe);
	~ErrorReport();

	const char * GetOperation() const;
	const char * GetCommand() const;
	__int64 GetErrorCode() const;
	unsigned __int64 GetLineNumber() const;
	const char * GetFileName() const;

	void LoadReport(const char * operation, __int64 errorCode, unsigned __int64 lineNumber, const char * fileName, const char * command = NULL);
	void SetCommand(const char * command);
	
	char * GetFullMessage() const;

	void DoMessageBox() const; 
	void DoSave() const;
	void DoRethrow() const;
	void DoErrorModeEvent() const;

	static bool TestClass();

private:
	static void ErrorReportsEnter();
	static void ErrorReportsLeave();
	static bool IsErrorReportsLockInUseByThisThread();
};