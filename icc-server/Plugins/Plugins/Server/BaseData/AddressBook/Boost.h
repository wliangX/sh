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

#include "Protocol/CSetBookOrgRequest.h"
#include "Protocol/CGetBookOrgRespond.h"

//#include <Protocol/CGetStaffRequest.h>
#include <Protocol/CGetBookPhoneRespond.h>
#include <Protocol/CSetBookPhoneRequest.h>
/*
#include <Protocol/CSetStaffRequest.h>
#include <Protocol/CSetStaffRespond.h>
#include <Protocol/CDeleteStaffRequest.h>
#include <Protocol/CDeleteStaffRespond.h>*/

#include <Protocol/CBaseRequest.h>
#include <Protocol/CBaseRespond.h>
#include <Protocol/CGetContactGrpRespond.h>
#include <Protocol/CSetContactGrpRequest.h>
#include <Protocol/CDeleteContactGrpRequest.h>

#include <Protocol/CGetContactInfoRespond.h>
#include <Protocol/CSetContactInfoRequest.h>
#include <Protocol/CDeleteContactInfoRequest.h>

#define MODULE_NAME "addressbook"

using namespace ICC;
