#pragma once

#include <boost/thread.hpp>

#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <algorithm>

#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>
#include <Redisclient/IRedisClientFactory.h>
#include <AmqClient/IAmqClient.h>
#include <Observer/IObserverFactory.h>
#include <Lock/ILockFactory.h>
#include <Log/ILogFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Json/IJsonFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <Config/IConfigFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <Thread/IThreadFactory.h>
#include <Udp/IUdpFactory.h>
#include <Xml/IXmlFactory.h>
#include <Timer/ITimerFactory.h>
#include <HelpTool/HelpToolFactory.h>

#include "Protocol/CCheckUserRequest.h"
#include "Protocol/CTIServer/CallStateEvent.h"
#include "Protocol/CTIServer/DeviceStateEvent.h"
#include "Protocol/CTIServer/CallToMrcc.h"
#include "Protocol/CTIServer/CallOverEvent.h"
#include "Protocol/CAlarmSync.h"

#include <Protocol/SyncServerLock.h>

#define MODULE_NAME "mrps"

using namespace ICC;
using namespace PROTOCOL;
