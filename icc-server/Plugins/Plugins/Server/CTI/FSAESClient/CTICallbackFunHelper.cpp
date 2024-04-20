#include "CTICallbackFunHelper.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCTICallbackFunHelper::CCTICallbackFunHelper()
	:m_pSwitchEventCallback(nullptr),
	m_pCallBackThread(nullptr),
	m_bThreadStart(false)
{

}

CCTICallbackFunHelper::~CCTICallbackFunHelper()
{
	Stop();
}

bool CCTICallbackFunHelper::Start()
{
	_InitCTICallbackEventFun();

	if (!m_pCallBackThread)
	{
		m_pCallBackThread = boost::make_shared<CWorkThread<CCTICallbackFunHelper>>(this, &CCTICallbackFunHelper::_ThreadFun);
	}
	if (m_pCallBackThread)
	{
		m_pCallBackThread->StartThread();
		m_bThreadStart = true;
	}

	return m_bThreadStart;
}

void CCTICallbackFunHelper::DispatchRequestResult(const std::string &p_strCallbackFunName, ISwitchNotifPtr m_pCallbackNotify,const std::string &p_strCmdName)
{
	ICallbackTaskPtr l_callbackObj = boost::make_shared<CCallbackTask>();
	if (l_callbackObj)
	{
		l_callbackObj->SetCallbackFunName(p_strCallbackFunName);
		l_callbackObj->SetCmdName(p_strCmdName);
		l_callbackObj->SetResultMsg();
		l_callbackObj->SetCallbackNotify(m_pCallbackNotify);
		{
			std::lock_guard<std::mutex> l_lock(m_mtxCallbackTask);
			m_listCallbackTask.push_back(l_callbackObj);
		}
		m_cvMsg.notify_one();
	}
}

void CCTICallbackFunHelper::DispatchCTIEvent(const std::string& p_strCallbackFunName, ISwitchNotifPtr m_pCallbackNotify, const std::string& p_strCmdName)
{
	ICallbackTaskPtr l_callbackObj = boost::make_shared<CCallbackTask>();
	if (l_callbackObj)
	{
		l_callbackObj->SetCallbackFunName(p_strCallbackFunName);
		l_callbackObj->SetCallbackNotify(m_pCallbackNotify);
		l_callbackObj->SetCmdName(p_strCmdName);
		{
			std::lock_guard<std::mutex> l_lock(m_mtxCallbackTask);
			m_listCallbackTask.push_back(l_callbackObj);
		}
		m_cvMsg.notify_one();
	}
}

void CCTICallbackFunHelper::Stop()
{
	m_bThreadStart = false;
	m_cvMsg.notify_all();
	m_pCallBackThread->StopThread();
}

void CCTICallbackFunHelper::_InitCTICallbackEventFun()
{
	REGISTER_CALLBACK_FUN(Event_CTIConnState);
	REGISTER_CALLBACK_FUN(Event_AgentState);
	REGISTER_CALLBACK_FUN(Event_CallState);
	REGISTER_CALLBACK_FUN(Event_CallOver);
	REGISTER_CALLBACK_FUN(Event_DeviceState);
	REGISTER_CALLBACK_FUN(Event_ConferenceHangup);
	REGISTER_CALLBACK_FUN(Event_RefuseCall);
	REGISTER_CALLBACK_FUN(Event_Failed);
	REGISTER_CALLBACK_FUN(Event_BlackTransfer);

	REGISTER_CALLBACK_FUN(CR_AgentLogin);
	REGISTER_CALLBACK_FUN(CR_AgentLogout);
	REGISTER_CALLBACK_FUN(CR_SetAgentState);

	REGISTER_CALLBACK_FUN(CR_AnswerCall);
	REGISTER_CALLBACK_FUN(CR_RefuseAnswer);
	REGISTER_CALLBACK_FUN(CR_Hangup);
	REGISTER_CALLBACK_FUN(CR_ClearCall);
	REGISTER_CALLBACK_FUN(CR_ListenCall);
	REGISTER_CALLBACK_FUN(CR_PickupCall);
	REGISTER_CALLBACK_FUN(CR_BargeInCall);
	REGISTER_CALLBACK_FUN(CR_ForcePopCall);
	REGISTER_CALLBACK_FUN(CR_DeflectCall);
	REGISTER_CALLBACK_FUN(CR_HoldCall);
	REGISTER_CALLBACK_FUN(CR_RetrieveCall);
	REGISTER_CALLBACK_FUN(CR_ReconnectCall);
	REGISTER_CALLBACK_FUN(CR_ConferenceCall);
	REGISTER_CALLBACK_FUN(CR_MakeCall);
	REGISTER_CALLBACK_FUN(CR_ConsultationCall);
	REGISTER_CALLBACK_FUN(CR_TransferCall);
	REGISTER_CALLBACK_FUN(CR_TakeOverCall);
	REGISTER_CALLBACK_FUN(CR_GetCTIConnState);
	REGISTER_CALLBACK_FUN(CR_GetDeviceList);
	REGISTER_CALLBACK_FUN(CR_GetACDList);
	REGISTER_CALLBACK_FUN(CR_GetAgentList);
	REGISTER_CALLBACK_FUN(CR_GetCallList);
	REGISTER_CALLBACK_FUN(CR_GetReadyAgent);
	REGISTER_CALLBACK_FUN(CR_GetFreeAgentList);

	REGISTER_CALLBACK_FUN(CR_FSAesEventEx);
}


void CCTICallbackFunHelper::_ThreadFun()
{
	while (true)
	{
		//if (m_listCallbackTask.empty())
		{
			std::unique_lock<std::mutex> l_lock(m_mutexWait);
			m_cvMsg.wait_for(l_lock, std::chrono::seconds(1));
		}

		if (!m_bThreadStart)
		{
			break;
		}

		std::list<ICallbackTaskPtr> l_listCallbackTask;
		{
			std::lock_guard<std::mutex> l_lock(m_mtxCallbackTask);
			l_listCallbackTask.swap(m_listCallbackTask);
		}

		for (auto l_callBackObj : l_listCallbackTask)
		{
			_ExeCTICallBack(l_callBackObj);
		}
	}
}

void CCTICallbackFunHelper::_ExeCTICallBack(ICallbackTaskPtr p_pCallbackTask)
{
	if (p_pCallbackTask == nullptr)
	{
		return;
	}

	std::string l_strFunName = p_pCallbackTask->GetCallbackFunName();
	auto l_iter = m_mapCTICallbackEvent.find(l_strFunName);
	if (l_iter != m_mapCTICallbackEvent.end())
	{
		PCTICallbackEvent l_pFunc = l_iter->second;
		if (l_pFunc)
		{
			int l_nRequestId = -1;
			if (p_pCallbackTask->IsResultMsg())
			{
				ISwitchResultNotifPtr l_pNotify = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pCallbackTask->GetCallbackNotify());
				if (l_pNotify)
				{
					l_nRequestId = l_pNotify->GetRequestId();
				}
			}
			
			ICC_LOG_DEBUG(m_pLogPtr, "cti callback begin,fun:[%s],requestId:[%d],cmdName:[%s]", l_strFunName.c_str(), l_nRequestId, p_pCallbackTask->GetCmdName().c_str());
			(this->*l_pFunc)(p_pCallbackTask->GetCmdName(), p_pCallbackTask->GetCallbackNotify());
			ICC_LOG_DEBUG(m_pLogPtr, "cti callback end,fun:[%s]", l_strFunName.c_str());
		}
		else
		{
			ICC_LOG_ERROR(m_pLogPtr, "callback fun is null,fun:[%s]", l_strFunName.c_str());
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLogPtr, "not find callback fun,fun:[%s]", l_strFunName.c_str());
	}
}
