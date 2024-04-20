#include "Boost.h"

#include "FSAesClientImpl.h"
#include "TaskManager.h"
#include "DeviceManager.h"
#include "AgentManager.h"
#include "CallManager.h"
#include "ConferenceManager.h"
#include "FSAesSwitchManager.h"
#include "CTIFSAesDefine.h"
#include "SysConfig.h"
#include "BlackListManager.h"
//////////////////////////////////////////////////////////////////////////
//
CFSAesClientImpl::CFSAesClientImpl(IResourceManagerPtr p_pResourceManager)
{
	m_pResourceManager = p_pResourceManager;
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, m_pResourceManager)->GetLogger(MODULE_NAME);
	m_pStringPtr = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, p_pResourceManager)->CreateString();
}

CFSAesClientImpl::~CFSAesClientImpl()
{
	CFSAesSwitchManager::Instance()->ExitInstance();
}

void CFSAesClientImpl::InitSwitch(ISwitchEventCallbackPtr p_pCallback)
{
	ICC_LOG_DEBUG(m_pLog, "Init FSAesClient !");
	CFSAesSwitchManager::Instance()->OnInit(m_pResourceManager, p_pCallback);

}

void CFSAesClientImpl::StartSwitch()
{
	m_pJsonFty = ICCGetResourceEx(JsonParser::IJsonFactory, ICCIJsonFactoryResourceName, m_pResourceManager);
	ICC_LOG_DEBUG(m_pLog, "Start FSAesClient !");
	CFSAesSwitchManager::Instance()->OnStart();
	
}

void CFSAesClientImpl::StopSwitch()
{
	ICC_LOG_DEBUG(m_pLog, "Stop FSAesClient !");
	CFSAesSwitchManager::Instance()->OnStop();
}

void CFSAesClientImpl::DestroySwitch()
{
	//CControlCenter::Instance()->OnDestroy();
}

//////////////////////////////////////////////////////////////////////////
//
long CFSAesClientImpl::AgentLogin(const std::string& p_strAgent, const std::string& p_strACD)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_AGENT_LOGIN_CMD);
	if (l_pCmdRequest)
	{
		l_pCmdRequest->SetRequestCmdType(REQUEST_AGENT_LOGIN_CMD);
		if (p_strAgent.empty())
		{
			l_pCmdRequest->SetParamError(ERROR_INVALID_DEVICE);
		}
		else
		{
			l_pCmdRequest->SetParam(CTI_PARAM_extension, p_strAgent);//前端传的是分机号
			l_pCmdRequest->SetParam(CTI_PARAM_agentid, CDeviceManager::Instance()->GetAgentID(p_strAgent));
			l_pCmdRequest->SetParam(CTI_PARAM_skill, p_strACD);
			l_pCmdRequest->SetParam(CTI_PARAM_pswd, CDeviceManager::Instance()->GetAgentPswd(p_strAgent));
			l_pCmdRequest->SetParam(CTI_PARAM_agentstatus, CTI_AGENT_STATUS_IDLE); //1:空闲,2:忙
		}

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "AgentLogin AddCmdTask.requestId:[%u],AgentId:[%s],ACD:[%s]", l_lRequestId, p_strAgent.c_str(), p_strACD.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSetAgentStateNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CFSAesClientImpl::AgentLogout(const std::string& p_strAgent, const std::string& p_strACD)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_AGENT_LOGOUT_CMD);
	if (l_pCmdRequest)
	{
		if (p_strAgent.empty())
		{
			l_pCmdRequest->SetParamError(ERROR_INVALID_DEVICE);
		}

		l_pCmdRequest->SetParam(CTI_PARAM_extension, p_strAgent);//前端传的是分机号
		l_pCmdRequest->SetParam(CTI_PARAM_agentid, CDeviceManager::Instance()->GetAgentID(p_strAgent));
		l_pCmdRequest->SetParam(CTI_PARAM_skill, p_strACD);
		//l_pCmdRequest->SetParam(CTI_PARAM_agentstatus, LoginMode_Logout);

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "AgentLogout AddCmdTask.requestId:[%u],AgentId:[%s],ACD:[%s]", l_lRequestId, p_strAgent.c_str(), p_strACD.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSetAgentStateNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

bool CFSAesClientImpl::_CheckMinOnlineAgentNum(int p_nMinOnLineCount,const std::string& p_strAgent, std::string& p_strACDList)
{
	if (p_nMinOnLineCount <= 0)
	{
		//不限制最小坐席数
		ICC_LOG_DEBUG(m_pLog, "check main online agent,minValue:%d", p_nMinOnLineCount);
		return false;
	}

	if (p_strACDList.empty())
	{
		p_strACDList = CAgentManager::Instance()->GetAgentACDGrpList(p_strAgent);
	}

	std::vector<std::string> vecParams;
	m_pStringPtr->Split(p_strACDList, ";", vecParams, false);

	size_t l_count = vecParams.size();
	if (l_count > 0)
	{
		for (size_t i = 0; i < vecParams.size(); i++)
		{
			if (vecParams[i].empty())
			{
				continue;
			}
			int l_nReadyAgentCount = CAgentManager::Instance()->GetReadyAgentCount(vecParams[i]);
			if (l_nReadyAgentCount <= p_nMinOnLineCount)
			{
				ICC_LOG_DEBUG(m_pLog, "check main online agent,minValue:%d,onlineCount:%d,ACD:%s", p_nMinOnLineCount, l_nReadyAgentCount, vecParams[i].c_str());
				p_strACDList = vecParams[i];
				return true;
			}
		}
	}
	else
	{
		int l_nReadyAgentCount = CAgentManager::Instance()->GetReadyAgentCount(p_strACDList);
		if (l_nReadyAgentCount <= p_nMinOnLineCount)
		{
			ICC_LOG_DEBUG(m_pLog, "check main online agent,minValue:%d,onlineCount:%d", p_nMinOnLineCount, l_nReadyAgentCount);
			return true;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "check main online agent,minValue:%d,ACDList:%s", p_nMinOnLineCount, p_strACDList.c_str());
	return false;

}

long CFSAesClientImpl::SetAgentState(const std::string& p_strAgent, const std::string& p_strACD, const std::string& p_strReadyState)
{
	std::string l_strLoginMode;
	std::string l_strReadyState;
	CAgentManager::Instance()->GetAgentState(p_strAgent, l_strLoginMode, l_strReadyState);
	IFSAesCmdRequestPtr l_pCmdRequest = nullptr ;
	bool l_isLoginFlag = true;

	if (l_strLoginMode == AgentModeTypeString[AGENT_MODE_LOGOUT])
	{
		l_isLoginFlag = false;
		l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_AGENT_LOGIN_CMD);
	}
	else
	{
		if (p_strReadyState == ReadyState_Busy || p_strReadyState == ReadyState_NotReady)
		{
			l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_SET_AGENTBUSY_CMD);//

			int l_nMinOnLineCount = CSysConfig::Instance()->GetMinOnlineAgentNum();
			std::string l_strAcdList = p_strACD;
			if (_CheckMinOnlineAgentNum(l_nMinOnLineCount,p_strAgent, l_strAcdList))
			{
				ICC_LOG_WARNING(m_pLog, "MinOnlineAgentNum [%d], Set Agent [%s] State %s Failed,ACDNum:[%s],currACDNum:[%s] !", 
					l_nMinOnLineCount, p_strAgent.c_str(), l_strReadyState.c_str(), p_strACD.c_str(), l_strAcdList.c_str());

				l_pCmdRequest->SetParamError(ERROR_MIN_ONLINE_AGENT);
			}
		}
		else
		{
			l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_SET_AGENTIDLE_CMD);
		}
	}
 
	if (l_pCmdRequest)
	{
		if (p_strAgent.empty())
		{
			//设备无效
			l_pCmdRequest->SetParamError(ERROR_INVALID_DEVICE);
		}
		else
		{
			l_pCmdRequest->SetParam(CTI_PARAM_extension, p_strAgent);//前端传的是分机号
			l_pCmdRequest->SetParam(CTI_PARAM_agentid, CDeviceManager::Instance()->GetAgentID(p_strAgent));
			l_pCmdRequest->SetParam(CTI_PARAM_skill, p_strACD);
			if (!l_isLoginFlag)
			{
				//如果没有登录,发登录，状态为请求的状态
				l_pCmdRequest->SetParam(CTI_PARAM_pswd, CDeviceManager::Instance()->GetAgentPswd(p_strAgent));
			}

			if (p_strReadyState == ReadyState_Busy || p_strReadyState == ReadyState_NotReady)
			{
				l_pCmdRequest->SetParam(CTI_PARAM_agentstatus, CTI_AGENT_STATUS_BUSY); //1:空闲,2:忙
			}
			else
			{
				l_pCmdRequest->SetParam(CTI_PARAM_agentstatus, CTI_AGENT_STATUS_IDLE); //1:空闲,2:忙
			}
		}
		
		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "SetAgentState AddCmdTask.requestId:[%u],AgentId:[%s],ACD:[%s],State:[%s],isLoginFlag:[%d]",
			l_lRequestId, p_strAgent.c_str(), p_strACD.c_str(), p_strReadyState.c_str(), l_isLoginFlag);

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSetAgentStateNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CFSAesClientImpl::MakeCall(const std::string& p_strCallerId, const std::string& p_strCalledId, const std::string& p_strTargetType, const std::string& p_strCaseId)
{
	int l_nCmdType = REQUEST_MAKE_CALL_CMD;
	
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_MAKE_CALL_CMD);
	if (l_pCmdRequest)
	{
		if (p_strCallerId.empty() || p_strCalledId.empty())
		{
			l_pCmdRequest->SetParamError(ERROR_INVALID_DEVICE);
		}
		l_pCmdRequest->SetParam(CTI_PARAM_agentid, CDeviceManager::Instance()->GetAgentID(p_strCallerId));
		l_pCmdRequest->SetParam(CTI_PARAM_caller, p_strCallerId);
		l_pCmdRequest->SetParam(CTI_PARAM_called, p_strCalledId);
		l_pCmdRequest->SetParam(CTI_PARAM_caseid, p_strCaseId);

		if (CDeviceManager::Instance()->IsExtension(p_strCalledId, true))
		{
			l_pCmdRequest->SetParam(CTI_PARAM_direction, AES_CALL_DIRECTION_INSIDE);
		}
		else
		{
			if (CDeviceManager::Instance()->FindACDDevice(p_strCalledId))
			{
				//呼叫技能组也是内部呼叫
				l_pCmdRequest->SetParam(CTI_PARAM_direction, AES_CALL_DIRECTION_INSIDE);
			}
			else
			{
				l_pCmdRequest->SetParam(CTI_PARAM_direction, AES_CALL_DIRECTION_OUTSIDE);
			}
		}
		l_pCmdRequest->SetParam(CTI_PARAM_calloutPre, CSysConfig::Instance()->GetOutCallHead());

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "MakeCall AddCmdTask.requestId:[%u],CallerId:[%s],CalledId:[%s],TargetType:[%s]",
			l_lRequestId, p_strCallerId.c_str(), p_strCalledId.c_str(), p_strTargetType.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CMakeCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

//************************************
// Method:    ConsultationCall	咨询
// FullName:  ACS::ISwitchClient::ConsultationCall
// Access:    virtual public 
// Returns:   long	任务编号
// Qualifier:
// Parameter: const std::string& p_strCTICallRefId	咨询话务ID
// Parameter: const std::string & p_strSponsor	发起咨询的坐席号
// Parameter: const std::string & p_strTarget	咨询目标号码
// Parameter: const std::string & p_strTargetDeviceType	咨询目标号码的类型
// Parameter: bool p_bIsTransferCall	是否为转移产生的咨询，是：true，否：false
long CFSAesClientImpl::ConsultationCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType, bool p_bIsTransferCall)
{
	ICTICallPtr l_pCall = CCallManager::Instance()->GetCallByCTICallId(p_strCTICallRefId);
	if (l_pCall == nullptr)
	{
		ICC_LOG_FATAL(m_pLog, "not find call,CallId:[%s]", p_strCTICallRefId.c_str());
		return DEFAULT_TASKID;
	}

	//if (!l_pCall->GetCTIConferenceID().empty() && p_bIsTransferCall)
	//{
	//	ICC_LOG_FATAL(m_pLog, "conference not supported consultation transfer,CallId:[%s]", p_strCTICallRefId.c_str());
	//	return DEFAULT_TASKID;
	//}
	// 
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_CONSULTATION_CALL_CMD);//consultationCall
	if (l_pCmdRequest)
	{
		l_pCmdRequest->SetParam(CTI_PARAM_agentid, CDeviceManager::Instance()->GetAgentID(p_strSponsor));
		l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strCTICallRefId);
		l_pCmdRequest->SetParam(CTI_PARAM_caller, p_strSponsor);
		l_pCmdRequest->SetParam(CTI_PARAM_called, p_strTarget);
		//l_pCmdRequest->SetParam(CTI_PARAM_caseid, p_strCaseId);
		int l_nTmpType;
		l_pCmdRequest->SetParam("requester", p_strSponsor);
		l_pCmdRequest->SetParam("dest_number", p_strTarget);
		if (CDeviceManager::Instance()->IsExtension(p_strTarget, true))
		{
			l_pCmdRequest->SetParam(CTI_PARAM_direction, AES_CALL_DIRECTION_INSIDE);
			l_nTmpType = TransferType_inline;

			l_pCmdRequest->SetParam(CTI_PRAM_dest_type, AES_MEMBER_TYPE_inline);
		}
		else
		{
			l_pCmdRequest->SetParam(CTI_PARAM_direction, AES_CALL_DIRECTION_OUTSIDE);
			l_nTmpType = TransferType_outline;

			l_pCmdRequest->SetParam(CTI_PRAM_dest_type, AES_MEMBER_TYPE_outline);
		}

		l_pCmdRequest->SetParam(CTI_PARAM_calloutPre, CSysConfig::Instance()->GetOutCallHead());

		l_pCmdRequest->SetCTICallRefId(p_strCTICallRefId);
		CCallManager::Instance()->SetCallTransferType(p_strCTICallRefId, l_nTmpType, p_strTarget);

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "ConsultationCall AddCmdTask.request:[%u],CallRefId:[%s],Sponsor:[%s],Target:[%s],TransferType:[%d]",
			l_lRequestId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str(), l_nTmpType);

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CConsultationCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CFSAesClientImpl::AnswerCall(const std::string& p_strCTICallRefId, const std::string& p_strDevice)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_ANSWER_CALL_CMD);
	if (l_pCmdRequest)
	{
		//先要查询话务是否存在
		l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strCTICallRefId);
		l_pCmdRequest->SetParam(CTI_PARAM_extension, p_strDevice);
		l_pCmdRequest->SetParam(CTI_PARAM_agentid, CDeviceManager::Instance()->GetAgentID(p_strDevice));
		l_pCmdRequest->SetParam(CTI_PARAM_number, p_strDevice);
		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "AnswerCall AddCmdTask.requestId:[%u],CallRefId:[%s],Device:[%s]",
			l_lRequestId, p_strCTICallRefId.c_str(), p_strDevice.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CFSAesClientImpl::RefuseAnswer(const std::string& p_strCTICallRefId, const std::string& p_strDevice)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_REFUSE_CALL_CMD);
	if (l_pCmdRequest)
	{
		//先要查询话务是否存在
		l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strCTICallRefId);
		l_pCmdRequest->SetParam(CTI_PARAM_extension, p_strDevice);
		l_pCmdRequest->SetParam(CTI_PARAM_agentid, CDeviceManager::Instance()->GetAgentID(p_strDevice));

		l_pCmdRequest->SetParam(CTI_PARAM_channelnumber, p_strDevice);

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "RefuseAnswer AddCmdTask.requestId:[%u], CallRefId [%s], Device [%s]",
			l_lRequestId, p_strCTICallRefId.c_str(), p_strDevice.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CFSAesClientImpl::Hangup(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice)
{
	/*IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceBySrcCallId(p_strCTICallRefId);
	if (l_pConferencePtr)
	{
		//兼容avaya处理流程,前端强插前会发一个挂断消息，对接HCP不处理
		if (l_pConferencePtr->IsListenCreate() && l_pConferencePtr->GetCompere() == p_strDevice)
		{
			ICC_LOG_FATAL(m_pLog, "Listen state BargeIn hangup return,callId:[%s],Sponsor:[%s],Device:[%s] !!!", p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strDevice.c_str());
			return -2;
		}
	}*/

	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_HANGUP_CALL_CMD);
	if (l_pCmdRequest)
	{
		std::string l_strCTICallRefId = p_strCTICallRefId;
		ICTICallPtr l_pCall = CCallManager::Instance()->GetCallByCTICallId(p_strCTICallRefId);
		if (l_pCall)
		{
			if (!l_pCall->GetReplaceCallId().empty())
			{
				l_strCTICallRefId = l_pCall->GetReplaceCallId();
			}
		}

		//先要查询话务是否存在
		l_pCmdRequest->SetParam(CTI_PARAM_callid, l_strCTICallRefId);
		l_pCmdRequest->SetParam(CTI_PARAM_channelnumber, p_strDevice);
		l_pCmdRequest->SetParam(CTI_PARAM_sponsor, p_strSponsor);
		//p_strSponsor为发起挂断的坐席分机号
		l_pCmdRequest->SetParam(CTI_PARAM_agentid, CDeviceManager::Instance()->GetAgentID(p_strSponsor));

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "Hangup AddCmdTask.requestId:[%u], CallRefId [%s], Sponsor [%s] Device [%s]", l_lRequestId,
			p_strCTICallRefId.c_str(), 
			p_strSponsor.c_str(), p_strDevice.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CFSAesClientImpl::ClearCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_CLEAR_CALL_CMD);
	if (l_pCmdRequest)
	{
		std::string l_strCTICallRefId = p_strCTICallRefId;
		ICTICallPtr l_pCall = CCallManager::Instance()->GetCallByCTICallId(p_strCTICallRefId);
		if (l_pCall)
		{
			if (!l_pCall->GetReplaceCallId().empty())
			{
				l_strCTICallRefId = l_pCall->GetReplaceCallId();
			}
		}

		l_pCmdRequest->SetParam(CTI_PARAM_callid, l_strCTICallRefId);
		l_pCmdRequest->SetParam(CTI_PARAM_dest, p_strTarget);
		l_pCmdRequest->SetParam(CTI_PARAM_sponsor, p_strSponsor); 

		l_pCmdRequest->SetParam(CTI_PARAM_channelnumber, p_strTarget);
		//p_strSponsor为发起挂断的坐席分机号
		l_pCmdRequest->SetParam(CTI_PARAM_agentid, CDeviceManager::Instance()->GetAgentID(p_strSponsor));

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "ClearCall AddCmdTask.requestId:[%u],CallRefId [%s], Sponsor [%s]",
			l_lRequestId, p_strCTICallRefId.c_str(), p_strSponsor.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CClearCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}


long CFSAesClientImpl::PickupCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_PICKUP_CALL_CMD);
	if (l_pCmdRequest)
	{
		l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strCTICallRefId);
		l_pCmdRequest->SetParam(CTI_PARAM_agentid, CDeviceManager::Instance()->GetAgentID(p_strSponsor));
		l_pCmdRequest->SetParam(CTI_PARAM_requester, p_strSponsor); //发起代答的坐席号码
		l_pCmdRequest->SetParam(CTI_PARAM_called, p_strTarget); //被代答的坐席分机号


		CCallManager::Instance()->SetCallTransferType(p_strCTICallRefId, TransferType_pickup, p_strTarget);

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "PickupCall AddCmdTask.requestId:[%u], CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lRequestId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CPickupCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}


long CFSAesClientImpl::TransferCall(const std::string& p_strActiveCTICallRefId, const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	IFSAesCmdRequestPtr l_pCmdRequest = nullptr;
	bool l_bIsEx = false;
	if (p_strHeldCTICallRefId.empty())
	{
		l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_TRANSFER_CALL_CMD);
	}
	else
	{
		l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_TRANSFER_EX_CALL_CMD);
		l_bIsEx = true;
	}
	if (l_pCmdRequest)
	{
		l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strActiveCTICallRefId);
		l_pCmdRequest->SetParam(CTI_PARAM_agentid, CDeviceManager::Instance()->GetAgentID(p_strSponsor));
		l_pCmdRequest->SetParam(CTI_PRAM_src_number, p_strSponsor );	//发起转移的坐席号码
		l_pCmdRequest->SetParam(CTI_PRAM_dest_number, p_strTarget);  //转移的目标号码

		int l_nTmpType = -1;
		l_nTmpType = TransferType_inline;
		if (CDeviceManager::Instance()->IsExtension(p_strTarget, true))
		{
			l_pCmdRequest->SetParam(CTI_PRAM_dest_type, "1");
			l_nTmpType = TransferType_inline;
		}
		else
		{
			l_pCmdRequest->SetParam(CTI_PRAM_dest_type, "4"); //aes 定义的外线号码
			l_nTmpType = TransferType_outline;
		}
		if (!l_bIsEx)
		{
			
			l_pCmdRequest->SetParam(CTI_PARAM_calloutPre, CSysConfig::Instance()->GetOutCallHead());
		}
		else
		{
			l_pCmdRequest->SetParam(CTI_PARAM_requester, p_strSponsor);	//发起转移的坐席号码
		}
		CCallManager::Instance()->SetCallTransferType(p_strActiveCTICallRefId, l_nTmpType, p_strTarget);

		ICC_LOG_DEBUG(m_pLog, "set call transfer,ctiCallId:[%s],type:[%d],srcDeviceNum:[%s]", p_strActiveCTICallRefId.c_str(), l_nTmpType, p_strSponsor.c_str());

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "TransferCall AddCmdTask.requestId:[%u], ActiveCallRefId [%s], HeldCallRefId [%s], Sponsor [%s], Target [%s]",
			l_lRequestId, p_strActiveCTICallRefId.c_str(), p_strHeldCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CTransferCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

//tzx ????
long CFSAesClientImpl::DeflectCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_DEFLECT_CALL_CMD);
	if (l_pCmdRequest)
	{
		l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strCTICallRefId);
		l_pCmdRequest->SetParam(CTI_PARAM_agentid, CDeviceManager::Instance()->GetAgentID(p_strSponsor));
		l_pCmdRequest->SetParam(CTI_PARAM_sponsor, p_strSponsor); //发起偏转的坐席号码
		l_pCmdRequest->SetParam(CTI_PARAM_dest, p_strTarget); //偏转的目标号码

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "DeflectCall AddCmdTask.retuqestId:[%u] CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lRequestId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CDeflectCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CFSAesClientImpl::HoldCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_HOLD_CALL_CMD);
	if (l_pCmdRequest)
	{
		l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strCTICallRefId);
		l_pCmdRequest->SetParam(CTI_PARAM_agentid, CDeviceManager::Instance()->GetAgentID(p_strSponsor));
		l_pCmdRequest->SetParam(CTI_PARAM_sponsor, p_strSponsor); //发起的坐席号码
		l_pCmdRequest->SetParam(CTI_PARAM_number, p_strDevice); //被保留的号码

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "HoldCall AddCmdTask.requestId:[%u], CallRefId [%s], Sponsor [%s], Device [%s]",
			l_lRequestId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strDevice.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CHoldCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CFSAesClientImpl::RetrieveCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_RETRIEVE_CALL_CMD);
	if (l_pCmdRequest)
	{
		l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strCTICallRefId);
		l_pCmdRequest->SetParam(CTI_PARAM_agentid, CDeviceManager::Instance()->GetAgentID(p_strSponsor));
		l_pCmdRequest->SetParam(CTI_PARAM_sponsor, p_strSponsor); //发起的坐席号码
		l_pCmdRequest->SetParam(CTI_PARAM_number, p_strDevice); //重拾的坐席号

		std::string l_strCTICallRefId = p_strCTICallRefId;
		ICTICallPtr l_pCall = CCallManager::Instance()->GetCallByCTICallId(p_strCTICallRefId);
		std::string l_strCurrHoldDevice;
		if (l_pCall)
		{
			l_strCurrHoldDevice = l_pCall->GetHoldDeviceNum();
			if (!l_strCurrHoldDevice.empty())
			{
				l_pCmdRequest->SetParam(CTI_PARAM_number, l_strCurrHoldDevice); //重拾的坐席号
			}
		}

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);

		ICC_LOG_DEBUG(m_pLog, "RetrieveCall AddCmdTask.requestId:[%u], CallRefId [%s], Sponsor [%s], Device [%s],currHoldDevice:[%s]",
			l_lRequestId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strDevice.c_str(), l_strCurrHoldDevice.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CRetriveCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CFSAesClientImpl::ConferenceCall(const std::string& p_strTarget, const std::string& p_strHeldCTICallRefId, const std::string& p_strActiveCTICallRefId)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_CONFERENCE_CALL_CMD);
	if (l_pCmdRequest)
	{
		//l_pRequestNotif->SetHeldCTICallRefId(p_strHeldCTICallRefId);
		//l_pRequestNotif->SetActiveCTICallRefId(p_strActiveCTICallRefId);
		//l_pRequestNotif->SetTarget(p_strTarget);

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "ConferenceCall AddCmdTask.requestId:[%u], HeldCallRefId [%s], ActiveCallRefId [%s], Target [%s]",
			l_lRequestId, p_strHeldCTICallRefId.c_str(), p_strActiveCTICallRefId.c_str(), p_strTarget.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CConferenceCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}


long CFSAesClientImpl::ReconnectCall(const std::string& p_strActiveCTICallRefId, const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_RECONNECT_CALL_CMD);
	if (l_pCmdRequest)
	{
		l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strActiveCTICallRefId);

		l_pCmdRequest->SetParam(CTI_PRAM_src_number, p_strSponsor);
		l_pCmdRequest->SetParam(CTI_PRAM_dest_number, p_strDevice);


		l_pCmdRequest->SetParam("requester", p_strSponsor);
		//l_pRequestNotif->SetActiveCTICallRefId(p_strActiveCTICallRefId);
		//l_pRequestNotif->SetHeldCTICallRefId(p_strHeldCTICallRefId);
		//l_pRequestNotif->SetSponsor(p_strSponsor);
		//l_pRequestNotif->SetDevice(p_strDevice);
		//if (CDeviceManager::Instance()->IsExtension(p_strDevice, true))
		//{
		//	CCallManager::Instance()->SetCallTransferType(p_strActiveCTICallRefId, TransferType_inline, p_strDevice);
		//}
		
		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "ReconnectCall AddCmdTask.requestId:[%u], ActiveCallRefId [%s], HeldCallRefId [%s], Sponsor [%s], Device [%s]",
			l_lRequestId, p_strActiveCTICallRefId.c_str(), p_strHeldCTICallRefId.c_str(), p_strSponsor.c_str(), p_strDevice.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CReconnectCallNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}


long CFSAesClientImpl::GetCTIConnectState()
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(GET_CTICONNECT_STATE_CMD);
	if (l_pCmdRequest)
	{
		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiLocal_Query_cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "GetCTIConnectState AddCmdTask.requestId:[%u]", l_lRequestId);

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CFSAesClientImpl::GetDeviceList()
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(GET_DEVICE_LIST_CMD);
	if (l_pCmdRequest)
	{
		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiLocal_Query_cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "GetDeviceList AddCmdTask.requestId:[%u]", l_lRequestId);

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CFSAesClientImpl::GetACDList()
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(GET_ACD_LIST_CMD);
	if (l_pCmdRequest)
	{
		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiLocal_Query_cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "GetACDList AddCmdTask.requestId:[%u]", l_lRequestId);

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CFSAesClientImpl::GetAgentList(const std::string& p_strACDGrp)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(GET_AGENT_LIST_CMD);
	if (l_pCmdRequest)
	{
		l_pCmdRequest->SetParam(CTI_PARAM_skill, p_strACDGrp);

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiLocal_Query_cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "GetAgentList AddCmdTask.requestId:[%u], ACDGrp [%s]", l_lRequestId, p_strACDGrp.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CFSAesClientImpl::GetReadyAgent(const std::string& p_strACDGrp)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(GET_READY_AGENT_CMD);
	if (l_pCmdRequest)
	{
		l_pCmdRequest->SetParam(CTI_PARAM_skill, p_strACDGrp);
		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiLocal_Query_cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "GetReadyAgent AddCmdTask.requestId:[%u],acdGrp:[%s]", l_lRequestId, p_strACDGrp.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CGetReadyAgentNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CFSAesClientImpl::GetFreeAgentList(const std::string& p_strACDGrp, const std::string& p_strDeptCode)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(GET_FREE_AGENT_LIST_CMD);
	if (l_pCmdRequest)
	{
		l_pCmdRequest->SetParam(CTI_PARAM_DEPT_CODE, p_strDeptCode); //
		l_pCmdRequest->SetParam(CTI_PARAM_skill, p_strACDGrp); //
		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiLocal_Query_cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "GetFreeAgentList AddCmdTask.requestId:[%u], ACDGrp [%s],StaffCode[%s]", l_lRequestId, p_strACDGrp.c_str(), p_strDeptCode.c_str());

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}

long CFSAesClientImpl::GetCallList()
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(GET_CALL_LIST_CMD);
	if (l_pCmdRequest)
	{
		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiLocal_Query_cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "GetCallList AddCmdTask.requestId:[%u]", l_lRequestId);

		return l_lRequestId;
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return DEFAULT_TASKID;
}



long CFSAesClientImpl::SetBlackList(const std::string& p_strLimitNum, const std::string& p_strLimitType)
{
	CBlackListManager::Instance()->AddBlackList(p_strLimitNum);
	return DEFAULT_TASKID;
}

long CFSAesClientImpl::DeleteBlackList(const std::string& p_strLimitNum)
{
	CBlackListManager::Instance()->DeleteBlackList(p_strLimitNum);
	return DEFAULT_TASKID;
}

long CFSAesClientImpl::DeleteAllBlackList()
{
	CBlackListManager::Instance()->ClearBlackList();
	return DEFAULT_TASKID;
}

std::string CFSAesClientImpl::GetDeviceState(const std::string& strTarget)
{
	return CDeviceManager::Instance()->GetDeviceCurrState(strTarget);
}

std::string CFSAesClientImpl::GetReadyAgentEx(const std::string& strTarget)
{
	if (CDeviceManager::Instance()->IsExtension(strTarget))
	{
		std::string l_strOutLodinMode;
		std::string l_strOutReadyState;
		CAgentManager::Instance()->GetAgentState(strTarget, l_strOutLodinMode, l_strOutReadyState);

		if (l_strOutLodinMode == AgentModeTypeString[AGENT_MODE_LOGOUT])
		{
			return ReadyState_NotReady;
		}

		if (l_strOutReadyState != AgentReadyTypeString[AGENT_READY])
		{
			return ReadyState_NotReady;
		}

		return ReadyState_Ready;
	}
	
	return "";
}

std::string CFSAesClientImpl::GetCallerID(const std::string& p_strInCTICallRefId, std::string& p_strCalled)
{
	std::string l_strInCTICallRefId = p_strInCTICallRefId;
	std::string l_strOutCallerId;
	std::string l_strOutCalledParty;
	//CCallManager::Instance()->GetCallByCTICallRefId(l_strInCTICallRefId, l_strOutCallerId, l_strOutCalledParty);
	p_strCalled = l_strOutCalledParty;
	return l_strOutCallerId;
}

bool CFSAesClientImpl::GetCallTime(const std::string& p_strCTICallRefId, std::string& p_strRingTime, std::string& p_strTalkTime)
{
	return false;
}

long CFSAesClientImpl::FSAesAsyncRequestEx(const std::string p_strCmdName, const std::string& p_strResquestData)
{
	if (p_strCmdName == "get_history_callList")
	{
		IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_GET_HISTORY_CALL_LIST_CMD);
		if (l_pCmdRequest)
		{
			if (m_pJsonFty == nullptr)
			{
				ICC_LOG_ERROR(m_pLog, "json object is nullptr.");
				return DEFAULT_TASKID;
			}
			JsonParser::IJsonPtr l_JsonPtr = m_pJsonFty->CreateJson();
			if (!l_JsonPtr)
			{
				ICC_LOG_ERROR(m_pLog, "create json object failed.");
				return DEFAULT_TASKID;
			}

			if (!l_JsonPtr->LoadJson(p_strResquestData))
			{
				ICC_LOG_ERROR(m_pLog, "load json[%s] object failed.", p_strResquestData.c_str());
				return DEFAULT_TASKID;
			}

			std::string l_strPageSize = l_JsonPtr->GetNodeValue("/body/page_size", "");
			std::string l_strPageIndex = l_JsonPtr->GetNodeValue("/body/page_index", "");
			if (l_strPageSize.empty())
			{
				l_strPageSize = "200";
			}

			if (l_strPageIndex.empty())
			{
				l_strPageIndex = "1";
			}
			l_pCmdRequest->SetParam(CTI_PARAM_page_size, l_strPageSize); //
			l_pCmdRequest->SetParam(CTI_PARAM_page_index, l_strPageIndex); //

			long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiSyncRquest_Cmd, l_pCmdRequest);
			ICC_LOG_DEBUG(m_pLog, "GetHistoryCallList AddCmdTask.requestId:[%u], page_size:[%s],index:[%s]", l_lRequestId, l_strPageSize.c_str(), l_strPageIndex.c_str());

			return l_lRequestId;
		}
	}
	else if(p_strCmdName == "del_history_record")
	{
		IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_DEL_HISTORY_RECORD_CMD);
		if (nullptr == l_pCmdRequest)
		{
			return DEFAULT_TASKID;
		}

		if (m_pJsonFty == nullptr)
		{
			ICC_LOG_ERROR(m_pLog, "json object is nullptr.");
			return DEFAULT_TASKID;
		}
		JsonParser::IJsonPtr l_JsonPtr = m_pJsonFty->CreateJson();
		if (!l_JsonPtr)
		{
			ICC_LOG_ERROR(m_pLog, "create json object failed.");
			return DEFAULT_TASKID;
		}

		if (!l_JsonPtr->LoadJson(p_strResquestData))
		{
			ICC_LOG_ERROR(m_pLog, "load json[%s] object failed.", p_strResquestData.c_str());
			return DEFAULT_TASKID;
		}
		std::string strTalkTime = l_JsonPtr->GetNodeValue("/body/talk_time", "");
		std::string strHangupTime= l_JsonPtr->GetNodeValue("/body/hangup_time", "");

		std::string l_strCallId = l_JsonPtr->GetNodeValue("/body/callref_id", "");
		std::string l_strFileName = l_JsonPtr->GetNodeValue("/body/file_name", "");
		std::string l_strAgent = l_JsonPtr->GetNodeValue("/body/agent", "");
		std::string strIsRelease;

		//从talk_time字段以及hangup字段判断消息为call_over_sync传来的，当talk_time为空时，判定没有录音产生
		if (strTalkTime.empty()&& !strHangupTime.empty())
		{
			strIsRelease = "1";
		}
		else
		{
			strIsRelease = "0";
		}
		l_pCmdRequest->SetParam(CTI_PARAM_IS_RELEASE, strIsRelease); //
		l_pCmdRequest->SetParam(CTI_PARAM_callid, l_strCallId); //
		l_pCmdRequest->SetParam(CTI_PARAM_recordfile, l_strFileName); //
		l_pCmdRequest->SetParam(CTI_PARAM_agent, l_strAgent);

		if (strIsRelease == "0" || strIsRelease == "1")
		{
			long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiSyncRquest_Cmd, l_pCmdRequest);
			ICC_LOG_LOWDEBUG(m_pLog, "not need delete record with empty record");
		}
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "not support,cmdName:[%s]", p_strCmdName.c_str());
	}

	return DEFAULT_TASKID;
}
////////////////////////
// 
//会议相关接口
long CFSAesClientImpl::CreateConference(const std::string& p_strCompere, std::string& p_strConferenceId)
{
	std::string l_strCallId = p_strConferenceId;
	p_strConferenceId = "";
	std::string l_strCTIConferenceId = "";
	ICTICallPtr l_pCall = CCallManager::Instance()->GetCallByCTICallId(l_strCallId);

	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_CONFERENCE_CREATE_CMD);
	if (l_pCmdRequest && l_pCall)
	{
		l_strCTIConferenceId = l_pCall->GetCTIConferenceID();
		if (!l_strCTIConferenceId.empty())
		{
			if (CConferenceManager::Instance()->GetConferenceById(l_strCTIConferenceId))
			{
				p_strConferenceId = l_strCTIConferenceId;
				ICC_LOG_DEBUG(m_pLog, "the conference has been created,conferenceId:%s,callId:%s", l_strCTIConferenceId.c_str(), l_strCallId.c_str());
				return 1;
			}
		}

		l_pCmdRequest->SetParam(CTI_PARAM_moderator, p_strCompere); //
		IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->CreateCTIConference(p_strCompere, l_pCall);
		if (l_pConferencePtr)
		{
			l_strCTIConferenceId = l_pConferencePtr->GetConferenceId();
			l_pCmdRequest->SetParam(CTI_PARAM_callid, l_strCallId); //
			l_pCmdRequest->SetParam(CTI_PARAM_moderator, p_strCompere); //
			l_pCmdRequest->SetParam(CTI_PARAM_cti_conferenceid, l_strCTIConferenceId); //

			l_pConferencePtr->SetSrcCTICallId(l_strCallId);
			p_strConferenceId = l_strCTIConferenceId;

			long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
			ICC_LOG_DEBUG(m_pLog, "CreateConference AddCmdTask.requestId:[%u],conferenceId:%s,callId:%s", l_lRequestId, l_strCTIConferenceId.c_str(), l_strCallId.c_str());

			return l_lRequestId;
		}
		else
		{
			ICC_LOG_FATAL(m_pLog, "Create conference Failed !!!");
		}
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}
	return DEFAULT_TASKID;
}

long CFSAesClientImpl::ListenCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	ICTICallPtr l_pCall = CCallManager::Instance()->GetCallByCTICallId(p_strCTICallRefId);
	if (l_pCall)
	{
		std::string l_strCTIConferenceId = l_pCall->GetCTIConferenceID();
		if (!l_strCTIConferenceId.empty())
		{
			IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceById(l_strCTIConferenceId);
			if (l_pConferencePtr)
			{
				if (!l_pConferencePtr->AddMemberEx(p_strSponsor, TARGET_DEVICE_TYPE_INSIDE))
				{
					ICC_LOG_DEBUG(m_pLog, "ListenCall_ex failed,conferenceId:[%s],Sponsor:[%s],Target:[%s]",l_strCTIConferenceId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());
					return DEFAULT_TASKID;
				}
				l_pConferencePtr->SetListen(p_strTarget, p_strTarget);
				l_strCTIConferenceId = l_pConferencePtr->GetConferenceId();
				IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_MONITOR_EX_CALL_CMD);

				l_pCmdRequest->SetParam(CTI_PARAM_conferencename, l_pConferencePtr->GetConferenceName());
				l_pCmdRequest->SetParam(CTI_PARAM_member, p_strSponsor);
				l_pCmdRequest->SetParam(CTI_PARAM_cti_conferenceid, l_strCTIConferenceId);
				l_pCmdRequest->SetParam(CTI_PARAM_callid, l_pConferencePtr->GetSrcCTICallId());
				l_pCmdRequest->SetParam(CTI_PARAM_member_type, AES_MEMBER_TYPE_inline);
				l_pCmdRequest->SetParam(CTI_PARAM_calloutPre, CSysConfig::Instance()->GetOutCallHead());

				long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);

				ICC_LOG_DEBUG(m_pLog, "ListenCall_ex AddCmdTask.requestId:[%u],conferenceId:[%s],Sponsor:[%s],Target:[%s]",
					l_lRequestId, l_strCTIConferenceId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

				return l_lRequestId;
			}
		}
		
		IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->CreateCTIConference(p_strTarget, l_pCall);
		if (l_pConferencePtr)
		{
			l_strCTIConferenceId = l_pConferencePtr->GetConferenceId();
			IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_MONITOR_CALL_CMD);
			if (l_pCmdRequest)
			{
				l_pCmdRequest->SetParam(CTI_PARAM_moderator, p_strTarget); //
				l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strCTICallRefId); //
				l_pCmdRequest->SetParam(CTI_PARAM_cti_conferenceid, l_strCTIConferenceId); //
				//l_pCmdRequest->SetParam(CTI_PARAM_conferencename, l_pConferencePtr->GetConferenceName());
				long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
				ICC_LOG_DEBUG(m_pLog, "ListenCall AddCmdTask.requestId:[%u],conferenceId:[%s],Sponsor:[%s],Target:[%s]",
					l_lRequestId, l_strCTIConferenceId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

				l_pConferencePtr->AddMemberEx(p_strSponsor, TARGET_DEVICE_TYPE_INSIDE);//
				//l_pConferencePtr->SetListen(p_strSponsor, p_strTarget);
				l_pConferencePtr->SetListen(p_strTarget, p_strTarget);

				return l_lRequestId;
			}
		}
	}
	return DEFAULT_TASKID;
}

//强拆,为释放监听创建的会议，ICC只有监听成功后会发强拆
long CFSAesClientImpl::ForcePopCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceBySrcCallId(p_strCTICallRefId);
	if (l_pConferencePtr)
	{
		std::string l_strCTIConferenceId = l_pConferencePtr->GetConferenceId();
		//强拆挂断会议
		IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_FORCEPOP_CALL_CMD);
		l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strCTICallRefId);
		l_pCmdRequest->SetParam(CTI_PARAM_cti_conferenceid, l_strCTIConferenceId); //
		l_pCmdRequest->SetParam(CTI_PARAM_conferencename, l_pConferencePtr->GetConferenceName()); //
		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);

		//强拆挂机类型为被叫挂
		l_pConferencePtr->SetConferenceHangupType(HangupTypeString[HANGUP_TYPE_CALLED]);

		ICC_LOG_DEBUG(m_pLog, "ForcePopCall(conference) AddCmdTask.requestId:[%u], CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lRequestId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		return l_lRequestId;
	}

	return DEFAULT_TASKID;
}

//************************************
// Method:    TakeOverCall	接管话务
// FullName:  ACS::CSwitchClientImpl::TakeOverCall
// Access:    virtual public 
// Returns:   long
// Qualifier:
// Parameter: const std::string& p_strCTICallRefId	被接管话务 ID
// Parameter: const std::string & p_strSponsor	发起接管的坐席号
// Parameter: const std::string & p_strTarget	被接管的目标坐席号
//************************************
long CFSAesClientImpl::TakeOverCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceBySrcCallId(p_strCTICallRefId);
	if (l_pConferencePtr)
	{
		std::string l_strCTIConferenceId = l_pConferencePtr->GetConferenceId();

		IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_TAKEOVER_CALL_CMD);

		l_pCmdRequest->SetParam(CTI_PRAM_src_number, p_strTarget);
		l_pCmdRequest->SetParam(CTI_PRAM_dest_number, p_strSponsor); //发起接管的坐席号

		l_pCmdRequest->SetParam(CTI_PARAM_moderator, p_strTarget);
		l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strCTICallRefId);
		l_pCmdRequest->SetParam(CTI_PARAM_cti_conferenceid, l_strCTIConferenceId);
		l_pCmdRequest->SetParam(CTI_PARAM_conferencename, l_pConferencePtr->GetConferenceName());
		l_pCmdRequest->SetParam(CTI_PARAM_member, p_strSponsor); //

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "TakeOverCall AddCmdTask.requestId:[%u], CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lRequestId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		return l_lRequestId;
	}

	return DEFAULT_TASKID;
}

//强插
long CFSAesClientImpl::BargeInCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceBySrcCallId(p_strCTICallRefId);
	if (l_pConferencePtr)
	{
		if (l_pConferencePtr->IsBargeInCreate())
		{
			return DEFAULT_TASKID;
		}
		std::string l_strCTIConferenceId = l_pConferencePtr->GetConferenceId();
		//IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_CONFMEMBER_UNMUTE_CMD);REQUEST_BARGEIN_CALL_CMD
		IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_BARGEIN_CALL_CMD);
		if (l_pCmdRequest)
		{
			//强插,ICC只有监听成功后才能发强插，监听通过会议实现,主持人为监听的发起者，强插将主持人禁言关闭，监听发起者可以参与讲话
			l_pCmdRequest->SetParam(CTI_PARAM_moderator, p_strTarget);
			l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strCTICallRefId);
			l_pCmdRequest->SetParam(CTI_PARAM_cti_conferenceid, l_strCTIConferenceId);
			l_pCmdRequest->SetParam(CTI_PARAM_conferencename, l_pConferencePtr->GetConferenceName());
			l_pCmdRequest->SetParam(CTI_PARAM_member, p_strSponsor); //
			long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);

			l_pConferencePtr->SetBargeIn(p_strSponsor,p_strTarget);
			ICC_LOG_DEBUG(m_pLog, "BargeInCall AddCmdTask.requestId:[%u],conferenceId:[%s],Sponsor:[%s],Target:[%s]",
				l_lRequestId, l_strCTIConferenceId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

			return l_lRequestId;
		}
	}
	return DEFAULT_TASKID;
}

long CFSAesClientImpl::AddConferencePartyEx(const std::string& p_strCallRefId, const std::string& p_strTarget, const std::string& p_strTargetType, std::string& p_strConferenceId)
{
	long l_lNewTaskId = DEFAULT_TASKID;
	IFSAesCmdRequestPtr l_pCmdRequest = nullptr;
	std::string l_strCTIConferenceId;

	ICTICallPtr l_pCall = CCallManager::Instance()->GetCallByCTICallId(p_strCallRefId);
	if (l_pCall == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "call not found!!,callid:[%s]", p_strCallRefId.c_str());
		return DEFAULT_TASKID;
	}

	if (p_strConferenceId.empty())
	{
		std::string l_strCompere = p_strTarget;
		IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceBySrcCallId(p_strCallRefId);
		if (l_pConferencePtr != nullptr)
		{
			p_strConferenceId = l_pConferencePtr->GetConferenceId();
			ICC_LOG_ERROR(m_pLog, "the conference is in progress!!!! conferenceId:[%s],ctiCallId:[%s]", p_strConferenceId.c_str(), p_strCallRefId.c_str());
			return DEFAULT_TASKID;
		}
		else
		{
			l_pConferencePtr = CConferenceManager::Instance()->CreateCTIConference(l_strCompere, l_pCall);
			if (l_pConferencePtr)
			{
				std::string l_strCTIConferenceId = l_pConferencePtr->GetConferenceId();
				p_strConferenceId = l_strCTIConferenceId;
				IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_CONFERENCE_CREATE_CMD);
				if (l_pCmdRequest)
				{
					l_pCmdRequest->SetParam(CTI_PARAM_moderator, l_strCompere); //
					l_pCmdRequest->SetParam(CTI_PARAM_cti_conferenceid, l_strCTIConferenceId); //
					l_pCmdRequest->SetParam(CTI_PARAM_conferencename, l_pConferencePtr->GetConferenceName()); //
					l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strCallRefId); //
					long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
					ICC_LOG_DEBUG(m_pLog, "CreateConference_ex AddCmdTask.requestId:[%u],conferenceId:%s,ctiCallId:[%s]", l_lRequestId, l_strCTIConferenceId.c_str(), p_strCallRefId.c_str());

					return l_lRequestId;
				}
			}
		}
	}
	/*else
	{
		//把已经存在的呼叫加入到已有的会议
		IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceById(p_strConferenceId);
		if (l_pConferencePtr)
		{
		}
	}
	*/
	return DEFAULT_TASKID;
}

long CFSAesClientImpl::AddConferenceParty(const std::string& p_strConferenceId, const std::string& p_strTarget, const std::string& p_strTargetType)
{
	long l_lRequestId = DEFAULT_TASKID;
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_CONFERENCE_ADDPARTY_CMD);
	if (l_pCmdRequest)
	{
		IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceById(p_strConferenceId);
		if (l_pConferencePtr)
		{
			l_lRequestId = 1;
			std::string l_strTargetType = TARGET_DEVICE_TYPE_INSIDE;
			if (!CDeviceManager::Instance()->IsExtension(p_strTarget, true))
			{
				l_strTargetType = TARGET_DEVICE_TYPE_OUTSIDE;
			}
			if (!l_pConferencePtr->AddMemberEx(p_strTarget, l_strTargetType))
			{
				//有重复的会议成员
				return DEFAULT_TASKID;
			}

			std::string l_strConferenceName = l_pConferencePtr->GetConferenceName();
			if (!l_strConferenceName.empty())
			{
				//不为空说明HCP端会议已经创建成功了，将加成员命令发给HCP
				l_pCmdRequest->SetParam(CTI_PARAM_conferencename, l_strConferenceName);
				l_pCmdRequest->SetParam(CTI_PARAM_member, p_strTarget);
				
				l_pCmdRequest->SetParam(CTI_PARAM_cti_conferenceid, p_strConferenceId);
				l_pCmdRequest->SetParam(CTI_PARAM_callid, l_pConferencePtr->GetSrcCTICallId());
				if (l_strTargetType == TARGET_DEVICE_TYPE_INSIDE)
				{
					l_pCmdRequest->SetParam(CTI_PARAM_member_type, AES_MEMBER_TYPE_inline); 
				}
				else
				{
					l_pCmdRequest->SetParam(CTI_PARAM_member_type, AES_MEMBER_TYPE_outline);
				}
				l_pCmdRequest->SetParam(CTI_PARAM_calloutPre, CSysConfig::Instance()->GetOutCallHead());
				bool l_bListen = false;
				if (l_pConferencePtr->IsListenCreate())
				{
					l_bListen = true;
					l_pCmdRequest->SetParam(CTI_PARAM_isListen, "1");
					//l_pConferencePtr->SetNormalMode();
				}
				l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
				ICC_LOG_DEBUG(m_pLog, "AddConferenceParty AddCmdTask.requestId:[%u],conferenceId:[%s],conferenceName:[%s],target:[%s],bListen:%d", 
					l_lRequestId, p_strConferenceId.c_str(), l_strConferenceName.c_str(), p_strTarget.c_str(), l_bListen);
			}
		}
		else
		{
			ICC_LOG_FATAL(m_pLog, "not find conference,conferenceId:[%s] !!!", p_strConferenceId.c_str());
		}
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return l_lRequestId;
}

long CFSAesClientImpl::DeleteConferenceParty(const std::string& p_strConferenceId, std::string& p_strTarget, const std::string& p_strTargetType)
{
	long l_lRequestId = DEFAULT_TASKID;

	IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceById(p_strConferenceId);
	if (l_pConferencePtr)
	{
		IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_CONFERENCE_DELPARTY_CMD);
		if (l_pCmdRequest)
		{
			//std::string l_strCTICallId = l_pConferencePtr->GetMemberCTICallId(p_strTarget, p_strTargetType);
			std::string l_strCTICallId = l_pConferencePtr->GetSrcCTICallId();
			if (!l_strCTICallId.empty())
			{
				ICTICallPtr l_pCall = CCallManager::Instance()->GetCallByCTICallId(l_strCTICallId);
				if (l_pCall)
				{
					//话机拉会适配
					if (!l_pCall->GetReplaceCallId().empty())
					{
						l_strCTICallId = l_pCall->GetReplaceCallId();
					}
				}
				std::string l_strConferenceName = l_pConferencePtr->GetConferenceName();

				l_pCmdRequest->SetParam(CTI_PARAM_conferencename, l_strConferenceName);
				l_pCmdRequest->SetParam(CTI_PARAM_callid, l_strCTICallId);
				l_pCmdRequest->SetParam(CTI_PARAM_member, p_strTarget);
				l_pCmdRequest->SetParam(CTI_PARAM_cti_conferenceid, p_strConferenceId);

				l_pCmdRequest->SetParam(CTI_PARAM_channelnumber, p_strTarget);


				l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
				ICC_LOG_DEBUG(m_pLog, "delConferenceParty AddCmdTask.requestId:[%u] ,conferenceId:[%s],conferenceName:[%s],callId:[%s]",
					l_lRequestId, p_strConferenceId.c_str(), l_strConferenceName.c_str(), l_strCTICallId.c_str());
			}
			else
			{
				ICC_LOG_FATAL(m_pLog, "member callid not found!!!,ctiConfenceId:[%s],Target:[%s]", p_strConferenceId.c_str(), p_strTarget.c_str());
				l_lRequestId = 1;
			}
		}
		else
		{
			ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
		}
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "cti conferenceId not found!!!,ctiConfenceId:[%s],Target:[%s]", p_strConferenceId.c_str(), p_strTarget.c_str());
	}
	return l_lRequestId;
}

std::string CFSAesClientImpl::QueryConferenceByCallid(const std::string& p_strCallRefId, std::string& p_strCompere, std::string & p_strBargeInSponsor)
{
	std::string l_strConferenceId ="";
	IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceBySrcCallId(p_strCallRefId);
	if (l_pConferencePtr == nullptr)
	{
		ICTICallPtr l_pCTICallPtr = CCallManager::Instance()->GetCallByCTICallId(p_strCallRefId);
		if (l_pCTICallPtr)
		{
			string l_strConfId = l_pCTICallPtr->GetCTIConferenceID();
			l_pConferencePtr = CConferenceManager::Instance()->GetConferenceById(l_strConfId);
		}
	}

	if (l_pConferencePtr)
	{
		if (!l_pConferencePtr->IsListenCreate() && !l_pConferencePtr->IsTakeoverCreate())
		{
			p_strCompere = l_pConferencePtr->GetCompere();
			if (l_pConferencePtr->IsBargeInCreate())
			{
				p_strBargeInSponsor = l_pConferencePtr->GetBargeInSponsor();
			}

			l_strConferenceId = l_pConferencePtr->GetConferenceId();
		}
	}

	ICC_LOG_DEBUG(m_pLog, "query conference by callId!!!,ctiCallId:[%s],ctiConferenceId:[%s],compere:[%s],BargeInSponsor:[%s]",
		p_strCallRefId.c_str(), l_strConferenceId.c_str(), p_strCompere.c_str(), p_strBargeInSponsor.c_str());

	return l_strConferenceId;

	
}

long CFSAesClientImpl::DisenableConferenceParty(const std::string& p_strConferenceId, const std::string& p_strTarget, const std::string& p_strTargetType)
{
	//会议成员挂断后从本地会议列表中删除
	IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceById(p_strConferenceId);
	if (l_pConferencePtr)
	{
		l_pConferencePtr->DeleteMember(p_strTarget, p_strTargetType);

		return 1;
	}
	return DEFAULT_TASKID;
}

long CFSAesClientImpl::GetConferenceParty(const std::string& p_strConferenceId, std::string& p_strPartyData)
{
	if (CConferenceManager::Instance()->GetMeetingMembers(p_strConferenceId, p_strPartyData))
	{
		return 1;
	}
	return DEFAULT_TASKID;
}



