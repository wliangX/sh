#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <ICC/ICCLibExport.h>
#include <ICC/ICCSetResource.h>
#include <ICC/ICCGetResource.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCExceptionDefaultHandle.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>
#include <MessageCenter/IMessageCenter.h>
#include <MessageCenter/IMessageCenterFactory.h>

#include <Log/ILogFactory.h>
#include <Config/IConfigFactory.h>
#include <StringUtil/IStringFactory.h>
#include <HttpClient/IHttpClientFactory.h>
#include <Json/IJsonFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <Redisclient/IRedisClientFactory.h>

#include <vector>
#include <string>
#include <map>
#include <boost/chrono.hpp>
#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>

#include <Protocol/CRedisRegisterInfo.h>
#include <Protocol/DeviceDataInfo.h>
#define MODULE_NAME "messagecenter"
#include <Protocol/ParamInfo.h>

using namespace ICC;
using namespace MsgCenter;