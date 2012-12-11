#pragma once

// Used to export to DLL (where necessary)
#ifdef DBP
	// Causes all DBP and CPP_DLL commands to be exported
	#define DBP_CPP_DLL __declspec ( dllexport )

	// Prevents name mangling, overloading of functions is disabled
	#define C_LINKAGE

	#define CPP_DLL

	#define NAMESPACE_PREFIX namespace
#else
	#ifdef CPP_DLL
		// Causes all DBP and CPP_DLL commands to be exported
		#define DBP_CPP_DLL __declspec ( dllexport )

		// Prevents name mangling, overloading of functions is disabled
		#define C_LINKAGE 

		// Causes commands not exported in DBP build to be exported for CPP_DLL build
		#undef CPP_DLL
		#define CPP_DLL	DBP_CPP_DLL
	#else
		// In C++ lib or CLR builds no commands are exported using __declspec ( dllexport )
		#define DBP_CPP_DLL

		#define CPP_DLL
	#endif
#endif



// Exception catch definitions
// Note: RM means return minus

// Used by all other CATCH definitions, specifies what message should be displayed where type other than ErrorReport is caught
// Decides how to deal with error based on error mode
#define _EventCATCH(operation) \
	catch(ErrorReport & erData){erData.SetCommand(cCommand); erData.DoErrorModeEvent();}\
	catch (...){ErrorReport erData; erData.LoadReport(operation,-1,__LINE__,__FILE__,cCommand); erData.DoErrorModeEvent();}

// Displays a message box when dealing with error
#define _MsgboxCATCH(operation) \
	catch(ErrorReport & erData){erData.SetCommand(cCommand); erData.DoMessageBox();}\
	catch (...){ErrorReport erData; erData.LoadReport(operation,-1,__LINE__,__FILE__,cCommand); erData.DoMessageBox();}

// Throws another exception when dealing with error
#define _RethrowCATCH(operation) \
	catch(ErrorReport & erData){erData.SetCommand(cCommand); erData.DoRethrow();}\
	catch (...){ErrorReport erData; erData.LoadReport(operation,-1,__LINE__,__FILE__,cCommand); erData.DoRethrow();}

// Decides how to deal with error based on error mode
#define _EventCATCH_RM(operation) \
	catch(ErrorReport & erData){erData.SetCommand(cCommand); erData.DoErrorModeEvent(); returnMe = -1;}\
	catch (...){ErrorReport erData; erData.LoadReport(operation,-1,__LINE__,__FILE__,cCommand); erData.DoErrorModeEvent(); returnMe = -1;}

// Displays a message box when dealing with previously uncaught errors
#define _MsgboxCATCH_RM_ALL(operation) \
	catch (...){const ErrorReport erData; erData.LoadReport(operation,-1,__LINE__,__FILE__,cCommand); erData.DoMessageBox(); returnMe = -1;}

// Displays a message box when dealing with error
#define _MsgboxCATCH_RM(operation) \
	catch(ErrorReport & erData){erData.SetCommand(cCommand); erData.DoMessageBox(); returnMe = -1;}\
	//_MsgboxCATCH_RM_ALL(operation)

// Throws another exception when dealing with error
#define _RethrowCATCH_RM(operation) \
	catch(ErrorReport & erData){erData.SetCommand(cCommand); erData.DoRethrow(); returnMe = -1;}\
	catch (...){ErrorReport erData; erData.LoadReport(operation,-1,__LINE__,__FILE__,cCommand); returnMe = -1;}


#define errorMessageUnexpected		"executing. The cause of the error is unclear"

#define STD_CATCH			_EventCATCH(errorMessageUnexpected)
#define STD_CATCH_RM		_EventCATCH_RM(errorMessageUnexpected)
#define MSG_CATCH			_MsgboxCATCH(errorMessageUnexpected)
#define MSG_CATCH_RM		_MsgBoxCATCH_RM(errorMessageUnexpected)
