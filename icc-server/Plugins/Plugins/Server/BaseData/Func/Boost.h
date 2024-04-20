#pragma once

#include "FuncInfo.h"
// boost
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>

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

#include <Protocol/CGetPrivilegeRequest.h>
#include <Protocol/CGetPrivilegeRespond.h>
#include <Protocol/CSetPrivilegeRequest.h>
#include <Protocol/CSetPrivilegeRespond.h>
#include <Protocol/CDeletePrivilegeRequest.h>
#include <Protocol/CDeletePrivilegeRespond.h>
#include <Protocol/CPrivilegeSync.h>

#define MODULE_NAME "func"

using namespace ICC;