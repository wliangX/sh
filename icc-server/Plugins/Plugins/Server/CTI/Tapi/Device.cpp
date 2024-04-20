#include "Boost.h"
#include "Device.h"

#define LINEDEVSTATE_ALL        0x01FFFFFF
#define LINEADDRESSSTATE_ALL    0x000001FF

//////////////////////////////////////////////////////////////////////////
CDevice::CDevice(const std::string& p_strDeviceNum, int p_iDeviceType, const std::string& p_strAgentId, const std::string& p_strPsw) :
m_strDeviceNum(p_strDeviceNum),
m_iDeviceType(p_iDeviceType),
m_strAgentId(p_strAgentId),
m_strPsw(p_strPsw)
{
	m_lCSTACallRefId = DEFAULT_CALLREFID;

	m_bIsMonitor = false;
	m_bIsAgent = false;

	m_lRouteRegId = 0;
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

std::string CDevice::GetDeviceState(long &p_lCSTACallRefId)
{
	std::string l_strDeviceState = CallStateString[STATE_FREE];

	if (this->GetLogicalCallCount() > 0)
	{
		l_strDeviceState = GetLastLogicalCallState(p_lCSTACallRefId);
	}
	
	return l_strDeviceState;
}

std::string CDevice::GetStateTime(long p_lCSTACallRefId)
{
	return GetLogicalCallStateTime(p_lCSTACallRefId);
}

void CDevice::GetAllCSTACallRefId(std::vector<long>& CallIDVec)
{
	auto it = m_mapLogicalCallQueue.begin();
	for (; it != m_mapLogicalCallQueue.end(); ++it)
	{
		CallIDVec.push_back(it->first);
	}
}

void CDevice::SetDeviceType(const std::string& p_strDeviceType)
{
	if (p_strDeviceType.compare(DEVICE_TYPE_IPPHONE) == 0) m_iDeviceType = CTC_TYPE_IPPHONE;
	else if (p_strDeviceType.compare(DEVICE_TYPE_DIGITAL) == 0) m_iDeviceType = CTC_TYPE_DIGITAL;
	else if (p_strDeviceType.compare(DEVICE_TYPE_ANALOG) == 0) m_iDeviceType = CTC_TYPE_ANALOG;
	else if (p_strDeviceType.compare(DEVICE_TYPE_VIRDN) == 0) m_iDeviceType = CTC_TYPE_VIRDN;
	else if (p_strDeviceType.compare(DEVICE_TYPE_ACDGROUP) == 0) m_iDeviceType = CTC_TYPE_ACDGROUP;
	else if (p_strDeviceType.compare(DEVICE_TYPE_ACDSUPER) == 0) m_iDeviceType = CTC_TYPE_ACDSUPER;
	else if (p_strDeviceType.compare(DEVICE_TYPE_ROUTE) == 0) m_iDeviceType = CTC_TYPE_ROUTE;
	else m_iDeviceType = CTC_TYPE_UNKNOW;

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
		ICC_LOG_DEBUG(m_pLog, "DeviceNum:[%s],CSTACallRefId:[new=%d],DeviceState:[%s]", m_strDeviceNum.c_str(), p_lCSTACallRefId, p_strDeviceState.c_str());
	}
}
bool CDevice::UpdateLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceState, const std::string& p_strStateTime)
{
	bool l_bRet = false;

	auto it = m_mapLogicalCallQueue.find(p_lCSTACallRefId);
	if (it != m_mapLogicalCallQueue.end())
	{
		if (it->second)
		{
			l_bRet = true;
			it->second->m_strDeviceState = p_strDeviceState;
			it->second->m_strStateTime = p_strStateTime;
			ICC_LOG_DEBUG(m_pLog, "DeviceNum:[%s],CSTACallRefId:[%d],DeviceState:[%s]", m_strDeviceNum.c_str(), p_lCSTACallRefId, p_strDeviceState.c_str());
		}
	}
	return l_bRet;
}

bool CDevice::CopyLogicalCallState(long p_lDestCSTACallRefId, long p_lSrcCSTACallRefId)
{
	bool l_bRet = false;
	bool l_bNewCSTACallRefId = false;
	if (!FindLogicalCall(p_lDestCSTACallRefId))
	{
		l_bNewCSTACallRefId = true;
	}
	auto it = m_mapLogicalCallQueue.find(p_lSrcCSTACallRefId);
	int l_lSize = m_mapLogicalCallQueue.size();
	if (it != m_mapLogicalCallQueue.end())
	{
		if (it->second)
		{
			l_bRet = true;
			m_mapLogicalCallQueue[p_lDestCSTACallRefId] = it->second;
			m_mapLogicalCallQueue[p_lDestCSTACallRefId]->m_lCSTACallRefId = p_lDestCSTACallRefId;
			ICC_LOG_DEBUG(m_pLog, "DeviceNum:[%s],DestCSTACallRefId:[new=%d],SrcCSTACallRefId:[old=%d_%d],size:[%d],bNewCSTACallRefId:[%d]", m_strDeviceNum.c_str(), p_lDestCSTACallRefId, p_lSrcCSTACallRefId, it->second->m_lCSTACallRefId, l_lSize, l_bNewCSTACallRefId);
		}
	}
	return l_bRet;
}

bool CDevice::DeleteLogicalCall(long p_lCSTACallRefId)
{
	bool l_bRet = false;
	auto it = m_mapLogicalCallQueue.find(p_lCSTACallRefId);
	int l_lSize = m_mapLogicalCallQueue.size();
	if (it != m_mapLogicalCallQueue.end())
	{
		ICC_LOG_DEBUG(m_pLog, "DeviceNum:[%s],CSTACallRefId:[%d],size:[%d]", m_strDeviceNum.c_str(), p_lCSTACallRefId, l_lSize);
		l_bRet = true;
		m_mapLogicalCallQueue.erase(it);
	}
	return l_bRet;
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
/*
bool CDevice::GetLastLogicalCall(long &p_lCSTACallRefId)
{
	bool l_bRet = false;

	auto it = m_mapLogicalCallQueue.begin();
	while (it != m_mapLogicalCallQueue.end())
	{
		//	待实现
		++it;
	}

	return l_bRet;
}*/
std::string CDevice::GetLastLogicalCallState(long &p_lCSTACallRefId)
{
	std::string l_strDeviceState = CallStateString[STATE_FREE];

	auto it = m_mapLogicalCallQueue.rbegin();
	if (it != m_mapLogicalCallQueue.rend())
	{
		p_lCSTACallRefId = it->first;

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
	long l_maxlCSTACallRefId = 0;
	{
		auto it = m_mapLogicalCallQueue.rbegin();
		if (it != m_mapLogicalCallQueue.rend())
		{
			if (it->second)
			{
				l_bRet = true;

				p_lCSTACallRefId = it->second->m_lCSTACallRefId;
				p_strDeviceState = it->second->m_strDeviceState;
				p_strStateTime = it->second->m_strStateTime;

				if (p_lCSTACallRefId == DEFAULT_CALLREFID)
				{
					return l_bRet;
				}
			}
		}
	}
	
	auto it = m_mapLogicalCallQueue.begin();
	while (it != m_mapLogicalCallQueue.end())
	{
		if (it->second)
		{
			if (it->second->m_lCSTACallRefId > l_maxlCSTACallRefId && it->second->m_lCSTACallRefId != DEFAULT_CALLREFID)
			{
				l_maxlCSTACallRefId = it->second->m_lCSTACallRefId;
			}
		}
		++it;
	}

	auto itFind = m_mapLogicalCallQueue.find(l_maxlCSTACallRefId);
	long l_lKey = 0;
	if (itFind != m_mapLogicalCallQueue.end())
	{
		if (itFind->second)
		{
			l_bRet = true;
			p_lCSTACallRefId = itFind->second->m_lCSTACallRefId;
			p_strDeviceState = itFind->second->m_strDeviceState;
			p_strStateTime = itFind->second->m_strStateTime;
			l_lKey = itFind->first;
		}
	}
	ICC_LOG_DEBUG(m_pLog, "DeviceNum:[%s],:key:[%d],lCSTACallRefId:[%d],Find=%d,size=%d", m_strDeviceNum.c_str(), l_lKey,l_maxlCSTACallRefId, l_bRet, m_mapLogicalCallQueue.size());
	return l_bRet;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CTapiLine::CTapiLine(const std::string& p_strDeviceNum, int p_iDeviceType, const std::string& p_strAgentId, const std::string& p_strPsw)
	: CDevice(p_strDeviceNum, p_iDeviceType, p_strAgentId, p_strPsw)
{
	m_bLineConnectState = false;

	m_nOffset = 0;
	m_dwExtension = 0;
	m_dwLineID = 0;
	m_hLine = 0;

	m_hConnectedCall = 0;
	m_hWaitingCall = 0;
	m_hHeldCall = 0;
	m_hPendingCall = 0;
	m_hConferenceCall = 0;
	m_hConsultationCall = 0;
	m_hLastCallIntoConf = 0;

	memset(m_szBuffer, 0, TAPI_LINE_BUFFER * MAX_SIZE);
}
CTapiLine::CTapiLine()
	: CDevice("", CTC_TYPE_UNKNOW, "", "")
{
	m_bLineConnectState = false;

	m_nOffset = 0;
	m_dwExtension = 0;
	m_dwLineID = 0;
	m_hLine = 0;

	m_hConnectedCall = 0;
	m_hWaitingCall = 0;
	m_hHeldCall = 0;
	m_hPendingCall = 0;
	m_hConferenceCall = 0;
	m_hConsultationCall = 0;
	m_hLastCallIntoConf = 0;

	memset(m_szBuffer, 0, TAPI_LINE_BUFFER * MAX_SIZE);
}
CTapiLine::~CTapiLine()
{
	if (m_hLine)
		::lineClose(m_hLine);

	m_hLine = 0;
}

void CTapiLine::ResetCallHandle(HCALL p_hCall)
{
	if (p_hCall == m_hConnectedCall) m_hConnectedCall = 0;
	/*else */if (p_hCall == m_hWaitingCall) m_hWaitingCall = 0;
	/*else */if (p_hCall == m_hHeldCall) m_hHeldCall = 0;
	/*else */if (p_hCall == m_hPendingCall) m_hPendingCall = 0;
	/*else */if (p_hCall == m_hConferenceCall) m_hConferenceCall = 0;
	/*else */if (p_hCall == m_hConsultationCall) m_hConsultationCall = 0;
	/*else {}*/
}
void CTapiLine::ProcessCallHandle(HCALL p_hCall, E_CALL_HANDLE_TYPE p_nHandleType)
{
	switch (p_nHandleType)
	{
	case FREE_CALL:
	{
		this->ResetCallHandle(p_hCall);
	}
	break;
	case PENDING_CALL:
	{
		m_hPendingCall = p_hCall;
	}
	break;
	case WAITING_CALL:
	{
		m_hWaitingCall = p_hCall;
	}
	break;
	case CONNECTED_CALL:
	{
		this->ResetCallHandle(p_hCall);
		m_hConnectedCall = p_hCall;
	}
	break;
	case HELD_CALL:
	{
		if (p_hCall == m_hConnectedCall) m_hConnectedCall = m_hHeldCall;
		m_hHeldCall = p_hCall;
	}
	break;
	case CONFERENCE_CALL:
	{
		m_hConferenceCall = p_hCall;
		m_hConnectedCall = p_hCall;
	}
	break;
	default:
		break;
	}
}
void CTapiLine::SetCallHandle(HCALL p_hCall, E_CALL_HANDLE_TYPE p_nHandleType)
{
	switch (p_nHandleType)
	{
	case CONNECTED_CALL:
		m_hConnectedCall = p_hCall;
		break;
	case WAITING_CALL:
		m_hWaitingCall = p_hCall;
		break;
	case HELD_CALL:
		m_hHeldCall = p_hCall;
		break;
	case PENDING_CALL:
		m_hPendingCall = p_hCall;
		break;
	case CONFERENCE_CALL:
		m_hConferenceCall = p_hCall;
		break;
	case CONSULTATION_CALL:
		m_hConsultationCall = p_hCall;
		break;
	case LAST_CALL_INTO_CONF:
		m_hLastCallIntoConf = p_hCall;
		break;
	default:
		break;
	}
}
HCALL CTapiLine::GetCallHandle(E_CALL_HANDLE_TYPE p_nHandleType)
{
	HCALL l_hCall = 0;

	switch (p_nHandleType)
	{
	case CONNECTED_CALL:
		l_hCall = m_hConnectedCall;
		break;
	case WAITING_CALL:
		l_hCall = m_hWaitingCall;
		break;
	case HELD_CALL:
		l_hCall = m_hHeldCall;
		break;
	case PENDING_CALL:
		l_hCall = m_hPendingCall;
		break;
	case CONFERENCE_CALL:
		l_hCall = m_hConferenceCall;
		break;
	case CONSULTATION_CALL:
		l_hCall = m_hConsultationCall;
		break;
	case LAST_CALL_INTO_CONF:
		l_hCall = m_hLastCallIntoConf;
		break;
	default:
		break;
	}

	return l_hCall;
}
//////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
// Open a specified TAPI line
bool CTapiLine::Open(HLINEAPP p_hLineApp, DWORD p_dwLineID, DWORD p_dwAPIVersion, DWORD p_dwCallPrivilege, DWORD p_dwMediaModes)
{
	m_hLine = 0;

	// Ask TAPI for a new line
	HLINE l_hNewLine = 0;
	HRESULT l_hResult = ::lineOpenA(p_hLineApp, p_dwLineID,
		&l_hNewLine, p_dwAPIVersion,
		0, (DWORD)this, p_dwCallPrivilege, p_dwMediaModes, 0);
	if (S_OK == l_hResult)
	{
		// Use the results
		m_hLine = l_hNewLine;
		m_dwLineID = p_dwLineID;

		// Set all status messages
		l_hResult = ::lineSetStatusMessages(m_hLine, LINEDEVSTATE_ALL, LINEADDRESSSTATE_ALL);
	}

	return (S_OK == l_hResult);
}
bool CTapiLine::Close()
{
	if (m_hLine)
	{
		HRESULT l_hResult = ::lineClose(m_hLine);
		m_hLine = 0;

		return (S_OK == l_hResult);
	}
	
	return false;
}
// ----------------------------------------------------------------------------
HRESULT CTapiLine::HytLineDevSpecific(HLINE p_hLine, DWORD p_dwAddressID, HCALL p_hCall, char* p_szBuffer, std::size_t p_nSize)
{
	char *l_pTempBuf = &m_szBuffer[TAPI_LINE_BUFFER * (++m_nOffset % MAX_SIZE)];
	memset(l_pTempBuf, 0, TAPI_LINE_BUFFER);
	strncpy_s(l_pTempBuf, TAPI_LINE_BUFFER, p_szBuffer, p_nSize);

	//	::lineDevSpecific 为异步函数，l_pTempBuf 在执行结果返回前，::lineDevSpecific 内部还会修改，因此不能使用局部变量
	return ::lineDevSpecific(p_hLine, p_dwAddressID, p_hCall, l_pTempBuf, p_nSize);
}

// Ask TAPI to make a call - only works if we are not on a call.
HRESULT CTapiLine::MakeCall(const std::string& p_strAddress)
{
	HRESULT l_hResult = 0;

	if ((m_hConnectedCall == 0) && (m_hWaitingCall == 0)) // No currently active call
	{
		// Calculate the size of the parameter structure, and allocate it.
		std::size_t l_nDestLen = p_strAddress.length() + 1;
		std::size_t l_nCallParamsLen = sizeof(LINECALLPARAMS) + l_nDestLen;
		LINECALLPARAMS* l_pCallParams = (LINECALLPARAMS*)(new BYTE[l_nCallParamsLen]);
		// Setup the parameters
		if (l_pCallParams)
		{
			ZeroMemory(l_pCallParams, l_nCallParamsLen);

			l_pCallParams->dwTotalSize = l_nCallParamsLen;
			l_pCallParams->dwBearerMode = LINEBEARERMODE_VOICE;
			l_pCallParams->dwMinRate = 0;   // Use device default
			l_pCallParams->dwMaxRate = 0;   // Use device default
			l_pCallParams->dwMediaMode = LINEMEDIAMODE_INTERACTIVEVOICE;
			l_pCallParams->dwCallParamFlags = 0; // No flags
			l_pCallParams->dwAddressMode = LINEADDRESSMODE_ADDRESSID;
			l_pCallParams->dwAddressID = 0; // Use the main (and only) line address
			// Variable length strings
			l_pCallParams->dwDisplayableAddressSize = l_nDestLen;
			l_pCallParams->dwDisplayableAddressOffset = sizeof(LINECALLPARAMS);
		}

		// Ask TAPI to make the call
		l_hResult = ::lineMakeCallA(m_hLine, &m_hWaitingCall, p_strAddress.c_str(), 0, l_pCallParams);
		delete[] l_pCallParams;
		l_pCallParams = NULL;
	}

	return l_hResult;
}
// ----------------------------------------------------------------------------
// Drop the line - only works if a call is connected or an outgoing call is
// alerting
HRESULT CTapiLine::DropCall(HCALL p_hCall/* = 0*/)
{
	HRESULT l_hResult = 0;

	if (p_hCall)
	{
		l_hResult = ::lineDrop(p_hCall, NULL, 0);
	}
	/*else if (m_hConferenceCall) // Conference call
	{
		l_hResult = ::lineDrop(m_hConferenceCall, NULL, 0);
	}*/
	else if (m_hConnectedCall) // Active call
	{
		l_hResult = ::lineDrop(m_hConnectedCall, NULL, 0);
	}
	else if (m_hWaitingCall) // Outgoing call
	{
		l_hResult = ::lineDrop(m_hWaitingCall, NULL, 0);
	}
	else if (m_hPendingCall) // Ring call
	{
		l_hResult = ::lineDrop(m_hPendingCall, NULL, 0);
	}
	else
	{
		//
	}

	return l_hResult;
}
HRESULT CTapiLine::DropCallByHandleType(E_CALL_HANDLE_TYPE p_nHandleType)
{
	HRESULT l_hResult = 0;

	switch (p_nHandleType)
	{
	case CONNECTED_CALL:
		l_hResult = ::lineDrop(m_hConnectedCall, NULL, 0);
		break;
	case WAITING_CALL:
		l_hResult = ::lineDrop(m_hWaitingCall, NULL, 0);
		break;
	case HELD_CALL:
		l_hResult = ::lineDrop(m_hHeldCall, NULL, 0);
		break;
	case PENDING_CALL:
		l_hResult = ::lineDrop(m_hPendingCall, NULL, 0);
		break;
	case CONFERENCE_CALL:
		l_hResult = ::lineDrop(m_hConferenceCall, NULL, 0);
		break;
	case CONSULTATION_CALL:
		l_hResult = ::lineDrop(m_hConsultationCall, NULL, 0);
		break;
	default:
		break;
	}

	return l_hResult;
}
// ----------------------------------------------------------------------------
// 挂断所有话务
HRESULT CTapiLine::ClearCall()
{
	HRESULT l_hResult = 0;

	if (m_hConferenceCall) // Conference call
	{
		l_hResult = ::lineDrop(m_hConferenceCall, NULL, 0);
	}
	if (m_hWaitingCall) // Outgoing call
	{
		l_hResult = ::lineDrop(m_hWaitingCall, NULL, 0);
	}
	if (m_hPendingCall) // Ring call
	{
		l_hResult = ::lineDrop(m_hPendingCall, NULL, 0);
	}
	if (m_hConnectedCall) // Active call
	{
		l_hResult = ::lineDrop(m_hConnectedCall, NULL, 0);
	}

	return l_hResult;
}
// ----------------------------------------------------------------------------
// Answer the current call - only works if call is currently pending
HRESULT CTapiLine::AnswerCall()
{
	HRESULT l_hResult = 0;

	if (m_hPendingCall)
	{
		l_hResult = ::lineAnswer(m_hPendingCall, NULL, 0);
	}

	return l_hResult;
}
// ----------------------------------------------------------------------------
// Hold the current call - only works if call is connected
HRESULT CTapiLine::HoldCall()
{
	HRESULT l_hResult = 0;

	if (m_hConnectedCall)
	{
		l_hResult = ::lineHold(m_hConnectedCall);
	}

	return l_hResult;
}
// ----------------------------------------------------------------------------
// Retrieve the call from hold - only works if call is currently proceeding
HRESULT CTapiLine::UnholdCall()
{
	HRESULT l_hResult = 0;

	if (m_hHeldCall)
	{
		l_hResult = ::lineUnhold(m_hHeldCall);
	}

	return l_hResult;
}
// ----------------------------------------------------------------------------
// Conference the current call with the call on hold - only works if call
// is currently connected and there is a call on hold.
HRESULT CTapiLine::ConferenceCall()
{
	HRESULT l_hResult = 0;

	if (m_hConnectedCall && m_hHeldCall)
	{
		l_hResult = ::lineCompleteTransfer(m_hHeldCall, m_hConnectedCall, &m_hConferenceCall,
			LINETRANSFERMODE_CONFERENCE);
		m_hLastCallIntoConf = m_hConnectedCall;

		if (m_hConferenceCall == 0)
		{
			//No Conference Call Handle returned
		}

	}

	return l_hResult;
}
// ----------------------------------------------------------------------------
// Transfer the call on hold to the current call
// - only works if call is currently connected
HRESULT CTapiLine::BlindTransferCall(const std::string& p_strAddress)
{
	HRESULT l_hResult = 0;

	if (m_hConnectedCall)
	{
		l_hResult = ::lineBlindTransferA(m_hConnectedCall, p_strAddress.c_str(), 0);
	}

	return l_hResult;
}
// ----------------------------------------------------------------------------
// If the current call is connected, then lineSetupTransfer will put the call
// on hold and create a new consultation call.  If the current call is already
// on hold, then lineSetupTransfer will just create a new consultation call.
HRESULT CTapiLine::SetupTransfer()
{
	HRESULT l_hResult = 0;

	if (m_hConnectedCall)
	{
		l_hResult = ::lineSetupTransferA(m_hConnectedCall, &m_hConsultationCall, 0);
		if (l_hResult > 0)
		{
			m_hHeldCall = m_hConnectedCall;
		}
	}
	else if (m_hHeldCall)
	{
		l_hResult = ::lineSetupTransferA(m_hHeldCall, &m_hConsultationCall, 0);
	}
	else
	{
		//
	}

	return l_hResult;
}
// ----------------------------------------------------------------------------
// Dial will dial the given number on the currently active call, where the call
// has been created using lineSetupTransfer.
HRESULT CTapiLine::Dial(const std::string& p_strAddress)
{
	HRESULT l_hResult = 0;

	if (!m_hHeldCall)
	{
//lineSetupTransfer: No Held Call
	}
	else if (!m_hConsultationCall)
	{
//lineSetupTransfer: No Consultation Call
	}
	else
	{
		l_hResult = ::lineDialA(m_hConsultationCall, p_strAddress.c_str(), 0);
		if (l_hResult > 0)
		{
			m_hConnectedCall = m_hConsultationCall;
		}
	}

	return l_hResult;
}
// ----------------------------------------------------------------------------
// CompleteTransfer will transfer the held call to the currently connected call
HRESULT CTapiLine::CompleteTransfer()
{
	HRESULT l_hResult = 0;

	if (!m_hHeldCall)
	{
		//lineCompleteTransfer: No Held Call
	}
	else if (!m_hConsultationCall)
	{
		if (!m_hConnectedCall)
		{
			// lineSetupTransfer: No Consultation Call
			return l_hResult;
		}
		else
		{
			m_hConsultationCall = m_hConnectedCall;
		}
	}

	l_hResult = ::lineCompleteTransfer(m_hHeldCall, m_hConsultationCall, NULL, LINETRANSFERMODE_TRANSFER);

	return l_hResult;
}
// ----------------------------------------------------------------------------
// SwapHold puts the currently active call on hold and retrieves the held call.
HRESULT CTapiLine::SwapHold()
{
	HRESULT l_hResult = 0;

	if (!m_hHeldCall)
	{
		//lineSwapHold: No Held Call
	}
	else if (!m_hConnectedCall)
	{
		//lineSwapHold: No Active Call
	}
	else
	{
		l_hResult = ::lineSwapHold(m_hConnectedCall, m_hHeldCall);
	}

	return l_hResult;
}
// ----------------------------------------------------------------------------
// Park will park a call. Note that only a directed park will succeed on 
// IP Office
HRESULT CTapiLine::Park(const std::string& p_strAddress)
{
	HRESULT l_hResult = 0;

	if (!m_hConnectedCall)
	{
		//linePark: No Active Call
	}
	else
	{
		if (!p_strAddress.empty())
		{
			l_hResult = ::lineParkA(m_hConnectedCall, LINEPARKMODE_DIRECTED, p_strAddress.c_str(), NULL);
		}
		else
		{
			DWORD size = 512;
			VARSTRING* pAddress = (VARSTRING*) new char[size]; // This should be big enough
			ZeroMemory(&pAddress[0], size);
			pAddress->dwTotalSize = size;

			l_hResult = ::lineParkA(m_hConnectedCall, LINEPARKMODE_NONDIRECTED, "", pAddress);
			delete[] pAddress;
		}
	}

	return l_hResult;
}
// ----------------------------------------------------------------------------
// Unpark retrieves a parked call from the given park address
HRESULT CTapiLine::Unpark(const std::string& p_strAddress)
{
	HRESULT l_hResult = 0;

	l_hResult = ::lineUnparkA(m_hLine, 0, &m_hConnectedCall, p_strAddress.c_str());

	return l_hResult;
}
// ----------------------------------------------------------------------------
// This function redirects an alerting call to the given extension
HRESULT CTapiLine::Redirect(const std::string& p_strAddress)
{
	HRESULT l_hResult = 0;

	if (!m_hPendingCall)
	{
		//No Offering Call
	}
	else
	{
		l_hResult = ::lineRedirectA(m_hPendingCall, p_strAddress.c_str(), 0);
	}

	return l_hResult;
}
// ----------------------------------------------------------------------------
// AddToConference adds the connected call to a held conference
HRESULT CTapiLine::AddToConference()
{
	HRESULT l_hResult = 0;

	if (m_hConnectedCall && m_hHeldCall)//m_hConferenceCall)
	{
		l_hResult = ::lineAddToConference(m_hHeldCall/*m_hConferenceCall*/, m_hConnectedCall);

		m_hLastCallIntoConf = m_hConnectedCall;
	}
	else
	{
		if (!m_hConnectedCall)	{}//lineAddToConference: No Active Call
		if (!m_hHeldCall)	{}//lineAddToConference: No Held Call
	}

	return l_hResult;
}
// ----------------------------------------------------------------------------
// RemoveFromConference removes the last call added to the conference
HRESULT CTapiLine::RemoveFromConference()
{
	HRESULT l_hResult = 0;

	if (m_hLastCallIntoConf)
	{
		l_hResult = ::lineRemoveFromConference(m_hLastCallIntoConf);
	}
	else
	{
		if (!m_hLastCallIntoConf){}	//lineRemoveFromConference: No Active Call
	}

	return l_hResult;
}

// ----------------------------------------------------------------------------
// Log On an extension
HRESULT CTapiLine::LogOn()
{
	HRESULT l_hResult = 0;

	char l_szBuffer[TAPI_LINE_BUFFER] = { 0 };
	l_szBuffer[0] = 8;
	std::size_t l_nLen = m_strDeviceNum.length();
	strncpy_s(&(l_szBuffer[1]), TAPI_LINE_BUFFER - 2, m_strDeviceNum.c_str(), l_nLen);
	l_nLen += 2;

//	l_hResult = ::lineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
	l_hResult = this->HytLineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);

	return l_hResult;
}
HRESULT CTapiLine::LogOff()
{
	HRESULT l_hResult = 0;

	char l_szBuffer[TAPI_LINE_BUFFER] = { 0 };
	l_szBuffer[0] = 9;
	l_szBuffer[1] = 47;
	std::size_t l_nLen = 3;

//	l_hResult = ::lineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
	l_hResult = this->HytLineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);

	return l_hResult;
}

// ----------------------------------------------------------------------------
// Set the divert destination
HRESULT CTapiLine::DivertDestination(const std::string& p_strAddress)
{
	HRESULT l_hResult = 0;

	if (!p_strAddress.empty())
	{
		char l_szBuffer[TAPI_LINE_BUFFER] = { 0 };
		l_szBuffer[0] = 9;
		l_szBuffer[1] = 6;

		size_t l_nLen = p_strAddress.length();
		strncpy_s(&(l_szBuffer[2]), TAPI_LINE_BUFFER - 3, p_strAddress.c_str(), l_nLen);
		l_nLen += 3;

//		l_hResult = ::lineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
		l_hResult = this->HytLineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
	}

	return l_hResult;
}
// ----------------------------------------------------------------------------
// Set the divert settings, i.e. forwarding or Do Not Disturb
HRESULT CTapiLine::SetDivertSettings(bool p_bFwdAll, bool p_bFwdBusy, bool p_bFwdNoAnsw, bool p_bDND)
{
	HRESULT l_hResult = 0;

	// Call lineDevSpecific for each of the four divert settings
	int l_nLen = 3;
	char l_szBuffer[TAPI_LINE_BUFFER] = { 0 };

	// The first and last byte are the same for all four
	l_szBuffer[0] = 9;
	l_szBuffer[2] = 0; // NULL terminate

	// Forward All
	l_szBuffer[1] = (char)((p_bFwdAll) ? 0 : 1);
//	l_hResult = ::lineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
	l_hResult = this->HytLineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);

	// Forward Busy
	l_szBuffer[1] = (char)((p_bFwdBusy) ? 2 : 3);
//	l_hResult = ::lineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
	l_hResult = this->HytLineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);

	// Forward No Answer
	l_szBuffer[1] = (char)((p_bFwdNoAnsw) ? 4 : 5);
//	l_hResult = ::lineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
	l_hResult = this->HytLineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);

	// Do Not Disturb
	l_szBuffer[1] = (char)((p_bDND) ? 7 : 8);
//	l_hResult = ::lineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
	l_hResult = this->HytLineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);

	return l_hResult;
}

// ----------------------------------------------------------------------------
// Set the App Specific value for a call. See below for the order in which the
// call is selected.
HRESULT CTapiLine::SetAppSpecific(DWORD p_dwNum)
{
	HRESULT l_hResult = 0;

	HCALL l_hCall = NULL;
	if (m_hConnectedCall)
		l_hCall = m_hConnectedCall;
	else if (m_hWaitingCall)
		l_hCall = m_hWaitingCall;
	else if (m_hHeldCall)
		l_hCall = m_hHeldCall;
	else if (m_hPendingCall)
		l_hCall = m_hPendingCall;
	else if (m_hConferenceCall)
		l_hCall = m_hConferenceCall;
	else if (m_hConsultationCall)
		l_hCall = m_hConsultationCall;

	if (l_hCall)
	{
		l_hResult = ::lineSetAppSpecific(l_hCall, p_dwNum);
	}

	return l_hResult;
}
HRESULT CTapiLine::SetMsgWaitLamp(DWORD p_dwNum)
{
	HRESULT l_hResult = 0;

	char l_szBuffer[TAPI_LINE_BUFFER] = { 0 };

	l_szBuffer[0] = 9;
	l_szBuffer[1] = 73; // DisplayMsg
	strcpy(&(l_szBuffer[2]), ";Mailbox Msgs=");
	l_szBuffer[16] = p_dwNum + '0';
	l_szBuffer[17] = 0; // NULL terminate

	// Set Message Waiting Lamp
//	l_hResult = ::lineDevSpecific(m_hLine, 0, NULL, l_szBuffer, 18);
	l_hResult = this->HytLineDevSpecific(m_hLine, 0, NULL, l_szBuffer, 18);

	return l_hResult;
}

HRESULT CTapiLine::GetDivertSettings(std::string& l_strReadyType)
{
	char buffer[sizeof(LINEDEVSTATUS) + 512] = {0};
	LPLINEDEVSTATUS lpLineDevStatus = reinterpret_cast<LPLINEDEVSTATUS>(buffer);
	lpLineDevStatus->dwTotalSize = sizeof(LINEDEVSTATUS) + 512;
	HRESULT tr = ::lineGetLineDevStatus(m_hLine, lpLineDevStatus);
	if (tr != 0)
	{
		return tr;
	}

	// Set the pointer to the start of the dev specific data
	char* pDevSpecific = (char*)lpLineDevStatus + lpLineDevStatus->dwDevSpecificOffset;

	std::string phoneExt(pDevSpecific);
	pDevSpecific += phoneExt.size() + 27;

	std::string extension(pDevSpecific);
	pDevSpecific += extension.size() + 1;

	std::string locale(pDevSpecific);
	pDevSpecific += locale.size() + 1;

	std::string forward(pDevSpecific);
	pDevSpecific += forward.size() + 1;

	std::string follow(pDevSpecific);
	pDevSpecific += follow.size() + 1;

	std::string absent(pDevSpecific);
	pDevSpecific += absent.size() + 1;

	bool isDisturb = false;
	std::string disturb(pDevSpecific);
	if (disturb.empty())
	{
		pDevSpecific += 1;
	}
	else
	{
		isDisturb = true;
	}

	if (isDisturb)
	{
		std::string temp = disturb;
		while (true)
		{
			if (temp.empty())
			{
				pDevSpecific += 1;
				break;
			}
			else
			{
				pDevSpecific += temp.size() + 1;
				temp.assign(pDevSpecific);
			}
		}
	}

	std::string busy(pDevSpecific);
	pDevSpecific += busy.size() + 1;

	if ((0 != pDevSpecific[1])||(0 == pDevSpecific[3]))
	{
		l_strReadyType = "ready";
	}
	else
	{
		l_strReadyType = "notready";
	}

	return 0;
}

HRESULT CTapiLine::SetInGroup(const std::string& p_strGroup)
{
	HRESULT l_hResult = 0;

	char l_szBuffer[TAPI_LINE_BUFFER] = { 0 };
	l_szBuffer[0] = 9;
	l_szBuffer[1] = 76; // HuntGroupEnable

	std::size_t l_nLen = p_strGroup.length();
	strncpy_s((&(l_szBuffer[2])), TAPI_LINE_BUFFER - 3, p_strGroup.c_str(), l_nLen);
	l_nLen += 3;

	// Set In Group status
//	l_hResult = ::lineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
	l_hResult = this->HytLineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);

	return l_hResult;
}
HRESULT CTapiLine::SetOutGroup(const std::string& p_strGroup)
{
	HRESULT l_hResult = 0;

	char l_szBuffer[TAPI_LINE_BUFFER] = { 0 };
	l_szBuffer[0] = 9;
	l_szBuffer[1] = 77; // HuntGroupDisable

	std::size_t l_nLen = p_strGroup.length();
	strncpy_s((&(l_szBuffer[2])), TAPI_LINE_BUFFER - 3, p_strGroup.c_str(), l_nLen);
	l_nLen += 3;

	// Set In Group status
//	l_hResult = ::lineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
	l_hResult = this->HytLineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);

	return l_hResult;
}
HRESULT CTapiLine::SetCallData(const std::string& p_strData)
{
	HRESULT l_hResult = 0;

	HCALL l_hCall = NULL;
	if (m_hConnectedCall)
		l_hCall = m_hConnectedCall;
	else if (m_hWaitingCall)
		l_hCall = m_hWaitingCall;
	else if (m_hHeldCall)
		l_hCall = m_hHeldCall;
	else if (m_hPendingCall)
		l_hCall = m_hPendingCall;
	else if (m_hConferenceCall)
		l_hCall = m_hConferenceCall;
	else if (m_hConsultationCall)
		l_hCall = m_hConsultationCall;

	if (l_hCall)
	{
		l_hResult = ::lineSetCallData(l_hCall, (void*)p_strData.c_str(), p_strData.size());
	}

	return l_hResult;
}
HRESULT CTapiLine::Listen(const std::string& p_strAddress)
{
	HRESULT l_hResult = 0;

	if (!p_strAddress.empty())
	{
		char l_szBuffer[TAPI_LINE_BUFFER] = { 0 };
		l_szBuffer[0] = 9;
		l_szBuffer[1] = 100; // Listen

		std::size_t l_nLen = p_strAddress.length();
		strncpy_s(&(l_szBuffer[2]), TAPI_LINE_BUFFER - 3, p_strAddress.c_str(), l_nLen);
		l_nLen += 3;

	//	l_hResult = ::lineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
		l_hResult = this->HytLineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
	}

	return l_hResult;
}
HRESULT CTapiLine::Intrude(const std::string& p_strAddress)
{
	HRESULT l_hResult = 0;

	if (!p_strAddress.empty())
	{
		char l_szBuffer[TAPI_LINE_BUFFER] = { 0 };
		l_szBuffer[0] = 9;
		l_szBuffer[1] = 83; // Intrude

		std::size_t l_nLen = p_strAddress.length();
		strncpy_s(&(l_szBuffer[2]), TAPI_LINE_BUFFER - 3, p_strAddress.c_str(), l_nLen);
		l_nLen += 3;
		
	//	l_hResult = ::lineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
		l_hResult = this->HytLineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
	}

	return l_hResult;
}
HRESULT CTapiLine::SetAccountCode(const std::string& p_strAddress)
{
	HRESULT l_hResult = 0;

	if (!p_strAddress.empty())
	{
		char l_szBuffer[TAPI_LINE_BUFFER] = { 0 };
		l_szBuffer[0] = 9;
		l_szBuffer[1] = 101; // Set Account Code

		std::size_t l_nLen = p_strAddress.length();
		strncpy_s(&(l_szBuffer[2]), TAPI_LINE_BUFFER - 3, p_strAddress.c_str(), l_nLen);
		l_nLen += 3;

	//	l_hResult = ::lineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
		l_hResult = this->HytLineDevSpecific(m_hLine, 0, NULL, l_szBuffer, l_nLen);
	}

	return l_hResult;
}
HRESULT CTapiLine::GenerateDigits(const std::string& p_strAddress)
{
	HRESULT l_hResult = 0;

	if (!p_strAddress.empty())
	{
		l_hResult = ::lineGenerateDigitsA(m_hConnectedCall, LINEDIGITMODE_DTMF, p_strAddress.c_str(), 5000);
	}

	return l_hResult;
}

