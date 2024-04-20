#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <Config/IConfigFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <Json/IJsonFactory.h>
#include <Log/ILogFactory.h>
#include <Lock/ILockFactory.h>
#include <Observer/IObserverFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Thread/IThreadFactory.h>
#include <MessageCenter/IMessageCenterFactory.h>
#include <Redisclient/IRedisClientFactory.h>
#include <MessageCenter/IMessageCenterFactory.h>

#include <DBConn/IDBConnFactory.h>
#include <Protocol/CMonitorSeatApplySync.h>
#include <Protocol/CMonitorSeatApplyQueryRequest.h>
#include <Protocol/CMonitorSeatApplyQueryRespond.h>
#include <Protocol/CMonitorSeatApplyRequest.h>
#include <Protocol/CMonitorSeatApplyRespond.h>

//2023.4.23添加，修改班长席审批逻辑
#include <Protocol/CMonitorSeatSingleApplyQueryRequest.h>
#include <Protocol/CMonitorSeatSingleApplyQueryRespond.h>

#include <Protocol/SyncNacosParams.h>
#include <boost/thread.hpp>

#include <Protocol/CSeatInfo.h>
#include <Protocol/CRedisRegisterInfo.h>

#include <Protocol/ParamInfo.h>

#define MODULE_NAME "monitorseat"

using namespace ICC;
