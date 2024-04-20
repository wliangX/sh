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

#include "Protocol/CSetDeptRequest.h"
#include "Protocol/CSetDeptRespond.h"
#include "Protocol/CGetDeptRequest.h"
#include "Protocol/CGetSubDeptRequest.h"
#include "Protocol/CGetDeptRespond.h"
#include "Protocol/CDeleteDeptRequest.h"
#include "Protocol/CDeleteDeptRespond.h"
#include "Protocol/CDeptSync.h"

#define MODULE_NAME "dept"

using namespace ICC;
