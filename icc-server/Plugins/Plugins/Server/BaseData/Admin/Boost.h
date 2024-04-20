#pragma once

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
#include <StringUtil/IStringFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <Redisclient/IRedisClientFactory.h>
//–≠“È
#include <Protocol/CGetAdminRespond.h>
#include <Protocol/CanAdminDept.h>
#include <Protocol/CCanAdminDeptSync.h>

#define MODULE_NAME "admin"

using namespace ICC;
