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

// Protocol
#include "Protocol/CGPSGetGpsRequest.h"
#include "Protocol/CGPSGetGpsRespond.h"
#include "Protocol/CGPSGetPhoneInfoRequest.h"
#include "Protocol/CGPSGetPhoneInfoRespond.h"
#include "Protocol/CGPSSetPhoneInfoRequest.h"
#include "Protocol/CGPSSetPhoneInfoRespond.h"
#include "Protocol/CGPSGetDivisionRequest.h"
#include "Protocol/CGPSGetDivisionRespond.h"
#include "Protocol/CGPSSetDivisionRequest.h"
#include "Protocol/CGPSSetDivisionRespond.h"
#include "Protocol/CGpsSync.h"
#include "Protocol/CGPSPhoneInfoSync.h"
#include "Protocol/CGPSDivisionSync.h"
#include "Protocol/CGPSGetBJRDHGpsRequest.h"
#include "Protocol/CGPSGetBJRDHGpsRespond.h"
#include "Protocol/CBJRDHPositionRequest.h"
#include "Protocol/CBJRDHPositionRespond.h"
#include "Protocol/CBJRDHTokenRequest.h"
#include "Protocol/CBJRDHTokenRespond.h"
#include <Protocol/SyncNacosParams.h>
#include <Thread/IThreadFactory.h>
#define MODULE_NAME "gps"

using namespace ICC;