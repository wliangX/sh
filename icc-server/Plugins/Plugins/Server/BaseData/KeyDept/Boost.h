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

#include <Protocol/CCheckKeyDeptRequest.h>
#include <Protocol/CCheckKeyDeptRespond.h>
#include <Protocol/CDeleteKeyDeptRequest.h>
#include <Protocol/CDeleteKeyDeptRespond.h>
#include <Protocol/CGetAllKeyDeptRequest.h>
#include <Protocol/CGetAllKeyDeptRespond.h>
#include <Protocol/CKeyDeptSync.h>
#include <Protocol/CSetKeyDeptRequest.h>
#include <Protocol/CSetKeyDeptRespond.h>

#define MODULE_NAME "keydept"

using namespace ICC;