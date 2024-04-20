#pragma once

// icc
#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <Lock/ILockFactory.h>
#include <Observer/IObserverFactory.h>
#include <AmqClient/IAmqClient.h>
#include <Config/IConfigFactory.h>
#include <Log/ILogFactory.h>
#include <Lock/ILockFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <Json/IJsonFactory.h>
#include <StringUtil/IStringFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <Redisclient/IRedisClientFactory.h>
#include <Protocol/CPhoneMarkSync.h>
#include <Protocol/CDeletePhoneMarkRequest.h>
#include <Protocol/CDeletePhoneMarkRespond.h>
#include <Protocol/CGetAllPhoneMarkRequest.h>
#include <Protocol/CGetPhoneMarkRespond.h>
#include <Protocol/CSetPhoneMarkRequest.h>
#include <Protocol/CSetPhoneMarkRespond.h>
#include <Protocol/CGetPhoneMarkRequest.h>

#define MODULE_NAME "phonemark"

using namespace ICC;