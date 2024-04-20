#pragma once

#include "DispatchInfo.h"
// boost
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>

// icc
#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <Lock/ILockFactory.h>
#include <Observer/IObserverFactory.h>
#include <AmqClient/IAmqClient.h>
#include <Config/IConfigFactory.h>
#include <Log/ILogFactory.h>
#include <Lock/ILockFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <Json/IJsonFactory.h>
#include <Redisclient/IRedisClientFactory.h>
#include <StringUtil/IStringFactory.h>
#include <DateTime/IDateTimeFactory.h>

// Protocol
#include <Protocol/CGetDispatchRequest.h>
#include <Protocol/CGetDispatchRespond.h>
#include <Protocol/CSetDispatchRequest.h>
#include <Protocol/CSetDispatchRespond.h>
#include <Protocol/CDeleteDispatchRequest.h>
#include <Protocol/CDeleteDispatchRespond.h>
#include <Protocol/CanDispatchDept.h>

#define MODULE_NAME "dispatch"

using namespace ICC;