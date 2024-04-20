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

#include "LanguageInfo.h"
#include "Protocol/CSetLanguageRequest.h"
#include "Protocol/CSetLanguageRespond.h"
#include "Protocol/CGetLanguageRequest.h"
#include "Protocol/CGetLanguageRespond.h"
#include "Protocol/CDeleteLanguageRequest.h"
#include "Protocol/CDeleteLanguageRespond.h"
#include "Protocol/CLanguageSync.h"

#define MODULE_NAME "language"

using namespace ICC;
