#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

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
#include <Timer/ITimerFactory.h>
#include<Thread/IThreadFactory.h>
#include <Threadpool/IThreadPoolFactory.h>
#include <Redisclient/IRedisClientFactory.h>
#include <HelpTool/HelpToolFactory.h>
// boost
#include <boost/regex.h>
#include <boost/regex_fwd.hpp>

// Protocol
#include <Protocol/CAlarmTransportAcceptRequest.h>
#include <Protocol/CAlarmTransportAcceptRespond.h>
#include <Protocol/CAlarmTransportRequest.h>
#include <Protocol/CTransPortAlarmRespond.h>
#include <Protocol/GetDeviceListRequestAndRespond.h>
#include <Protocol/CTransportAlarmSync.h>
#include <Protocol/CTransportAcceptAlarmSync.h>
#include <Protocol/CTransportDefectCallRequest.h>
#include <Protocol/CTransportDefectCallRespond.h>
#include <Protocol/CTransferCallRespond.h>
#include <Protocol/CTransportGetNewAlarmIDRespond .h>
#include <Protocol/CAddOrUpdateAlarmWithProcessRequest.h>
#include <Protocol/CGetTransportSendRequest.h>
#include <Protocol/CAlarmLogSync.h>
#include <Protocol/Timer.h>
#include <Protocol/CAlarmSync.h>
#include <Protocol/CGetSeatRespond.h>
#include <Protocol/CTIServer/CallInfo.h>

#include <Protocol/CGetAlarmLogRespond.h>

#include <Protocol/CRedisRegisterInfo.h>
#include <Protocol/CRedisStaff.h>
#include <Protocol/DeptCodeInfo.h>

#define MODULE_NAME "transportalarm"

using namespace ICC;