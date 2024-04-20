#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <Thread/IThreadFactory.h>
#include <AmqClient/IAmqClient.h>
#include <Observer/IObserverFactory.h>
#include <Json/IJsonFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <Timer/ITimerFactory.h> 
#include <StringUtil/IStringFactory.h>
#include <Log/ILogFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <Config/IConfigFactory.h>
#include <Redisclient/IRedisClientFactory.h>
#include <HttpClient/IHttpClientFactory.h>
#include <MessageCenter/IMessageCenterFactory.h>

//	CTIServer Defines
#include <CTIServer/IProtocolDefine.h>
#include <CTIServer/ISwitchNotifaction.h>

#include <Protocol/CTIServer/CallStateEvent.h>
#include <Protocol/CTIServer/CallOverEvent.h>
#include <Protocol/CTIServer/DeviceStateEvent.h>
#include <Protocol/CTIServer/CallBackSync.h>

#include <Protocol/CTIServer/CallEventQueryRequest.h>
#include <Protocol/CTIServer/CallEventQueryRespond.h>
#include <Protocol/CTIServer/BlackCallQueryRequest.h>
#include <Protocol/CTIServer/BlackCallQueryRespond.h>

#include <Protocol/CTIServer/DetailCallOverSync.h>
#include <Protocol/CTIServer/DetailCallQueryRequest.h>
#include <Protocol/CTIServer/DetailCallQueryRespond.h>

#include <Protocol/CTIServer/SetBlackListRequest.h>
#include <Protocol/CTIServer/SetBlackListRespond.h>
#include <Protocol/CTIServer/SetBlackListSync.h>
#include <Protocol/CTIServer/DeleteBlackListRequest.h>
#include <Protocol/CTIServer/DeleteBlackListRespond.h>
#include <Protocol/CTIServer/DeleteBlackListSync.h>
#include <Protocol/CTIServer/DeleteAllBlackListRequest.h>
#include <Protocol/CTIServer/DeleteAllBlackListRespond.h>
#include <Protocol/CTIServer/DeleteAllBlackListSync.h>
#include <Protocol/CTIServer/GetAllBlackListRequest.h>
#include <Protocol/CTIServer/GetAllBlackListRespond.h>
#include <Protocol/CTIServer/LoadAllBlackListRequest.h>
#include <Protocol/CTIServer/LoadAllBlackListRespond.h>

#include <Protocol/CTIServer/SetRedListRequest.h>
#include <Protocol/CTIServer/SetRedListRespond.h>
#include <Protocol/CTIServer/DeleteRedListRequest.h>
#include <Protocol/CTIServer/DeleteRedListRespond.h>
#include <Protocol/CTIServer/GetAllRedListRequest.h>
#include <Protocol/CTIServer/GetAllRedListRespond.h>

#include <Protocol/CTIServer/RemoveReleaseCallRequest.h>
#include <Protocol/CTIServer/RemoveReleaseCallRespond.h>
#include <Protocol/CTIServer/RemoveReleaseCallSync.h>

#include <Protocol/CTIServer/TakeOverCallRespond.h>
#include <Protocol/CTIServer/TakeOverCallSync.h>

#include <Protocol/CTIServer/CallBackSync.h>
#include <Protocol/CTIServer/CallInfo.h>

#include <Protocol/CTIServer/GetViolationRequest.h>
#include <Protocol/CTIServer/GetViolationRespond.h>
#include <Protocol/CTIServer/CViolationSync.h>
#include <Protocol/CTIServer/AgentStateEvent.h>
#include <Protocol/CTIServer/GetViolationTimeOutProtocol.h>

#include "Protocol/CTIServer/DeviceStateSyncToVcs.h"
#include "Protocol/CTIServer/VcsAlarmRelationRequest.h"
#include "Protocol/CTIServer/VcsQueryCallInfoRequest.h"
#include "Protocol/CTIServer/VcsQueryCallInfoResponse.h"

#include <Protocol/CAlarmLogSync.h>
#include <Protocol/SyncNacosParams.h>
#include <boost/thread.hpp>

#include <Protocol/CRedisRegisterInfo.h>
#include <Protocol/ParamInfo.h>

#include <Protocol/CReceiveRecordFileUpRequest.h>
#include <Protocol/RecordIDToDB.h>
#include <Protocol/CGetHistoryCallListRequest.h>
#include <Protocol/AcdInfo.h>
#include <Protocol/DeptCodeInfo.h>
#include <Protocol/CSeatDataInfo.h>
#include <Protocol/Staff.h>

// Protocol
#define MODULE_NAME							 "dbprocess"

using namespace ICC;
