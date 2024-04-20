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

#include <Protocol/AlarmExtensionRequest.h>
#include <Protocol/AlarmExtensionResponse.h>

#define MODULE_NAME "AlarmPlan"

using namespace ICC;