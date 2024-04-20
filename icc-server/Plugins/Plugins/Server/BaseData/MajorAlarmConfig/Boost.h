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

#include <Protocol/MajorAlarmConfigAddRequest.h>
#include <Protocol/MajorAlarmConfigDeleteRequest.h>
#include <Protocol/MajorAlarmConfigQueryRequest.h>
#include <Protocol/MajorAlarmConfigQueryResponse.h>
#include <Protocol/MajorAlarmConfigSync.h>
#include <Protocol/SyncNacosParams.h>

#include <boost/thread.hpp>

#define MODULE_NAME "majoralarmconfig"

using namespace ICC;