#include "Boost.h"

#include "SwitchClientImpl.h"
#include "TaskManager.h"
#include "ControlCenter.h"
#include "DeviceManager.h"
#include "CallManager.h"

CSwitchClientImpl::CSwitchClientImpl(IResourceManagerPtr p_pResourceManager)
{
	m_pResourceManager = p_pResourceManager;

	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, m_pResourceManager)->GetLogger(MODULE_NAME);
}
CSwitchClientImpl::~CSwitchClientImpl()
{
	CControlCenter::Instance()->ExitInstance();
}

//////////////////////////////////////////////////////////////////////////
void CSwitchClientImpl::InitSwitch(ISwitchEventCallbackPtr p_pCallback)
{
	ICC_LOG_DEBUG(m_pLog, "Init TSAPIClient !");

	CControlCenter::Instance()->OnInit(m_pResourceManager, p_pCallback);
}
void CSwitchClientImpl::StartSwitch()
{
	ICC_LOG_DEBUG(m_pLog, "Start TSAPIClient !");

	CControlCenter::Instance()->OnStart();
}
void CSwitchClientImpl::StopSwitch()
{
	ICC_LOG_DEBUG(m_pLog, "Stop TSAPIClient !");

	CControlCenter::Instance()->OnStop();
}
void CSwitchClientImpl::DestroySwitch()
{
	CControlCenter::Instance()->OnDestroy();
}

//////////////////////////////////////////////////////////////////////////
long CSwitchClientImpl::AgentLogin(const std::string& p_strAgent, const std::string& p_strACD)
{
	ISetAgentStateNotifPtr l_pRequestNotif = boost::make_shared<CSetAgentStateNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetAgentId(p_strAgent);
		l_pRequestNotif->SetACDGrp(p_strACD);
		l_pRequestNotif->SetLoginMode(LoginMode_Login);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_AgentLogin, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], AgentLogin, AgentId [%s], ACD [%s]",
			l_lNewTaskId, p_strAgent.c_str(), p_strACD.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSetAgentStateNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}
long CSwitchClientImpl::AgentLogout(const std::string& p_strAgent, const std::string& p_strACD)
{
	ISetAgentStateNotifPtr l_pRequestNotif = boost::make_shared<CSetAgentStateNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetAgentId(p_strAgent);
		l_pRequestNotif->SetACDGrp(p_strACD);
		l_pRequestNotif->SetLoginMode(LoginMode_Logout);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_AgentLogout, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], AgentLogout, AgentId [%s], ACD [%s]",
			l_lNewTaskId, p_strAgent.c_str(), p_strACD.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSetAgentStateNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::SetAgentState(const std::string& p_strAgent, const std::string& p_strACD, const std::string& p_strReadyState)
{
	ISetAgentStateNotifPtr l_pRequestNotif = boost::make_shared<CSetAgentStateNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetAgentId(p_strAgent);
		l_pRequestNotif->SetACDGrp(p_strACD);
		l_pRequestNotif->SetReadyState(p_strReadyState);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_SetAgentState, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], SetAgentState, AgentId [%s], ACD [%s], State [%s]",
			l_lNewTaskId, p_strAgent.c_str(), p_strACD.c_str(), p_strReadyState.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSetAgentStateNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::MakeCall(const std::string& p_strCallerId, const std::string& p_strCalledId, const std::string& p_strTargetType, const std::string& p_strCaseId)
{
	IMakeCallNotifPtr l_pRequestNotif = boost::make_shared<CMakeCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCallerId(p_strCallerId);
		l_pRequestNotif->SetCalledId(p_strCalledId);
		l_pRequestNotif->SetTargetPhoneType(p_strTargetType);
		l_pRequestNotif->SetCaseId(p_strCaseId);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_MakeCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], MakeCall, CallerId [%s], CalledId [%s], TargetType [%s]",
			l_lNewTaskId, p_strCallerId.c_str(), p_strCalledId.c_str(), p_strTargetType.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CMakeCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::AnswerCall(const std::string& p_strCTICallRefId, const std::string& p_strDevice)
{
	IAnswerCallNotifPtr l_pRequestNotif = boost::make_shared<CAnswerCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetDeviceNum(p_strDevice);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_AnswerCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], AnswerCall, CallRefId [%s], Device [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strDevice.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::RefuseAnswer(const std::string& p_strCTICallRefId, const std::string& p_strDevice)
{
	IAnswerCallNotifPtr l_pRequestNotif = boost::make_shared<CAnswerCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetDeviceNum(p_strDevice);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_RefuseAnswer, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], RefuseAnswer, CallRefId [%s], Device [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strDevice.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::Hangup(const std::string& p_strCTICallRefId, const std::string & p_strSponsor, const std::string& p_strDevice)
{
	IAnswerCallNotifPtr l_pRequestNotif = boost::make_shared<CAnswerCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetDeviceNum(p_strDevice);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_Hangup, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], Hangup, CallRefId [%s], Sponsor [%s] Device [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strDevice.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::ClearCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	IClearCallNotifPtr l_pRequestNotif = boost::make_shared<CClearCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strTarget);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_ClearCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ClearCall, CallRefId [%s], Sponsor [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CClearCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::ListenCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	IListenCallNotifPtr l_pRequestNotif = boost::make_shared<CListenCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strTarget);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_ListenCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ListenCall, CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create  CListenCallNotifObject Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::PickupCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	IPickupCallNotifPtr l_pRequestNotif = boost::make_shared<CPickupCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strTarget);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_PickupCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], PickupCall, CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CPickupCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::BargeInCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	IBargeInCallNotifPtr l_pRequestNotif = boost::make_shared<CBargeInCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strTarget);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_BargeInCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], BargeInCall, CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CBargeInCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::ForcePopCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	IForcePopCallNotifPtr l_pRequestNotif = boost::make_shared<CForcePopCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strTarget);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_ForcePopCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ForcePopCall, CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CForcePopCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::ConsultationCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType, bool p_bIsTransferCall)
{
	IConsultationCallNotifPtr l_pRequestNotif = boost::make_shared<CConsultationCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTargetDevice(p_strTarget);
		l_pRequestNotif->SetTargetDeviceType(p_strTargetDeviceType);
		l_pRequestNotif->SetIsTransferCall(p_bIsTransferCall);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_ConsultationCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ConsultationCall, CallRefId [%s], Sponsor [%s], Target [%s], TargetDeviceType [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str(), p_strTargetDeviceType.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CConsultationCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}
/*
long CSwitchClientImpl::ConsultationCallEx(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	IConsultationCallNotifPtr l_pRequestNotif = boost::make_shared<CConsultationCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetSelectTargetDevice(true);
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTargetACDGrp(p_strTarget);

		ITaskPtr l_pNewTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pNewTask)
		{
			long l_lNewTaskId = l_pNewTask->GetTaskId();

			l_pRequestNotif->SetRequestId(l_lNewTaskId);

			l_pNewTask->SetTaskName(Task_ConsultationCall);
			l_pNewTask->SetSwitchNotif(l_pRequestNotif);
			CTaskManager::Instance()->AddCmdTask(l_pNewTask);

			ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ConsultationCallEx, CallRefId [%s], Sponsor [%s], TargetACDGrp [%s]",
				l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

			return l_lNewTaskId;
		}
		else
		{
			ICC_LOG_FATAL(m_pLog, "Create ConsultationCallEx Task Object Failed !!!");
		}
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CConsultationCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}*/
long CSwitchClientImpl::TransferCall(const std::string& p_strActiveCTICallRefId, const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	ITransferCallNotifPtr l_pRequestNotif = boost::make_shared<CTransferCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetActiveCTICallRefId(p_strActiveCTICallRefId);
		l_pRequestNotif->SetHeldCTICallRefId(p_strHeldCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strTarget);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_TransferCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], TransferCall, ActiveCallRefId [%s], HeldCallRefId [%s], Sponsor [%s], Target [%s]",
			l_lNewTaskId, p_strActiveCTICallRefId.c_str(), p_strHeldCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CTransferCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::DeflectCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	IDeflectCallNotifPtr l_pRequestNotif = boost::make_shared<CDeflectCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strTarget);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_DeflectCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], DeflectCall, CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CDeflectCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::HoldCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice)
{
	IHoldCallNotifPtr l_pRequestNotif = boost::make_shared<CHoldCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetDevice(p_strDevice);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_HoldCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], HoldCall, CallRefId [%s], Sponsor [%s], Device [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strDevice.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CHoldCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}
long CSwitchClientImpl::RetrieveCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice)
{
	IRetriveCallNotifPtr l_pRequestNotif = boost::make_shared<CRetriveCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetDevice(p_strDevice);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_RetrieveCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], RetrieveCall, CallRefId [%s], Sponsor [%s], Device [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strDevice.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CRetriveCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::ConferenceCall(const std::string& p_strTarget, const std::string& p_strHeldCTICallRefId, const std::string& p_strActiveCTICallRefId)
{
	IConferenceCallNotifPtr l_pRequestNotif = boost::make_shared<CConferenceCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetHeldCTICallRefId(p_strHeldCTICallRefId);
		l_pRequestNotif->SetActiveCTICallRefId(p_strActiveCTICallRefId);
		l_pRequestNotif->SetTarget(p_strTarget);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_ConferenceCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ConferenceCall, HeldCallRefId [%s], ActiveCallRefId [%s], Target [%s]",
			l_lNewTaskId, p_strHeldCTICallRefId.c_str(), p_strActiveCTICallRefId.c_str(), p_strTarget.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CConferenceCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}


long CSwitchClientImpl::ReconnectCall(const std::string& p_strActiveCTICallRefId, const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice)
{
	IReconnectCallNotifPtr l_pRequestNotif = boost::make_shared<CReconnectCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetActiveCTICallRefId(p_strActiveCTICallRefId);
		l_pRequestNotif->SetHeldCTICallRefId(p_strHeldCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetDevice(p_strDevice);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_ReconnectCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ReconnectCall, ActiveCallRefId [%s], HeldCallRefId [%s], Sponsor [%s], Device [%s]",
			l_lNewTaskId, p_strActiveCTICallRefId.c_str(), p_strHeldCTICallRefId.c_str(), p_strSponsor.c_str(), p_strDevice.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CReconnectCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}
long CSwitchClientImpl::TakeOverCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	ITakeOverCallNotifPtr l_pRequestNotif = boost::make_shared<CTakeOverCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strTarget);
		l_pRequestNotif->SetStep(Step_StopListenCall);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_TakeOverCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], TakeOverCall, CallRefId [%s], Sponsor [%s], Device [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CTakeOverCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::GetCTIConnectState()
{
	ISwitchRequestNotifPtr l_pRequestNotif = boost::make_shared<CSwitchRequestNotif>();
	if (l_pRequestNotif)
	{
		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_GetCTIConnState, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], GetAesState", l_lNewTaskId);

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}
long CSwitchClientImpl::GetDeviceList()
{
	ISwitchRequestNotifPtr l_pRequestNotif = boost::make_shared<CSwitchRequestNotif>();
	if (l_pRequestNotif)
	{
		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_GetDeviceList, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], GetDeviceList", l_lNewTaskId);

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}
long CSwitchClientImpl::GetACDList()
{
	IGetACDListRequestNotifPtr l_pRequestNotif = boost::make_shared<CGetACDListRequestNotif>();
	if (l_pRequestNotif)
	{
		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_GetACDList, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], GetACDList", l_lNewTaskId);

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::GetAgentList(const std::string & p_strACDGrp)
{
	IGetAgentListRequestNotifPtr l_pRequestNotif = boost::make_shared<CGetAgentListRequestNotif>();
	if (l_pRequestNotif)
	{
		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_GetAgentList, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], GetAgentList, ACDGrp [%s]", l_lNewTaskId, p_strACDGrp.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::GetFreeAgentList(const std::string& p_strACDGrp, const std::string& p_strDeptCode)
{
	IGetAgentListRequestNotifPtr l_pRequestNotif = boost::make_shared<CGetAgentListRequestNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetACDGrp(p_strACDGrp);
		l_pRequestNotif->SetDeptCode(p_strDeptCode);
		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_GetFreeAgentList, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], GetFreeAgentList, ACDGrp [%s],StaffCode[%s]", l_lNewTaskId, p_strACDGrp.c_str(), p_strDeptCode.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::GetCallList()
{
	ISwitchRequestNotifPtr l_pRequestNotif = boost::make_shared<CSwitchRequestNotif>();
	if (l_pRequestNotif)
	{
		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_GetCallList, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], GetCallList", l_lNewTaskId);

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}
	
	return DEFAULT_TASKID;
}

long CSwitchClientImpl::GetReadyAgent(const std::string& p_strACDGrp)
{
	IGetReadyAgentNotifPtr l_pRequestNotif = boost::make_shared<CGetReadyAgentNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetACDGrp(p_strACDGrp);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_GetReadyAgent, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], GetReadyAgent", l_lNewTaskId);

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CGetReadyAgentNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::SetBlackList(const std::string& p_strLimitNum, const std::string& p_strLimitType)
{
	ISetBlackListNotifPtr l_pSetBlackListRequest = boost::make_shared<CSetBlackListNotif>();
	if (l_pSetBlackListRequest)
	{
		l_pSetBlackListRequest->SetLimitNum(p_strLimitNum);
		l_pSetBlackListRequest->SetLimitType(p_strLimitType);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_SetBlackList, l_pSetBlackListRequest);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], SetBlackList, LimitNum [%s]",
			l_lNewTaskId, p_strLimitNum.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSetBlackListNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::DeleteBlackList(const std::string& p_strLimitNum)
{
	ISetBlackListNotifPtr l_pSetBlackListRequest = boost::make_shared<CSetBlackListNotif>();
	if (l_pSetBlackListRequest)
	{
		l_pSetBlackListRequest->SetLimitNum(p_strLimitNum);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_DeleteBlackList, l_pSetBlackListRequest);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], DeleteBlackList, LimitNum [%s]",
			l_lNewTaskId, p_strLimitNum.c_str());

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSetBlackListNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CSwitchClientImpl::DeleteAllBlackList()
{
	ISwitchRequestNotifPtr l_pRequestNotif = boost::make_shared<CSwitchRequestNotif>();
	if (l_pRequestNotif)
	{
		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_DeleteAllBlackList, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], DeleteAllBlackList", l_lNewTaskId);

		return l_lNewTaskId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

std::string CSwitchClientImpl::GetDeviceState(const std::string& strTarget)
{
	return CDeviceManager::Instance()->GetDeviceState(strTarget);
}

std::string CSwitchClientImpl::GetCallerID(const std::string& p_strInCTICallRefId,std::string &p_strCalled)
{
	std::string l_strInCTICallRefId = p_strInCTICallRefId;
	std::string l_strOutCallerId;
	std::string l_strOutCalledParty;
	CCallManager::Instance()->GetCallByCTICallRefId(l_strInCTICallRefId, l_strOutCallerId, l_strOutCalledParty);
	p_strCalled = l_strOutCalledParty;
	return l_strOutCallerId;
}

bool CSwitchClientImpl::GetCallTime(const std::string& p_strCTICallRefId, std::string& p_strRingTime, std::string& p_strTalkTime)
{
	std::string l_strRingTime;
	std::string l_strTalkTime;
	bool bRes = CCallManager::Instance()->GetCallTime(p_strCTICallRefId, l_strRingTime, l_strTalkTime);
	if (bRes)
	{
		p_strRingTime = l_strRingTime;
		p_strTalkTime = p_strTalkTime;
		ICC_LOG_DEBUG(m_pLog, "get call time,CTICallRefId:[%s],ringTime:[%s],talkTime:[%s]", p_strCTICallRefId.c_str(), l_strRingTime.c_str(), l_strTalkTime.c_str());
	}
	return false;
}




