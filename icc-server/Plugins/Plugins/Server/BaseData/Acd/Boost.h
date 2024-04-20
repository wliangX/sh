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

#include <Protocol/CAcdAddRequest.h>
#include <Protocol/CAcdDeleteRequest.h>
#include <Protocol/CAcdQueryResponse.h>
#include <Protocol/CAcdUpdatesync.h>
#include <Protocol/CAcdUpdateRequest.h>
#include <Protocol/CAcdQueryRequest.h>

#include <Protocol/AcdInfo.h>
#include <Protocol/DeptCodeInfo.h>

#define MODULE_NAME "ACD"

using namespace ICC;