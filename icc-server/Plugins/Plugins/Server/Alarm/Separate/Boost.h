#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

//boost
#include <boost/thread.hpp>

#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <Config/IConfigFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <Json/IJsonFactory.h>
#include <Lock/ILockFactory.h>
#include <Log/ILogFactory.h>
#include <Observer/IObserverFactory.h>
#include <StringUtil/IStringFactory.h>
#include <HelpTool/HelpToolFactory.h>
#include <Timer/ITimerFactory.h>
#include <Redisclient/IRedisClientFactory.h>

// Protocol
#include <Protocol/CAlarmSync.h>
#include <Protocol/CAddOrUpdateAlarmRequest.h>
#include <Protocol/CAddOrUpdateAlarmRespond.h>
#include <Protocol/CGetAlarmLogAlarmResourceRequest.h>
#include <Protocol/CGetAlarmLogAlarmResourceRespond.h>
#include <Protocol/CAlarmLogSync.h>

#include <Protocol/CAddAlarmRemarkRequest.h>
#include <Protocol/CAddAlarmRemarkRespond.h>
#include <Protocol/CGetAlarmRemarkRequest.h>
#include <Protocol/CGetAlarmRemarkRespond.h>
#include <Protocol/CGetAlarmAllRemarkRequest.h>
#include <Protocol/CGetAlarmAllRemarkRespond.h>
#include <Protocol/CAddAlarm.h>
#include <Protocol/CGetAlarmLogCallrefRequest.h>
#include <Protocol/CGetAlarmLogCallrefRespond.h>
#include <Protocol/CAlarmAssignRequest.h>
#include <Protocol/CAlarmAssignRespond.h>
#include <Protocol/CAddOrUpdateProcessRequest.h>
#include <Protocol/CAddOrUpdateProcessRespond.h>
#include <Protocol/CAddOrUpdateAlarmWithProcessRequest.h>
#include <Protocol/CAddOrUpdateAlarmWithProcessRespond.h>
#include <Protocol/CAlarmProcessSync.h>
#include <Protocol/CSeparate.h>

#include <Protocol/CProcessDoneRequest.h>
#include <Protocol/CProcessDoneRespond.h>
#include <Protocol/CGetProcessCashRequest.h>
#include <Protocol/CGetProcessCashRespond.h>
#include <Protocol/CEditAgainRequest.h>
#include <Protocol/CEditAgainRespond.h>
#include <Protocol/CRedisStaff.h>

#include "Define.h"

#define MODULE_NAME "separate"

using namespace ICC;
using namespace ICC::Separate;
