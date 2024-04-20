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
#include <Redisclient/IRedisClientFactory.h>

#include "Protocol/CGetFeedBackRequest.h"
#include "Protocol/CGetFeedBackRespond.h"
#include "Protocol/CGetFeedBackByProcessRequest.h"
#include "Protocol/CGetFeedBackByProcessRespond.h"
#include "Protocol/CSetFeedbackRequest.h"
#include "Protocol/CSetFeedBackRespond.h"
#include "Protocol/CFeedBackSync.h"
#include "Protocol/CAlarmLogSync.h"
#include "Protocol/CAlarmSync.h"
#include "Protocol/CAlarmProcessSync.h"
#include "Protocol/CGetAlarmLogFeedBackResourceRequest.h"
#include "Protocol/CGetAlarmLogFeedBackResourceRespond.h"

#include <Protocol/CRedisStaff.h>

#define MODULE_NAME "feedback"

using namespace ICC;
