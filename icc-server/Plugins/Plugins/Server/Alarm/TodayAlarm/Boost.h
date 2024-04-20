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

#include <DBConn/IDBConnFactory.h>
#include <Protocol/CAlarmProcessSync.h>
#include <Protocol/CSetAlarmProcessRequest.h>
#include <Protocol/CSetAlarmProcessRespond.h>
#include <Protocol/CAddOrUpdateAlarmRequest.h>
#include <Protocol/TodayAlarm.h>
#include <Protocol/SimilarAlarm.h>
#include <Protocol/TodayCallCountRequest.h>
#include <Protocol/TodayCallCountRespond.h>
#include <Protocol/TodayCallInStatisticsRequest.h>
#include <Protocol/TodayCallInStatisticsRespond.h>
#include <Protocol/TodayStatisticsByReceiveTypeRequest.h>
#include <Protocol/TodayStatisticsByReceiveTypeRespond.h>
#include <Protocol/TodayStatisticsByFirstTypeRequest.h>
#include <Protocol/TodayStatisticsByFirstTypeRespond.h>
#include "Protocol/TodayAlarmInfoByFirstTypeRequest.h"
#include "Protocol/TodayAlarmInfoByFirstTypeRespond.h"
#include <Redisclient/IRedisclient.h>
#include <Redisclient/IRedisClientFactory.h>
#define MODULE_NAME "todayalarm"

using namespace ICC;
