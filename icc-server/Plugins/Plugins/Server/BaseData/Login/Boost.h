#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

// c++
#include <iostream>
#include <string>
#include <map>

// icc
#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <Redisclient/IRedisclient.h>
#include <AmqClient/IAmqClient.h>
#include <Observer/IObserverFactory.h>
#include <Lock/ILockFactory.h>
#include <Log/ILogFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Json/IJsonFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <Config/IConfigFactory.h>
#include <HelpTool/IHelpTool.h>
#include <HelpTool/HelpToolFactory.h>
#include <Config/IConfigFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <Observer/IObserverFactory.h>
#include <Log/ILogFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <Json/IJsonFactory.h>
#include <Thread/IThreadFactory.h>
#include <Redisclient/IRedisClientFactory.h>
#include <StringUtil/IStringFactory.h>
#include <SqlBuilder/ISqlBuilderFactory.h>
#include <PGClient/IPGClientFactory.h>
#include <SqlBuilder/ISqlBuilder.h>
#include <Thread/IThreadFactory.h>
#include <Timer/ITimerFactory.h>
// protocol
#include <Protocol/CClientRegisterRequest.h>
#include <Protocol/CClientRegisterRespond.h>
#include <Protocol/CGetClientRequest.h>
#include <Protocol/CGetClientRespond.h>
#include <Protocol/CClientRegisterSync.h>
#include <Protocol/CConnect.h>
#include <Protocol/CDisConnect.h>
#include <Protocol/GetServerStatus.h>
#include <Protocol/GetOnOffInfo.h>
#include <Protocol/COnOffSync.h>
#include <Protocol/CGetClientInOutRequest.h>
#include <Protocol/CGetClientInOutRespond.h>
#include <Protocol/CAgentStateSyncRequest.h>
#include <Protocol/CDeleteUserRequest.h>
#include <Protocol/CDeleteUserRespond.h>
#include <Protocol/CCheckUserRequest.h>
#include <Protocol/CCheckUserRespond.h>
#include <Protocol/CSetUserRequest.h>
#include <Protocol/CSetUserRespond.h>
#include <Protocol/CGetUserRequest.h>
#include <Protocol/CGetUserRespond.h>
#include <Protocol/CLoadConfigRequest.h>
#include <Protocol/CLoadConfigRespond.h>
#include <Protocol/CUploadConfigRequest.h>
#include <Protocol/CUploadConfigRespond.h>
#include <Protocol/CUpdateParamRequest.h>
#include <Protocol/CUpdateParamRespond.h>
#include <Protocol/CGetAllParamRequest.h>
#include <Protocol/CUpdateParamSync.h>
#include <Protocol/CSetUserSync.h>
#include <Protocol/ServerStatusSyn.h>
#include <Protocol/CBindIpSeatRequest.h>
#include <Protocol/CGetBindIpSeatRespond.h>
#include <Protocol/CSimpleRespond.h>

#include <Protocol/SyncServerLock.h>
#include <Protocol/CClientHeartbeatRespond.h>
#include <sstream>
#include <Protocol/CAgentTimeoutSync.h>

#include <Protocol/CQueryFreeSeatRequest.h>
#include <Protocol/CQueryFreeSeatRespond.h>

#include <Protocol/CQueryPreSeatBindByUserRequest.h>
#include <Protocol/CQueryPreSeatBindByUserRespond.h>

#include <Protocol/ParamInfo.h>

#define MODULE_NAME "login"

using namespace ICC;
