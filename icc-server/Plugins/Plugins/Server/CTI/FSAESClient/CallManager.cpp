#include "Boost.h"
#include "CallManager.h"
#include "CTIFSAesDefine.h"
#include "DeviceManager.h"
#include "ConferenceManager.h"
#include "FSAesSwitchManager.h"
#include "SysConfig.h"
////////////////////////////////////////////////////////////////////////////////
//
CCTICall::CCTICall(const std::string& p_strCTICallRefId,const long p_lCallIndex, CHcpDeviceCallEvent& p_HcpDeviceStateEvt)
	:m_strCTICallRefId(p_strCTICallRefId),
	m_lCallIndex(p_lCallIndex),
	m_strCurrCallState(""),
	m_strDialTime(""),
	m_strIncomingTime(""),
	m_strWaitTime(""),
	m_strAssignTime(""),
	m_strRingTime(""),
	m_strRingbackTime(""),
	m_strTalkTime(""),
	m_strHangupTime(""),
	m_strReleaseTime(""),
	m_nTransferType(TransferType_null),
	m_bBlackCall(false)
{
	m_strCaller = p_HcpDeviceStateEvt.m_oBody.m_strCaller;
	m_strCalled = p_HcpDeviceStateEvt.m_oBody.m_strCalled;
	m_strOriginalCalled = p_HcpDeviceStateEvt.m_oBody.m_strOrgCalled;

	m_strAcdSkillNum = p_HcpDeviceStateEvt.m_oBody.m_strSkill;

	m_strCallDirection = p_HcpDeviceStateEvt.GetCTICallDirection();
	m_strAgentExtension = p_HcpDeviceStateEvt.m_oBody.m_strAgentExtension;//坐席分机号码

	m_nSecondTicks = 0;
}

CCTICall::CCTICall(const std::string& p_strCTICallRefId, const long p_lCallIndex, const std::string& p_strCaller, const std::string& p_strCalled)
	:m_strCTICallRefId(p_strCTICallRefId),
	m_lCallIndex(p_lCallIndex),
	m_strCaller(p_strCaller),
	m_strCalled(p_strCalled),
	m_strAgentExtension(m_strCaller),
	m_strDialTime(""),
	m_strIncomingTime(""),
	m_strWaitTime(""),
	m_strAssignTime(""),
	m_strRingTime(""),
	m_strRingbackTime(""),
	m_strTalkTime(""),
	m_strHangupTime(""),
	m_strReleaseTime(""),
	m_nTransferType(TransferType_null),
	m_bBlackCall(false)
{
	m_strCallDirection = CallDirectionString[CALL_DIRECTION_OUT];
	m_strOriginalCalled = m_strCalled;

	m_nSecondTicks = 0;
}

void CCTICall::SetCallState(const std::string &p_strCallState,const std::string &p_strTime)
{
	if (p_strCallState == CallStateString[STATE_INCOMING])
	{
		if (m_strIncomingTime.empty())
		{
			m_strIncomingTime = p_strTime;
		}
	}
	else if (p_strCallState == CallStateString[STATE_QUEUE])
	{
		if (m_strIncomingTime.empty())
		{
			m_strIncomingTime = p_strTime;
		}
		if (m_strWaitTime.empty())
		{
			m_strWaitTime = p_strTime;
		}
	}
	else if (p_strCallState == CallStateString[STATE_ASSIGN])
	{
		if (m_strAssignTime.empty())
		{
			m_strAssignTime = p_strTime;
		}
	}
	else if (p_strCallState == CallStateString[STATE_RING])
	{
		if (m_strAssignTime.empty() && !m_strIncomingTime.empty())
		{
			m_strAssignTime = p_strTime;
		}

		if (!GetAcdNum().empty() && m_strIncomingTime.empty())
		{
			m_strAssignTime = p_strTime;
			m_strIncomingTime = p_strTime;
		}
		if (m_strRingTime.empty())
		{
			m_strRingTime = p_strTime;
		}
	}
	else if (p_strCallState == CallStateString[STATE_SIGNALBACK])
	{
		if (m_strRingbackTime.empty())
		{
			m_strRingbackTime = p_strTime;
		}

		if (m_strRingTime.empty())
		{
			m_strRingTime = p_strTime;
		}
	}
	else if (p_strCallState == CallStateString[STATE_TALK] || p_strCallState == CallStateString[STATE_CONFERENCE])
	{
		if (m_strTalkTime.empty())
		{
			m_strTalkTime = p_strTime;
		}
	}
	else if (p_strCallState == CallStateString[STATE_HANGUP])
	{
		//if (m_strHangupTime.empty())
		{
			m_strHangupTime = p_strTime;
		}
		//
		//m_strCTIConferenceId = "";
	}
	else if (p_strCallState == CallStateString[STATE_RELEASE])
	{
		m_strReleaseTime = p_strTime;
		//if (m_strHangupTime.empty())
		{
			m_strHangupTime = p_strTime;
		}
		//
		//m_strCTIConferenceId = "";
	}

	if (m_strCurrCallState == CallStateString[STATE_CONFERENCE])
	{
		return;
	}
	m_strCurrCallState = p_strCallState;
	m_strStateTime = p_strTime;
}


bool CCTICall::UpdateCTICall(CHcpDeviceCallEvent& p_HcpDeviceStateEvt,const std::string &p_strCurrTime)
{
	std::string l_strCallState = p_HcpDeviceStateEvt.GetCTICallState();
	std::string l_strLastCallState = m_strCurrCallState;
	if (l_strLastCallState != l_strCallState)
	{
		m_nSecondTicks = 0;
	}
	if (l_strCallState == CallStateString[STATE_TALK] && !GetCTIConferenceID().empty())
	{
		l_strCallState = CallStateString[STATE_CONFERENCE];
	}
	SetCallState(l_strCallState, p_strCurrTime);

	if (l_strLastCallState == CallStateString[STATE_INCOMING] ||
		l_strLastCallState == CallStateString[STATE_QUEUE] ||
		l_strLastCallState == CallStateString[STATE_ASSIGN])
	{
		if (p_HcpDeviceStateEvt.GetEventName() == CTIEVT_CALL_ASSIGNED || p_HcpDeviceStateEvt.GetEventName() == CTIEVT_CALL_RING)
		{
			SetCalled(p_HcpDeviceStateEvt.m_oBody.m_strCalled);
			SetAgentExtension(p_HcpDeviceStateEvt.m_oBody.m_strCalled);
		}
	}
	
	return true;
}


void CCTICall::SetCallTransferType(int p_nTransferType, const std::string& p_strTarget,const std::string &p_strDeviceState)
{
	if (p_nTransferType == TransferType_null && !p_strDeviceState.empty())
	{
		if (m_nTransferType != TransferType_null)
		{
			m_nSecondTicks = 0;
			//呼叫转移成功
			m_strCalled = p_strTarget;
			SetAgentExtension(m_strCalled);
			m_strCurrCallState = p_strDeviceState;
		}
		m_strTransferDstDevice = "";
		m_nTransferType = p_nTransferType;
	}
	else
	{
		m_nTransferType = p_nTransferType;
		m_strTransferDstDevice = p_strTarget;
	}
}

int CCTICall::GetCallTransferType()
{
	return m_nTransferType;
}

std::string CCTICall::GetCallTransferDstDevice()
{
	return m_strTransferDstDevice;
}

void CCTICall::SetCTIConferenceID(const std::string& p_strCTIConferenceId, const std::string& p_strCompere)
{ 
	m_strCTIConferenceId = p_strCTIConferenceId;
	m_strConferenceCompere = p_strCompere;
	if (m_strCTIConferenceId.empty())
	{
		m_strConferenceCompere = "";
	}
}

bool CCTICall::CheckCallTimeOut(int p_nWaitAnswerMaxTime, int m_nTalkMaxTime)
{
	m_nSecondTicks++;
	if (!GetTalkTime().empty())
	{
		if (m_nTalkMaxTime > 0 && m_nSecondTicks > m_nTalkMaxTime)
		{
			m_nSecondTicks = 0;
			return true;
		}
	}
	else
	{
		if (p_nWaitAnswerMaxTime > 0 && m_nSecondTicks > p_nWaitAnswerMaxTime)
		{
			m_nSecondTicks = 0;
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
#define CHECK_TIMESPAN			5		//	话务检测时间间隔，单位：秒
#define SLEEP_TIMESPAN			100		//	单位：毫秒

boost::shared_ptr<CCallManager> CCallManager::m_pManagerInstance = nullptr;
boost::shared_ptr<CCallManager> CCallManager::Instance()
{
	if (m_pManagerInstance == nullptr)
	{
		m_pManagerInstance = boost::make_shared<CCallManager>();
	}

	return m_pManagerInstance;
}

void CCallManager::ExitInstance()
{
	if (m_pManagerInstance)
	{
		m_pManagerInstance.reset();
	}
}

CCallManager::CCallManager(void)
{
	m_pLogPtr = nullptr;
	m_pStrUtil = nullptr;
	m_pDateTimePtr = nullptr;

	m_pCallCheckThread = nullptr;
	m_oLastCheckTime = 0;

	m_nWaitAnswerMaxTime = 5;
	m_nTalkMaxTime = 0;
}

CCallManager::~CCallManager(void)
{
	
}

void CCallManager::OnInit(IResourceManagerPtr p_pResourceManager)
{
	m_pLogPtr = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_pResourceManager)->GetLogger(MODULE_NAME);
	m_pStrUtil = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, p_pResourceManager)->CreateString();
	m_pDateTimePtr = ICCGetResourceEx(DateTime::IDateTimeFactory, ICCIDateTimeFactoryResourceName, p_pResourceManager)->CreateDateTime();

	m_oLastCheckTime = m_pDateTimePtr->CurrentDateTime();

	m_nWaitAnswerMaxTime = CSysConfig::Instance()->GetWaitAnswerMaxTime();
	if (m_nWaitAnswerMaxTime < 0)
	{
		m_nWaitAnswerMaxTime = 0;
	}

	m_nTalkMaxTime = CSysConfig::Instance()->GetCallInvalidTime() * 60;
	if (m_nTalkMaxTime < 0)
	{
		m_nTalkMaxTime = 0;
	}
}

void CCallManager::OnStart()
{
	if (!m_pCallCheckThread)
	{
		m_pCallCheckThread = boost::make_shared<CWorkThread<CCallManager>>(this, &CCallManager::_ThreadFunCheckCall);
	}

	if (m_pCallCheckThread)
	{
		m_oLastCheckTime = m_pDateTimePtr->CurrentDateTime();
		m_pCallCheckThread->StartThread();
	}
}

void CCallManager::OnStop()
{
	if (m_pCallCheckThread)
	{
		m_pCallCheckThread->StopThread();
	}
}

bool CCallManager::_CheckIsTimeout()
{
	DateTime::CDateTime l_oCurrentTime = m_pDateTimePtr->CurrentDateTime();
	//DateTime::CDateTime l_oEndTime = m_pDateTimePtr->AddSeconds(m_oLastCheckTime, CHECK_TIMESPAN);
	DateTime::CDateTime l_oEndTime = m_pDateTimePtr->AddSeconds(m_oLastCheckTime, 60);
	if (l_oCurrentTime > l_oEndTime)
	{
		m_oLastCheckTime = m_pDateTimePtr->CurrentDateTime();
		return true;
	}

	return false;
}

void CCallManager::_ThreadFunCheckCall()
{
	ICC_LOG_DEBUG(m_pLogPtr, "------------------ CheckCall Thread Start -----WaitAnswerMaxTime:%d,TalkMaxTime:%d------------", m_nWaitAnswerMaxTime, m_nTalkMaxTime);
	while (m_pCallCheckThread->isAlive())
	{
		if (_CheckIsTimeout())
		{
			ICC_LOG_DEBUG(m_pLogPtr, "------------------ CheckCall Thread Tick ------------------");
			_CheckCallTimeOut();
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN));
		}
	}
	ICC_LOG_DEBUG(m_pLogPtr, "------------------ CheckCall Thread Exit!! ------------------");
}

void CCallManager::_CheckCallTimeOut()
{
	if (m_nWaitAnswerMaxTime > 0 || m_nTalkMaxTime > 0)
	{
		std::map<std::string, ICTICallPtr> l_mapCalls;
		{
			SAFE_LOCK(m_callQueueMutex);
			l_mapCalls = m_mapCallQueue;
		}

		for (auto l_callIter : l_mapCalls)
		{
			if (l_callIter.second->CheckCallTimeOut(m_nWaitAnswerMaxTime, m_nTalkMaxTime))
			//if (l_callIter.second->CheckCallTimeOut(3, 0))
			{
				//DeleteCTICall(l_callIter.first);
				//ICHCPNotifyEeventPtr l_pHcpEvent = boost::make_shared<CHCPNotifyEevent>(p_strEventName, p_strNotifyEvent);
				CFSAesSwitchManager::Instance()->OnCheckCallTimeOut(l_callIter.first, l_callIter.second->GetCallState(), l_callIter.second->GetCallerId(), l_callIter.second->GetCalledId());
			}
		}
	}
	
}

void CCallManager::ReleaseAllCall()
{
	SAFE_LOCK(m_callQueueMutex);
	m_mapCallQueue.clear();
}

bool CCallManager::GetAllCall(std::map<std::string, ICTICallPtr> &p_mapCallQueue)
{
	SAFE_LOCK(m_callQueueMutex);
	p_mapCallQueue = m_mapCallQueue;

	return true;
}

int CCallManager::GetCallCount()
{
	SAFE_LOCK(m_callQueueMutex);
	return m_mapCallQueue.size();
}

std::string CCallManager::GetTalkTime(const std::string &p_strHcpCallId)
{
	SAFE_LOCK(m_callQueueMutex);
	auto l_iter = m_mapCallQueue.find(p_strHcpCallId);
	if (l_iter != m_mapCallQueue.end())
	{
		return l_iter->second->GetTalkTime();
	}
	return "";
}

//
ICTICallPtr CCallManager::CreateCTICallOut(const std::string& p_strHcpCallId, const std::string& p_strCaller, const std::string& p_strCalled)
{
	long l_lCallIndex = 0;
	//std::string l_strCTICallRefId = _CreateCTICallRefId(p_strHcpCallId, l_lCallIndex);
	//boost::shared_ptr<CCTICall> l_pCall = boost::make_shared<CCTICall>(l_strCTICallRefId, l_lCallIndex, p_strCaller, p_strCalled);
	boost::shared_ptr<CCTICall> l_pCall = boost::make_shared<CCTICall>(p_strHcpCallId, l_lCallIndex, p_strCaller, p_strCalled);
	if (l_pCall != nullptr)
	{
		std::string l_strCallState = CallStateString[STATE_DIAL];
		l_pCall->SetCallState(l_strCallState, m_pDateTimePtr->CurrentDateTimeStr());
		{
			SAFE_LOCK(m_callQueueMutex);
			m_mapCallQueue[p_strHcpCallId] = l_pCall;
		}

		//ICC_LOG_DEBUG(m_pLogPtr, "Create New Call,callout, HcpCallId:[%s], CTICallRefId:[%s],callState:[%s], QueueSize:[%d]",
		//	p_strHcpCallId.c_str(), l_strCTICallRefId.c_str(), l_strCallState.c_str(), m_mapCallQueue.size());
		ICC_LOG_DEBUG(m_pLogPtr, "Create New Call,callout, HcpCallId:[%s], CTICallRefId:[%s],callState:[%s],caller:[%s],called:[%s] QueueSize:[%d]",
			p_strHcpCallId.c_str(), p_strHcpCallId.c_str(), l_strCallState.c_str(), p_strCaller.c_str(), p_strCalled.c_str(), m_mapCallQueue.size());

		//if (!l_strCTICallRefId.empty())
		//{
		//	SAFE_LOCK(m_CallIdHelperMutex);
		//	m_mapCallIdHelperTB[l_strCTICallRefId] = p_strHcpCallId;
		//}

		return l_pCall;
	}
	else
	{
		ICC_LOG_FATAL(m_pLogPtr, "Create call Object Failed !!!");
	}
	return l_pCall;
}

ICTICallPtr CCallManager::CreateCTICall(CHcpDeviceCallEvent& p_HcpDeviceStateEvt, long& p_lCallIndex)
{
	long l_lCallIndex = 0;
	std::string l_strHcpCallId = p_HcpDeviceStateEvt.m_oBody.m_strHcpCallId;
	if (l_strHcpCallId.empty())
	{
		ICC_LOG_ERROR(m_pLogPtr, "Create call Failed,hcp call id is empty !!!");
		return nullptr;
	}

	if (p_HcpDeviceStateEvt.GetEventName() == CTIEVT_CALL_CONNECTED)
	{
		std::string l_strDeviceNum = p_HcpDeviceStateEvt.m_oBody.m_strDeviceNum;
		if (l_strDeviceNum.empty())
		{
			l_strDeviceNum = p_HcpDeviceStateEvt.m_oBody.m_strAgentExtension;
		}
		if (!CDeviceManager::Instance()->IsExtension(l_strDeviceNum))
		{
			ICC_LOG_ERROR(m_pLogPtr, "Create call Failed,Wrong number,number:[%s] !!!", l_strDeviceNum.c_str());
			return nullptr;
		}
	}

	std::string l_strCTICallRefId = _CreateCTICallRefId(l_strHcpCallId, l_lCallIndex);
	//boost::shared_ptr<CCTICall> l_pCall = boost::make_shared<CCTICall>(l_strCTICallRefId, l_lCallIndex, p_HcpDeviceStateEvt);
	boost::shared_ptr<CCTICall> l_pCall = boost::make_shared<CCTICall>(l_strHcpCallId, l_lCallIndex,p_HcpDeviceStateEvt);
	if(l_pCall != nullptr)
	{
		std::string l_strCallState = p_HcpDeviceStateEvt.GetCTICallState();
		l_pCall->SetCallState(l_strCallState, m_pDateTimePtr->CurrentDateTimeStr());
		p_lCallIndex = l_lCallIndex;
		{
			SAFE_LOCK(m_callQueueMutex);
			m_mapCallQueue[l_strHcpCallId] = l_pCall;
		}

		ICC_LOG_DEBUG(m_pLogPtr, "Create New Call, HcpCallId:[%s], CTICallRefId:[%s],callState:[%s],caller:[%s],called:[%s],OrgCalled:[%s], QueueSize:[%d]",
			l_strHcpCallId.c_str(), l_strHcpCallId.c_str(), l_strCallState.c_str(), 
			p_HcpDeviceStateEvt.m_oBody.m_strCaller.c_str(),
			p_HcpDeviceStateEvt.m_oBody.m_strCalled.c_str(),
			p_HcpDeviceStateEvt.m_oBody.m_strOrgCalled.c_str(),
			m_mapCallQueue.size());

		//ICC_LOG_DEBUG(m_pLogPtr, "Create New Call, HcpCallId:[%s], CTICallRefId:[%s],callState:[%s], QueueSize:[%d]",
		//	l_strHcpCallId.c_str(), l_strCTICallRefId.c_str(), l_strCallState.c_str(), m_mapCallQueue.size());
		//if (!l_strCTICallRefId.empty())
		//{
		//	SAFE_LOCK(m_CallIdHelperMutex);
		//	m_mapCallIdHelperTB[l_strCTICallRefId] = p_HcpDeviceStateEvt.m_oBody.m_strHcpCallId;
		//}
		if (p_HcpDeviceStateEvt.GetEventName() == CTIEVT_CALL_RINGBACK)
		{
			l_pCall->SetCallDirection(CallDirectionString[CALL_DIRECTION_OUT]);
		}
		else if (p_HcpDeviceStateEvt.GetEventName() == CTIEVT_CALL_RING)
		{
			if (CDeviceManager::Instance()->IsACDDevice(p_HcpDeviceStateEvt.m_oBody.m_strOrgCalled))
			{
				l_pCall->SetAcdNum(p_HcpDeviceStateEvt.m_oBody.m_strOrgCalled);
				l_pCall->SetCallState(CallStateString[STATE_RING], m_pDateTimePtr->CurrentDateTimeStr());
			}
			
			//CAgentManager::Instance()->GetACDList
			//m_oBody.m_strOrgCalled
		}
		
	}
	else
	{
		ICC_LOG_FATAL(m_pLogPtr, "Create call Object Failed !!!");
	}
	return l_pCall;
}

bool CCallManager::DeleteCTICall(const std::string& p_strHcpCallId)
{
	bool l_bReturn = false;
	std::string l_strCTICallRefId;
	int l_nQueueSize = 0;
	int l_nCallidTBSize = 0;
	{
		SAFE_LOCK(m_callQueueMutex);
		auto iter = m_mapCallQueue.find(p_strHcpCallId);
		if (iter != m_mapCallQueue.end())
		{
			if (iter->second)
			{
				l_bReturn = true;
				l_strCTICallRefId = iter->second->GetCTICallRefId();
				m_mapCallQueue.erase(iter);
			}
		}
		l_nQueueSize = m_mapCallQueue.size();
	}
	
	/*
	if (!l_strCTICallRefId.empty())
	{
		SAFE_LOCK(m_CallIdHelperMutex);
		m_mapCallIdHelperTB.erase(l_strCTICallRefId);

		l_nCallidTBSize = m_mapCallIdHelperTB.size();
	}
	*/
	ICC_LOG_DEBUG(m_pLogPtr, "Delete Call, HcpCallId:[%s], CTICallRefId:[%s], QueueSize:[%d_%d],res:%d", p_strHcpCallId.c_str(), l_strCTICallRefId.c_str(), l_nQueueSize, l_nCallidTBSize, l_bReturn);
	return l_bReturn;
}

bool CCallManager::GetCallList(IGetCallListResultNotifPtr l_pResultNotif)
{
	std::map<std::string, ICTICallPtr> l_mapCalls;
	{
		SAFE_LOCK(m_callQueueMutex);
		l_mapCalls = m_mapCallQueue;
	}

	for (auto l_callIter : l_mapCalls)
	{
		/*
		* 
		l_oData.m_strACDGrp = iter->m_strCalledId;
		l_oData.m_strAgentId = iter->m_strCalledParty;
		l_oData.m_strCalledId = iter->m_strCalledParty;

		l_oData.m_strCallRefId = iter->m_strCTICallRefId;
		l_oData.m_strCallerId = iter->m_strCallerId;
		
		l_oData.m_strState = iter->m_strCallState;
		l_oData.m_strStateTime = iter->m_strStateTime;
		*/
		//if (l_callIter.second->GetCallState().compare(CallStateString[STATE_HANGUP]) != 0)
		{
			CGetCallListResultNotif::CCallObj l_callObj;
			l_callObj.m_lCSTACallRefId = l_callIter.second->GetCallIndex();
			l_callObj.m_strCTICallRefId = l_callIter.second->GetCTICallRefId();
			l_callObj.m_strCallerId = l_callIter.second->GetCallerId();
			if (!l_callIter.second->GetAcdNum().empty())
			{
				l_callObj.m_strCalledId = l_callIter.second->GetAcdNum();
			}
			//else
			//{
			//	l_callObj.m_strCalledId = l_callIter.second->GetCalledId();
			//}

			if (l_callIter.second->GetCalledId() == l_callIter.second->GetOriginalCalled())
			{
				l_callObj.m_strCalledParty = l_callIter.second->GetCalledId();
			}
			else
			{
				l_callObj.m_strCalledParty = l_callIter.second->GetAgentExtension();
			}
			

			l_callObj.m_strCallDierection = l_callIter.second->GetCallDirection();
			l_callObj.m_strCallState = l_callIter.second->GetCallState();

			if (l_callObj.m_strCallState == CallStateString[STATE_CONFERENCE])
			{
				l_callObj.m_strCallState = CallStateString[STATE_TALK];
			}

			l_callObj.m_strStateTime = l_callIter.second->GetStateTime();

			if (!l_callIter.second->GetCTIConferenceID().empty())
			{
				l_callObj.m_strCallMode = "single";
			}
			else
			{
				l_callObj.m_strCallMode = "Conference";
			}

			l_pResultNotif->m_CallList.push_back(l_callObj);
		}
	}
	return true;
}


std::string CCallManager::GetHcpCallId(const std::string& p_strCITCallId)
{
	return p_strCITCallId;
	/*
	std::string l_strCTICallRefId = "";
	SAFE_LOCK(m_CallIdHelperMutex);
	auto l_iter = m_mapCallIdHelperTB.find(p_strCITCallId);
	if (l_iter != m_mapCallIdHelperTB.end())
	{
		l_strCTICallRefId = l_iter->second;
	}
	return l_strCTICallRefId;
	*/
}

ICTICallPtr CCallManager::GetCallByCTICallId(const std::string& p_strCTICallId)
{
	return GetCallByHcpCallId(p_strCTICallId);
	/*
	ICTICallPtr l_pCTICallPtr = nullptr;
	if (p_strCTICallId.empty())
	{
		return l_pCTICallPtr;
	}
	std::map<std::string, ICTICallPtr> l_mapCalls;
	{
		SAFE_LOCK(m_callQueueMutex);
		l_mapCalls = m_mapCallQueue;
	}
	
	auto l_iter = m_mapCallQueue.begin();
	while (l_iter != m_mapCallQueue.end())
	{
		if (l_iter->second->GetCTICallRefId() == p_strCTICallId)
		{
			l_pCTICallPtr = l_iter->second;
			break;
		}
		l_iter++;
	}
	return l_pCTICallPtr;
	*/
}

void CCallManager::SetCallTransferType(const std::string& p_strCTICallId, int p_nTransferType, const std::string& p_strTarget)
{
	if (p_strCTICallId.empty())
	{
		return;
	}
	ICTICallPtr l_pCTICallPtr = nullptr;
	{
		SAFE_LOCK(m_callQueueMutex);
		auto l_iter = m_mapCallQueue.begin();
		while (l_iter != m_mapCallQueue.end())
		{
			if (l_iter->second->GetCTICallRefId() == p_strCTICallId)
			{
				l_pCTICallPtr = l_iter->second;
				break;
			}
			l_iter++;
		}
	}
	
	if (l_pCTICallPtr)
	{
		l_pCTICallPtr->SetCallTransferType(p_nTransferType, p_strTarget);
	}
}

ICTICallPtr CCallManager::GetCallByHcpCallId(const std::string& p_strHcpCallId)
{
	ICTICallPtr l_pCTICallPtr = nullptr;

	if (p_strHcpCallId.empty())
	{
		return l_pCTICallPtr;
	}

	{
		SAFE_LOCK(m_callQueueMutex);
		auto iter = m_mapCallQueue.find(p_strHcpCallId);
		if (iter != m_mapCallQueue.end())
		{
			if (iter->second)
			{
				l_pCTICallPtr = iter->second;
			}
		}
	}

	if (l_pCTICallPtr == nullptr)
	{
		std::map<std::string, ICTICallPtr> l_mapCalls;
		{
			SAFE_LOCK(m_callQueueMutex);
			l_mapCalls = m_mapCallQueue;
		}

		for (auto l_callIter : l_mapCalls)
		{
			if (l_callIter.second->GetCTICallRefId() == p_strHcpCallId || l_callIter.second->GetReplaceCallId() == p_strHcpCallId)
			{
				l_pCTICallPtr = l_callIter.second;
			}
		}
	}

	return l_pCTICallPtr;
}

std::string CCallManager::_CreateCTICallRefId(const std::string &p_strHcpCallId, long &p_lCallIndex)
{
	static unsigned int l_lCSTACallRefId = 1000;

	time_t l_tCurrentTime = time(0);
	char l_szDayTime[MAX_BUFFER] = { 0 };
	strftime(l_szDayTime, sizeof(l_szDayTime), "%Y%m%d%H%M%S", gmtime(&l_tCurrentTime));

#if defined(WIN32) || defined(WIN64)
#ifdef _DEBUG
	std::string l_strCallRefId = m_pStrUtil->Format("110%u", l_lCSTACallRefId);
#else
	std::string l_strCallRefId = m_pStrUtil->Format("%s%06u", l_szDayTime, l_lCSTACallRefId);
#endif // _DEBUG
#else
	std::string l_strCallRefId = m_pStrUtil->Format("%s%06u", l_szDayTime, l_lCSTACallRefId);
#endif
	p_lCallIndex = l_lCSTACallRefId;

	l_lCSTACallRefId++;
	if (l_lCSTACallRefId >= 999999)
	{
		l_lCSTACallRefId = 1000;
	}
	return l_strCallRefId;
}
