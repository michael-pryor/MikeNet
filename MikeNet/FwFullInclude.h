#pragma once

// Windows firewall header files
#include "clFwAuthorizedApplication.h"
#include "clFwOpenPort.h"
#include "clFwOpenPorts.h"
#include "clFwService.h"
#include "clFwProfile.h"
#include "clFwManager.h"

#include "FwGlobal.h"

#ifdef C_LINKAGE
	extern "C"
	{
#endif
#include "mnFwMain.h"
#include "mnFwProfile.h"
#include "mnFwService.h"
#include "mnFwApplication.h"
#ifdef C_LINKAGE
	}
#endif