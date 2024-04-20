#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <mutex>
#include <vector>
#include <unordered_map>

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

#include "DictInfo.h"
#include "Protocol/CSetDictRequest.h"
#include "Protocol/CSetDictRespond.h"
#include "Protocol/CGetDictRequest.h"
#include "Protocol/CGetDictRespond.h"
#include "Protocol/CDeleteDictRequest.h"
#include "Protocol/CDeleteDictRespond.h"
#include "Protocol/CDictSync.h"

#define MODULE_NAME "dict"

using namespace ICC;
