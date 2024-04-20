#pragma once

// icc
#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>
#include <Observer/IObserverFactory.h>
#include <StringUtil/IStringFactory.h>
#include <HelpTool/HelpToolFactory.h>

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
#include <HttpClient/IHttpClientFactory.h>
#include <Timer/ITimerFactory.h> 
#include <MessageCenter/IMessageCenterFactory.h>

#include <Protocol/CAlarmSync.h>
#include <Protocol/CAlarmLogSync.h>

#include <Protocol/TelHotlineRequest.h>
#include <Protocol/CAcdDeleteRequest.h>
#include <Protocol/CAcdQueryResponse.h>
#include <Protocol/CAcdUpdatesync.h>
#include <Protocol/CAcdUpdateRequest.h>
#include <Protocol/CAcdQueryRequest.h>

//加入对接12345头文件
#include <Protocol/TelHotlineRequest.h>
#include <Protocol/CPushAlarmTo12345Request.h>
#include <Protocol/CPushAlarmTo12345Respond.h>

#include <Protocol/SyncNacosParams.h>
#include <boost/thread.hpp>

#include <Protocol/ParamInfo.h>
#include <Protocol/CRedisStaff.h>

#define MODULE_NAME "TelHotline"

using namespace ICC;