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
#include <Timer/ITimerFactory.h>

// Protocol


#define MODULE_NAME "AlarmStatistics"

using namespace ICC;


