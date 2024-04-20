#pragma once

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

#include <Protocol/CBaseDataEntities.h>
#include <Protocol/CRoleSync.h>
#include <Protocol/CGetRoleRequest.h>
#include <Protocol/CGetRoleRespond.h>
#include <Protocol/CSetRoleRequest.h>
#include <Protocol/CSetRoleRespond.h>
#include <Protocol/CDeleteRoleRequest.h>
#include <Protocol/CDeleteRoleRespond.h>

#define MODULE_NAME "role"

using namespace ICC;