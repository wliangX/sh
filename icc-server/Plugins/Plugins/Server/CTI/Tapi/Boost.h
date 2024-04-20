#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>

#include <ICC/ICCLibExport.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <Observer/IObserverFactory.h>
#include <Lock/ILockFactory.h>
#include <Log/ILogFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Redisclient/IRedisClientFactory.h>
#include <Thread/IThreadFactory.h>
#include <Json/IJsonFactory.h>
#include <Config/IConfigFactory.h>
#include <Log/ILogFactory.h>
#include <SqlBuilder/ISqlBuilder.h>
#include <DBConn/IDBConnFactory.h>

#include <DateTime/IDateTime.h>
#include <DateTime/IDateTimeFactory.h>
#include <Timer/ITimerFactory.h> 

#include <HelpTool/HelpToolFactory.h>
#include <Udp/IUdpFactory.h>
#include <windows.h>
#include <tapi.h>

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <ctime>
#include <mutex>
#include <thread>

//
#include <CTIServer/IProtocolDefine.h>
#include <CTIServer/ISwitchDefine.h>
#include <CTIServer/ISwitchNotifaction.h>
#include <CTIServer/ISwitchClientFactory.h>
#include <CTIServer/ISwitchEventCallback.h>

// protocol
#include <Protocol/CHeader.h>
#include <Protocol/CAlarmLogSync.h>
#include <Protocol/CTIServer/CTIConnStateEvent.h>
#include <Protocol/CTIServer/CallStateEvent.h>
#include <Protocol/CTIServer/CallOverEvent.h>
#include <Protocol/CTIServer/DeviceStateEvent.h>
#include <Protocol/CTIServer/AgentStateEvent.h>
#include <Protocol/CTIServer/RefuseCallEvent.h>

#include <Protocol/CTIServer/GetCTIConnStateRequest.h>
#include <Protocol/CTIServer/GetCTIConnStateRespond.h>
#include <Protocol/CTIServer/GetACDListRequest.h>
#include <Protocol/CTIServer/GetACDListRespond.h>
#include <Protocol/CTIServer/GetAgentListRequest.h>
#include <Protocol/CTIServer/GetAgentListRespond.h>
#include <Protocol/CTIServer/GetDeviceListRequest.h>
#include <Protocol/CTIServer/GetDeviceListRespond.h>
#include <Protocol/CTIServer/GetCallListRequest.h>
#include <Protocol/CTIServer/GetCallListRespond.h>
#include <Protocol/CTIServer/GetReadyAgentRequest.h>
#include <Protocol/CTIServer/GetReadyAgentRespond.h>
#include <Protocol/CTIServer/CallEventQueryRequest.h>
#include <Protocol/CTIServer/CallEventQueryRespond.h>
#include <Protocol/CTIServer/BlackCallQueryRequest.h>
#include <Protocol/CTIServer/BlackCallQueryRespond.h>

#include <Protocol/CTIServer/GetCurrentTimeRespond.h>

#include <Protocol/CTIServer/ClientRegisterSync.h>
#include <Protocol/CTIServer/AgentStateEvent.h>
#include <Protocol/CTIServer/AgentLoginModeRequest.h>
#include <Protocol/CTIServer/AgentLoginModeRespond.h>
#include <Protocol/CTIServer/SetAgentStateRequest.h>
#include <Protocol/CTIServer/SetAgentStateRespond.h>

#include <Protocol/CTIServer/SetBlackListRequest.h>
#include <Protocol/CTIServer/SetBlackListRespond.h>
#include <Protocol/CTIServer/SetBlackListSync.h>
#include <Protocol/CTIServer/DeleteBlackListRequest.h>
#include <Protocol/CTIServer/DeleteBlackListRespond.h>
#include <Protocol/CTIServer/DeleteBlackListSync.h>
#include <Protocol/CTIServer/DeleteAllBlackListRequest.h>
#include <Protocol/CTIServer/DeleteAllBlackListRespond.h>
#include <Protocol/CTIServer/DeleteAllBlackListSync.h>
#include <Protocol/CTIServer/GetAllBlackListRequest.h>
#include <Protocol/CTIServer/GetAllBlackListRespond.h>
#include <Protocol/CTIServer/LoadAllBlackListRequest.h>
#include <Protocol/CTIServer/LoadAllBlackListRespond.h>

#include <Protocol/CTIServer/MakeCallRequest.h>
#include <Protocol/CTIServer/MakeCallRespond.h>
#include <Protocol/CTIServer/CallBackSync.h>
#include <Protocol/CTIServer/AnswerCallRequest.h>
#include <Protocol/CTIServer/AnswerCallRespond.h>
#include <Protocol/CTIServer/HangupRequest.h>
#include <Protocol/CTIServer/HangupRespond.h>
#include <Protocol/CTIServer/ClearCallRequest.h>
#include <Protocol/CTIServer/ClearCallRespond.h>
#include <Protocol/CTIServer/ListenCallRequest.h>
#include <Protocol/CTIServer/ListenCallRespond.h>
#include <Protocol/CTIServer/PickupCallRequest.h>
#include <Protocol/CTIServer/PickupCallRespond.h>
#include <Protocol/CTIServer/BargeInCallRequest.h>
#include <Protocol/CTIServer/BargrInCallRespond.h>
#include <Protocol/CTIServer/ForcepopCallRequest.h>
#include <Protocol/CTIServer/ForcepopCallRespond.h>
#include <Protocol/CTIServer/ConsultationCallRequest.h>
#include <Protocol/CTIServer/ConsultationCallRespond.h>
#include <Protocol/CTIServer/TransferCallRequest.h>
#include <Protocol/CTIServer/TransferCallRespond.h>
#include <Protocol/CTIServer/DeflectCallRequest.h>
#include <Protocol/CTIServer/DeflectCallRespond.h>
#include <Protocol/CTIServer/HoldCallRequest.h>
#include <Protocol/CTIServer/HoldCallRespond.h>
#include <Protocol/CTIServer/RetrieveCallRequest.h>
#include <Protocol/CTIServer/RetrieveCallRespond.h>
#include <Protocol/CTIServer/ReConnectCallRequest.h>
#include <Protocol/CTIServer/ReConnectCallRespond.h>
#include <Protocol/CTIServer/ConferenceCallRequest.h>
#include <Protocol/CTIServer/ConferenceCallRespond.h>
#include <Protocol/CTIServer/MakeConferenceRequest.h>
#include <Protocol/CTIServer/MakeConferenceRespond.h>
#include <Protocol/CTIServer/AddConferencePartyRequest.h>
#include <Protocol/CTIServer/AddConferencePartyRespond.h>
#include <Protocol/CTIServer/GetConferencePartyRequest.h>
#include <Protocol/CTIServer/GetConferencePartyRespond.h>
#include <Protocol/CTIServer/DeleteConferencePartyRequest.h>
#include <Protocol/CTIServer/DeleteConferencePartyRespond.h>
#include <Protocol/CTIServer/ConferenceSync.h>
#include <Protocol/CTIServer/RemoveReleaseCallRequest.h>
#include <Protocol/CTIServer/RemoveReleaseCallRespond.h>
#include <Protocol/CTIServer/RemoveReleaseCallSync.h>
#include <Protocol/CAgentTimeoutSync.h>
#include <Protocol/SyncServerLock.h>

#include <Protocol/AcdInfo.h>
#define MODULE_NAME "tapi"

using namespace std;
using namespace ICC;

#define	SAFE_LOCK(lk) std::lock_guard<std::mutex> l_guard(lk)

static void ShowMsg(const std::string& strMessage)
{
#ifdef _DEBUG
	std::cout << strMessage.c_str() << std::endl;
//	printf("%s\r\n", strMessage.c_str());
#endif
}