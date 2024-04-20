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
#include <Lock/ILockFactory.h>
#include <Json/IJsonFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Log/ILogFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <Config/IConfigFactory.h>
#include <Timer/ITimerFactory.h>
#include <Redisclient/IRedisClientFactory.h>

// Protocol
#include <Protocol/CBaseRequest.h>
#include <Protocol/CBaseRespond.h>
#include <Protocol/CNoticeInfo.h>
#include <Protocol/CAddNoticeRequest.h>
#include <Protocol/CAddNoticeRespond.h>
#include <Protocol/CAddNoticeSync.h>
#include <Protocol/CGetNoticesRespond.h>

#include <Protocol/CTipsInfo.h>
#include <Protocol/CTipsRequest.h>
#include <Protocol/CGetTipsRespond.h>

#include <Protocol/CUrgesRequest.h>
#include <Protocol/CAlarmLogSync.h>

#include <Protocol/TimeOutEvent.h>
#include <Protocol/GetTimeOutEventRespond.h>
#include <Protocol/CTIServer/CallOverEvent.h>


#define SENDTYPE_TOPIC						("1")	// 主题
#define SENDTYPE_QUEUE						("0")	// 队列

#define MODULE_NAME "notice"

using namespace ICC;
