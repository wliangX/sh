#pragma once


#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <queue>
#include <vector>

// c++
#include <iostream>
#include <string>
#include <map>

#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <AmqClient/IAmqClient.h>
#include <Config/IConfigFactory.h>
#include <Log/ILogFactory.h>
#include <Lock/ILockFactory.h>
#include <Json/IJsonFactory.h>
#include <StringUtil/IStringFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <Lock/ILockFactory.h>
#include <Observer/IObserverFactory.h>
#include <Timer/ITimerFactory.h>
#include <Xml/IXmlFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <Redisclient/IRedisClientFactory.h>
#include <HttpClient/IHttpClientFactory.h>

#include <Protocol/CGetBJRSFXXRequest.h>
#include <Protocol/CGetBJRSFXXRespond.h>
#include <Protocol/CGetBJRSFZHRequest.h>
#include <Protocol/CGetBJRSFZHRespond.h>

#define MODULE_NAME "IdentityInfo"


using namespace ICC;