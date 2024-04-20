#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <mutex>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>
#include <Redisclient/IRedisclient.h>
#include <Redisclient/IRedisClientFactory.h>
#include <Config/IConfigFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <Log/ILogFactory.h>
#include <Json/IJsonFactory.h>
#include <Observer/IObserverFactory.h>
#include <StringUtil/IStringFactory.h>
#include <HelpTool/HelpToolFactory.h>
#include <HttpClient/IHttpClientFactory.h>
#include <MessageCenter/IMessageCenterFactory.h>

// Protocol
#include <Protocol/CAddOrUpdateAlarmWithProcessRequest.h>
#include <Protocol/CAddOrUpdateAlarmWithProcessRespond.h>
#include <Protocol/CAddOrUpdateLinkedRequest.h>
#include <Protocol/CAddAlarmRemarkRequest.h>
#include <Protocol/CAddAlarmRemarkRespond.h>
#include <Protocol/CGetAlarmRemarkRequest.h>
#include <Protocol/CGetAlarmRemarkRespond.h>
#include <Protocol/CGetAlarmAllRemarkRequest.h>
#include <Protocol/CGetAlarmAllRemarkRespond.h>

#include <Protocol/CAddSignRequest.h>
#include <Protocol/CAddSignRespond.h>
#include <Protocol/CGetSignRequest.h>
#include <Protocol/CGetSignRespond.h>

#include <Protocol/CAddMergeRequest.h>
#include <Protocol/CAddMergeRespond.h>
#include <Protocol/CCancelMergeRequest.h>
#include <Protocol/CCancelMergeRespond.h>
#include <Protocol/CGetMergeAlarmRequest.h>
#include <Protocol/CGetMergeAlarmRespond.h>

#include <Protocol/CGetAlarmLogRequest.h>
#include <Protocol/CGetAlarmLogRespond.h>
#include <Protocol/CGetAlarmLogAlarmResourceRequest.h>
#include <Protocol/CGetAlarmLogAlarmResourceRespond.h>
#include <Protocol/CGetAlarmProcessResourceRequest.h>
#include <Protocol/CGetAlarmProcessResourceRespond.h>
#include <Protocol/CGetAlarmLogCallrefRequest.h>
#include <Protocol/CGetAlarmLogCallrefRespond.h>
#include <Protocol/CGetAlarmIDBySourceID.h>
#include <Protocol/CGetAlarmLogInfoRequest.h>
#include <Protocol/CAlarmVisitRequest.h>
#include <Protocol/CAlarmVisitRespond.h>
#include <Protocol/CSearchAlarmVisitRequest.h>
#include <Protocol/CSearchAlarmVisitRespond.h>
#include <Protocol/CSearchAlarmVisitStatisticRequest.h>
#include <Protocol/CSearchAlarmVisitStatisticRespond.h>
#include <Protocol/CAddOrUpdateProcessRequest.h>
#include <Protocol/CAddOrUpdateProcessRespond.h>
#include <Protocol/CPutAlarmLogInfoRespond.h>
#include <Protocol/CAlarmSync.h>
#include <Protocol/CAlarmLogSync.h>
#include <Protocol/CAlarmProcessSync.h>
#include <Protocol/CAlarmLinkedSync.h>
#include <Protocol/CAlarmVisitSync.h>

#include <Protocol/CUpdateCalleventFlagRequest.h>
#include <Protocol/CUpdateCalleventFlagRespond.h>

#include <Protocol/AlarmStructDefine.h>
#include "Protocol/CAlarmRelatedCarsProtocol.h"
#include "Protocol/CAlarmRelatedPersonsProtocol.h"

/*加入敏感警情头文件*/
#include <Protocol/CKeyWordAlarmSync.h>
#include <Protocol/CKeyWordAlarmQryRequest.h>
#include <Protocol/CKeyWordAlarmQryRespond.h>


#include <Protocol/CDeleteAlarmRequest.h>
#include <Protocol/CGetAlarmIDRequest.h>
#include <Timer/ITimerFactory.h> 
#include <Protocol/CTIServer/CViolationSync.h>

#include <Protocol/CReviveAlarmRequest.h>
#include <Protocol/CSetAlarmCallrefProtocol.h>

#include <Protocol/UpdateBJRGpsRequest.h>
#include <Protocol/UpdateBJRGpsResponse.h>

//加入对接12345头文件
#include <Protocol/CPushAlarmTo12345Request.h>
#include <Protocol/CPushAlarmTo12345Respond.h>
#include <Protocol/SyncNacosParams.h>
#include <Protocol/TelHotlineRequest.h>
#include <Protocol/CPostSmsSend.h>

//加入自动催办头文件
#include <Protocol/CAutourgeOnAlarmRequset.h>
#include <Protocol/CAutourgeOnAlarmRespond.h>

//暂存警单相关
#include <Protocol/OpenTemporaryAlarmRequest.h>

//警情附件
#include <Protocol/AlarmAttachRequest.h>
#include <Protocol/AlarmAttachResponse.h>

#include <boost/thread.hpp>

#include <Protocol/DeptCodeInfo.h>
#include <Protocol/CRedisStaff.h>

#include <Protocol/ParamInfo.h>
#include <Protocol/CTIServer/CallOverEvent.h>

#define        SAFE_LOCK(lk) std::lock_guard<std::recursive_mutex> l_guard(lk)
#define MODULE_NAME "synthetical"

#define REDIS_Serial_Data "SerialData"
using namespace ICC;
