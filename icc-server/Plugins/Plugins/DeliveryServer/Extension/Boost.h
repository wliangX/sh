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
#include <MessageCenter/IMessageCenterFactory.h>
#include <Timer/ITimerFactory.h>
#include <Observer/IObserver.h>
#include <Thread/IThreadFactory.h>
#include <HttpClient/IHttpClientFactory.h>

#include "CommonTimer.h"
#include <Protocol/CTIServer/DeviceStateEvent.h>
#include <Protocol/SyncNacosParams.h>
#include <Protocol/DeviceDataInfo.h>
#include <Protocol/CPlayRecordRequest.h>
#include <Protocol/CDownloadRecordRequest.h>
#include <Protocol/CDownloadRecordRespond.h>
#include <Protocol/RecordIDToDB.h>
#include <Protocol/PostFmsRecord.h>
#include <Protocol/CGetFmsRecordFile.h>

using namespace ICC;