#pragma once

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

#include <Protocol/CKeyWordSetRequest.h>
#include <Protocol/CKeyWordSetRespond.h>
#include <Protocol/CKeyWordDeleteRequest.h>
#include <Protocol/CKeyWordDeleteRespond.h>
#include <Protocol/CKeyWordQueryRequest.h>
#include <Protocol/CKeyWordQueryRespond.h>
#include <Protocol/CKeyWordSync.h>

#define MODULE_NAME "keyword"

using namespace ICC;