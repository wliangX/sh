#pragma once

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
#include <StringUtil/IStringUtil.h>
#include <HelpTool/HelpToolFactory.h>
#include <Lock/ILockFactory.h>
#include <Timer/ITimerFactory.h>
#include <Protocol/IRequest.h>


// Protocol

// define
#define MODULE_NAME				"ifds"
#define SYSTEMID                "ICC"
#define SUBSYSTEMID             "ICC-ifds"

#define REQESTMODE_QUEUE        "0"
#define REQESTMODE_TOPIC        "1"

using namespace ICC;