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
#include <Protocol/DeptCodeInfo.h>
#include <Protocol/CSeatAddressSetRequest.h>
#include <Protocol/CSeatAddressGetRespond.h>
#include <Protocol/CSeatDataInfo.h>
#include <Protocol/CClientRegisterSync.h>
#include <Protocol/CAgentStateSyncRequest.h>
#include <Protocol/CTIServer/DeviceStateEvent.h>
#include <Protocol/CAlarmSync.h>
#include <Protocol/CTIServer/GetDeviceListRequest.h>
#include <Protocol/CTIServer/GetSeatDeviceState.h>
#include <Protocol/CSeatInfoManagerSync.h>
#include "CommonTimer.h"
#include <Timer/ITimerFactory.h>
#include <HttpClient/IHttpClientFactory.h>
#include <MessageCenter/IMessageCenterFactory.h>
#include <Protocol/SyncNacosParams.h>
#include <Protocol/ParamInfo.h>
#include <Protocol/CTIServer/CallInfo.h>
#include <Protocol/CGetSeatAlarmStatisticsRequest.h>

#include <Protocol/CRedisStaff.h>

#define MODULE_NAME "SeatInfo"

using namespace ICC;