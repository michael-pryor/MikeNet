#include "FullInclude.h"

/**
 * @brief	Deals with errors by throwing an exception. 
 *
 * @exception	Error	Thrown when error. 
 *
 * @param	error		If true then an error has occurred and an exception will be thrown.
 * @param	operation	Operation that was being performed at the time of the error.
 * @param	errorCode	Error code associated with error.
 * @param	line		Line at which error occurred.
 * @param	file		File within which the error occurred.
 *
 * @throws ErrorReport When @a error is true. 
 */
void _ErrorException(bool error, const char * operation, __int64 errorCode, unsigned __int64 line, const char * file)
{
	if(error == true)
	{
		ErrorReport errorReport;
		errorReport.LoadReport(operation,errorCode,line,file);
		
		errorReport.DoRethrow();
	}
}

/**
 * @brief	Deals with errors by displaying a message box. 
 *
 * @exception	Error	Thrown when error. 
 *
 * @param	error		If true then an error has occurred and an exception will be thrown.
 * @param	operation	Operation that was being performed at the time of the error.
 * @param	errorCode	Error code associated with error.
 * @param	line		Line at which error occurred.
 * @param	file		File within which the error occurred.
 *
 * @throws ErrorReport When @a error is true. 
 */
void _ErrorMessageBox(bool error, const char * operation, __int64 errorCode, unsigned __int64 line, const char * file)
{
	if(error == true)
	{
		ErrorReport errorReport;
		errorReport.LoadReport(operation,errorCode,line,file);
		
		errorReport.DoMessageBox();
	}
}