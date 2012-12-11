#pragma once

//#define DBP	  // Comment this out when compiling for C++ lib, C++ DLL, or CLR
//#define CLR     // Comment this out when compiling for C++ lib, C++ DLL, or DBP
//#define CPP_DLL // Comment this out when compiling for C++ lib, DBP or CLR

// If defined the main page will be designed for the index page of the website.
// If not, the main page will be designed for the index page of documentation given to users.
//#define WEBSITE_MODE

// If undefined then libraries will be included. This should be defined when compiling
// MikeNet, and undefined if precompiled libraries are being used.
#define ALREADY_INCLUDED_LIBS

// Required header files used throughout MikeNet
#include <winsock2.h>
#include <Windows.h>
#include <time.h>
#include <queue>
#include <ws2tcpip.h>
#include <Wspiapi.h>
#include <exception>
#include <stdlib.h>
#include <Netfw.h>
#include <MMReg.h>
#include <cmath>
#include <iostream>

#ifdef DBP
	#include <globstruct.h>
#endif
using namespace std;

// MikeNet modules
#include "GlobalInclude.h"
#include "NetworkFullInclude.h"
#include "UPnPFullInclude.h"
#include "SoundFullInclude.h"

// Wrapper for DBP compatibility
#ifdef DBP
	#include "mnDBPWrapper.h"
#endif
