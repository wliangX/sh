#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <Redisclient/IRedisClientFactory.h>
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

#include "Protocol/CGetPhoneInfoRequest.h"
#include "Protocol/CGetPhoneInfoRespond.h"
#include "Protocol/CGetPhoneInfoByPhoneRequest.h"
#include "Protocol/CGetPhoneInfoByPhoneRespond.h"
#include "Protocol/CSetPhoneInfoRequest.h"
#include "Protocol/CSetPhoneInfoRespond.h"
#include "Protocol/CDeletePhoneInfoRequest.h"
#include "Protocol/CDeletePhoneInfoRespond.h"
#include "Protocol/CPhoneInfoSync.h"

#define MODULE_NAME "phoneinfo"

using namespace ICC;
