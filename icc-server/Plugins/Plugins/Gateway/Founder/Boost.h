#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

// boost
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
// icc
#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <AmqClient/IAmqClient.h>
#include <Config/IConfigFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <Json/IJsonFactory.h>
#include <Lock/ILockFactory.h>
#include <Log/ILogFactory.h>
#include <Observer/IObserverFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Redisclient/IRedisClientFactory.h>
#include <HttpClient/IHttpClientFactory.h>
#include <HelpTool/HelpToolFactory.h>
#include <SqlBuilder/ISqlBuilderFactory.h>
#include <SqlBuilder/ISqlRequestFactory.h>
#include <PGClient/IPGClientFactory.h>
#include <PGClient/IResultSetFactory.h>

// Protocol
#include <Protocol/CAddOrUpdateAlarmWithProcessRequest.h>
#include <Protocol/CAddOrUpdateProcessRequest.h>
#include <Protocol/CSetFeedbackRequest.h>
#include <Protocol/FeedbackRequest.h>
#include "Protocol/CAlarmSync.h"
#include "Protocol/CAlarmProcessSync.h"
#include "Protocol/CAlarmLogSync.h"
#include <Protocol/CAlarmRelatedCarsProtocol.h>

#define MODULE_NAME "founder"

using namespace ICC;