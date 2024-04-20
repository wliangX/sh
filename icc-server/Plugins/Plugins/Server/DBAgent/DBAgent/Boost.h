#pragma once

#include <thread>
//#include <iostream>
//#include <libpq-fe.h>

#include <boost/thread.hpp>
#include <boost/format.hpp>

#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <Log/ILogFactory.h>
#include <Lock/ILockFactory.h>
#include <Xml/IXmlFactory.h>
#include <Json/IJsonFactory.h>
#include <Config/IConfigFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <Timer/ITimerFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <Observer/IObserverFactory.h>
#include <StringUtil/IStringFactory.h>
#include <SqlBuilder/ISqlBuilderFactory.h>
#include <SqlBuilder/ISqlRequestFactory.h>
#include <Threadpool/IThreadPoolFactory.h>
#include <Redisclient/IRedisclient.h>
#include <Redisclient/IRedisClientFactory.h>

// protocol
#include <Protocol/CSQLRespond.h>
#include <Protocol/CSearchAlarmRequest.h>
#include <Protocol/CSearchAlarmRespond.h>
#include <Protocol/CSearchAlarmProcessRequest.h>
#include <Protocol/CSearchAlarmProcessRespond.h>
#include <Protocol/CSearchAlarmFeedbackRequest.h>
#include <Protocol/CSearchAlarmFeedbackRespond.h>
#include <Protocol/CGetCalleventInfoRequest.h>
#include <Protocol/CGetCallEventInfoRespond.h>
#include <Protocol/CSearchStatisticsAlarmRequest.h>
#include <Protocol/CSearchStatisticsAlarmRespond.h>
#include <Protocol/CSearchStatisticsReceiveAlarmRequest.h>
#include <Protocol/CSearchStatisticsReceiveAlarmRespond.h>
#include <Protocol/CSearchStatisticsCallInRequest.h>
#include <Protocol/CSearchStatisticsCallInRespond.h>
#include <Protocol/CSearchCallWaitingRequest.h>
#include <Protocol/CSearchCallWaitingRespond.h>
#include <Protocol/CSearchBackAlarmRequest.h>
#include <Protocol/CSearchBackAlarmRespond.h>
#include <Protocol/CSearchCallerRequest.h>
#include <Protocol/csearchstatisticinforequest.h>
#include <Protocol/CSearchStatisticInfoRespond.h>
#include <Protocol/CSearchAlarmCountRequest.h>
#include <Protocol/CSearchAlarmCountRespond.h>
#include <Protocol/CSearchCallrefCountRequest.h>
#include <Protocol/CSearchCallrefCountRespond.h>
#include <Protocol/CCallEventSearchRequest.h>
#include <Protocol/CCallEventSearchRespond.h>
#include <Protocol/CCallEventSearchExRequest.h>
#include <Protocol/CCallEventSearchExRespond.h>
#include <Protocol/CDBConnectionStatusSync.h>
#include <Protocol/CSearchReleaseCount.h>
#include <Protocol/CSearchOnlineCount.h>
#include <Protocol/CSearchLogoutCount.h>
#include <Protocol/CGetClientInOutRequest.h>
#include <Protocol/CGetClientInOutRespond.h>
#include <Protocol/CVcsSynDataSar.h>
#include <Protocol/CSearchAlarmAllRespond.h>
#include <Protocol/CAddOrUpdateProcessRequest.h>
#include <Protocol/CGetAlarmStatisticsRequest.h>
#include <Protocol/CGetCalleventStatisticsRequest.h>
#include <Protocol/CGetViolationStatisticsRequest.h>
#include <Protocol/CGetBlacklistStatisticsRequest.h>
#include <Protocol/CGetBlacklistStatisticsRespond.h>
#include <Protocol/CGetViolationStatisticsRespond.h>
#include <Protocol/CGetCalleventStatisticsRespond.h>
#include <Protocol/CGetAlarmStatisticsRespond.h>
#include <Protocol/CGetPhoneMarkStatisticsRequest.h>
#include <Protocol/CGetPhoneMarkStatisticsRespond.h>
#include <Protocol/CGetSeatAlarmStatisticsRequest.h>
#include <Protocol/CGetSeatAlarmStatisticsRespond.h>
#include "Protocol/CGetAlarmCommonInfoProtocol.h"

#include "Protocol/GetNumberBlackCountProtocol.h"

#define MODULE_NAME "dbagent"

using namespace ICC;
