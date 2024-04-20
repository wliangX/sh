#include "Boost.h"
#include "Device.h"

CDeviceCallInfo::CDeviceCallInfo(const std::string& p_strCTICallId)
	:m_strCTICallId(p_strCTICallId)
{

}

void CDeviceCallInfo::SetDeviceCallState(const std::string& p_strState, const std::string& p_strTime)
{
	if (p_strState == CallStateString[STATE_TALK])
	{
		if (m_strRingTime.empty())
		{
			m_strRingTime = p_strTime;
		}

		if (m_strTalkTime.empty())
		{
			m_strTalkTime = p_strTime;
		}
	}
	else if (p_strState == CallStateString[STATE_RING])
	{
		if (m_strRingTime.empty())
		{
			m_strRingTime = p_strTime;
		}
	}
	m_strCurrCallState = p_strState;
	m_strCurrStateTime = p_strTime;
}



void CDeviceCallInfo::SaveRecordInfo(const std::string& p_strSwitchAddr, const std::string& p_strRecordFile)
{
	m_strSwitchAddr = p_strSwitchAddr;
	m_strRecordFile = p_strRecordFile;
}


bool CDeviceCallInfo::GetRecordInfo( std::string& p_strSwitchAddr, std::string& p_strRecordFile)
{
	p_strSwitchAddr = m_strSwitchAddr;
	p_strRecordFile = m_strRecordFile;
	return true;
}


//////////////////////////////////////////////////////////////////////////
CDevice::CDevice(const std::string& p_strDeviceNum, int p_iDeviceType, const std::string& p_strAgentId, const std::string& p_strPsw) :
m_strDeviceNum(p_strDeviceNum),
m_iDeviceType(p_iDeviceType),
m_strAgentId(p_strAgentId),
m_strPsw(p_strPsw)
{
	m_strCurrCallId = "";
}

CDevice::~CDevice()
{
	//
}

IDeviceCallInfoPtr CDevice::_GetDeviceCallInfo(const std::string& p_strHcpCallId)
{
	IDeviceCallInfoPtr l_pCallInfoPtr = nullptr;
	{
		SAFE_LOCK(m_callInfoMutex);
		auto iter = m_mapDeviceCalls.find(p_strHcpCallId);
		if (iter != m_mapDeviceCalls.end())
		{
			if (iter->second)
			{
				l_pCallInfoPtr = iter->second;
			}
		}
	}

	return l_pCallInfoPtr;
}

IDeviceCallInfoPtr CDevice::_GetDeviceNextCallInfo()
{
	IDeviceCallInfoPtr l_pCallInfoPtr = nullptr;
	{
		SAFE_LOCK(m_callInfoMutex);
		auto iter = m_mapDeviceCalls.begin();
		if (iter != m_mapDeviceCalls.end())
		{
			if (iter->second)
			{
				l_pCallInfoPtr = iter->second;
			}
		}
	}
	return l_pCallInfoPtr;
}

void CDevice::_DeleteDeviceCallInfo(const std::string& p_strHcpCallId)
{
	SAFE_LOCK(m_callInfoMutex);
	m_mapDeviceCalls.erase(p_strHcpCallId);
}

void CDevice::ClearCallInfo()
{
	m_strCurrCallId = "";
	SAFE_LOCK(m_callInfoMutex);
	m_mapDeviceCalls.clear();
}

bool CDevice::SetDeviceCallState(std::string& p_strHcpCallId, const std::string& p_strState, const std::string& p_strTime,bool p_bUpdateCurrCallId)
{
	if (p_strState == CallStateString[STATE_FREE])
	{
		_DeleteDeviceCallInfo("");
		_DeleteDeviceCallInfo(p_strHcpCallId);
	}
	else
	{
		IDeviceCallInfoPtr l_pCallInfoPtr = _GetDeviceCallInfo(p_strHcpCallId);
		if (l_pCallInfoPtr)
		{
			l_pCallInfoPtr->SetDeviceCallState(p_strState, p_strTime);
		}
		else
		{
			boost::shared_ptr<CDeviceCallInfo> l_pDeviceCallInfo = boost::make_shared<CDeviceCallInfo>(p_strHcpCallId);
			if (l_pDeviceCallInfo)
			{
				l_pDeviceCallInfo->SetDeviceCallState(p_strState, p_strTime);
				SAFE_LOCK(m_callInfoMutex);
				m_mapDeviceCalls[p_strHcpCallId] = l_pDeviceCallInfo;
			}
		}
	}
	
	if (m_strCurrCallId.empty())
	{
		m_strCurrCallId = p_strHcpCallId;
	}
	else
	{
		if (m_strCurrCallId != p_strHcpCallId)
		{
			if (p_bUpdateCurrCallId)
			{
				m_strCurrCallId = p_strHcpCallId;
			}
		}
	}

	if (m_strCurrCallId == p_strHcpCallId && p_strState == CallStateString[STATE_FREE])
	{
		m_strCurrCallId = "";
	}
	p_strHcpCallId = m_strCurrCallId;
	return true;
}

std::string CDevice::GetNextCallState(std::string& p_strState)
{
	std::string l_strCallId;
	IDeviceCallInfoPtr l_pCallInfoPtr = _GetDeviceNextCallInfo();
	if (l_pCallInfoPtr)
	{
		p_strState = l_pCallInfoPtr->GetState();
		l_strCallId = l_pCallInfoPtr->GetCallId();
		m_strCurrCallId = l_strCallId;
	}
	return l_strCallId;
}

std::string CDevice::GetDeviceTypeString()
{
	std::string l_strDeviceType = DEVICE_TYPE_IPPHONE;

	return l_strDeviceType;
}


std::string CDevice::GetCurrCallState()
{
	IDeviceCallInfoPtr l_pCallInfoPtr = _GetDeviceCallInfo(m_strCurrCallId);
	if (l_pCallInfoPtr)
	{
		return l_pCallInfoPtr->GetState(); 
	}

	return "";
}

std::string CDevice::GetDeviceCallState(const std::string &p_strCTICallId)
{
	IDeviceCallInfoPtr l_pCallInfoPtr = _GetDeviceCallInfo(p_strCTICallId);
	if (l_pCallInfoPtr)
	{
		return l_pCallInfoPtr->GetState();
	}

	return "";
}

std::string CDevice::GetCurrCallStateTime()
{
	IDeviceCallInfoPtr l_pCallInfoPtr = _GetDeviceCallInfo(m_strCurrCallId);
	if (l_pCallInfoPtr)
	{
		return l_pCallInfoPtr->GetStateTime(); 
	}

	return "";
}

std::string CDevice::GetCurrCallTalkTime()
{
	IDeviceCallInfoPtr l_pCallInfoPtr = _GetDeviceCallInfo(m_strCurrCallId);
	if (l_pCallInfoPtr)
	{
		return l_pCallInfoPtr->GetTalkTime(); 
	}
	return "";
}

bool CDevice::SaveCurrRecordInfo(const std::string& p_strHcpCallId,const std::string& p_strSwitchAddr, const std::string& p_strRecordFile)
{
	IDeviceCallInfoPtr l_pCallInfoPtr = _GetDeviceCallInfo(p_strHcpCallId);
	if (l_pCallInfoPtr)
	{
		l_pCallInfoPtr->SaveRecordInfo(p_strSwitchAddr, p_strRecordFile);
		return true; 
		
	}
	return false;
	
}

bool CDevice::GetCurrRecordInfo(const std::string& p_strHcpCallId,std::string& p_strSwitchAddr, std::string& p_strRecordFile)
{
	IDeviceCallInfoPtr l_pCallInfoPtr = _GetDeviceCallInfo(p_strHcpCallId);
	if (l_pCallInfoPtr)
	{
		l_pCallInfoPtr->GetRecordInfo(p_strSwitchAddr, p_strRecordFile);
		return true;
	}
	
	return false;
}

void CDevice::DeleteCallInfo(const std::string& p_strHcpCallId)
{
	_DeleteDeviceCallInfo(p_strHcpCallId);

	if (p_strHcpCallId == m_strCurrCallId)
	{
		m_strCurrCallId = "";
	}
}

int CDevice::GetCallListSize()
{
	return m_mapDeviceCalls.size();
}

