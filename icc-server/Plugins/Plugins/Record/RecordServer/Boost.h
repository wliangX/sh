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
#include <Redisclient/IRedisClientFactory.h>
#include <Xml/IXml.h>
#include <Xml/IXmlFactory.h>
#include <FtpClient/IFtpClient.h>
#include <FtpClient/IFtpClientFactory.h>

#include "Protocol/CTIServer/CallStateEvent.h"
#include "Protocol/CTIServer/DeviceStateEvent.h"
#include "Protocol/CTIServer/CallOverEvent.h"
#include "Protocol/CTIServer/CTIConnStateEvent.h"
#include "Protocol/CAlarmSync.h"
#include "Protocol/CRecordFileUploadSync.h"
#include "Protocol/SyncServerLock.h"

// Protocol

// define
#define		MODULE_NAME				            "record_server"
#define     RECORDSERVER_OBSERVER_NAME          "record_observer"

using namespace ICC;