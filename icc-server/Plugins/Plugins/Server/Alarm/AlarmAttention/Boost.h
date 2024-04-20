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
#include <Redisclient/IRedisclient.h>
#include <Redisclient/IRedisClientFactory.h>

#include <DBConn/IDBConnFactory.h>
#include <Protocol/CAlarmProcessSync.h>
#include <Protocol/CAlarmSync.h>
#include <Protocol/CAlarmLogSync.h>
#include <HelpTool/HelpToolFactory.h>

#include <Protocol/AlarmAttentionAddRequest.h>
#include <Protocol/AlarmAttentionAddResponse.h>
#include <Protocol/AlarmAttentionDeleteRequest.h>
#include <Protocol/AlarmAttentionDeleteResponse.h>
#include <Protocol/AlarmAttentionQueryRequest.h>
#include <Protocol/AlarmAttentionQueryResponse.h>
#include <Protocol/AlarmAttentionUpdateRequest.h>
#include <Protocol/AlarmAttentionUpdateResponse.h>
#include <Protocol/AlarmAttentionJudgeRequest.h>
#include <Protocol/AlarmAttentionJudgeResponse.h>

#include <Protocol/AlarmAttentionInfo.h>
#include <Protocol/CRedisStaff.h>

#define MODULE_NAME "alarmattention"

using namespace ICC;
