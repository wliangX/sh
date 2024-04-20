#include "Boost.h"

#include "Device.h"

//////////////////////////////////////////////////////////////////////////
CDevice::CDevice(const std::string& p_strDeviceNum, int p_iDeviceType, const std::string& p_strAgentId, const std::string& p_strPsw) :
m_strDeviceNum(p_strDeviceNum),
m_iDeviceType(p_iDeviceType),
m_strAgentId(p_strAgentId),
m_strPsw(p_strPsw)
{
	m_lMonitorId = 0;
	m_lRouteRegId = 0;
	m_lCSTACallRefId = DEFAULT_CALLREFID;

	m_bIsMonitor = false;
	m_bIsAgent = false;
}

CDevice::~CDevice()
{
	//
}

void CDevice::SetDeviceState(long p_lCSTACallRefId, const std::string& p_strDeviceState, const std::string& p_strStateTime)
{
	if (!this->FindLogicalCall(p_lCSTACallRefId))
	{
		this->AddLogicalCall(p_lCSTACallRefId, p_strDeviceState, p_strStateTime);
	}
	else
	{
		this->UpdateLogicalCallState(p_lCSTACallRefId, p_strDeviceState, p_strStateTime);
	}
}

std::string CDevice::GetDeviceState()
{
	std::string l_strDeviceState = CallStateString[STATE_FREE];

	if (this->GetLogicalCallCount() > 0)
	{
		l_strDeviceState = GetLastLogicalCallState();
	} 
	
	return l_strDeviceState;
}

std::string CDevice::GetStateTime(long p_lCSTACallRefId)
{
	return GetLogicalCallStateTime(p_lCSTACallRefId);
}

std::string CDevice::GetDeviceTypeString()
{
	std::string l_strDeviceType = DEVICE_TYPE_UNKNOW;

	switch (m_iDeviceType)
	{
	case CTC_TYPE_IPPHONE:
		l_strDeviceType = DEVICE_TYPE_IPPHONE;
		break;
	case CTC_TYPE_DIGITAL:
		l_strDeviceType = DEVICE_TYPE_DIGITAL;
		break;
	case CTC_TYPE_ANALOG:
		l_strDeviceType = DEVICE_TYPE_ANALOG;
		break;
	case CTC_TYPE_VIRDN:
		l_strDeviceType = DEVICE_TYPE_VIRDN;
		break;
	case CTC_TYPE_ACDGROUP:
		l_strDeviceType = DEVICE_TYPE_ACDGROUP;
		break;
	case CTC_TYPE_ACDSUPER:
		l_strDeviceType = DEVICE_TYPE_ACDSUPER;
		break;
	case CTC_TYPE_ROUTE:
		l_strDeviceType = DEVICE_TYPE_ROUTE;
		break;
	default:
		break;
	}

	return l_strDeviceType;
}

//////////////////////////////////////////////////////////////////////////
void CDevice::AddLogicalCall(long p_lCSTACallRefId, const std::string& p_strDeviceState, const std::string& p_strStateTime)
{
	boost::shared_ptr<CLogicalCall> l_pLogicalCall = boost::make_shared<CLogicalCall>();
	if (l_pLogicalCall)
	{
		l_pLogicalCall->m_lCSTACallRefId = p_lCSTACallRefId;
		l_pLogicalCall->m_strDeviceState = p_strDeviceState;
		l_pLogicalCall->m_strStateTime = p_strStateTime;

		m_mapLogicalCallQueue[p_lCSTACallRefId] = l_pLogicalCall;
	}
}
void CDevice::UpdateLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceState, const std::string& p_strStateTime)
{
	auto it = m_mapLogicalCallQueue.find(p_lCSTACallRefId);
	if (it != m_mapLogicalCallQueue.end())
	{
		if (it->second)
		{
			it->second->m_strDeviceState = p_strDeviceState;
			it->second->m_strStateTime = p_strStateTime;
		}
	}
}
void CDevice::DeleteLogicalCall(long p_lCSTACallRefId)
{
	auto it = m_mapLogicalCallQueue.find(p_lCSTACallRefId);
	if (it != m_mapLogicalCallQueue.end())
	{
		m_mapLogicalCallQueue.erase(it);
	}
}
bool CDevice::FindLogicalCall(long p_lCSTACallRefId)
{
	bool l_bFind = false;

	auto it = m_mapLogicalCallQueue.find(p_lCSTACallRefId);
	if (it != m_mapLogicalCallQueue.end())
	{
		l_bFind = true;
	}

	return l_bFind;
}
int CDevice::GetLogicalCallCount()
{
	return m_mapLogicalCallQueue.size();
}
bool CDevice::GetLastLogicalCall(long &p_lCSTACallRefId)
{
	bool l_bRet = false;

	auto it = m_mapLogicalCallQueue.begin();
	while (it != m_mapLogicalCallQueue.end())
	{
		//	´ýÊµÏÖ
		++it;
	}

	return l_bRet;
}
std::string CDevice::GetLastLogicalCallState()
{
	std::string l_strDeviceState = CallStateString[STATE_FREE];

	auto it = m_mapLogicalCallQueue.rbegin();
	if (it != m_mapLogicalCallQueue.rend())
	{
		if (it->second)
		{
			l_strDeviceState = it->second->m_strDeviceState;
		}
	}

	return l_strDeviceState;
}
std::string CDevice::GetLogicalCallState(long p_lCSTACallRefId)
{
	std::string l_strDeviceState = CallStateString[STATE_FREE];

	auto it = m_mapLogicalCallQueue.find(p_lCSTACallRefId);
	if (it != m_mapLogicalCallQueue.end())
	{
		if (it->second)
		{
			l_strDeviceState = it->second->m_strDeviceState;
		}
	}

	return l_strDeviceState;
}
std::string CDevice::GetLogicalCallStateTime(long p_lCSTACallRefId)
{
	std::string l_strDeviceStateTime = "";

	auto it = m_mapLogicalCallQueue.find(p_lCSTACallRefId);
	if (it != m_mapLogicalCallQueue.end())
	{
		if (it->second)
		{
			l_strDeviceStateTime = it->second->m_strStateTime;
		}
	}

	return l_strDeviceStateTime;
}
bool CDevice::GetCurrentLogicalCall(long &p_lCSTACallRefId, std::string& p_strDeviceState, std::string& p_strStateTime)
{
	bool l_bRet = false;

	auto it = m_mapLogicalCallQueue.rbegin();
	if (it != m_mapLogicalCallQueue.rend())
	{
		if (it->second)
		{
			l_bRet = true;

			p_lCSTACallRefId = it->second->m_lCSTACallRefId;
			p_strDeviceState = it->second->m_strDeviceState;
			p_strStateTime = it->second->m_strStateTime;
		}
	}

	return l_bRet;
}