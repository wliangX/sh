#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

// boost
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast/lexical_cast_old.hpp>
#include "boost/program_options.hpp"
#include "boost/asio.hpp"

// icc
#include <ICC/ICCLibExport.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <AmqClient/IAmqClient.h>
#include <Config/IConfigFactory.h>
#include <Json/IJsonFactory.h>
#include <Lock/ILockFactory.h>
#include <Log/ILogFactory.h>
#include <Observer/IObserverFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Thread/IThreadFactory.h>
#include <License/LicenselFactory.h>
#include <Timer/ITimerFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <Redisclient/IRedisClientFactory.h>
#include <HttpsServer/IHttpsServer.h>
#include <HttpsServer/IHttpsServerFactory.h>
#include <HttpServer/IHttpServer.h>
#include <HttpServer/IHttpServerFactory.h>
#include "HelpTool/HelpToolFactory.h"
#include <HttpClient/IHttpClientFactory.h>
#include <DBConn/IDBConnFactory.h>
// Protocol
#include <Protocol/CHeader.h>
#include <Protocol/SyncServerLock.h>
#include <Protocol/SyncNacosParams.h>
#include <Protocol/SyncReloadConfigSuccess.h>

#define MODULE_NAME "message"

using namespace ICC;
