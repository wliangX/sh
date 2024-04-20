#include "Boost.h"

#include "TransferCallManager.h"
#include "HytProtocol.h"

#define CHECK_TIMESPAN			1	//单位：秒
#define SLEEP_TIME				100  //单位：毫秒
#define TRANSFER_RING_TIMEOUT	20   //单位：秒

CTransferCall::CTransferCall()
{
	m_bTargetIsACD = false;

	m_lGetReadyAgentTaskId = DEFAULT_TASKID;
	m_lConsultationCallTaskId = DEFAULT_TASKID;
	m_lTransferCallTaskId = DEFAULT_TASKID;

	m_strRelatedId = "";
	m_strHeldCallRefId = "";		//	保留的话务 ID
	m_strActiveCallRefId = "";		//	咨询产生的话务 ID
	m_strRingTime = "";				//	被咨询方振铃开始时间
	m_strTargetState = "";			//	被咨询方的状态，振铃、通话、挂机
	m_strSponsor = "";				//	发起咨询的坐席号
	m_strTarget = "";				//	咨询目标坐席号
	m_strTargetDeviceType = "";

	m_pNotifiRequest = nullptr;
}
CTransferCall::~CTransferCall()
{
	//
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
boost::shared_ptr<CTransferCallManager> CTransferCallManager::m_pManagerInstance = nullptr;
boost::shared_ptr<CTransferCallManager> CTransferCallManager::Instance()
{
	if (m_pManagerInstance == nullptr)
	{
		m_pManagerInstance = boost::make_shared<CTransferCallManager>();
	}

	return m_pManagerInstance;
}
void CTransferCallManager::ExitInstance()
{
	StopThread();
}

CTransferCallManager::CTransferCallManager(void)
{
	m_bThreadAlive = false;

	m_nRingTimeout = TRANSFER_RING_TIMEOUT;
	m_LogPtr = nullptr;
	m_DateTimePtr = nullptr;
	m_SwitchClientPtr = nullptr;
	m_pCheckRingTimeoutThread = nullptr;

	m_oLastCheckTime = 0;
	m_bFsAesMode = false;
}

CTransferCallManager::~CTransferCallManager(void)
{
	//
}
//////////////////////////////////////////////////////////////////////////

void CTransferCallManager::AddNewTransferCall(const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget,
	const std::string& p_strTargetDeviceType, const std::string& p_strMsgId, bool p_bIsACD, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	boost::shared_ptr<CTransferCall> l_pTransferCall = boost::make_shared<CTransferCall>();
	l_pTransferCall->m_strHeldCallRefId = p_strHeldCallRefId;
	l_pTransferCall->m_strSponsor = p_strSponsor;
	l_pTransferCall->m_strTarget = p_strTarget;
	l_pTransferCall->m_strTargetDeviceType = p_strTargetDeviceType;
	l_pTransferCall->m_bTargetIsACD = p_bIsACD;
	l_pTransferCall->m_strRelatedId = p_strMsgId;
	l_pTransferCall->m_pNotifiRequest = p_pNotifiRequest;
	if (p_bIsACD)
	{
		//	转机构，按 ACD查找空闲坐席
		l_pTransferCall->m_lGetReadyAgentTaskId = GetReadyAgent(p_strTarget);
	} 
	else
	{
		//	直接转坐席，发起咨询
		l_pTransferCall->m_lConsultationCallTaskId = ConsultationCall(p_strHeldCallRefId, p_strSponsor, p_strTarget, p_strTargetDeviceType);
	}

	SAFE_LOCK(m_transferCallMutex);
	m_mTransferCallList[p_strHeldCallRefId] = l_pTransferCall;

	ICC_LOG_DEBUG(m_LogPtr, "AddNewTransferCall, HeldCallRefId: %s, Sponsor: %s, Target: %s, TargetDeviceType: %s",
		p_strHeldCallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str(), p_strTargetDeviceType.c_str());
}

void CTransferCallManager::CancelTransferCall(const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	{
		SAFE_LOCK(m_transferCallMutex);
		auto it = m_mTransferCallList.find(p_strHeldCallRefId);
		if (it != m_mTransferCallList.end())
		{
			if (it->second)
			{
				std::string l_strActiveCallRefId = it->second->m_strActiveCallRefId;
				if (m_bFsAesMode)
				{
					ReconnectCall(l_strActiveCallRefId, p_strHeldCallRefId, p_strSponsor, p_strTarget);
				}
				else
				{
					ReconnectCall(l_strActiveCallRefId, p_strHeldCallRefId, p_strSponsor, p_strSponsor);
				}
				
			}
		}
	}

	SendTransferCallResult(p_strHeldCallRefId, ERROR_CANCEL_TRANSFER_CALL);
}
void CTransferCallManager::DeleteTransferCall(const std::string& p_strHeldCallRefId)
{
	SAFE_LOCK(m_transferCallMutex);
	auto it = m_mTransferCallList.find(p_strHeldCallRefId);
	if (it != m_mTransferCallList.end())
	{
		ICC_LOG_DEBUG(m_LogPtr, "DeleteTransferCall, HeldCallRefId: %s ",
			p_strHeldCallRefId.c_str());

		m_mTransferCallList.erase(it);
	}
}

bool CTransferCallManager::FindTransferCallByHeldCallRefId(const std::string& p_strHeldCallRefId)
{
	bool l_bFind = false;

	SAFE_LOCK(m_transferCallMutex);
	auto it = m_mTransferCallList.find(p_strHeldCallRefId);
	if (it != m_mTransferCallList.end())
	{
		l_bFind = true;
	}

	return l_bFind;
}
bool CTransferCallManager::FindTransferCallByActiveCallRefId(const std::string& p_strActiveCallRefId)
{
	bool l_bFind = false;

	SAFE_LOCK(m_transferCallMutex);
	for (auto l_callObj : m_mTransferCallList)
	{
		if (l_callObj.second->m_strActiveCallRefId.compare(p_strActiveCallRefId) == 0)
		{
			l_bFind = true;

			break;
		}
	}

	return l_bFind;
}
bool CTransferCallManager::FindTransferCallByGetReadyAgentTaskId(long p_lTaskId)
{
	bool l_bFind = false;

	SAFE_LOCK(m_transferCallMutex);
	for (auto l_callObj : m_mTransferCallList)
	{
		if (l_callObj.second->m_lGetReadyAgentTaskId == p_lTaskId)
		{
			l_bFind = true;

			break;
		}
	}

	return l_bFind;
}

bool CTransferCallManager::FindTransferCallByConsultationCallTaskId(long p_lTaskId)
{
	bool l_bFind = false;

	SAFE_LOCK(m_transferCallMutex);
	for (auto l_callObj : m_mTransferCallList)
	{
		if (l_callObj.second->m_lConsultationCallTaskId == p_lTaskId)
		{
			l_bFind = true;

			break;
		}
	}

	return l_bFind;
}

bool CTransferCallManager::FindTransferCallByTransferCallTaskId(long p_lTaskId)
{
	bool l_bFind = false;

	SAFE_LOCK(m_transferCallMutex);
	for (auto l_callObj : m_mTransferCallList)
	{
		if (l_callObj.second->m_lTransferCallTaskId == p_lTaskId)
		{
			l_bFind = true;

			break;
		}
	}

	return l_bFind;
}

void CTransferCallManager::ClearTransferCallList()
{
	SAFE_LOCK(m_transferCallMutex);
	m_mTransferCallList.clear();
}
//////////////////////////////////////////////////////////////////////////

void CTransferCallManager::SendTransferCallResult(const std::string& p_strHeldCallRefId, int p_nResult)
{
	std::string l_strHeldCallRefId = "";
	std::string l_strActiveCallRefId = "";
	std::string l_strSponsor = "";
	std::string l_strTarget = "";
	std::string l_strRelatedId = "";

	ObserverPattern::INotificationPtr l_pNotifiRequest = nullptr;
	ICC_LOG_DEBUG(m_LogPtr, "SendTransferCallResult,callId:[%s],res:%d", p_strHeldCallRefId.c_str(), p_nResult);
	{
		SAFE_LOCK(m_transferCallMutex);
		auto it = m_mTransferCallList.find(p_strHeldCallRefId);
		if (it != m_mTransferCallList.end())
		{
			if (it->second)
			{
				l_strHeldCallRefId = it->second->m_strHeldCallRefId;
				l_strActiveCallRefId = it->second->m_strActiveCallRefId;
				l_strSponsor = it->second->m_strSponsor;
				l_strTarget = it->second->m_strTarget;
				l_strRelatedId = it->second->m_strRelatedId;
				l_pNotifiRequest = it->second->m_pNotifiRequest;
			}
		}
	}

	if (!l_strHeldCallRefId.empty())
	{
		if (l_pNotifiRequest)
		{
			bool l_bRes = true;
			if (p_nResult != Result_GetReadyAgent_Success && p_nResult != ERROR_CMD_SUCCESS)
			{
				l_bRes = false;
			}
			CHytProtocol::Instance()->CR_TransferCallEx(l_pNotifiRequest, l_strActiveCallRefId, l_strSponsor, l_strTarget, l_strRelatedId, l_bRes);
		}
		
		CHytProtocol::Instance()->Sync_TransferCall(l_strHeldCallRefId, l_strActiveCallRefId, l_strSponsor, l_strTarget, l_strRelatedId, p_nResult);

		if (p_nResult != Result_GetReadyAgent_Success)
		{
			// 查询空闲坐席成功，继续转移流程
			// 转移成功 或 失败，删除转移记录
			DeleteTransferCall(p_strHeldCallRefId);
		}
	}
}

//	查询空闲坐席
long CTransferCallManager::GetReadyAgent(const std::string& p_strACDGrp)
{
	long l_lTaskId = DEFAULT_TASKID;

	if (m_SwitchClientPtr)
	{
		l_lTaskId = m_SwitchClientPtr->GetReadyAgent(p_strACDGrp);

		ICC_LOG_DEBUG(m_LogPtr, "Exe GetReadyAgent, TaskId: %u, ACDGrp: %s",
			l_lTaskId, p_strACDGrp.c_str());
	}

	return l_lTaskId;
}

//	查询空闲坐席结果
void CTransferCallManager::ProcessGetReadyAgentResult(long p_lTaskId, const std::string& p_strTarget, bool p_bResult)
{
	ICC_LOG_DEBUG(m_LogPtr, "GetReadyAgent Result, RequestId: %u Target: %s, Result: %d",
		p_lTaskId, p_strTarget.c_str(), p_bResult);

	if (p_bResult)
	{
		SAFE_LOCK(m_transferCallMutex);
		for (auto l_callObj : m_mTransferCallList)
		{
			// 查询空闲坐席成功
			if (l_callObj.second->m_lGetReadyAgentTaskId == p_lTaskId)
			{
				std::string l_strHeldCallRefId = l_callObj.second->m_strHeldCallRefId;
				std::string l_strSponsor = l_callObj.second->m_strSponsor;
				std::string l_strTargetDeviceType = l_callObj.second->m_strTargetDeviceType;

				l_callObj.second->m_strTarget = p_strTarget;
				l_callObj.second->m_lConsultationCallTaskId = ConsultationCall(l_strHeldCallRefId, l_strSponsor, p_strTarget, l_strTargetDeviceType);

				break;
			}
		}
	}
	else
	{
		bool l_bFindTaskId = false;
		std::string l_strHeldCallRefId = "";

		{
			SAFE_LOCK(m_transferCallMutex);
			for (auto l_callObj : m_mTransferCallList)
			{
				// 无空闲坐席，转移失败
				if (l_callObj.second->m_lGetReadyAgentTaskId == p_lTaskId)
				{
					l_bFindTaskId = true;
					l_strHeldCallRefId = l_callObj.second->m_strHeldCallRefId;

					break;
				}
			}
		}

		if (l_bFindTaskId)
		{
			SendTransferCallResult(l_strHeldCallRefId, ERROR_NO_READY_AGENT);
		}
	}
}

//	咨询转移目标
long CTransferCallManager::ConsultationCall(const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType)
{
	long l_lTaskId = DEFAULT_TASKID;

	if (m_SwitchClientPtr)
	{
		l_lTaskId = m_SwitchClientPtr->ConsultationCall(p_strHeldCallRefId, p_strSponsor, p_strTarget, p_strTargetDeviceType, true);

		ICC_LOG_DEBUG(m_LogPtr, "ExecuteTransferCall, TaskId: %u, HeldCallRefId: %s, Sponsor: %s, Target: %s, TargetDeviceType: %s",
			l_lTaskId, p_strHeldCallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str(), p_strTargetDeviceType.c_str());
	}

	return l_lTaskId;
}

//咨询结果处理
void CTransferCallManager::ProcessConsultationCallResult(long p_lTaskId, const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId, bool p_bResult, int p_nErrorCode)
{
	ICC_LOG_DEBUG(m_LogPtr, "ConsultationCall Result, RequestId: %u HeldCallRefId: %s, ActiveCallRefId: %s, Result: %d, ErrorCode: %d",
		p_lTaskId, p_strHeldCallRefId.c_str(), p_strActiveCallRefId.c_str(), p_bResult, p_nErrorCode);

	if (p_bResult)
	{
		SAFE_LOCK(m_transferCallMutex);
		auto it = m_mTransferCallList.find(p_strHeldCallRefId);
		if (it != m_mTransferCallList.end())
		{
			// 咨询成功，保存咨询产生的话务 ID,等待对方接通后再转移
			if (it->second && it->second->m_lConsultationCallTaskId == p_lTaskId)
			{
				it->second->m_strActiveCallRefId = p_strActiveCallRefId;
			}
		}
	}
	else
	{
		if (m_bFsAesMode)
		{
			SAFE_LOCK(m_transferCallMutex);
			auto it = m_mTransferCallList.find(p_strHeldCallRefId);
			if (it != m_mTransferCallList.end())
			{
				// 咨询返回失败，发取回
				if (it->second && it->second->m_lConsultationCallTaskId == p_lTaskId)
				{
					ReconnectCall(p_strHeldCallRefId, p_strHeldCallRefId, it->second->m_strSponsor, it->second->m_strTarget);
				}
			}
		}
		//SendTransferCallResult(p_strHeldCallRefId, p_nErrorCode);
		SendTransferCallResult(p_strHeldCallRefId, ERROR_DEST_NOT_CONNECT);
	}
}
void CTransferCallManager::ProcessRingState(const std::string& p_strDeviceNum, const std::string& p_strActiveCallRefId, const std::string& p_strStateTime)
{
	std::string l_strHeldCallRefId = "";
	std::string l_strActiveCallRefId = "";
	std::string l_strSponsor = "";
	std::string l_strTarget = "";
	std::string l_strRelatedId = "";

	{
		SAFE_LOCK(m_transferCallMutex);
		for (auto l_callObj : m_mTransferCallList)
		{
			if (l_callObj.second->m_strTarget.compare(p_strDeviceNum) == 0 &&
				l_callObj.second->m_strActiveCallRefId.compare(p_strActiveCallRefId) == 0)
			{
				l_strHeldCallRefId = l_callObj.second->m_strHeldCallRefId;
				l_strActiveCallRefId = l_callObj.second->m_strActiveCallRefId;
				l_strSponsor = l_callObj.second->m_strSponsor;
				l_strTarget = l_callObj.second->m_strTarget;
				l_strRelatedId = l_callObj.second->m_strRelatedId;

				l_callObj.second->m_strTargetState = CallStateString[STATE_RING];
				l_callObj.second->m_strRingTime = p_strStateTime;

				break;
			}
		}
	}

	//	转移目标方振铃，通知警情服务
	SendTransferCallResult(l_strHeldCallRefId, Result_GetReadyAgent_Success);
}

void CTransferCallManager::ProcessTalkState(const std::string& p_strDeviceNum, const std::string& p_strActiveCallRefId)
{
	SAFE_LOCK(m_transferCallMutex);
	for (auto l_callObj : m_mTransferCallList)
	{
		std::string l_strTarget = l_callObj.second->m_strTarget;
		std::string l_strActiveCallRefId = l_callObj.second->m_strActiveCallRefId;

		if (l_strTarget.compare(p_strDeviceNum) == 0 &&
			l_strActiveCallRefId.compare(p_strActiveCallRefId) == 0)
		{
			//	转移目标方接通，转移话务
			std::string l_strHeldCallRefId = l_callObj.first;
			std::string l_strSponsor = l_callObj.second->m_strSponsor;
			long l_lTaskId = TransferCall(l_strActiveCallRefId, l_strHeldCallRefId, l_strSponsor, l_strTarget);

			l_callObj.second->m_lTransferCallTaskId = l_lTaskId;
			l_callObj.second->m_strTargetState = CallStateString[STATE_TALK];

			break;
		}
	}
}
void CTransferCallManager::ProcessHangupState(const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId)
{
	bool b_lFindTransferCall = false;
	std::string l_strHeldCallRefId = "";
	std::string l_strTarget;
	int l_nErrorCode = ERROR_UNKNOWN;

	{
		SAFE_LOCK(m_transferCallMutex);
		for (auto l_callObj : m_mTransferCallList)
		{
			std::string l_strSponsor = l_callObj.second->m_strSponsor;
			std::string l_strActiveCallRefId = l_callObj.second->m_strActiveCallRefId;

			l_strTarget = l_callObj.second->m_strTarget;
			l_strHeldCallRefId = l_callObj.second->m_strHeldCallRefId;

			if (l_strSponsor.compare(p_strDeviceNum) == 0 && l_strActiveCallRefId.compare(p_strCTICallRefId) == 0)
			{
				// 转移发起方或目标方挂机 
				b_lFindTransferCall = true;
				l_nErrorCode = ERROR_CALL_HANGUP;
				std::string l_strTarget = l_callObj.second->m_strTarget;
				long l_lRequestId = l_callObj.second->m_lConsultationCallTaskId;
				if (!m_bFsAesMode)
				{
					ReconnectCall(l_strActiveCallRefId, l_strHeldCallRefId, l_strSponsor, l_strSponsor);
				}
				break;
			}
			else if (l_strSponsor.compare(p_strDeviceNum) == 0 && l_strHeldCallRefId.compare(p_strCTICallRefId) == 0)
			{
				
				// 原主叫挂机
				b_lFindTransferCall = true;
				l_nErrorCode = ERROR_CALLER_HANGUP;
				if (!m_bFsAesMode)
				{
					ForcePopCall(l_strActiveCallRefId, l_strSponsor);;
				}
				break;
			}
			else
			{
				if (l_strTarget == p_strDeviceNum && m_bFsAesMode && l_callObj.second->m_strTargetState != CallStateString[STATE_TALK])
				{
					b_lFindTransferCall = true;
					l_nErrorCode = ERROR_CALLER_HANGUP;
					ReconnectCall(l_strActiveCallRefId, l_strHeldCallRefId, l_strSponsor, l_strTarget);
				}
			}
		}
	}

	

	if (b_lFindTransferCall)
	{
		ICC_LOG_DEBUG(m_LogPtr, "TransferCall ProcessHangupState,DeviceNum:%s,CallRefId:%s,HeldCallRefId:%s,Target:%s",
			p_strDeviceNum.c_str(),
			p_strCTICallRefId.c_str(),
			l_strHeldCallRefId.c_str(),
			l_strTarget.c_str());

		SendTransferCallResult(l_strHeldCallRefId, l_nErrorCode);
	}
}
void CTransferCallManager::DeviceStateNotif(const std::string& p_strDeviceNum, const std::string& p_strActiveCallRefId,
	const std::string& p_strDeviceState, const std::string& p_strStateTime)
{

	ICC_LOG_DEBUG(m_LogPtr, "DeviceStateNotif,DeviceNum:%s,ActiveCallRefId:%s,DeviceState:%s",p_strDeviceNum.c_str(), p_strActiveCallRefId.c_str(), p_strDeviceState.c_str());

	if (p_strDeviceState.compare(CallStateString[STATE_RING]) == 0)
	{
		// 转移目标方振铃，启动振铃超时计算
		ProcessRingState(p_strDeviceNum, p_strActiveCallRefId, p_strStateTime);
	}
	else if (p_strDeviceState.compare(CallStateString[STATE_TALK]) == 0)
	{
		// 转移目标方应答，发起转移
		ProcessTalkState(p_strDeviceNum, p_strActiveCallRefId);
	}
	else if (p_strDeviceState.compare(CallStateString[STATE_HANGUP]) == 0)
	{
		ProcessHangupState(p_strDeviceNum, p_strActiveCallRefId);
	}
	else
	{
		//
	}	 
}

long CTransferCallManager::TransferCall(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId,
	const std::string& p_strSponsor, const std::string& p_strTarget)
{
	long l_lTaskId = DEFAULT_TASKID;

	if (m_SwitchClientPtr)
	{
		std::string l_strHeldCallRefId = p_strHeldCallRefId;
		if (p_strHeldCallRefId.empty())
		{
			l_strHeldCallRefId = p_strActiveCallRefId;
		}
		l_lTaskId = m_SwitchClientPtr->TransferCall(p_strActiveCallRefId, l_strHeldCallRefId, p_strSponsor, p_strTarget);

		ICC_LOG_DEBUG(m_LogPtr, "ExecuteTransferCall, TaskId: %u, ActiveCallRefId: %s, HeldCallRefId: %s, Sponsor: %s Target: %s",
			l_lTaskId, p_strActiveCallRefId.c_str(), l_strHeldCallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());
	}

	return l_lTaskId;
}
void CTransferCallManager::ProcessTransferCallResult(long p_lTaskId, const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId, bool p_bResult)
{
	if (p_bResult)
	{
		{
			SAFE_LOCK(m_transferCallMutex);
			auto it = m_mTransferCallList.find(p_strHeldCallRefId);
			if (it != m_mTransferCallList.end())
			{
				// 转移成功
				if (it->second && it->second->m_lTransferCallTaskId == p_lTaskId)
				{
					it->second->m_strActiveCallRefId = p_strActiveCallRefId;
				}
			}
		}

		SendTransferCallResult(p_strHeldCallRefId, ERROR_CMD_SUCCESS);
	}
	else
	{
		{
			SAFE_LOCK(m_transferCallMutex);
			auto it = m_mTransferCallList.find(p_strHeldCallRefId);
			if (it != m_mTransferCallList.end())
			{
				// 转移失败，取回原话务
				if (it->second && it->second->m_lTransferCallTaskId == p_lTaskId)
				{
					std::string l_strSponsor = it->second->m_strSponsor;
					std::string l_strTarget = it->second->m_strTarget;

					if (m_bFsAesMode)
					{
						ReconnectCall(p_strActiveCallRefId, p_strHeldCallRefId, l_strSponsor, l_strTarget);
					}
					else
					{
						ReconnectCall(p_strActiveCallRefId, p_strHeldCallRefId, l_strSponsor, l_strSponsor);
					}
					
				}
			}
		}

		SendTransferCallResult(p_strHeldCallRefId, ERROR_DEST_NOT_CONNECT);
	}
}

void CTransferCallManager::ReconnectCall(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId,
	const std::string& p_strSponsor, const std::string& p_strTarget)
{
	if (m_SwitchClientPtr)
	{
		long l_lTaskId = 0;
		if (m_bFsAesMode)
		{
			l_lTaskId = m_SwitchClientPtr->ReconnectCall(p_strActiveCallRefId, p_strHeldCallRefId, p_strSponsor, p_strTarget);
		}
		else
		{
			l_lTaskId = m_SwitchClientPtr->ReconnectCall(p_strActiveCallRefId, p_strHeldCallRefId, p_strSponsor, p_strSponsor);
		}
		

		ICC_LOG_DEBUG(m_LogPtr, "Execute ReconnectCall,FsAesMode:%d, TaskId: %u, ActiveCallRefId: %s, HeldCallRefId: %s, Sponsor: %s Target: %s",
			m_bFsAesMode,l_lTaskId, p_strActiveCallRefId.c_str(), p_strHeldCallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());
	}
}

void CTransferCallManager::ForcePopCall(const std::string& p_strActiveCallRefId, const std::string& p_strTarget)
{
	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->ForcePopCall(p_strActiveCallRefId, p_strTarget, p_strTarget);

		ICC_LOG_DEBUG(m_LogPtr, "Execute ForcePopCall, TaskId: %u,ActiveCallRefId: %s, Sponsor: %s Target: %s",
			l_lTaskId, p_strActiveCallRefId.c_str(), p_strTarget.c_str(), p_strTarget.c_str());
	}
}

void CTransferCallManager::ProcessFailedEvent(const std::string& p_strDeviceNum, const std::string& p_strCalledId, const std::string& p_strCTICallRefId)
{
	std::string l_strHeldCallRefId = "";
	int l_nErrorCode = ERROR_UNKNOWN;

	{
		SAFE_LOCK(m_transferCallMutex);
		for (auto l_callObj : m_mTransferCallList)
		{
			std::string l_strSponsor = l_callObj.second->m_strSponsor;
			std::string l_strTarget = l_callObj.second->m_strTarget;
			std::string l_strActiveCallRefId = l_callObj.second->m_strActiveCallRefId;
			l_strHeldCallRefId = l_callObj.second->m_strHeldCallRefId;

			if (l_strSponsor.compare(p_strDeviceNum) == 0 && l_strTarget.find(p_strCalledId) != std::string::npos/*&& l_strActiveCallRefId.compare(p_strCTICallRefId) == 0*/)
			{
				// 呼叫失败
				l_nErrorCode = ERROR_DEST_NOT_CONNECT;
				std::string l_strTarget = l_callObj.second->m_strTarget;
				l_strActiveCallRefId = p_strCTICallRefId;
				if (m_bFsAesMode)
				{
					ReconnectCall(l_strActiveCallRefId, l_strHeldCallRefId, l_strSponsor, l_strTarget);
				}
				else
				{
					ReconnectCall(l_strActiveCallRefId, l_strHeldCallRefId, l_strSponsor, l_strSponsor);
				}
				

				break;
			}
		}
	}

	if (!l_strHeldCallRefId.empty())
	{
		SendTransferCallResult(l_strHeldCallRefId, l_nErrorCode);
	}
}
//////////////////////////////////////////////////////////////////////////
void CTransferCallManager::ResetCheckTime()
{
	m_oLastCheckTime = m_DateTimePtr->CurrentDateTime();
}
bool CTransferCallManager::CheckIsTimeout()
{
	DateTime::CDateTime l_oCurrentTime = m_DateTimePtr->CurrentDateTime();
	DateTime::CDateTime l_oEndTime = m_DateTimePtr->AddSeconds(m_oLastCheckTime, CHECK_TIMESPAN);

	if (l_oCurrentTime > l_oEndTime)
	{
		return true;
	}

	return false;
}
void CTransferCallManager::ProcessRingTimeOut(const std::string& p_strHeldCallRefId)
{
	{
		SAFE_LOCK(m_transferCallMutex);
		auto it = m_mTransferCallList.find(p_strHeldCallRefId);
		if (it != m_mTransferCallList.end())
		{
			if (it->second)
			{
				it->second->m_strTargetState = "";

				std::string l_strSponsor = it->second->m_strSponsor;
				std::string l_strTarget = it->second->m_strTarget;
				std::string l_strActiveCallRefId = it->second->m_strActiveCallRefId;

				ICC_LOG_DEBUG(m_LogPtr, "TranserCall RingTimeOut, Sponsor: %s, Target: %s, HeldCallRefId: %s, ActiveCallRefId: %s",
					l_strSponsor.c_str(), l_strTarget.c_str(), p_strHeldCallRefId.c_str(), l_strActiveCallRefId.c_str());

				if (m_bFsAesMode)
				{
					// 振铃超时，取回原话务
					ReconnectCall(l_strActiveCallRefId, p_strHeldCallRefId, l_strSponsor, l_strTarget);
				}
				else
				{
					// 振铃超时，取回原话务
					ReconnectCall(l_strActiveCallRefId, p_strHeldCallRefId, l_strSponsor, l_strSponsor);
				}
				
			}
		}
	}

	if (!p_strHeldCallRefId.empty())
	{
		// 通知客户端振铃超时，转移失败
		SendTransferCallResult(p_strHeldCallRefId, ERROR_RING_TIMEOUT);
	}
}
bool CTransferCallManager::FindRingTimeOut(std::string& p_strHeldCallRefId)
{
	bool b_lRingTimeOut = false;

	SAFE_LOCK(m_transferCallMutex);
	for (auto l_callObj : m_mTransferCallList)
	{
		if (l_callObj.second->m_strTargetState.compare(CallStateString[STATE_RING]) == 0)
		{
			DateTime::CDateTime l_CurrentTime = m_DateTimePtr->CurrentDateTime();
			DateTime::CDateTime l_RingTime = m_DateTimePtr->FromString(l_callObj.second->m_strRingTime);
			DateTime::CDateTime l_EndTime = m_DateTimePtr->AddSeconds(l_RingTime, m_nRingTimeout);

			if (l_CurrentTime > l_EndTime)
			{
				b_lRingTimeOut = true;
				p_strHeldCallRefId = l_callObj.second->m_strHeldCallRefId;
				//	重置振铃时间，避免重复检测
				l_callObj.second->m_strRingTime = m_DateTimePtr->CurrentDateTimeStr();

				std::string l_strTarget = l_callObj.second->m_strTarget;
				ICC_LOG_DEBUG(m_LogPtr, "Transfer Target: [%s] Ring Timeout !", l_strTarget.c_str());

				break;
			}
		}
	}

	return b_lRingTimeOut;
}
void CTransferCallManager::DoCheckRingTime()
{
	ICC_LOG_DEBUG(m_LogPtr, "================== Begin Transfer CheckRingTime Thread ==================");
	while (CTransferCallManager::Instance()->m_bThreadAlive)
	{
		if (CTransferCallManager::Instance()->CheckIsTimeout())
		{
			std::string l_strHeldCallRefId = "";

			while (CTransferCallManager::Instance()->FindRingTimeOut(l_strHeldCallRefId))
			{
				CTransferCallManager::Instance()->ProcessRingTimeOut(l_strHeldCallRefId);
			}

			CTransferCallManager::Instance()->ResetCheckTime();
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
		}
	}
	ICC_LOG_DEBUG(m_LogPtr, "--------------------- End Transfer CheckRingTime Thread ---------------------");
}

void CTransferCallManager::StartThread()
{
	if (!m_bThreadAlive)
	{
		m_bThreadAlive = true;
		m_oLastCheckTime = m_DateTimePtr->CurrentDateTime();

		m_pCheckRingTimeoutThread = boost::make_shared<boost::thread>(boost::bind(&CTransferCallManager::DoCheckRingTime, this));
	}
}
void CTransferCallManager::StopThread()
{
	if (m_bThreadAlive)
	{
		m_bThreadAlive = false;

		if (m_pCheckRingTimeoutThread)
		{
			m_pCheckRingTimeoutThread->join();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
