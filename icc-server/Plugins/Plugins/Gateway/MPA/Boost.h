#pragma once

// boost
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>

// icc
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
#include <Redisclient/IRedisClientFactory.h>
#include <Timer/ITimerFactory.h>

#include "Protocol/CFeedBackSync.h"
#include "Protocol/CEditAlarm.h"
#include "Protocol/CAlarmSync.h"
#include "Protocol/CAddOrUpdateAlarmWithProcessRespond.h"

#include "Protocol/CSetFeedBackRespond.h"
#include "Protocol/CGetFeedBackByProcessRequest.h"
#include "Protocol/CGetFeedBackByProcessRespond.h"

#include "Protocol/CAppBackAlarmRequest.h"
#include "Protocol/CAppBackAlarmResponse.h"
#include "Protocol/CAlarmLogSync.h"

#include "Protocol/CAppGetAlarmIdRequest.h"
#include "Protocol/CAppGetAlarmIdResponse.h"
#include "Protocol/CAppGetAlarmIdFromICCRequest.h"
#include "Protocol/CAddOrUpdateAlarmWithProcessRequest.h"

#include "Protocol/CAppAlarmProcessSync.h"

#include <Protocol/CRedisStaff.h>

#define MODULE_NAME "MPA"

using namespace ICC;