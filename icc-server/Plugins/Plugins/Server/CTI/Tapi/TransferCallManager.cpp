#include "Boost.h"

#include "TransferCallManager.h"
#include "HytProtocol.h"
#include "TaskManager.h"
#include "SysConfig.h"

#define CHECK_TIMESPAN			1	//��λ����
#define SLEEP_TIME				100  //��λ������
#define TRANSFER_DIAL_TIMEOUT	5   //��λ����

CTransferCall::CTransferCall()
{
	m_bTargetIsACD = false;

	m_lGetReadyAgentTaskId = DEFAULT_TASKID;
	m_lConsultationCallTaskId = DEFAULT_TASKID;
	m_lTransferCallTaskId = DEFAULT_TASKID;

	m_strRelatedId = "";
	m_strHeldCallRefId = "";		//	�����Ļ��� ID
	m_strActiveCallRefId = "";		//	��ѯ�����Ļ��� ID
	m_strDialTime = "";
	m_strRingTime = "";				//	����ѯ�����忪ʼʱ��
	m_strTargetState = "";			//	����ѯ����״̬�����塢ͨ�����һ�
	m_strSponsor = "";				//	������ѯ����ϯ��
	m_strTarget = "";				//	��ѯĿ����ϯ��
	m_strTargetDeviceType = "";
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
	//
}

CTransferCallManager::CTransferCallManager(void)
{
	m_nRingTimeout = DEFAULT_RING_TIMEOUT;
	m_pLog = nullptr;
	m_pDateTime = nullptr;
}

CTransferCallManager::~CTransferCallManager(void)
{
	m_mTransferCallList.clear();
}


void CTransferCallManager::AddTransferCall(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId)
{
	std::lock_guard<std::mutex> guard(m_transferCallMutex);

	m_mTransferCallMap[p_lCSTACallRefId] = p_lRelatedCSTACallRefId;

	ICC_LOG_DEBUG(m_pLog, "AddTransferCall, CSTACallRefId: %u, RelatedCSTACallRefId: %u",
		p_lCSTACallRefId, p_lRelatedCSTACallRefId);
}

void CTransferCallManager::DeleteTransferCall(long p_lCSTACallRefId)
{
	std::lock_guard<std::mutex> guard(m_transferCallMutex);
	auto it = m_mTransferCallMap.find(p_lCSTACallRefId);
	if (it != m_mTransferCallMap.end())
	{
		ICC_LOG_DEBUG(m_pLog, "DeleteTransferCall, lCSTACallRefId: %u ",
			p_lCSTACallRefId);

		m_mTransferCallMap.erase(it);
	}
}

long CTransferCallManager::GetRelatedCSTACallRefId(long p_lCSTACallRefId)
{
	std::lock_guard<std::mutex> guard(m_transferCallMutex);
	auto it = m_mTransferCallMap.find(p_lCSTACallRefId);
	if (it != m_mTransferCallMap.end())
	{
		ICC_LOG_DEBUG(m_pLog, "GetTransferCall, lCSTACallRefId: %u ",
			p_lCSTACallRefId);

		return it->second;
	}

	return 0;
}

long CTransferCallManager::GetCSTACallRefId(long p_lRelatedCSTACallRefId)
{
	std::lock_guard<std::mutex> guard(m_transferCallMutex);
	std::map<long, long>::iterator it = m_mTransferCallMap.begin();
	for (; it!= m_mTransferCallMap.end(); ++it)
	{
		if (it->second == p_lRelatedCSTACallRefId)
		{
			return it->first;
		}
	}

	return 0;
}

void CTransferCallManager::ClearTransferCallList()
{
	std::lock_guard<std::mutex> guard(m_transferCallMutex);
	m_mTransferCallMap.clear();
	m_mTransferCallList.clear();
}
//////////////////////////////////////////////////////////////////////////

void CTransferCallManager::AddNewTransferCall(const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget,
	const std::string& p_strTargetDeviceType, const std::string& p_strMsgId, bool p_bIsACD)
{
	boost::shared_ptr<CTransferCall> l_pTransferCall = boost::make_shared<CTransferCall>();
	l_pTransferCall->m_strHeldCallRefId = p_strHeldCallRefId;
	l_pTransferCall->m_strSponsor = p_strSponsor;
	l_pTransferCall->m_strTarget = p_strTarget;
	l_pTransferCall->m_strTargetDeviceType = p_strTargetDeviceType;
	l_pTransferCall->m_bTargetIsACD = p_bIsACD;
	l_pTransferCall->m_strRelatedId = p_strMsgId;

	if (p_bIsACD)
	{
		//	ת�������� ACD���ҿ�����ϯ
		l_pTransferCall->m_lGetReadyAgentTaskId = GetReadyAgent(p_strTarget);
	} 
	else
	{
		//	ֱ��ת��ϯ��������ѯ
		l_pTransferCall->m_strDialTime = m_pDateTime->CurrentDateTimeStr();
		l_pTransferCall->m_strTargetState = CallStateString[STATE_DIAL];
		l_pTransferCall->m_lConsultationCallTaskId = ConsultationCall(p_strHeldCallRefId, p_strSponsor, p_strTarget, p_strTargetDeviceType);
	}

	std::lock_guard<std::mutex> guard(m_transferCallMutex);
	m_mTransferCallList[p_strHeldCallRefId] = l_pTransferCall;

	ICC_LOG_DEBUG(m_pLog, "AddNewTransferCall, HeldCallRefId: %s, Sponsor: %s, Target: %s, TargetDeviceType: %s",
		p_strHeldCallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str(), p_strTargetDeviceType.c_str());
}
void CTransferCallManager::CancelTransferCall(const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	{
		std::lock_guard<std::mutex> guard(m_transferCallMutex);
		auto it = m_mTransferCallList.find(p_strHeldCallRefId);
		if (it != m_mTransferCallList.end())
		{
			if (it->second)
			{
			//	std::string l_strActiveCallRefId = it->second->m_strActiveCallRefId;

				this->RetrieveCall(p_strHeldCallRefId, p_strSponsor, p_strSponsor);
			}
		}
	}

	SendTransferCallResult(p_strHeldCallRefId, ERROR_CANCEL_TRANSFER_CALL);
}
void CTransferCallManager::DeleteTransferCall(const std::string& p_strHeldCallRefId)
{
	std::lock_guard<std::mutex> guard(m_transferCallMutex);
	auto it = m_mTransferCallList.find(p_strHeldCallRefId);
	if (it != m_mTransferCallList.end())
	{
		ICC_LOG_DEBUG(m_pLog, "DeleteTransferCall, HeldCallRefId: %s ",
			p_strHeldCallRefId.c_str());

		m_mTransferCallList.erase(it);
	}
}

bool CTransferCallManager::FindTransferCallByHeldCallRefId(const std::string& p_strHeldCallRefId)
{
	bool l_bFind = false;

	std::lock_guard<std::mutex> guard(m_transferCallMutex);
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

	std::lock_guard<std::mutex> guard(m_transferCallMutex);
	for each (auto l_pTransferObj in m_mTransferCallList)
	{
		if (l_pTransferObj.second->m_strActiveCallRefId.compare(p_strActiveCallRefId) == 0)
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

	std::lock_guard<std::mutex> guard(m_transferCallMutex);
	for each (auto l_pTransferObj in m_mTransferCallList)
	{
		if (l_pTransferObj.second->m_lGetReadyAgentTaskId == p_lTaskId)
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

	std::lock_guard<std::mutex> guard(m_transferCallMutex);
	for each (auto l_pTransferObj in m_mTransferCallList)
	{
		if (l_pTransferObj.second->m_lConsultationCallTaskId == p_lTaskId)
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

	std::lock_guard<std::mutex> guard(m_transferCallMutex);
	for each (auto l_pTransferObj in m_mTransferCallList)
	{
		if (l_pTransferObj.second->m_lTransferCallTaskId == p_lTaskId)
		{
			l_bFind = true;

			break;
		}
	}

	return l_bFind;
}
void CTransferCallManager::SetActiveCallRefId(const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId)
{
	std::lock_guard<std::mutex> guard(m_transferCallMutex);
	auto it = m_mTransferCallList.find(p_strHeldCallRefId);
	if (it != m_mTransferCallList.end())
	{
		if (it->second)
		{
			it->second->m_strActiveCallRefId = p_strActiveCallRefId;
		}
	}
}
bool CTransferCallManager::GetCTICallRefIdByActiveCallRefId(const std::string& p_strActiveCTICallRefId, std::string& p_strCTICallRefId)
{
	bool l_bFind = false;

	std::lock_guard<std::mutex> guard(m_transferCallMutex);
	for each (auto l_pTransferObj in m_mTransferCallList)
	{
		if (l_pTransferObj.second->m_strActiveCallRefId.compare(p_strActiveCTICallRefId) == 0)
		{
			l_bFind = true;
			p_strCTICallRefId = l_pTransferObj.first;

			break;
		}
	}

	return l_bFind;
}

//////////////////////////////////////////////////////////////////////////

void CTransferCallManager::SendTransferCallResult(const std::string& p_strHeldCallRefId, int p_nResult)
{
	std::string l_strHeldCallRefId = "";
	std::string l_strActiveCallRefId = "";
	std::string l_strSponsor = "";
	std::string l_strTarget = "";
	std::string l_strRelatedId = "";

	{
		std::lock_guard<std::mutex> guard(m_transferCallMutex);
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
			}
		}
	}

	if (!l_strHeldCallRefId.empty())
	{
		CHytProtocol::Instance()->Sync_TransferCall(l_strHeldCallRefId, l_strActiveCallRefId, l_strSponsor, l_strTarget, l_strRelatedId, p_nResult);

		if (p_nResult != Result_GetReadyAgent_Success)
		{
			// ��ѯ������ϯ�ɹ�������ת������
			// ת�Ƴɹ� �� ʧ�ܣ�ɾ��ת�Ƽ�¼
			DeleteTransferCall(p_strHeldCallRefId);
		}
	}
}

//	��ѯ������ϯ
long CTransferCallManager::GetReadyAgent(const std::string& p_strACDGrp)
{
	ICC_LOG_DEBUG(m_pLog, "TransferCall Exe GetReadyAgent, Target ACDGrp: %s",
		p_strACDGrp.c_str());

	return CTaskManager::Instance()->GetReadyAgent(p_strACDGrp);
}
//	��ѯ������ϯ���
void CTransferCallManager::ProcessGetReadyAgentResult(long p_lTaskId, const std::string& p_strTarget, bool p_bResult)
{
	ICC_LOG_DEBUG(m_pLog, "GetReadyAgent Result, RequestId: %u Target: %s, Result: %s",
		p_lTaskId, p_strTarget.c_str(), p_bResult ? "Success" : "Failed");

	if (p_bResult)
	{
		std::lock_guard<std::mutex> guard(m_transferCallMutex);
		for each (auto l_pTransferObj in m_mTransferCallList)
		{
			// ��ѯ������ϯ�ɹ�
			if (l_pTransferObj.second->m_lGetReadyAgentTaskId == p_lTaskId)
			{
				std::string l_strHeldCallRefId = l_pTransferObj.second->m_strHeldCallRefId;
				std::string l_strSponsor = l_pTransferObj.second->m_strSponsor;
				std::string l_strTargetDeviceType = l_pTransferObj.second->m_strTargetDeviceType;

				l_pTransferObj.second->m_strTarget = p_strTarget;
				l_pTransferObj.second->m_strDialTime = m_pDateTime->CurrentDateTimeStr();
				l_pTransferObj.second->m_strTargetState = CallStateString[STATE_DIAL];
				l_pTransferObj.second->m_lConsultationCallTaskId = ConsultationCall(l_strHeldCallRefId, l_strSponsor, p_strTarget, l_strTargetDeviceType);

				break;
			}
		}
	}
	else
	{
		bool l_bFindTaskId = false;
		std::string l_strHeldCallRefId = "";

		{
			std::lock_guard<std::mutex> guard(m_transferCallMutex);
			for each (auto l_pTransferObj in m_mTransferCallList)
			{
				// �޿�����ϯ��ת��ʧ��
				if (l_pTransferObj.second->m_lGetReadyAgentTaskId == p_lTaskId)
				{
					l_bFindTaskId = true;
					l_strHeldCallRefId = l_pTransferObj.second->m_strHeldCallRefId;

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

//	��ѯת��Ŀ��
long CTransferCallManager::ConsultationCall(const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType)
{
	ICC_LOG_DEBUG(m_pLog, "TransferCall ExecuteTransferCall, HeldCallRefId: %s, Sponsor: %s, Target: %s, TargetDeviceType: %s",
		p_strHeldCallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str(), p_strTargetDeviceType.c_str());

	return CTaskManager::Instance()->ConsultationCall(p_strHeldCallRefId, p_strSponsor, p_strTarget, p_strTargetDeviceType, true);
}
//��ѯ�������
void CTransferCallManager::ProcessConsultationCallResult(long p_lTaskId, bool p_bResult, int p_nErrorCode)
{
	ICC_LOG_DEBUG(m_pLog, "ConsultationCall Result, RequestId: %u, Result: %s",
		p_lTaskId, p_bResult ? "Success" : "Failed");

	bool l_bFind = false;
	std::string l_strHeldCallRefId = "";
	std::string l_strActiveCallRefId = "";
	std::string l_strSponsor = "";
	std::string l_strTarget = "";

	{
		std::lock_guard<std::mutex> guard(m_transferCallMutex);
		for each (auto l_pTransferObj in m_mTransferCallList)
		{
			// ��ѯ�ɹ�
			if (l_pTransferObj.second->m_lConsultationCallTaskId == p_lTaskId)
			{
				l_bFind = true;
				l_strSponsor = l_pTransferObj.second->m_strSponsor;
				l_strTarget = l_pTransferObj.second->m_strTarget;
				l_strHeldCallRefId = l_pTransferObj.second->m_strHeldCallRefId;

				break;
			}
		}
	}

	if (l_bFind)
	{
		if (!p_bResult)
		{
			SendTransferCallResult(l_strHeldCallRefId, p_nErrorCode);
		}
	}
}
void CTransferCallManager::ProcessConsultationCallResult(long p_lTaskId, const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId, bool p_bResult, int p_nErrorCode)
{
	ICC_LOG_DEBUG(m_pLog, "ConsultationCall Result, RequestId: %u, HeldCallRefId: %s, ActiveCallRefId: %s, Result: %s, ErrorCode: %d",
		p_lTaskId, p_strHeldCallRefId.c_str(), p_strActiveCallRefId.c_str(), p_bResult ? "Success" : "Failed", p_nErrorCode);

	if (p_bResult)
	{
		std::lock_guard<std::mutex> guard(m_transferCallMutex);
		auto it = m_mTransferCallList.find(p_strHeldCallRefId);
		if (it != m_mTransferCallList.end())
		{
			// ��ѯ�ɹ���������ѯ�����Ļ��� ID,�ȴ��Է���ͨ����ת��
			if (it->second && it->second->m_lConsultationCallTaskId == p_lTaskId)
			{
				it->second->m_strActiveCallRefId = p_strActiveCallRefId;
			}
		}
	}
	else
	{
		SendTransferCallResult(p_strHeldCallRefId, p_nErrorCode);
	}
}
void CTransferCallManager::ProcessRingState(const std::string& p_strDeviceNum, const std::string& p_strActiveCallRefId)
{
	std::string l_strHeldCallRefId = "";
	std::string l_strActiveCallRefId = "";
	std::string l_strSponsor = "";
	std::string l_strTarget = "";
	std::string l_strRelatedId = "";

	{
		std::lock_guard<std::mutex> guard(m_transferCallMutex);
		for each (auto l_pTransferObj in m_mTransferCallList)
		{
			if (l_pTransferObj.second->m_strTarget.compare(p_strDeviceNum) == 0 &&
				l_pTransferObj.second->m_strActiveCallRefId.compare(p_strActiveCallRefId) == 0)
			{
				l_strHeldCallRefId = l_pTransferObj.second->m_strHeldCallRefId;
				l_strActiveCallRefId = l_pTransferObj.second->m_strActiveCallRefId;
				l_strSponsor = l_pTransferObj.second->m_strSponsor;
				l_strTarget = l_pTransferObj.second->m_strTarget;
				l_strRelatedId = l_pTransferObj.second->m_strRelatedId;

				l_pTransferObj.second->m_strTargetState = CallStateString[STATE_RING];
				l_pTransferObj.second->m_strRingTime = m_pDateTime->CurrentDateTimeStr();

				break;
			}
		}
	}

	//	ת��Ŀ�귽���壬֪ͨ�������
	SendTransferCallResult(l_strHeldCallRefId, Result_GetReadyAgent_Success);
}
void CTransferCallManager::ProcessTalkState(const std::string& p_strDeviceNum, const std::string& p_strActiveCallRefId)
{
	std::lock_guard<std::mutex> guard(m_transferCallMutex);
	for each (auto l_pTransferObj in m_mTransferCallList)
	{
		std::string l_strTarget = l_pTransferObj.second->m_strTarget;
		std::string l_strActiveCallRefId = l_pTransferObj.second->m_strActiveCallRefId;

		if (l_strTarget.compare(p_strDeviceNum) == 0 && l_strActiveCallRefId.compare(p_strActiveCallRefId) == 0)
		{
			//	ת��Ŀ�귽��ͨ��ת�ƻ���
			std::string l_strHeldCallRefId = l_pTransferObj.first;
			std::string l_strSponsor = l_pTransferObj.second->m_strSponsor;
			long l_lTaskId = TransferCall(l_strActiveCallRefId, l_strHeldCallRefId, l_strSponsor, l_strTarget);

			l_pTransferObj.second->m_lTransferCallTaskId = l_lTaskId;
			l_pTransferObj.second->m_strTargetState = CallStateString[STATE_TALK];

			break;
		}
	}
}
void CTransferCallManager::ProcessHangupState(const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId)
{
	bool b_lFindTransferCall = false;
	std::string l_strHeldCallRefId = "";
	int l_nErrorCode = ERROR_UNKNOWN;

	{
		std::lock_guard<std::mutex> guard(m_transferCallMutex);
		for each (auto l_pTransferObj in m_mTransferCallList)
		{
			std::string l_strSponsor = l_pTransferObj.second->m_strSponsor;
			std::string l_strTarget = l_pTransferObj.second->m_strTarget;
			std::string l_strState = l_pTransferObj.second->m_strTargetState;
			std::string l_strActiveCallRefId = l_pTransferObj.second->m_strActiveCallRefId;
			l_strHeldCallRefId = l_pTransferObj.second->m_strHeldCallRefId;

			if (l_strSponsor.compare(p_strDeviceNum) == 0 && l_strActiveCallRefId.compare(p_strCTICallRefId) == 0 && l_strState.compare(CallStateString[STATE_RING]) == 0)
			{
				// ת�Ʒ��𷽻�Ŀ�귽�һ� 
				b_lFindTransferCall = true;
				l_nErrorCode = ERROR_CALL_HANGUP;
				/*std::string l_strTarget = l_pTransferObj.second->m_strTarget;
				long l_lRequestId = l_pTransferObj.second->m_lConsultationCallTaskId;*/

				this->RetrieveCall(l_strHeldCallRefId, l_strSponsor, l_strSponsor);

				break;
			}
			else if (l_strSponsor.compare(p_strDeviceNum) == 0 && l_strHeldCallRefId.compare(p_strCTICallRefId) == 0 && l_strState.compare(CallStateString[STATE_RING]) == 0)
			{
				// ԭ���йһ�
				b_lFindTransferCall = true;
				l_nErrorCode = ERROR_CALLER_HANGUP;
				/*this->ForcePopCall(l_strActiveCallRefId, l_strTarget);*/

				break;
			}
			else
			{
				//
			}
		}
	}

	if (b_lFindTransferCall)
	{
		SendTransferCallResult(l_strHeldCallRefId, l_nErrorCode);
	}
}
void CTransferCallManager::DeviceStateNotif(const std::string& p_strDeviceNum, const std::string& p_strActiveCallRefId,
	const std::string& p_strDeviceState)
{
	if (p_strDeviceState.compare(CallStateString[STATE_RING]) == 0)
	{
		// ת��Ŀ�귽���壬�������峬ʱ����
		ProcessRingState(p_strDeviceNum, p_strActiveCallRefId);
	}
	else if (p_strDeviceState.compare(CallStateString[STATE_TALK]) == 0)
	{
		// ת��Ŀ�귽Ӧ�𣬷���ת��
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
	ICC_LOG_DEBUG(m_pLog, "ExecuteTransferCall, ActiveCallRefId: %s, HeldCallRefId: %s, Sponsor: %s Target: %s",
		p_strActiveCallRefId.c_str(), p_strHeldCallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

	return CTaskManager::Instance()->TransferCall(p_strActiveCallRefId, p_strHeldCallRefId, p_strSponsor, p_strTarget);
}
void CTransferCallManager::ProcessTransferCallResult(long p_lTaskId, bool p_bResult)
{
	bool l_bFind = false;
	std::string l_strHeldCallRefId = "";
	std::string l_strSponsor = "";
	std::string l_strTarget = "";

	{
		std::lock_guard<std::mutex> guard(m_transferCallMutex);
		for each (auto l_pTransferObj in m_mTransferCallList)
		{
			// ת�Ƴɹ�
			if (l_pTransferObj.second->m_lTransferCallTaskId == p_lTaskId)
			{
				l_bFind = true;
				l_strSponsor = l_pTransferObj.second->m_strSponsor;
				l_strTarget = l_pTransferObj.second->m_strTarget;
				l_strHeldCallRefId = l_pTransferObj.second->m_strHeldCallRefId;

				break;
			}
		}
	}

	if (l_bFind)
	{
		if (p_bResult)
		{
			SendTransferCallResult(l_strHeldCallRefId, ERROR_CMD_SUCCESS);
		} 
		else
		{
		//	ReconnectCall(p_strActiveCallRefId, p_strHeldCallRefId, l_strSponsor, l_strSponsor);
			SendTransferCallResult(l_strHeldCallRefId, ERROR_DEST_NOT_CONNECT);
		}
	}
}
void CTransferCallManager::ProcessTransferCallResult(long p_lTaskId, const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId, bool p_bResult)
{
	if (p_bResult)
	{
		{
			std::lock_guard<std::mutex> guard(m_transferCallMutex);
			auto it = m_mTransferCallList.find(p_strHeldCallRefId);
			if (it != m_mTransferCallList.end())
			{
				// ת�Ƴɹ�
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
			std::lock_guard<std::mutex> guard(m_transferCallMutex);
			auto it = m_mTransferCallList.find(p_strHeldCallRefId);
			if (it != m_mTransferCallList.end())
			{
				// ת��ʧ�ܣ�ȡ��ԭ����
				if (it->second && it->second->m_lTransferCallTaskId == p_lTaskId)
				{
					std::string l_strSponsor = it->second->m_strSponsor;
					std::string l_strTarget = it->second->m_strTarget;

					ReconnectCall(p_strActiveCallRefId, p_strHeldCallRefId, l_strSponsor, l_strSponsor);
				}
			}
		}

		SendTransferCallResult(p_strHeldCallRefId, ERROR_DEST_NOT_CONNECT);
	}
}

void CTransferCallManager::HangupCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	ICC_LOG_DEBUG(m_pLog, "TransferCall HangupCall, CTICallRefId: %s  Sponsor: %s Target: %s",
		p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

	CTaskManager::Instance()->Hangup(p_strCTICallRefId, p_strSponsor, p_strTarget);
}
long CTransferCallManager::RetrieveCall(const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	ICC_LOG_DEBUG(m_pLog, "TransferCall Execute RetrieveCall, HeldCallRefId: %s, Sponsor: %s Target: %s",
		p_strHeldCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

	return CTaskManager::Instance()->RetrieveCall(p_strHeldCTICallRefId, p_strSponsor, p_strTarget);
}
void CTransferCallManager::ReconnectCall(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId,
	const std::string& p_strSponsor, const std::string& p_strTarget)
{
	/*if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->ReconnectCall(p_strActiveCallRefId, p_strHeldCallRefId, p_strSponsor, p_strSponsor);

		ICC_LOG_DEBUG(m_pLog, "Execute ReconnectCall, TaskId: %u, ActiveCallRefId: %s, HeldCallRefId: %s, Sponsor: %s Target: %s",
			l_lTaskId, p_strActiveCallRefId.c_str(), p_strHeldCallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());
	}*/
}

void CTransferCallManager::ForcePopCall(const std::string& p_strActiveCallRefId, const std::string& p_strTarget)
{
	long l_lTaskId = CTaskManager::Instance()->ForcePopCall(p_strActiveCallRefId, p_strTarget, p_strTarget);

	ICC_LOG_DEBUG(m_pLog, "Execute ForcePopCall, TaskId: %u,ActiveCallRefId: %s, Sponsor: %s Target: %s",
		l_lTaskId, p_strActiveCallRefId.c_str(), p_strTarget.c_str(), p_strTarget.c_str());
}

void CTransferCallManager::ProcessFailedEvent(const std::string& p_strDeviceNum, const std::string& p_strCalledId, const std::string& p_strCTICallRefId)
{
	std::string l_strHeldCallRefId = "";
	int l_nErrorCode = ERROR_UNKNOWN;

	{
		std::lock_guard<std::mutex> guard(m_transferCallMutex);
		for each (auto l_pTransferObj in m_mTransferCallList)
		{
			std::string l_strSponsor = l_pTransferObj.second->m_strSponsor;
			std::string l_strTarget = l_pTransferObj.second->m_strTarget;
			std::string l_strActiveCallRefId = l_pTransferObj.second->m_strActiveCallRefId;
			l_strHeldCallRefId = l_pTransferObj.second->m_strHeldCallRefId;

			if (l_strSponsor.compare(p_strDeviceNum) == 0 && l_strTarget.find(p_strCalledId) != std::string::npos/*&& l_strActiveCallRefId.compare(p_strCTICallRefId) == 0*/)
			{
				// ����ʧ��
				l_nErrorCode = ERROR_DEST_NOT_CONNECT;
				std::string l_strTarget = l_pTransferObj.second->m_strTarget;
				l_strActiveCallRefId = p_strCTICallRefId;

				ReconnectCall(l_strActiveCallRefId, l_strHeldCallRefId, l_strSponsor, l_strSponsor);

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
void CTransferCallManager::ProcessDialTimeout(const std::string& p_strHeldCallRefId)
{
	{
		std::lock_guard<std::mutex> guard(m_transferCallMutex);
		auto it = m_mTransferCallList.find(p_strHeldCallRefId);
		if (it != m_mTransferCallList.end())
		{
			if (it->second)
			{
				it->second->m_strTargetState = "";

				std::string l_strSponsor = it->second->m_strSponsor;
				std::string l_strTarget = it->second->m_strTarget;
				std::string l_strActiveCallRefId = it->second->m_strActiveCallRefId;

				ICC_LOG_DEBUG(m_pLog, "TranserCall DialTimeOut, Sponsor: %s, Target: %s, HeldCallRefId: %s, ActiveCallRefId: %s",
					l_strSponsor.c_str(), l_strTarget.c_str(), p_strHeldCallRefId.c_str(), l_strActiveCallRefId.c_str());

				// ���ų�ʱ��ȡ��ԭ����
				this->RetrieveCall(p_strHeldCallRefId, l_strSponsor, l_strSponsor);
			}
		}
	}

	if (!p_strHeldCallRefId.empty())
	{
		// ֪ͨ�ͻ���ת��Ŀ���޷���ͨ��ת��ʧ��
		SendTransferCallResult(p_strHeldCallRefId, ERROR_DEST_NOT_CONNECT);
	}
}
bool CTransferCallManager::FindDialTimeout(std::string& p_strHeldCallRefId)
{
	bool b_lDialTimeOut = false;

	std::lock_guard<std::mutex> guard(m_transferCallMutex);
	for each (auto l_pTransferObj in m_mTransferCallList)
	{
		if (l_pTransferObj.second->m_strTargetState.compare(CallStateString[STATE_DIAL]) == 0)
		{
			DateTime::CDateTime l_CurrentTime = m_pDateTime->CurrentDateTime();
			DateTime::CDateTime l_DialTime = m_pDateTime->FromString(l_pTransferObj.second->m_strDialTime);
			DateTime::CDateTime l_EndTime = m_pDateTime->AddSeconds(l_DialTime, TRANSFER_DIAL_TIMEOUT);

			if (l_CurrentTime > l_EndTime)
			{
				b_lDialTimeOut = true;
				p_strHeldCallRefId = l_pTransferObj.second->m_strHeldCallRefId;
				//	��������ʱ�䣬�����ظ����
				l_pTransferObj.second->m_strDialTime = m_pDateTime->CurrentDateTimeStr();

				std::string l_strTarget = l_pTransferObj.second->m_strTarget;
				ICC_LOG_DEBUG(m_pLog, "Transfer Target: [%s] Dial Timeout !", l_strTarget.c_str());

				break;
			}
		}
	}

	return b_lDialTimeOut;
}
void CTransferCallManager::ProcessRingTimeout(const std::string& p_strHeldCallRefId)
{
	{
		std::lock_guard<std::mutex> guard(m_transferCallMutex);
		auto it = m_mTransferCallList.find(p_strHeldCallRefId);
		if (it != m_mTransferCallList.end())
		{
			if (it->second)
			{
				it->second->m_strTargetState = "";

				std::string l_strSponsor = it->second->m_strSponsor;
				std::string l_strTarget = it->second->m_strTarget;
				std::string l_strActiveCallRefId = it->second->m_strActiveCallRefId;

				ICC_LOG_DEBUG(m_pLog, "TranserCall RingTimeOut, Sponsor: %s, Target: %s, HeldCallRefId: %s, ActiveCallRefId: %s",
					l_strSponsor.c_str(), l_strTarget.c_str(), p_strHeldCallRefId.c_str(), l_strActiveCallRefId.c_str());

				// ���峬ʱ��ȡ��ԭ����
			//	this->HangupCall(l_strActiveCallRefId, l_strSponsor, l_strSponsor);
				this->RetrieveCall(p_strHeldCallRefId, l_strSponsor, l_strSponsor);
			}
		}
	}

	if (!p_strHeldCallRefId.empty())
	{
		// ֪ͨ�ͻ������峬ʱ��ת��ʧ��
		SendTransferCallResult(p_strHeldCallRefId, ERROR_RING_TIMEOUT);
	}
}
bool CTransferCallManager::FindRingTimeout(std::string& p_strHeldCallRefId)
{
	bool b_lRingTimeOut = false;

	std::lock_guard<std::mutex> guard(m_transferCallMutex);
	for each (auto l_pTransferObj in m_mTransferCallList)
	{
		if (l_pTransferObj.second->m_strTargetState.compare(CallStateString[STATE_RING]) == 0)
		{
			m_nRingTimeout = CSysConfig::Instance()->GetTransferRingTimeout();
			DateTime::CDateTime l_CurrentTime = m_pDateTime->CurrentDateTime();
			DateTime::CDateTime l_RingTime = m_pDateTime->FromString(l_pTransferObj.second->m_strRingTime);
			DateTime::CDateTime l_EndTime = m_pDateTime->AddSeconds(l_RingTime, m_nRingTimeout);

			if (l_CurrentTime > l_EndTime)
			{
				b_lRingTimeOut = true;
				p_strHeldCallRefId = l_pTransferObj.second->m_strHeldCallRefId;
				//	��������ʱ�䣬�����ظ����
				l_pTransferObj.second->m_strRingTime = m_pDateTime->CurrentDateTimeStr();

				std::string l_strTarget = l_pTransferObj.second->m_strTarget;
				ICC_LOG_DEBUG(m_pLog, "Transfer Target: [%s] Ring Timeout !", l_strTarget.c_str());

				break;
			}
		}
	}

	return b_lRingTimeOut;
}
//////////////////////////////////////////////////////////////////////////
