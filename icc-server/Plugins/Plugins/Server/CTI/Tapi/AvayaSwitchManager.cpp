#include "Boost.h"

#include "AvayaSwitchManager.h"
#include "SysConfig.h"
#include "DeviceManager.h"
#include "AgentManager.h"
#include "BlackListManager.h"
#include "AvayaEvent.h"
#include "AvayaResult.h"
#include "Task.h"
#include "TaskManager.h"
#include "CallManager.h"
#include "HytProtocol.h"
#include "ConferenceManager.h"
#include "TransferCallManager.h"

#define MAX_HEARTBEAT		3		//	心跳检测次数
#define CHECK_TIMESPAN		1		//	单位：秒
#define SLEEP_TIMESPAN		100		//	单位：毫秒

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
boost::shared_ptr<CAvayaSwitchManager> CAvayaSwitchManager::m_pInstance = nullptr;
CAvayaSwitchManager::CAvayaSwitchManager()
{
	m_pWindowsMsgThread = nullptr;
	m_pCmdTaskThread = nullptr;
	m_pSwitchEventTaskThread = nullptr;
	m_pCheckExcusedTaskThread = nullptr;

	m_bCmdTaskThreadAlive = false;
	m_bCheckExcusedTaskThreadAlive = false;
	m_bSwitchEventTaskThreadAlive = false;
	m_bOpenTapiThreadAlive = false;

	m_bSwitchConnect = false;
	m_bExitSys = false;

	m_oLastCheckExcusedTime = 0;
}
CAvayaSwitchManager::~CAvayaSwitchManager()
{
	//	m_bExitSys = true;
}    

boost::shared_ptr<CAvayaSwitchManager> CAvayaSwitchManager::Instance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = boost::make_shared<CAvayaSwitchManager>();
	}

	return m_pInstance;
}
void CAvayaSwitchManager::ExitInstance()
{
	m_mapCmdNameToSwitch.clear();
	m_mapSwitchEvent.clear();
}

void CAvayaSwitchManager::OnInit()
{
	CAvayaEvent::Instance()->SetLogPtr(m_pLog);
	CAvayaEvent::Instance()->SetDateTimePtr(m_pDateTime);
	CAvayaEvent::Instance()->SetStringUtilPtr(m_pStrUtil);

	CAvayaResult::Instance()->SetStringUtilPtr(m_pStrUtil);

	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_QueryDeviceInfo, &CAvayaSwitchManager::QueryDeviceInfo));

	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_MonitorDevice, &CAvayaSwitchManager::MonitorDevice));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_MonitorCallVirDevice, &CAvayaSwitchManager::MonitorCallsViaDevice));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_RouteRequest, &CAvayaSwitchManager::RouteRequest));

	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_AgentLogin, &CAvayaSwitchManager::AgentLogin));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_AgentLogout, &CAvayaSwitchManager::AgentLogout));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_SetAgentState, &CAvayaSwitchManager::SetAgentState));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_QueryAgentState, &CAvayaSwitchManager::QueryAgentState));

	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_MakeCall, &CAvayaSwitchManager::MakeCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_Hangup, &CAvayaSwitchManager::Hangup));
	//m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_ForceHangup, &CAvayaSwitchManager::ForceHangup));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_AnswerCall, &CAvayaSwitchManager::AnswerCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_RefuseAnswer, &CAvayaSwitchManager::RefuseAnswer));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_ClearCall, &CAvayaSwitchManager::ClearCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_RefuseBlackCall, &CAvayaSwitchManager::RefuseBlackCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_ListenCall, &CAvayaSwitchManager::ListenCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_PickupCall, &CAvayaSwitchManager::PickupCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_BargeInCall, &CAvayaSwitchManager::BargeInCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_ForcePopCall, &CAvayaSwitchManager::ForcePopCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_ConsultationCall, &CAvayaSwitchManager::ConsultationCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_TransferCall, &CAvayaSwitchManager::TransferCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_DeflectCall, &CAvayaSwitchManager::DeflectCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_HoldCall, &CAvayaSwitchManager::HoldCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_RetrieveCall, &CAvayaSwitchManager::RetrieveCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_ReconnectCall, &CAvayaSwitchManager::ReconnectCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_ConferenceCall, &CAvayaSwitchManager::ConferenceCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_AddConferenceParty, &CAvayaSwitchManager::AddConferenceParty));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_DeleteConferenceParty, &CAvayaSwitchManager::DeleteConferenceParty));

	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_GetCTIConnState, &CAvayaSwitchManager::GetCTIConnState));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_GetDeviceList, &CAvayaSwitchManager::GetDeviceList));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_GetACDList, &CAvayaSwitchManager::GetACDList));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_GetAgentList, &CAvayaSwitchManager::GetAgentList));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_GetCallList, &CAvayaSwitchManager::GetCallList));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_GetReadyAgent, &CAvayaSwitchManager::GetReadyAgent));

	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_SetBlackList, &CAvayaSwitchManager::SetBlackList));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_DeleteBlackList, &CAvayaSwitchManager::DeleteBlackList));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_DeleteAllBlackList, &CAvayaSwitchManager::DeleteAllBlackList));

	//////////////////////////////////////////////////////////////////////////
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_UniversalFailRet, &CAvayaSwitchManager::UniversalFailRet));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_ConnectAesEvent, &CAvayaSwitchManager::OpenStreamConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_FailedEvent, &CAvayaSwitchManager::FailedEvent));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_AgentStateEvent, &CAvayaSwitchManager::AgentStateEvent));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_DeviceStateEvent, &CAvayaSwitchManager::DeviceStateEvent));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_ConferenceHangupEvent, &CAvayaSwitchManager::ConferenceHangupEvent));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_CallStateEvent, &CAvayaSwitchManager::CallStateEvent));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_CallOverEvent, &CAvayaSwitchManager::CallOverEvent));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_RefuseCallEvent, &CAvayaSwitchManager::RefuseCallEvent));

	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_QueryAgentStateRet, &CAvayaSwitchManager::QueryAgentStateRet));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_SetAgentStateConf, &CAvayaSwitchManager::SetAgentStateConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_MakeCallConf, &CAvayaSwitchManager::MakeCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_AnswerCallConf, &CAvayaSwitchManager::AnswerCallConf));
	//	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_RefuseAnswerConf, &CAvayaSwitchManager::RefuseAnswerConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_ClearConnectionConf, &CAvayaSwitchManager::ClearConnectionConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_SingleStepConferenceConf, &CAvayaSwitchManager::SingleStepConferenceConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_PickupCallConf, &CAvayaSwitchManager::PickupCallConf));
	//	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_BargeInCallConf, &CAvayaSwitchManager::BargeInCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_ClearCallConf, &CAvayaSwitchManager::ClearCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_ConsultationCallConf, &CAvayaSwitchManager::ConsultationCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_TransferCallConf, &CAvayaSwitchManager::TransferCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_DeflectCallConf, &CAvayaSwitchManager::DeflectCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_HoldCallConf, &CAvayaSwitchManager::HoldCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_RetrieveCallConf, &CAvayaSwitchManager::RetrieveCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_ReconnectCallConf, &CAvayaSwitchManager::ReconnectCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_ConferenceCallConf, &CAvayaSwitchManager::ConferenceCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_AddConferencePartyConf, &CAvayaSwitchManager::AddConferencePartyConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_DeleteConferencePartyConf, &CAvayaSwitchManager::DeleteConferencePartyConf));

	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_GetCTIConnStateRet, &CAvayaSwitchManager::GetCTIConnStateRet));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_GetDeviceListRet, &CAvayaSwitchManager::GetDeviceListRet));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_GetACDListRet, &CAvayaSwitchManager::GetACDListRet));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_GetAgentListRet, &CAvayaSwitchManager::GetAgentListRet));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_GetCallListRet, &CAvayaSwitchManager::GetCallListRet));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_GetReadyAgentRet, &CAvayaSwitchManager::GetReadyAgentRet));
}
void CAvayaSwitchManager::OnStart()
{
	OpenStream();

	StartCmdTaskThread();
	StartSwitchEventTaskThread();
	StartCheckExcusedTaskThread();
}
void CAvayaSwitchManager::OnStop()
{
	if (m_bSwitchConnect)
	{
		this->CloseStream();
	}

	StopCheckExcusedTaskThread();
	StopSwitchEventTaskThread();
	StopCmdTaskThread();
}

extern bool g_bCTITestStart;
extern bool g_bCTITestStartOkFlag;
//////////////////////////////////////////////////////////////////////////
void CAvayaSwitchManager::DoOpenTapiLines()
{
	ICC_LOG_DEBUG(m_pLog, "===================== OpenTapiLines Thread Start!! =====================");

	bool l_bInit = false;

	//	1.初始化
	while (!l_bInit)
	{
		if (g_bCTITestStart)
		{
			l_bInit = true;
			break;
			ICC_LOG_DEBUG(m_pLog, "------------------- g_bCTITestStart true -------------------");
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "------------------- g_bCTITestStart false -------------------");
		}

		ShowMsg("------------------- Begin InitTAPI -------------------");
		ICC_LOG_DEBUG(m_pLog, "------------------- Begin InitTAPI -------------------");
		l_bInit = CDeviceManager::Instance()->InitTAPI();
		if (!l_bInit)
		{
			ShowMsg("------------------- InitTAPI Failed !!-------------------");
			ICC_LOG_DEBUG(m_pLog, "------------------- InitTAPI Failed !! -------------------");
		//	boost::this_thread::sleep_for(boost::chrono::milliseconds(SLEEP_TIMESPAN * 100)); 时间同步会有问题
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN * 100));
		}
		else
		{
			ShowMsg("------------------- InitTAPI Success !!-------------------");
			ICC_LOG_DEBUG(m_pLog, "------------------- InitTAPI Success !! -------------------");

			break;
		}

		ICC_LOG_DEBUG(m_pLog, "------------------- End InitTAPI -------------------");

		if (!CAvayaSwitchManager::Instance()->m_bOpenTapiThreadAlive) break;
	}

	if (l_bInit)
	{
		bool l_bOpenLines = false;

		ShowMsg("------------------- Begin OpenAllAddresses -------------------");
		ICC_LOG_DEBUG(m_pLog, "------------------- Begin OpenAllAddresses -------------------");
		while (!l_bOpenLines)
		{
			if (g_bCTITestStart)
			{
				ICC_LOG_DEBUG(m_pLog, "--------tttttttttttttttttttt------ cti test!!-------------------");
				CDeviceManager::Instance()->TestOpenAllAddresses();
				this->PostConnectStatus(true);
				l_bOpenLines = true;

				g_bCTITestStartOkFlag = true;
				break;
			}

			//	2.′ò?a?ùóDí¨μà
			l_bOpenLines = CDeviceManager::Instance()->OpenAllAddresses();
			if (!l_bOpenLines)
			{
				ShowMsg("------------------- OpenAllAddresses Failed !!-------------------");
				ICC_LOG_DEBUG(m_pLog, "------------------- OpenAllAddresses Failed !! -------------------");
				this->PostConnectStatus(false);

			//	boost::this_thread::sleep_for(boost::chrono::milliseconds(SLEEP_TIMESPAN * 100));
				std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN * 100));
			}
			else
			{
				ShowMsg("------------------- OpenAllAddresses Success !!-------------------");
				ICC_LOG_DEBUG(m_pLog, "------------------- OpenAllAddresses Success !! -------------------");
				this->PostConnectStatus(true);

				break;
			}

			if (!CAvayaSwitchManager::Instance()->m_bOpenTapiThreadAlive) break;
		}

		if (l_bOpenLines)
		{
			m_bSwitchConnect = true;
			//CAgentManager::Instance()->SetDefaultAgentState();
			CAgentManager::Instance()->SetInitAgentState();
			//	3.获取 TAPI 事件
			MSG msg;
			while (::GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (!CAvayaSwitchManager::Instance()->m_bOpenTapiThreadAlive) break;
			}
		}
	}

	CDeviceManager::Instance()->CloseAllAddresses();
	CDeviceManager::Instance()->ShutdownTAPI();

	ICC_LOG_DEBUG(m_pLog, "------------------- OpenTapiLines Thread Exit!! -------------------");
}
void CAvayaSwitchManager::StartOpenTapiLinesThread()
{
	if (!m_bOpenTapiThreadAlive)
	{
		m_bOpenTapiThreadAlive = true;

		m_pWindowsMsgThread = boost::make_shared<boost::thread>(boost::bind(&CAvayaSwitchManager::DoOpenTapiLines, this));
	}
}
void CAvayaSwitchManager::StopOpenTapiThread()
{
	if (m_bOpenTapiThreadAlive)
	{
		m_bOpenTapiThreadAlive = false;

		if (m_pWindowsMsgThread)
		{
			boost::thread::id l_lTempThreadId = m_pWindowsMsgThread->get_id();
			std::stringstream l_ssThreadStream;
			l_ssThreadStream << std::hex << l_lTempThreadId;
			unsigned long l_ulThreadId;
			l_ssThreadStream >> std::hex >> l_ulThreadId;
			::PostThreadMessage(l_ulThreadId, WM_QUIT, 0, 0);

			m_pWindowsMsgThread->join();
		}
	}
}

// Open AvayaSwitch TAPI Stream
bool CAvayaSwitchManager::OpenStream()
{
	this->StartOpenTapiLinesThread();

	return true;
}

bool CAvayaSwitchManager::ReOpenStream()
{
	//1, 关闭流
	this->CloseStream();

	// 2, 重新打开一个流
	return this->OpenStream();
}

void CAvayaSwitchManager::CloseStream()
{
	m_bSwitchConnect = false;

	/*CDeviceManager::Instance()->CloseAllAddresses();
	CDeviceManager::Instance()->ShutdownTAPI();*/

	StopOpenTapiThread();
}

//////////////////////////////////////////////////////////////////////////
void CAvayaSwitchManager::ExeCommand(E_TASK_NAME p_nTaskName, long p_lTaskId, ITaskPtr p_pTask)
{
	int l_nResult = ERROR_UNKNOWN;

	std::string l_strTaskName = TaskNameString[p_nTaskName];
	//long l_lTaskId = p_pTask->GetTaskId();

	if (m_mapCmdNameToSwitch.find(p_nTaskName) != m_mapCmdNameToSwitch.end())
	{
		ICC_LOG_DEBUG(m_pLog, "Begin Cmd Task, TaskName: [%s] TaskId: [%u]",
			l_strTaskName.c_str(), p_lTaskId);

		PCmdSwitch pFunc = m_mapCmdNameToSwitch[p_nTaskName];
		l_nResult = (this->*pFunc)(p_pTask);
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "TaskName: [%s] TaskId: [%u] is not supply",
			l_strTaskName.c_str(), p_lTaskId);
	}

	if (l_nResult != RESULT_SUCCESS)
	{
		// 调用命令失败,通知应用层
		ProcessFailedCmd(p_nTaskName, p_lTaskId, l_nResult);

		ICC_LOG_ERROR(m_pLog, "End Cmd Task, Cmd Invoke failed. TaskName: [%s] TaskId: [%u] ErrorCode: [%d]",
			l_strTaskName.c_str(), p_lTaskId, l_nResult);
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "End Cmd Task, Cmd Invoke success. TaskName: [%s] TaskId: [%u]",
			l_strTaskName.c_str(), p_lTaskId);
	}
}
void CAvayaSwitchManager::DoCmdTask()
{
	ICC_LOG_DEBUG(m_pLog, "===================== Cmd Task Thread Start!! =====================");

	while (CAvayaSwitchManager::Instance()->m_bCmdTaskThreadAlive)
	{
		ITaskPtr l_pTask = CTaskManager::Instance()->GetCmdTaskHeader();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			E_TASK_NAME l_nTaskName = l_pTask->GetTaskName();

			// 添加到执行队列
			CTaskManager::Instance()->AddExcutedTask(l_pTask);

			// 执行处理流程
			CAvayaSwitchManager::Instance()->ExeCommand(l_nTaskName, l_lTaskId, l_pTask);

			CTaskManager::Instance()->DeleteCmdTask(l_lTaskId);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN * 3));
		}
	}

	ICC_LOG_DEBUG(m_pLog, "------------------ Cmd Task Thread Exit!! ------------------");
}
void CAvayaSwitchManager::StartCmdTaskThread()
{
	if (!m_bCmdTaskThreadAlive)
	{
		m_bCmdTaskThreadAlive = true;

		m_pCmdTaskThread = boost::make_shared<boost::thread>(boost::bind(&CAvayaSwitchManager::DoCmdTask, this));
	}
}
void CAvayaSwitchManager::StopCmdTaskThread()
{
	m_bCmdTaskThreadAlive = false;

	if (m_pCmdTaskThread)
	{
		m_pCmdTaskThread->join();
	}
}


void CAvayaSwitchManager::ExeSwitchEvent(E_TASK_NAME p_nTaskName, long p_lTaskId, ITaskPtr p_pTask)
{
	std::string l_strTaskName = TaskNameString[p_nTaskName];

	if (m_mapSwitchEvent.find(p_nTaskName) != m_mapSwitchEvent.end())
	{
		ICC_LOG_DEBUG(m_pLog, "Begin ExeSwitchEvent, TaskName[%s] TaskId[%u]",
			l_strTaskName.c_str(), p_lTaskId);

		PSwitchEvent l_pFunc = m_mapSwitchEvent[p_nTaskName];
		(this->*l_pFunc)(p_pTask);

		if (p_nTaskName != Task_QueryAgentStateRet/* && p_nTaskName != Task_QueryDeviceInfoRet*/)
		{
			ICC_LOG_DEBUG(m_pLog, "End ExeSwitchEvent, TaskName: [%s] TaskId: [%u]",
				l_strTaskName.c_str(), p_lTaskId);
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "End ExeSwitchEvent, TaskName: [%s] TaskId: [%u]",
				l_strTaskName.c_str(), p_lTaskId);
		}
	}
	else
	{

		ICC_LOG_ERROR(m_pLog, "End ExeSwitchEvent, TaskName: [%s] TaskId: [%u] is not supply",
			l_strTaskName.c_str(), p_lTaskId);
	}
}
void CAvayaSwitchManager::DoSwitchEventTask()
{
	ICC_LOG_DEBUG(m_pLog, "=============== DoSwitchEvent Task Thread Start!! =====================");

	while (CAvayaSwitchManager::Instance()->m_bSwitchEventTaskThreadAlive)
	{
		ITaskPtr l_pTask = CTaskManager::Instance()->GetSwitchEventTaskHeader();
		if (l_pTask)
		{
			// 执行处理流程
			long l_lTaskId = l_pTask->GetTaskId();
			E_TASK_NAME l_nTaskName = l_pTask->GetTaskName();

			CAvayaSwitchManager::Instance()->ExeSwitchEvent(l_nTaskName, l_lTaskId, l_pTask);

			CTaskManager::Instance()->DeleteSwitchEventTask(l_lTaskId);
		}
		else
		{
			//boost::this_thread::sleep_for(boost::chrono::milliseconds(SLEEP_TIMESPAN)); 时间同步会有问题
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN));
		}
	}

	ICC_LOG_DEBUG(m_pLog, "------------------ DoSwitchEvent Task Thread Exit!! ------------------");
}
void CAvayaSwitchManager::StartSwitchEventTaskThread()
{
	if (!m_bSwitchEventTaskThreadAlive)
	{
		m_bSwitchEventTaskThreadAlive = true;

		m_pSwitchEventTaskThread = boost::make_shared<boost::thread>(boost::bind(&CAvayaSwitchManager::DoSwitchEventTask, this));
	}
}
void CAvayaSwitchManager::StopSwitchEventTaskThread()
{
	m_bSwitchEventTaskThreadAlive = false;

	if (m_pSwitchEventTaskThread)
	{
		m_pSwitchEventTaskThread->join();
	}
}

void CAvayaSwitchManager::ResetCheckExcusedTime()
{
	m_oLastCheckExcusedTime = m_pDateTime->CurrentDateTime();
}
bool CAvayaSwitchManager::CheckExcusedIsTimeout()
{
	DateTime::CDateTime l_oCurrentTime = m_pDateTime->CurrentDateTime();
	DateTime::CDateTime l_oEndTime = m_pDateTime->AddSeconds(m_oLastCheckExcusedTime, CHECK_TIMESPAN);

	if (l_oCurrentTime > l_oEndTime)
	{
		return true;
	}

	return false;
}
void CAvayaSwitchManager::DoCheckExcusedTask()
{
	ICC_LOG_DEBUG(m_pLog, "=============== DoCheckExcusedTask Task Thread Start!! =====================");

	while (CAvayaSwitchManager::Instance()->m_bCheckExcusedTaskThreadAlive)
	{
		long l_lTaskId = DEFAULT_TASKID;
		E_TASK_NAME l_nTaskName = Task_NULL;

		if (CTaskManager::Instance()->ExcuseTaskTimeout(l_nTaskName, l_lTaskId))
		{
			// CMD 执行超时
			CAvayaSwitchManager::Instance()->ProcessFailedCmd(l_nTaskName, l_lTaskId, ERROR_CMD_TIMEOUT);
		}

		//boost::this_thread::sleep_for(boost::chrono::milliseconds(SLEEP_TIMESPAN * 2));	//时间同步会有问题
		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN * 2));
	}

	ICC_LOG_DEBUG(m_pLog, "------------------ DoCheckExcusedTask Task Thread Exit!! ------------------");
}
void CAvayaSwitchManager::StartCheckExcusedTaskThread()
{
	if (!m_bCheckExcusedTaskThreadAlive)
	{
		m_bCheckExcusedTaskThreadAlive = true;
		m_oLastCheckExcusedTime = m_pDateTime->CurrentDateTime();

		m_pCheckExcusedTaskThread = boost::make_shared<boost::thread>(boost::bind(&CAvayaSwitchManager::DoCheckExcusedTask, this));
	}
}
void CAvayaSwitchManager::StopCheckExcusedTaskThread()
{
	m_bCheckExcusedTaskThreadAlive = false;

	if (m_pCheckExcusedTaskThread)
	{
		m_pCheckExcusedTaskThread->join();
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CAvayaSwitchManager::PostConnectStatus(bool p_bConnectStatus)
{
	ICTIConnStateNotifPtr l_pAESStateNotif = boost::make_shared<CCTIConnStateNotif>();
	if (l_pAESStateNotif)
	{
		l_pAESStateNotif->SetStreamOpen(p_bConnectStatus);
		l_pAESStateNotif->SetSwitchType(SWITCH_TYPE_TAPI);

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_ConnectAesEvent, l_pAESStateNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask ConnectCTIEvent, Open Stream [%s], TaskId: [%u]", p_bConnectStatus ? "Success" : "Failed", l_lTaskId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAESStateNotif Object Failed !!!");
	}
}
void CAvayaSwitchManager::FailedEvent(ITaskPtr p_pTask)
{
	IFailedEventNotifPtr l_pFailedEventNotif = boost::dynamic_pointer_cast<CFailedEventNotif>(p_pTask->GetSwitchNotif());
	if (l_pFailedEventNotif)
	{
		CHytProtocol::Instance()->Event_Failed(l_pFailedEventNotif);
	}
}

void CAvayaSwitchManager::AgentStateEvent(ITaskPtr p_pTask)
{
	IAgentStateNotifPtr l_pNotif = boost::dynamic_pointer_cast<CAgentStateNotif>(p_pTask->GetSwitchNotif());
	if (l_pNotif)
	{
		CHytProtocol::Instance()->Event_AgentState(l_pNotif);
		CHytProtocol::Instance()->Sync_AgentState(l_pNotif);
	}
}
void CAvayaSwitchManager::DeviceStateEvent(ITaskPtr p_pTask)
{
	IDeviceStateNotifPtr l_pNotif = boost::dynamic_pointer_cast<CDeviceStateNotif>(p_pTask->GetSwitchNotif());
	if (l_pNotif)
	{
		bool l_bSendDeviceState = true;
		std::string l_strOriginalCTICallRefId = "";
		std::string l_strOriginalCallerId = "";
		std::string l_strOriginalCalledId = "";
		std::string l_strCTICallRefId = l_pNotif->GetCTICallRefId();
		std::string l_strDeviceNum = l_pNotif->GetDeviceNum();
		std::string l_strDeviceState = l_pNotif->GetDeviceState();

		if (CConferenceManager::Instance()->FindConferenceByCTICallRefId(l_strCTICallRefId) ||
			CConferenceManager::Instance()->FindConferenceByActiveCallRefId(l_strCTICallRefId))
		{
			if (CConferenceManager::Instance()->GetCTICallRefIdByActiveCallRefId(l_strCTICallRefId, l_strOriginalCTICallRefId))
			{
				//	拨号、振铃为新话务 ID ，需在此修改为原话务 ID 后再发给客户端
				CCallManager::Instance()->GetCallByCTICallRefId(l_strOriginalCTICallRefId, l_strOriginalCallerId, l_strOriginalCalledId);

				l_pNotif->SetCTICallRefId(l_strOriginalCTICallRefId);
				l_pNotif->SetOriginalCallerId(l_strOriginalCallerId);
				l_pNotif->SetOriginalCalledId(l_strOriginalCalledId);

				if ( (l_strDeviceState.compare(CallStateString[STATE_HANGUP]) == 0 || l_strDeviceState.compare(CallStateString[STATE_FREE]) == 0) &&
					!CConferenceManager::Instance()->GetAddCurrentPartyResult(l_strDeviceNum, l_strCTICallRefId))
				{
					// 呼叫会议成员失败，执行重拾，先收到原话务的恢复通话事件，再收到新话务的挂机事件
					// 因此，不发送挂机事件到接警席
					l_bSendDeviceState = false;
				}
			}

			CConferenceManager::Instance()->DeviceStateNotif(l_strDeviceNum, l_strDeviceState, l_strCTICallRefId);

			//	会议成员接通，状态修改为 会议 后再发送到客户端
			if (l_strDeviceState.compare(CallStateString[STATE_TALK]) == 0)
			{
				l_pNotif->SetDeviceState(CallStateString[STATE_CONFERENCE]);
			}
		}
		if (CTransferCallManager::Instance()->FindTransferCallByActiveCallRefId(l_strCTICallRefId) ||
			CTransferCallManager::Instance()->FindTransferCallByHeldCallRefId(l_strCTICallRefId))
		{
			if (CTransferCallManager::Instance()->GetCTICallRefIdByActiveCallRefId(l_strCTICallRefId, l_strOriginalCTICallRefId))
			{
				//	拨号、振铃为新话务 ID ，需在此修改为原话务 ID 后再发给客户端
				CCallManager::Instance()->GetCallByCTICallRefId(l_strOriginalCTICallRefId, l_strOriginalCallerId, l_strOriginalCalledId);

				l_pNotif->SetOriginalCallerId(l_strOriginalCallerId);
				l_pNotif->SetOriginalCalledId(l_strOriginalCalledId);
			}

			if (l_strDeviceState.compare(CallStateString[STATE_TALK]) == 0 && !l_strOriginalCallerId.empty())
			{
				//	转移，修改为原始主叫
				l_pNotif->SetCallerId(l_strOriginalCallerId);
			}

			CTransferCallManager::Instance()->DeviceStateNotif(l_strDeviceNum, l_strCTICallRefId, l_strDeviceState);
		}

		if (l_bSendDeviceState)
		{
			CHytProtocol::Instance()->Event_DeviceState(l_pNotif);
		}
	}
}
void CAvayaSwitchManager::ConferenceHangupEvent(ITaskPtr p_pTask)
{
	/*if (m_pSwitchEventCallback)
	{
	IDeviceStateNotifPtr l_pNotif = boost::dynamic_pointer_cast<CDeviceStateNotif>(p_pTask->GetSwitchNotif());
	if (l_pNotif)
	{
	m_pSwitchEventCallback->Event_ConferenceHangup(l_pNotif);
	}
	}*/
}
void CAvayaSwitchManager::CallStateEvent(ITaskPtr p_pTask)
{
	ICallStateNotifPtr l_pNotif = boost::dynamic_pointer_cast<CCallStateNotif>(p_pTask->GetSwitchNotif());
	if (l_pNotif)
	{
		//	ACD 组名称转换，如 11001 -> 110
		std::string l_strACDGrpConver = "";
		if (CSysConfig::Instance()->GetACDGrpConver(l_strACDGrpConver, l_pNotif->GetACDGrp()))
		{
			l_pNotif->SetACDGrp(l_strACDGrpConver);
		}

		CHytProtocol::Instance()->Event_CallState(l_pNotif);
	}
}
void CAvayaSwitchManager::CallOverEvent(ITaskPtr p_pTask)
{
	ICallOverNotifPtr l_pNotif = boost::dynamic_pointer_cast<CCallOverNotif>(p_pTask->GetSwitchNotif());
	if (l_pNotif)
	{
		std::string l_strCTICallRefId = l_pNotif->GetCTICallRefId();
		if (!CConferenceManager::Instance()->FindConferenceByActiveCallRefId(l_strCTICallRefId))
		{
			//	会议成员咨询失败，不保存话务
			CConferenceManager::Instance()->DeleteConference(l_strCTICallRefId);

			//	ACD 组名称转换，如 11001 -> 110
			std::string l_strACDGrpConver = "";
			if (CSysConfig::Instance()->GetACDGrpConver(l_strACDGrpConver, l_pNotif->GetACDGrp()))
			{
				l_pNotif->SetACDGrp(l_strACDGrpConver);
			}

			CHytProtocol::Instance()->Event_CallOver(l_pNotif);
		}
	}
}
void CAvayaSwitchManager::RefuseCallEvent(ITaskPtr p_pTask)
{
	IRefuseCallNotifPtr l_pNotif = boost::dynamic_pointer_cast<CRefuseCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pNotif)
	{
		CHytProtocol::Instance()->Event_RefuseCall(l_pNotif);
	}
}

//////////////////////////////////////////////////////////////////////////
void CAvayaSwitchManager::ProcessFailedCmd(E_TASK_NAME p_nTaskName, long p_lRequestId, int p_nErrorCode)
{
	switch (p_nTaskName)
	{
	case Task_AgentLogin:
		SetAgentStateFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_AgentLogout:
		SetAgentStateFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_SetAgentState:
		SetAgentStateFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_MakeCall:
		MakeCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_Hangup:
		ClearConnectionFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_RefuseAnswer:
		ClearCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_ForcePopCall:
		ClearCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_AnswerCall:
		AnswerCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_BargeInCall:
		SingleStepConferenceFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_ListenCall:
		SingleStepConferenceFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_PickupCall:
		PickupCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_ConsultationCall:
		ConsultationCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_TransferCall:
		TransferCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_DeflectCall:
		DeflectCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_HoldCall:
		HoldCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_RetrieveCall:
		RetrieveCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_ReconnectCall:
		ReconnectCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_ConferenceCall:
		ConferenceCallFailed(p_lRequestId, p_nErrorCode);
		break;
	default:
		ICC_LOG_WARNING(m_pLog, "Delete Failed ExcusedTask, TaskName: [%s] TaskId: [%u]",
			TaskNameString[p_nTaskName].c_str(), p_lRequestId);
		CTaskManager::Instance()->DeleteExcusedTask(p_lRequestId);
		break;
	}
}
void CAvayaSwitchManager::SetAgentStateFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "SetAgentStateFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISetAgentStateResultNotifPtr l_pResultNotif = boost::make_shared<CSetAgentStateResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_SetAgentStateConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->SetAgentStateConf(l_pTask);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create SetAgentStateConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create SetAgentStateConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::MakeCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "MakeCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	IMakeCallResultNotifPtr l_pResultNotif = boost::make_shared<CMakeCallResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_MakeCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->MakeCallConf(l_pTask);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create MakeCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create MakeCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::ClearConnectionFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "ClearConnectionFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_ClearConnectionConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->ClearConnectionConf(l_pTask);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create ClearConnectionConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create ClearConnectionConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::ClearCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "ClearCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_ClearCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->ClearCallConf(l_pTask);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create ClearCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create ClearCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::AnswerCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "AnswerCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_AnswerCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->AnswerCallConf(l_pTask);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create AnswerCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create AnswerCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::SingleStepConferenceFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "SingleStepConferenceFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_SingleStepConferenceConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->SingleStepConferenceConf(l_pTask);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create SingleStepConferenceConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create SingleStepConferenceConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::PickupCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "PickupCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_PickupCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->PickupCallConf(l_pTask);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create PickupCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create PickupCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::ConsultationCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "ConsultationCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	IConsultationCallResultNotifPtr l_pResultNotif = boost::make_shared<CConsultationCallResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_ConsultationCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->ConsultationCallConf(l_pTask);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create ConsultationCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create ConsultationCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::TransferCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "TransferCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ITransferCallResultNotifPtr l_pResultNotif = boost::make_shared<CTransferCallResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_TransferCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->TransferCallConf(l_pTask);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create TransferCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create TransferCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::DeflectCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "DeflectCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_DeflectCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->DeflectCallConf(l_pTask);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create DeflectCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create DeflectCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::HoldCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "HoldCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_HoldCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->HoldCallConf(l_pTask);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create HoldCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create HoldCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::RetrieveCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "RetrieveCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_RetrieveCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->ReconnectCallConf(l_pTask);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create RetrieveCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create RetrieveCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::ReconnectCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "ReconnectCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_ReconnectCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->ReconnectCallConf(l_pTask);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create ReconnectCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create ReconnectCallConf Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::ConferenceCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "ConferenceCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_ConferenceCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->ConferenceCallConf(l_pTask);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create ConferenceCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create ConferenceCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
//////////////////////////////////////////////////////////////////////////
void CAvayaSwitchManager::UniversalFailRet(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pSwitchResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pSwitchResultNotif)
	{
		int l_nErrorCode = l_pSwitchResultNotif->GetErrorCode();
		std::string l_strErrorMsg = l_pSwitchResultNotif->GetErrorMsg();
		long l_lRequestId = l_pSwitchResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lRequestId);
		if (l_pExcutedTask)
		{
			E_TASK_NAME l_nTaskName = l_pExcutedTask->GetTaskName();
			switch (l_nTaskName)
			{
			case Task_AgentLogin:
				SetAgentStateFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_AgentLogout:
				SetAgentStateFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_SetAgentState:
				SetAgentStateFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_MakeCall:
				MakeCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_Hangup:
				ClearConnectionFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_RefuseAnswer:
				ClearCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_ForcePopCall:
				ClearCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_AnswerCall:
				AnswerCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_BargeInCall:
				SingleStepConferenceFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_ListenCall:
				SingleStepConferenceFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_PickupCall:
				PickupCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_ConsultationCall:
				ConsultationCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_TransferCall:
				TransferCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_DeflectCall:
				DeflectCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_HoldCall:
				HoldCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_RetrieveCall:
				RetrieveCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_ReconnectCall:
				ReconnectCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_ConferenceCall:
				ConferenceCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			default:
				ICC_LOG_WARNING(m_pLog, "Delete Failed ExcusedTask, TaskName: [%s] TaskId: [%u]",
					TaskNameString[l_nTaskName].c_str(), l_lRequestId);
				CTaskManager::Instance()->DeleteExcusedTask(l_lRequestId);
				break;
			}
		}
	}
}
void CAvayaSwitchManager::OpenStreamConf(ITaskPtr p_pTask)
{
	ICTIConnStateNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CCTIConnStateNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		m_bSwitchConnect = l_pResultNotif->GetStreamOpen();

		CHytProtocol::Instance()->Event_CTIConnState(l_pResultNotif);

		if (!m_bSwitchConnect)
		{
			if (l_pResultNotif->GetStreamException())
			{
				ICC_LOG_WARNING(m_pLog, "StreamException, ReOpenStream !!!!");
				this->ReOpenStream();
			}
		}
	}
}

void CAvayaSwitchManager::SetAgentStateConf(ITaskPtr p_pTask)
{
	ISetAgentStateResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSetAgentStateResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		bool l_bResult = l_pResultNotif->GetResult();
		long l_lTaskId = l_pResultNotif->GetRequestId();
		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			ISetAgentStateNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CSetAgentStateNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				bool l_bPreLogin = l_pRequestNotif->GetPreLogin();
				bool l_bInitAgentState = l_pRequestNotif->GetInitAgentState();
				std::string l_strAgent = l_pRequestNotif->GetAgentId();
				std::string l_strACD = l_pRequestNotif->GetACDGrp();
				std::string l_strLoginMode = l_pRequestNotif->GetLoginMode();
				std::string l_strReadyState = l_pRequestNotif->GetReadyState();

				l_pResultNotif->SetAgentId(l_strAgent);
				l_pResultNotif->SetACDGrp(l_strACD);
				l_pResultNotif->SetLoginMode(l_strLoginMode);
				l_pResultNotif->SetReadyState(l_strReadyState);

				E_TASK_NAME l_nTaskName = l_pExcutedTask->GetTaskName();
				switch (l_nTaskName)
				{
				case Task_AgentLogin:
					if (l_bResult && l_bInitAgentState)
					{
						CAgentManager::Instance()->LoginModeSync(l_strAgent, l_strACD, AGENT_MODE_LOGIN);
					}
					CHytProtocol::Instance()->CR_AgentLogin(l_pResultNotif);
					break;
				case Task_AgentLogout:
					if (l_bResult && l_bInitAgentState)
					{
						CAgentManager::Instance()->LoginModeSync(l_strAgent, l_strACD, AGENT_MODE_LOGOUT);
					}
					CHytProtocol::Instance()->CR_AgentLogout(l_pResultNotif);
					break;
				case Task_SetAgentState:
					if (l_bResult && l_bInitAgentState)
					{
						CAgentManager::Instance()->ReadyStateSync(l_strAgent, l_strACD, l_strReadyState);
					}
					CHytProtocol::Instance()->CR_SetAgentState(l_pResultNotif);
					break;
				default:
					break;
				}
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "SetAgentStateConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::QueryAgentStateRet(ITaskPtr p_pTask)
{
	IQueryAgentStateResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CQueryAgentStateResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();
		std::string l_strAgentState = l_pResultNotif->GetAgentState();
		bool l_bResult = l_pResultNotif->GetResult();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			IQueryAgentStateNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CQueryAgentStateNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				std::string l_strDeviceNum = l_pRequestNotif->GetAgentId();

				CAgentManager::Instance()->OnAgentState(l_strDeviceNum, l_strAgentState);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "QueryAgentStateRet, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::MakeCallConf(ITaskPtr p_pTask)
{
	IMakeCallResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CMakeCallResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();
		bool l_bResult = l_pResultNotif->GetResult();
		std::string l_strCTICallRefId = l_pResultNotif->GetCTICallRefId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			IMakeCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CMakeCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				long l_lRequestId = l_pRequestNotif->GetRequestId();
				std::string l_strCallerId = l_pRequestNotif->GetCallerId();
				std::string l_strCalledId = l_pRequestNotif->GetCalledId();
				std::string l_strCaseId = l_pRequestNotif->GetCaseId();

				l_pResultNotif->SetCallerId(l_strCallerId);
				l_pResultNotif->SetCalledId(l_strCalledId);
				l_pResultNotif->SetCaseId(l_strCaseId);			

				if (CConferenceManager::Instance()->FindConferenceByMakeCallTaskId(l_lRequestId))
				{
					CConferenceManager::Instance()->ProcessMakeCallResult(l_lRequestId, l_strCTICallRefId, l_bResult);
				}
				else
				{
					CHytProtocol::Instance()->CR_MakeCall(l_pResultNotif);
				}
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "MakeCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::ClearConnectionConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			E_TASK_NAME l_nTaskName = l_pExcutedTask->GetTaskName();
			switch (l_nTaskName)
			{
			case Task_Hangup:			// 正常挂机
				CHytProtocol::Instance()->CR_Hangup(l_pResultNotif);
				break;
			case Task_RefuseAnswer:		//正常拒接
				CHytProtocol::Instance()->CR_RefuseAnswer(l_pResultNotif);
				break;
			case Task_ForcePopCall:		//	强拆
				CHytProtocol::Instance()->CR_ForcePopCall(l_pResultNotif);
				break;
			default:
				break;
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "ClearConnectionConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::ClearCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			long l_lCSTACallRefId = DEFAULT_CALLREFID;
			IForcePopCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CForcePopCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
				CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId);
			}

			E_TASK_NAME l_nTaskName = l_pExcutedTask->GetTaskName();
			switch (l_nTaskName)
			{
			case Task_ClearCall:		//	强拆整个话务
				CHytProtocol::Instance()->CR_ClearCall(l_pResultNotif);
				break;
			case Task_ForcePopCall:		//	强拆
				CHytProtocol::Instance()->CR_ForcePopCall(l_pResultNotif);
				break;
			case Task_RefuseAnswer:		//正常拒接
				CHytProtocol::Instance()->CR_RefuseAnswer(l_pResultNotif);
				break;
			case Task_RefuseBlackCall:	//黑名单拦截
				break;
			default:
				break;
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "ClearCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::AnswerCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			/*IAnswerCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CAnswerCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
			long l_lCSTACallRefId = l_pRequestNotif->GetCSTACallRefId();
			std::string l_strDevice = l_pRequestNotif->GetDeviceNum();

			l_pResultNotif->SetCSTACallRefId(l_lCSTACallRefId);
			l_pResultNotif->SetDeviceNum(l_strDevice);}*/

			IAnswerCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CAnswerCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				std::string l_strDevice = l_pRequestNotif->GetDeviceNum();

				if (CConferenceManager::Instance()->FindConferenceByAnswerCallTaskId(l_lTaskId))
				{
					bool l_bResult = l_pResultNotif->GetResult();
					CConferenceManager::Instance()->ProcessAnswerCallResult(l_lTaskId, l_strDevice, l_bResult);
				}
				else
				{
					CHytProtocol::Instance()->CR_AnswerCall(l_pResultNotif);
				}
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "AnswerCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::SingleStepConferenceConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();
		bool l_bResult = l_pResultNotif->GetResult();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			E_TASK_NAME l_nTaskName = l_pExcutedTask->GetTaskName();
			switch (l_nTaskName)
			{
			case Task_ListenCall:
				CHytProtocol::Instance()->CR_ListenCall(l_pResultNotif/*boost::dynamic_pointer_cast<CListenCallNotif>(l_pRequestNotif)*/);
				break;
			case Task_BargeInCall:
				CHytProtocol::Instance()->CR_BargeInCall(l_pResultNotif/*boost::dynamic_pointer_cast<CBargeInCallNotif>(l_pRequestNotif)*/);
				break;
			default:
				break;
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "SingleStepConferenceConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::PickupCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			/*IPickupCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CPickupCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
			l_pResultNotif->SetSponsor(l_pRequestNotif->GetSponsor());
			l_pResultNotif->SetTarget(l_pRequestNotif->GetTarget());
			}*/

			CHytProtocol::Instance()->CR_PickupCall(l_pResultNotif);

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "PickupCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::ConsultationCallConf(ITaskPtr p_pTask)
{
	IConsultationCallResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CConsultationCallResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		bool l_bResult = l_pResultNotif->GetResult();
		int l_nErrorCode = l_pResultNotif->GetErrorCode();
		long l_lTaskId = l_pResultNotif->GetRequestId();
		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			IConsultationCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CConsultationCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				bool l_bPreConsultationCall = l_pRequestNotif->GetPreConsultationCall();
				bool l_bIsTransferCall = l_pRequestNotif->GetIsTransferCall();
				std::string l_strSponsor = l_pRequestNotif->GetSponsor();
				std::string l_strTargetACDGrp = l_pRequestNotif->GetTargetACDGrp();
				std::string l_strTargetDevice = l_pRequestNotif->GetTargetDevice();

				if (l_bPreConsultationCall && l_bResult)
				{
					std::string l_strHeldCTICallRefId = l_pResultNotif->GetHeldCTICallRefId();
					std::string l_strActiveCTICallRefId = l_pResultNotif->GetActiveCTICallRefId();

					if (CConferenceManager::Instance()->FindConferenceByConsultationCallTaskId(l_lTaskId))
					{
						CConferenceManager::Instance()->ProcessConsultationCallResult(l_lTaskId, l_strHeldCTICallRefId, l_strActiveCTICallRefId, l_bResult);
					}

					//	咨询前置操作成功，接着执行 Dial 操作
					l_pRequestNotif->SetPreConsultationCall(false);
					CTaskManager::Instance()->AddCmdTask(l_pExcutedTask); 
				}
				else
				{
					if (CConferenceManager::Instance()->FindConferenceByConsultationCallTaskId(l_lTaskId))
					{
						CConferenceManager::Instance()->ProcessConsultationCallResult(l_lTaskId, l_bResult);
					} 
					else if (CTransferCallManager::Instance()->FindTransferCallByConsultationCallTaskId(l_lTaskId))
					{
						CTransferCallManager::Instance()->ProcessConsultationCallResult(l_lTaskId, l_bResult, l_nErrorCode);
					}
					else
					{
						CHytProtocol::Instance()->CR_ConsultationCall(l_pResultNotif);
					}
				}
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "ConsultationCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::TransferCallConf(ITaskPtr p_pTask)
{
	ITransferCallResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CTransferCallResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		bool l_bResult = l_pResultNotif->GetResult();
		long l_lTaskId = l_pResultNotif->GetRequestId();
		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			ITransferCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CTransferCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				l_pResultNotif->SetActiveCTICallRefId(l_pRequestNotif->GetActiveCTICallRefId());
				l_pResultNotif->SetHeldCTICallRefId(l_pRequestNotif->GetHeldCTICallRefId());
				l_pResultNotif->SetSponsor(l_pRequestNotif->GetSponsor());
				l_pResultNotif->SetTargetDevice(l_pRequestNotif->GetTarget());

				if (CTransferCallManager::Instance()->FindTransferCallByTransferCallTaskId(l_lTaskId))
				{
					CTransferCallManager::Instance()->ProcessTransferCallResult(l_lTaskId, l_bResult);
				}
				else
				{
					CHytProtocol::Instance()->CR_TransferCall(l_pResultNotif);
				}
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "TransferCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::DeflectCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			/*IDeflectCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CDeflectCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
			l_pResultNotif->SetSponsor(l_pRequestNotif->GetSponsor());
			l_pResultNotif->SetTarget(l_pRequestNotif->GetTarget());

			m_pSwitchEventCallback->CR_DeflectCall(l_pResultNotif);
			}*/

			CHytProtocol::Instance()->CR_DeflectCall(l_pResultNotif);

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "DeflectCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::HoldCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			CHytProtocol::Instance()->CR_HoldCall(l_pResultNotif);

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "HoldCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::RetrieveCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		bool l_bResult = l_pResultNotif->GetResult();
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			CConferenceManager::Instance()->ProcessRetrieveCallResult(l_lTaskId, l_bResult);

			CHytProtocol::Instance()->CR_RetrieveCall(l_pResultNotif);

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "RetrieveCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::ReconnectCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			CHytProtocol::Instance()->CR_ReconnectCall(l_pResultNotif);

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "ReconnectCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::ConferenceCallConf(ITaskPtr p_pTask) //tttt
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		bool l_bResult = l_pResultNotif->GetResult();
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			if (l_bResult)
			{
				IConferenceCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CConferenceCallNotif>(l_pExcutedTask->GetSwitchNotif());
				if (l_pRequestNotif)
				{
					long l_lHeldCSTACallRefId = DEFAULT_CALLREFID;
					long l_lActiveCSTACallRefId = DEFAULT_CALLREFID;
					std::string l_strHeldCTICallRefId = l_pRequestNotif->GetHeldCTICallRefId();
					std::string l_strActiveCTICallRefId = l_pRequestNotif->GetActiveCTICallRefId();

					// 合并会议，删除新产生的话务
					if (CCallManager::Instance()->GetCSTACallRefId(l_strHeldCTICallRefId, l_lHeldCSTACallRefId) &&
						CCallManager::Instance()->GetCSTACallRefId(l_strActiveCTICallRefId, l_lActiveCSTACallRefId))
					{
						/*CDeviceManager::Instance()->DeleteAllLogicalCallState(l_lCSTACallRefId);
						CCallManager::Instance()->DeleteCall(l_lCSTACallRefId);*/

						CCallManager::Instance()->CopyCall(l_lActiveCSTACallRefId, l_lHeldCSTACallRefId);
						CCallManager::Instance()->AddRelatedCSTACallRefId(l_lActiveCSTACallRefId, l_lHeldCSTACallRefId);
						CCallManager::Instance()->DeleteCall(l_lHeldCSTACallRefId);

						CDeviceManager::Instance()->CopyLogicalCallState(l_lActiveCSTACallRefId, l_lHeldCSTACallRefId);
						CDeviceManager::Instance()->DeleteAllLogicalCallState(l_lHeldCSTACallRefId);
					}
				}
			}

			CHytProtocol::Instance()->CR_ConferenceCall(l_pResultNotif);

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "ConferenceCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::AddConferencePartyConf(ITaskPtr p_pTask)
{
	//
}
void CAvayaSwitchManager::DeleteConferencePartyConf(ITaskPtr p_pTask)
{
	//
}

void CAvayaSwitchManager::GetCTIConnStateRet(ITaskPtr p_pTask)
{
	IGetCTIConnStateResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CGetCTIConnStateResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			CHytProtocol::Instance()->CR_GetCTIConnState(l_pResultNotif);

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetAESConnStateRet, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::GetDeviceListRet(ITaskPtr p_pTask)
{
	IGetDeviceListResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CGetDeviceListResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			CHytProtocol::Instance()->CR_GetDeviceList(l_pResultNotif);

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetDeviceListRet, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::GetACDListRet(ITaskPtr p_pTask)
{
	IGetACDListResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CGetACDListResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			CHytProtocol::Instance()->CR_GetACDList(l_pResultNotif);

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetACDListRet, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::GetAgentListRet(ITaskPtr p_pTask)
{
	IGetAgentListResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CGetAgentListResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			CHytProtocol::Instance()->CR_GetAgentList(l_pResultNotif);

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetAgentListRet, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::GetCallListRet(ITaskPtr pTask)
{
	IGetCallListResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CGetCallListResultNotif>(pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			CHytProtocol::Instance()->CR_GetCallList(l_pResultNotif);

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetCallListRet, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::GetReadyAgentRet(ITaskPtr p_pTask)
{
	IGetReadyAgentResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CGetReadyAgentResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			if (CTransferCallManager::Instance()->FindTransferCallByGetReadyAgentTaskId(l_lTaskId))
			{
				bool l_bResult = l_pResultNotif->GetResult();
				std::string l_strReadyAgent = l_pResultNotif->GetReadyAgent();
				CTransferCallManager::Instance()->ProcessGetReadyAgentResult(l_lTaskId, l_strReadyAgent, l_bResult);
			}
			else
			{
				CHytProtocol::Instance()->CR_GetReadyAgent(l_pResultNotif);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetReadyAgentRet, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
int CAvayaSwitchManager::QueryDeviceInfo(ITaskPtr p_pTask)
{
	IQueryDeviceInfoNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CQueryDeviceInfoNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		std::string l_strDeviceNum = l_pRequestNotif->GetDeviceNum();
		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strDeviceNum);
		if (l_pDevice)
		{
			long l_lTaskId = p_pTask->GetTaskId();

			/*bool l_bRet = CAvayaSwitch::Instance()->QueryDeviceInfo(l_strDeviceNum, l_lTaskId);
			if (!l_bRet)
			{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Query Device [%s] Info Failed!!",
			l_lTaskId, l_strDeviceNum.c_str());
			}
			else
			{
			ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Query Device [%s] Info Success!!",
			l_lTaskId, l_strDeviceNum.c_str());
			}*/
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::MonitorDevice(ITaskPtr p_pTask)
{
	IMonitorDeviceNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CMonitorDeviceNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		std::string l_strDeviceNum = l_pRequestNotif->GetDeviceNum();
		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strDeviceNum);
		if (l_pDevice /*&& !l_pDevice->GetIsMonistor()*/)
		{
			long l_lTaskId = p_pTask->GetTaskId();

			/*bool l_bRet = CAvayaSwitch::Instance()->MonitorDevice(l_strDeviceNum, l_lTaskId);
			if (!l_bRet)
			{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] Monitor Failed!!",
			l_lTaskId, l_strDeviceNum.c_str());
			}
			else
			{
			ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] Monitor Success!!",
			l_lTaskId, l_strDeviceNum.c_str());
			}*/
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::MonitorCallsViaDevice(ITaskPtr p_pTask)
{
	IMonitorDeviceNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CMonitorDeviceNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		std::string l_strDeviceNum = l_pRequestNotif->GetDeviceNum();
		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strDeviceNum);
		if (l_pDevice)
		{
			long l_lTaskId = p_pTask->GetTaskId();

			/*bool l_bRet = CAvayaSwitch::Instance()->MonitorCallsViaDevice(l_strDeviceNum, l_lTaskId);
			if (!l_bRet)
			{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device[ %s] MonitorCallsViaDevice Failed!!",
			l_lTaskId, l_strDeviceNum.c_str());
			}
			else
			{
			ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] MonitorCallsViaDevice Success!!",
			l_lTaskId, l_strDeviceNum.c_str());
			}*/
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::AgentLogin(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	ISetAgentStateNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CSetAgentStateNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		bool l_bInitAgentState = l_pRequestNotif->GetInitAgentState();
		std::string l_strACD = l_pRequestNotif->GetACDGrp();
		std::string l_strDeviceNum = l_pRequestNotif->GetAgentId();
		std::string l_strLoginMode = l_pRequestNotif->GetLoginMode();

		if (!l_bInitAgentState && !CAgentManager::Instance()->IsInitOver())
		{
			//初始化未完成，不执行设置操作
			ICC_LOG_WARNING(m_pLog, "TaskId [%u], Agent [%s] Login Failed !! Waiting Agent Init",
				l_lRequestId, l_strDeviceNum.c_str());

			return RESULT_FAILED;
		}
		
		if (CAgentManager::Instance()->AgentIsLogin(l_strDeviceNum, l_strACD))
		{
			ISetAgentStateResultNotifPtr l_pResultnotif = boost::make_shared<CSetAgentStateResultNotif>();
			if (l_pResultnotif)
			{
				l_pResultnotif->SetRequestId(l_lRequestId);
				l_pResultnotif->SetAgentId(l_strDeviceNum);
				l_pResultnotif->SetACDGrp(l_strACD);
				l_pResultnotif->SetLoginMode(l_strLoginMode);
				l_pResultnotif->SetResult(true);

				long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_SetAgentStateConf, l_pResultnotif);
				ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask, SetAgentStateConf, TaskId [%u], RequestId [%u], Agent [%s] Was Login!!",
					l_lTaskId, l_lRequestId, l_strDeviceNum.c_str());
			}

			return RESULT_SUCCESS;
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strDeviceNum);
		if (l_pDevice)
		{
			long l_lInvokeId = l_pDevice->SetInGroup("")/*LogOn()*/;
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId: [%d], InvokeId: [%d], Agent: [%s] Login Success!!",
					l_lRequestId, l_lInvokeId, l_strDeviceNum.c_str());
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Agent [%s] Login Failed!!",
					l_lRequestId, l_strDeviceNum.c_str());

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::AgentLogout(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	ISetAgentStateNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CSetAgentStateNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		bool l_bInitAgentState = l_pRequestNotif->GetInitAgentState();
		std::string l_strACD = l_pRequestNotif->GetACDGrp();
		std::string l_strDeviceNum = l_pRequestNotif->GetAgentId();
		std::string l_strLoginMode = l_pRequestNotif->GetLoginMode();

		if (!l_bInitAgentState && !CAgentManager::Instance()->IsInitOver())
		{
			//初始化未完成，不执行设置操作
			ICC_LOG_WARNING(m_pLog, "TaskId [%u], Agent [%s] Logout Failed !! Waiting Agent Init",
				l_lRequestId, l_strDeviceNum.c_str());

			return RESULT_FAILED;
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strDeviceNum);
		if (l_pDevice)
		{
			long l_lInvokeId = l_pDevice->SetOutGroup("")/*LogOff()*/;
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%d],  InvokeId [%d], Agent [%s] Logout Success !",
					l_lRequestId, l_lInvokeId, l_strDeviceNum.c_str());
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Agent [%s] Logout Failed !",
					l_lRequestId, l_strDeviceNum.c_str());

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::SetAgentState(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	ISetAgentStateNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CSetAgentStateNotif>(p_pTask->GetSwitchNotif());
	if (g_bCTITestStart)
	{
		if (l_pRequestNotif)
		{
			printf("------1111111111111111111111111-----------------RequestId:%d---l_bInitAgentState=%d-------l_strDeviceNum:%s-----\n", 
				l_pRequestNotif->GetRequestId(), l_pRequestNotif->GetInitAgentState(), l_pRequestNotif->GetAgentId().c_str());
		}
		else
		{

			printf("--------------------------------1111111111111111111111111-----------------------------------\n");
		}

		CAvayaEvent::Instance()->SetAgentStateConf(p_pTask->GetInvokeId(),true);
		return RESULT_SUCCESS;
	}

	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		bool l_bInitAgentState = l_pRequestNotif->GetInitAgentState();
		std::string l_strACD = l_pRequestNotif->GetACDGrp();
		std::string l_strDeviceNum = l_pRequestNotif->GetAgentId();
		std::string l_strReadyState = l_pRequestNotif->GetReadyState();

		if (!l_bInitAgentState && !CAgentManager::Instance()->IsInitOver())
		{
			//初始化未完成，不执行设置操作
			ICC_LOG_WARNING(m_pLog, "TaskId [%u], Set Agent [%s] State [%s] Failed ! Waiting Agent Init",
				l_lRequestId, l_strDeviceNum.c_str(), l_strReadyState.c_str());

			return RESULT_FAILED;
		}

		bool l_bSetReady = false;
		if (l_strReadyState.compare(ReadyState_Ready) == 0)
		{
			l_bSetReady = true;
		}	

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strDeviceNum);
		if (l_pDevice)
		{
			if (!l_bSetReady && !l_bInitAgentState)
			{
				//	最小在席数判断，接警席配置文件中的ACD默认为 110 ，
				//	按分机号重新获取取 ACDGrp
				std::string l_strRealACDGrp = l_strACD;
				CAgentManager::Instance()->GetACDGrpByDn(l_strRealACDGrp, l_strDeviceNum);
				int l_nReadyAgentCount = CAgentManager::Instance()->GetReadyAgentCount(l_strRealACDGrp);
				if (l_nReadyAgentCount <= CSysConfig::Instance()->GetMinOnlineAgentNum())
				{
					ICC_LOG_WARNING(m_pLog, "MinOnlineAgentNum [%d], TaskId [%u], Set Agent [%s] State %s Failed !",
						l_nReadyAgentCount, l_lRequestId, l_strDeviceNum.c_str(), l_strReadyState.c_str());

					return ERROR_MIN_ONLINE_AGENT;
				}
			}

			long l_lInvokeId = 0;
			if (l_bSetReady)
			{
				l_lInvokeId = l_pDevice->SetInGroup(""/*l_strRealACDGrp*/);
			} 
			else
			{
				l_lInvokeId = l_pDevice->SetOutGroup(""/*l_strRealACDGrp*/);
			}

			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%d], Set Agent [%s] State [%s] Success !",
					l_lRequestId, l_lInvokeId, l_strDeviceNum.c_str(), l_strReadyState.c_str());
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%d], Set Agent [%s] State [%s] Failed !",
					l_lRequestId, l_lInvokeId, l_strDeviceNum.c_str(), l_strReadyState.c_str());

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::QueryAgentState(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return RESULT_FAILED;
	}

	IQueryAgentStateNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CQueryAgentStateNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strDeviceNum = l_pRequestNotif->GetAgentId();

		/*bool l_bRet = CAvayaSwitch::Instance()->QueryAgentState(l_strDeviceNum, l_lRequestId);
		if (!l_bRet)
		{
		ICC_LOG_ERROR(m_pLog, "TaskId [%u], Get Agent [%s] State Failed !",
		l_lRequestId, l_strDeviceNum.c_str());
		}
		else
		{
		ICC_LOG_LOWDEBUG(m_pLog, "TaskId [%u], Get Agent [%s] State Success !",
		l_lRequestId, l_strDeviceNum.c_str());
		}*/
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::RouteRequest(ITaskPtr p_pTask)
{
	IRouteCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CRouteCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCallerId = l_pRequestNotif->GetCallerId();
		std::string l_strRouteDest = l_pRequestNotif->GetRouteDest();
		long l_lCSTACallRefId = l_pRequestNotif->GetCSTACallRefId();
		long l_lRegisterReqId = l_pRequestNotif->GetRegisterReqId();
		long l_lCrossRefId = l_pRequestNotif->GetCrossRefId();
		bool l_bIsBlackCall = l_pRequestNotif->GetIsBlackCall();

		if (l_bIsBlackCall)
		{
			// 路由目标
			/*bool l_bRet = CAvayaSwitch::Instance()->RouteSelectInv(l_lRegisterReqId, l_lCrossRefId, l_strRouteDest, l_lRequestId);
			if (!l_bRet)
			{
			ICC_LOG_ERROR(m_pLog, "RequestId [%u], RouteSelectInv CallerId [%s] RouteSelected [%s] Failed !",
			l_lRequestId, l_strCallerId.c_str(), l_strRouteDest.c_str());
			}
			else
			{
			ICC_LOG_DEBUG(m_pLog, "RequestId [%u], RouteSelectInv CallerId [%s] RouteSelected [%s] Success !",
			l_lRequestId, l_strCallerId.c_str(), l_strRouteDest.c_str());
			}*/
		}
		else
		{
			/*bool l_bRet = CAvayaSwitch::Instance()->RouteEndInv(l_lRegisterReqId, l_lCrossRefId, l_lRequestId);
			if (!l_bRet)
			{
			ICC_LOG_ERROR(m_pLog, "RequestId [%u], RouteEndInv CallerId [%s] Failed !",
			l_lRequestId, l_strCallerId.c_str());
			}
			else
			{
			ICC_LOG_DEBUG(m_pLog, "RequestId [%u], RouteEndInv CallerId [%s] Success !",
			l_lRequestId, l_strCallerId.c_str());
			}*/
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::MakeCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IMakeCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CMakeCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCallerId = l_pRequestNotif->GetCallerId();
		std::string l_strCalledId = l_pRequestNotif->GetCalledId();
		std::string l_strTargetType = l_pRequestNotif->GetTargetPhoneType();

		if (l_strCallerId.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], MakeCall CallerId [%s] CalledId [%s] Failed , CallerId is Empty!",
				l_lRequestId, l_strCallerId.c_str(), l_strCalledId.c_str());

			return ERROR_INVALID_CALLING_DEVICE;
		}
		if (l_strCalledId.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], MakeCall CallerId [%s] CalledId [%s] Failed  , CalledId is Empty!",
				l_lRequestId, l_strCallerId.c_str(), l_strCalledId.c_str());

			return ERROR_INVALID_CALLED_DEVICE;
		}


		std::string l_strDestNum = l_strCalledId;
		if (CSysConfig::Instance()->GetOutCallNum(l_strCalledId, l_strDestNum))
		{
			l_strCalledId = l_strDestNum;
		}
		else
		{
			l_strCalledId = CSysConfig::Instance()->GetCheckNum(l_strCalledId);
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strCallerId);
		if (l_pDevice)
		{
			long l_lInvokeId = l_pDevice->MakeCall(l_strCalledId);
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], MakeCall CallerId [%s] CalledId [%s] Success !",
					l_lRequestId, l_lInvokeId, l_strCallerId.c_str(), l_strCalledId.c_str());
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], MakeCall CallerId [%s] CalledId [%s] Failed !",
					l_lRequestId, l_lInvokeId, l_strCallerId.c_str(), l_strCalledId.c_str());

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::Hangup(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IAnswerCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CAnswerCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strDevice = l_pRequestNotif->GetDeviceNum();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strDevice);
		if (l_pDevice)
		{
			long l_lInvokeId = 0;
			if (CConferenceManager::Instance()->FindConference(l_strCTICallRefId, l_strDevice))
			{
				// 会议，挂断整个话务
				l_lInvokeId = l_pDevice->ClearCall();
			} 
			else
			{
				l_lInvokeId = l_pDevice->DropCall();
			}
			
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], Sopnsor [%s] Hangup Device [%s] CallRefId [%u] Success !",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strDevice.c_str(), l_lCSTACallRefId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], Sopnsor [%s] Hangup Device [%s] CallRefId [%u] Failed !",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strDevice.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::AnswerCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IAnswerCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CAnswerCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strDevice = l_pRequestNotif->GetDeviceNum();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strDevice);
		if (l_pDevice)
		{
			long l_lInvokeId = l_pDevice->AnswerCall();
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], AnswerCall Device [%s] CallRefId [%u] Success !",
					l_lRequestId, l_lInvokeId, l_strDevice.c_str(), l_lCSTACallRefId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], AnswerCall Device [%s] CallRefId [%u] Failed !",
					l_lRequestId, l_lInvokeId, l_strDevice.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::RefuseAnswer(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IAnswerCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CAnswerCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strDevice = l_pRequestNotif->GetDeviceNum();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strDevice);
		if (l_pDevice)
		{
			long l_lInvokeId = l_pDevice->DropCall();
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], RefuseCall Device [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_lInvokeId, l_strDevice.c_str(), l_lCSTACallRefId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], RefuseCall Device [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_lInvokeId, l_strDevice.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::RefuseBlackCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IAnswerCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CAnswerCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strDevice = l_pRequestNotif->GetDeviceNum();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		if (CSysConfig::Instance()->GetUseAnalogueTrunk())
		{
			// 如果通过模拟中继呼入的电话，需先接通后再挂断
			std::string l_strCallerId = "";
			std::string l_strCalledParty = "";
			CCallManager::Instance()->GetCallByCTICallRefId(l_strCTICallRefId, l_strCallerId, l_strCalledParty);
			boost::shared_ptr<CTapiLine> l_pCalledPartyDevice = CDeviceManager::Instance()->FindLineByDn(l_strCalledParty);
			if (l_pCalledPartyDevice)
			{
				l_strDevice = l_strCalledParty;

				long l_lInvokeId = l_pCalledPartyDevice->AnswerCall();
				if (l_lInvokeId > 0)
				{
					CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

					ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], RefuseBlcakCall PreAnswer Device [%s] CallRefId [%u] Success !",
						l_lRequestId, l_lInvokeId, l_strCalledParty.c_str(), l_lCSTACallRefId);
				}
				else
				{
					ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], RefuseBlcakCall PreAnswer Device [%s] CallRefId [%u] Failed !",
						l_lRequestId, l_lInvokeId, l_strCalledParty.c_str(), l_lCSTACallRefId);

					return RESULT_FAILED;
				}
			}

			Sleep(300);
		}
		
		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strDevice);
		if (l_pDevice)
		{
			long l_lInvokeId = l_pDevice->DropCall();
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], RefuseBlcakCall Device [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_lInvokeId, l_strDevice.c_str(), l_lCSTACallRefId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], RefuseBlcakCall Device [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_lInvokeId, l_strDevice.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::ClearCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IClearCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CClearCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		std::vector<std::string> l_vCallMembers;
		if (CCallManager::Instance()->GetCallMembers(l_lCSTACallRefId, l_vCallMembers))
		{
			for each (auto l_strMemberObj in l_vCallMembers)
			{
				boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strMemberObj);
				if (l_pDevice)
				{
					long l_lInvokeId = l_pDevice->ClearCall();
					if (l_lInvokeId > 0)
					{
						CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

						ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] ClearCall Target [%s] CallRefId [%u] Success !!",
							l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strMemberObj.c_str(), l_lCSTACallRefId);
					}
					else
					{
						ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] ClearCall Target [%s] CallRefId [%u] Failed !!",
							l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strMemberObj.c_str(), l_lCSTACallRefId);

						//	return RESULT_FAILED;
					}
				}
			}
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::ListenCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	return RESULT_FAILED;
#if 0 // TAPI提供的监听方法，抛出来的事件，目前尚未有好的方法关联
	IListenCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CListenCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		if (l_strSponsor.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ListenCall Target [%s] Failed, Sponsor is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}
		if (l_strTarget.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ListenCall Target [%s] Failed, Target is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		//需判断被监听的话务是否存在
		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strSponsor);
		if (l_pDevice)
		{
			long l_lInvokeId = l_pDevice->Listen(l_strTarget);
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] ListenCall  Target [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] ListenCall Target [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
#endif
}

int CAvayaSwitchManager::PickupCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IPickupCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CPickupCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		if (l_strSponsor.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] PickupCall Target [%s] Failed, Sponsor is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}
		if (l_strTarget.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] PickupCall Target [%s] Failed, Target is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strTarget);
		if (l_pDevice)
		{
			long l_lInvokeId = l_pDevice->Redirect(l_strSponsor);
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] PickupCall Target [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] PickupCall Target [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::BargeInCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	return RESULT_FAILED;		// 暂不支持强插
#if 0  // TAPI提供的强插方法，抛出来的事件，目前尚未有好的处理方式
	IBargeInCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CBargeInCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		if (l_strSponsor.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] BargeInCall Target [%s] Failed, Sponsor is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}
		if (l_strTarget.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] BargeInCall Target [%s] Failed, Target is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strSponsor);
		if (l_pDevice)
		{
			long l_lInvokeId = l_pDevice->Intrude(l_strTarget);
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] BargeInCall Target [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] BargeInCall Target [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
#endif
}
int CAvayaSwitchManager::ForcePopCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IForcePopCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CForcePopCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strTarget);
		if (l_pDevice)
		{
			long l_lInvokeId = l_pDevice->DropCall();
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], Sopnsor [%s] ForcePop Target [%s] CallRefId [%u] Success !",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], Sopnsor [%s] ForcePop Target [%s] CallRefId [%u] Failed !",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::ConsultationCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IConsultationCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CConsultationCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		bool l_bPreConsultationCall = l_pRequestNotif->GetPreConsultationCall();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTargetACDGrp = l_pRequestNotif->GetTargetACDGrp();
		std::string l_strTargetType = l_pRequestNotif->GetTargetDeviceType();
		std::string l_strTargetDevice = l_pRequestNotif->GetTargetDevice();

		if (l_strSponsor.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ConsultationCall TargetACDGrp [%s] TargetDevice [%s] Failed, Sponsor is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTargetACDGrp.c_str(), l_strTargetDevice.c_str());

			return ERROR_INVALID_DEVICE;
		}
		if (l_strTargetDevice.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ConsultationCall TargetACDGrp [%s] TargetDevice [%s] Failed, Target is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTargetACDGrp.c_str(), l_strTargetDevice.c_str());

			return ERROR_INVALID_DEVICE;
		}

		std::string l_strDestNum = l_strTargetDevice;
		if (CSysConfig::Instance()->GetOutCallNum(l_strTargetDevice, l_strDestNum))
		{
			l_strTargetDevice = l_strDestNum;
		}
		else
		{
			l_strTargetDevice = CSysConfig::Instance()->GetCheckNum(l_strTargetDevice);
		}

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strSponsor);
		if (l_pDevice)
		{
			if (l_bPreConsultationCall)
			{
				long l_lInvokeId = l_pDevice->SetupTransfer();
				if (l_lInvokeId > 0)
				{
					CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

					ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] ConsultationCall:SetupTransfer TargetACDGrp [%s] TargetDevice [%s] CallRefId [%u] Success !!",
						l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTargetACDGrp.c_str(), l_strTargetDevice.c_str(), l_lCSTACallRefId);
				}
				else
				{
					ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] ConsultationCall:SetupTransfer TargetACDGrp [%s] TargetDevice [%s] CallRefId [%u] Failed !!",
						l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTargetACDGrp.c_str(), l_strTargetDevice.c_str(), l_lCSTACallRefId);

					return RESULT_FAILED;
				}
			} 
			else
			{
				Sleep(500);
				long l_lInvokeId = l_pDevice->Dial(l_strTargetDevice);
				if (l_lInvokeId > 0)
				{
					CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

					ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] ConsultationCall:Dial TargetACDGrp [%s] TargetDevice [%s] CallRefId [%u] Success !!",
						l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTargetACDGrp.c_str(), l_strTargetDevice.c_str(), l_lCSTACallRefId);
				}
				else
				{
					ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] ConsultationCall:Dial TargetACDGrp [%s] TargetDevice [%s] CallRefId [%u] Failed !!",
						l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTargetACDGrp.c_str(), l_strTargetDevice.c_str(), l_lCSTACallRefId);

					return RESULT_FAILED;
				}
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::TransferCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	ITransferCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CTransferCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strActiveCTICallRefId = l_pRequestNotif->GetActiveCTICallRefId();
		std::string l_strHeldCTICallRefId = l_pRequestNotif->GetHeldCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		if (l_strSponsor.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] TransferCall Target [%s] Failed, Sponsor is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}
		if (l_strTarget.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] TransferCall Target [%s] Failed, Target is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}

		long l_lActiveCSTACallRefId = DEFAULT_CALLREFID;
		long l_lHeldCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strActiveCTICallRefId, l_lActiveCSTACallRefId) || !CCallManager::Instance()->GetCSTACallRefId(l_strHeldCTICallRefId, l_lHeldCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strSponsor);
		if (l_pDevice)
		{
			long l_lInvokeId = l_pDevice->CompleteTransfer();
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] TransferCall Target [%s] ActiveCallRefId %u HeldCallRefId %u Success !!",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lActiveCSTACallRefId, l_lHeldCSTACallRefId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] TransferCall Target [%s] ActiveCallRefId %u HeldCallRefId %u Failed !!",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lActiveCSTACallRefId, l_lHeldCSTACallRefId);

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::DeflectCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IDeflectCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CDeflectCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		if (l_strSponsor.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] DeflectCall Target [%s] Failed, Sponsor is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}
		if (l_strTarget.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] DeflectCall Target [%s] Failed, Target is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}

		std::string l_strDestNum = l_strTarget;
		if (CSysConfig::Instance()->GetOutCallNum(l_strTarget, l_strDestNum))
		{
			l_strTarget = l_strDestNum;
		}
		else
		{
			l_strTarget = CSysConfig::Instance()->GetCheckNum(l_strTarget);
		}

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strSponsor);
		if (l_pDevice)
		{
			long l_lInvokeId = l_pDevice->Redirect(l_strTarget);
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] DeflectCall Target [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] DeflectCall Target [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::HoldCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IHoldCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CHoldCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetDevice();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strTarget);
		if (l_pDevice)
		{
			long l_lInvokeId = l_pDevice->HoldCall();
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] HoldCall Target [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] HoldCall Target [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::RetrieveCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IRetriveCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CRetriveCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetDevice();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strTarget);
		if (l_pDevice)
		{
			long l_lInvokeId = l_pDevice->UnholdCall();
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] RetrieveCall Target [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], Device [%s] RetrieveCall Target [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_lInvokeId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::ReconnectCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IReconnectCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CReconnectCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strActiveCTICallRefId = l_pRequestNotif->GetActiveCTICallRefId();
		std::string l_strHeldCTICallRefId = l_pRequestNotif->GetHeldCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strDevice = l_pRequestNotif->GetDevice();

		long l_lActiveCSTACallRefId = DEFAULT_CALLREFID;
		long l_lHeldCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strActiveCTICallRefId, l_lActiveCSTACallRefId) || !CCallManager::Instance()->GetCSTACallRefId(l_strHeldCTICallRefId, l_lHeldCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		//需判断重连的话务是否存在
		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strSponsor);
		if (l_pDevice)
		{
			/*bool l_bRet = CAvayaSwitch::Instance()->ReconnectCall(l_strDevice, l_lActiveCSTACallRefId, l_lHeldCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ReconnectCall Target [%s] ActiveCallRefId %u HeldCallRefId %u Failed !!",
			l_lRequestId, l_strSponsor.c_str(), l_strDevice.c_str(), l_lActiveCSTACallRefId, l_lHeldCSTACallRefId);
			}
			else
			{
			ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] ReconnectCall Target [%s] ActiveCallRefId %u HeldCallRefId %u Success !!",
			l_lRequestId, l_strSponsor.c_str(), l_strDevice.c_str(), l_lActiveCSTACallRefId, l_lHeldCSTACallRefId);
			}*/
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::ConferenceCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IConferenceCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CConferenceCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strActiveCTICallRefId = l_pRequestNotif->GetActiveCTICallRefId();
		std::string l_strHeldCTICallRefId = l_pRequestNotif->GetHeldCTICallRefId();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		long l_lActiveCSTACallRefId = DEFAULT_CALLREFID;
		long l_lHeldCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strActiveCTICallRefId, l_lActiveCSTACallRefId) || !CCallManager::Instance()->GetCSTACallRefId(l_strHeldCTICallRefId, l_lHeldCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strTarget);
		if (l_pDevice)
		{
			long l_lInvokeId = 0;
			if (CCallManager::Instance()->GetCallMode(l_lHeldCSTACallRefId).compare(CallModeString[CALL_MODE_CONFERENCE]) != 0)
			{
				l_lInvokeId = l_pDevice->ConferenceCall();
			}
			else
			{
				l_lInvokeId = l_pDevice->AddToConference();
			}

			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], Target [%s]  ConferenceCall, HeldCallRefId [%u], ActiveCallRefId [%u] Success !!",
					l_lRequestId, l_lInvokeId, l_strTarget.c_str(), l_lHeldCSTACallRefId, l_lActiveCSTACallRefId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], Target [%s]  ConferenceCall, HeldCallRefId [%u], ActiveCallRefId [%u] Failed !!",
					l_lRequestId, l_lInvokeId, l_strTarget.c_str(), l_lHeldCSTACallRefId, l_lActiveCSTACallRefId);

				return RESULT_FAILED;
			}
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::AddConferenceParty(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::DeleteConferenceParty(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IDeleteConferencePartyNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CDeleteConferencePartyNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strCompere = l_pRequestNotif->GetCompere();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strCompere);
		if (l_pDevice)
		{
			long l_lInvokeId = l_pDevice->RemoveFromConference();
			if (l_lInvokeId > 0)
			{
				CTaskManager::Instance()->SetInvokeId(l_lRequestId, l_lInvokeId);

				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], InvokeId [%u], Compere [%s], Target [%s]  RemoveFromConference, CSTACallRefId [%u] Success !!",
					l_lRequestId, l_lInvokeId, l_strCompere.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], InvokeId [%u], Compere [%s], Target [%s]  RemoveFromConference, CSTACallRefId [%u] Failed !!",
					l_lRequestId, l_lInvokeId, l_strCompere.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
		}
	}

	return RESULT_SUCCESS; 
}

//////////////////////////////////////////////////////////////////////////
int CAvayaSwitchManager::GetCTIConnState(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		long l_lRequestId = p_pTask->GetTaskId();

		IGetCTIConnStateResultNotifPtr l_pResultNotif = boost::make_shared<CGetCTIConnStateResultNotif>();
		if (l_pResultNotif)
		{
			l_pResultNotif->SetRequestId(l_lRequestId);
			l_pResultNotif->SetStreamOpen(m_bSwitchConnect);
			l_pResultNotif->SetSwitchType(SWITCH_TYPE_TAPI);

			long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_GetCTIConnStateRet, l_pResultNotif);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask, TaskId [%u], GetCTIConnStateRet, RequestId [%u]",
				l_lTaskId, l_lRequestId);
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::GetDeviceList(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		long l_lRequestId = p_pTask->GetTaskId();

		IGetDeviceListResultNotifPtr l_pResultNotif = CDeviceManager::Instance()->GetDeviceList(l_lRequestId);
		if (l_pResultNotif)
		{
			for each (auto l_pDeviceStateObj in l_pResultNotif->m_DeviceList)
			{
				if (l_pDeviceStateObj)
				{
					std::string l_strCTICallRefId = "";
					std::string l_strCallerId = "";
					std::string l_strCalledId = "";
					std::string l_strOriginalCallerId = "";
					std::string l_strOriginalCalledId = "";
					std::string l_strCallDirection = "";
					std::string	l_strLoginMode;		//	login/logout 状态
					std::string	l_strReadyState;
					std::string l_strDeviceNum = l_pDeviceStateObj->GetDeviceNum();
					long l_lCSTACallRefId = l_pDeviceStateObj->GetCSTACallRefId();

					bool l_bFind = CCallManager::Instance()->GetCallByCSTACallRefId(l_lCSTACallRefId, l_strCTICallRefId, l_strCallerId, l_strCalledId, l_strOriginalCallerId, l_strOriginalCalledId, l_strCallDirection);
					l_pDeviceStateObj->SetCTICallRefId(l_strCTICallRefId);
					l_pDeviceStateObj->SetCallerId(l_strCallerId);
					l_pDeviceStateObj->SetCalledId(l_strCalledId);
					l_pDeviceStateObj->SetCallDirection(l_strCallDirection);

					CAgentManager::Instance()->GetAgentState(l_strDeviceNum, l_strLoginMode, l_strReadyState);
					l_pDeviceStateObj->SetLoginMode(l_strLoginMode);
					l_pDeviceStateObj->SetReadyState(l_strReadyState);
					l_pDeviceStateObj->SetDeptCode(CDeviceManager::Instance()->GetDeptCodeByDeviceNum(l_strDeviceNum));

					ICC_LOG_DEBUG(m_pLog, "GetDeviceList l_lCSTACallRefId:[%u], l_strDeviceNum:[%s],l_strCTICallRefId:[%s],bFind=%d",
						l_lCSTACallRefId, l_strDeviceNum.c_str(), l_strCTICallRefId.c_str(), l_bFind);

				}
			}

			long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_GetDeviceListRet, l_pResultNotif);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask, TaskId [%u], GetDeviceListRet, RequestId [%u]",
				l_lTaskId, l_lRequestId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetDeviceListRet, GetDeviceListResultNotif Obj  Failed, RequestId [%u]", l_lRequestId);

			return RESULT_FAILED;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::GetACDList(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		IGetACDListRequestNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CGetACDListRequestNotif>(p_pTask->GetSwitchNotif());
		if (l_pRequestNotif)
		{
			long l_lRequestId = p_pTask->GetTaskId();

			CAgentManager::Instance()->GetACDList(l_lRequestId);
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::GetAgentList(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		IGetAgentListRequestNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CGetAgentListRequestNotif>(p_pTask->GetSwitchNotif());
		if (l_pRequestNotif)
		{
			long l_lRequestId = p_pTask->GetTaskId();
			std::string l_strACDGrp = l_pRequestNotif->GetACDGrp();

			CAgentManager::Instance()->GetAgentList(l_lRequestId, l_strACDGrp);
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::GetCallList(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		long l_lRequestId = p_pTask->GetTaskId();
		CCallManager::Instance()->GetCallList(l_lRequestId);
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::GetReadyAgent(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		long l_lRequestId = p_pTask->GetTaskId();

		IGetReadyAgentNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CGetReadyAgentNotif>(p_pTask->GetSwitchNotif());
		if (l_pRequestNotif)
		{
			std::string strACDGrp = l_pRequestNotif->GetACDGrp();

			CAgentManager::Instance()->GetReadyAgent(strACDGrp, l_lRequestId);
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::SetBlackList(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		ISetBlackListNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CSetBlackListNotif>(p_pTask->GetSwitchNotif());
		if (l_pRequestNotif)
		{
			//	
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::DeleteBlackList(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		ISetBlackListNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CSetBlackListNotif>(p_pTask->GetSwitchNotif());
		if (l_pRequestNotif)
		{
			//
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::DeleteAllBlackList(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		//
	}

	return RESULT_SUCCESS;
}
