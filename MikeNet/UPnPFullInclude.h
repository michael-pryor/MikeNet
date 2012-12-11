#pragma once

#include "LibInclude.h"

// UPnP required headers
#include <Upnp.h>
#include <Natupnp.h>
#define _WIN32_DCOM    // for CoInitializeEx
#include <objbase.h>

// NAT
#include "UpnpNatPortMap.h"
#include "UpnpNat.h"
#include "UpnpNatPortMapAction.h"
#include "UpnpNatAction.h"
#include "ThreadMessageItemAddPortMap.h"
#include "ThreadMessageItemDeletePortMap.h"
#include "ThreadMessageItemSetPortMapDescription.h"
#include "ThreadMessageItemSetPortMapEnabled.h"
#include "ThreadMessageItemSetPortMapInternalIP.h"
#include "ThreadMessageItemSetPortMapInternalPort.h"
#include "ThreadMessageItemUpdateNat.h"
#include "UpnpNatActionThread.h"
#include "UpnpNatCommunication.h"
#include "UpnpNatUtility.h"
#include "mnNAT.h"