#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <map>

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

#include "Protocol/CGetSeatRequest.h"
#include "Protocol/CGetSeatRespond.h"
#include "Protocol/CGetSeatByDeptRequest.h"
#include "Protocol/CGetSeatByDeptRespond.h"
#include "Protocol/CSetSeatRequest.h"
#include "Protocol/CSetSeatRespond.h"
#include "Protocol/CDeleteSeatRequest.h"
#include "Protocol/CDeleteSeatRespond.h"
#include "Protocol/CSeatSync.h"

#define MODULE_NAME "seat"

using namespace ICC;
