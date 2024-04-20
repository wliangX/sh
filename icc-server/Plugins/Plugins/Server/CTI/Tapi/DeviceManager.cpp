#include "Boost.h"

#include "DeviceManager.h"
#include "SysConfig.h"
#include "TaskManager.h"
#include "AvayaResult.h"
#include "AvayaEvent.h"
#include <Protocol/CSeatInfo.h>
#define SLEEP_TIMESPAN				300			//	单位：微秒

//////////////////////////////////////////////////////////////////////////
static void CALLBACK TapiLineCallback(
	DWORD   dwDevice,
	DWORD   nMsg,
	DWORD   dwInstance,
	DWORD   dwParam1,
	DWORD   dwParam2,
	DWORD   dwParam3)
{
	switch (nMsg)
	{
	case LINE_CREATE:
		// We have been asked to create a line.  We can't do this, the number
		// of lines is dictated by the IP Office Telephony Service Provider, so
		// we'll ignore this request.
		break;
	case LINE_REQUEST:
		// We have received an assisted telephony request.  This application
		// doesn't handle assisted telephony requests, so we'll ignore it.
		break;
	default:
		// We have received an event relevant to an existing line
		CAvayaEvent::Instance()->OnEvent(dwDevice, nMsg, dwParam1, dwParam2, dwParam3);
		break;
	}
}

//////////////////////////////////////////////////////////////////////
boost::shared_ptr<CDeviceManager> CDeviceManager::m_pInstance = nullptr;
boost::shared_ptr<CDeviceManager> CDeviceManager::Instance()
{
	if(m_pInstance == nullptr)
		m_pInstance = boost::make_shared<CDeviceManager>();

	return m_pInstance;
}
void CDeviceManager::ExitInstance()
{
	if (m_pApiVersions != NULL)
	{
		delete[] m_pApiVersions;
		m_pApiVersions = NULL;
	}

	this->DeleteAllDevice();
}

CDeviceManager::CDeviceManager()
{
	m_hLineApp = NULL;		// Application handle returned from TAPI
	m_pApiVersions = NULL; // API versions supported by each device
	m_dwDevicesCount = 0;

	m_pLog = nullptr;
	m_pDateTime = nullptr;
	m_pStrUtil = nullptr;

	m_pRedisClient = nullptr;
	m_bDeviceIsLoad = false;
}

CDeviceManager::~CDeviceManager()
{
	//
}

//////////////////////////////////////////////////////////////////////////
void CDeviceManager::DeleteAllDevice()
{
	m_bDeviceIsLoad = false;

	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	m_deviceQueue.clear();
}

void CDeviceManager::LoadACDDevice()
{
	std::list<CACDGroup> l_ACDGroupList = CSysConfig::Instance()->m_ACDGroupList;

	for each (CACDGroup l_acdGrpObj in l_ACDGroupList)
	{
		std::string l_strACDNum = l_acdGrpObj.m_strACDNum;
		std::string l_strACDAlias = l_acdGrpObj.m_strACDAlias;
		if (!l_strACDNum.empty())
		{
			boost::shared_ptr<CTapiLine> pDevice = boost::make_shared<CTapiLine>(l_strACDNum, CTC_TYPE_ACDGROUP, "", "");
			pDevice->SetLineHandle(atoi(l_strACDNum.c_str()));
			pDevice->SetLogPtr(m_pLog);
			m_deviceQueue.push_back(pDevice);
			ICC_LOG_INFO(m_pLog, "LoadACDDevice ACDNum: [%s]", l_strACDNum.c_str());

			if (!l_strACDAlias.empty() && l_strACDNum.compare(l_strACDAlias) != 0)
			{
				boost::shared_ptr<CTapiLine> pAliasDevice = boost::make_shared<CTapiLine>(l_strACDAlias, CTC_TYPE_ACDGROUP, "", "");
				pAliasDevice->SetLogPtr(m_pLog);
				m_deviceQueue.push_back(pAliasDevice);
				ICC_LOG_INFO(m_pLog, "LoadACDDevice ACDAliasNum: [%s]", l_strACDAlias.c_str());
			}
		}
	}
}
void CDeviceManager::LoadPhoneDevice()
{
	std::list<CDevicePhone>	l_phoneList = CSysConfig::Instance()->m_DevicePhoneList;

	for each (CDevicePhone l_phoneObj in l_phoneList)
	{
		std::string l_strDeviceNum = l_phoneObj.m_strDeviceNum;
		std::string l_strDevicetype = l_phoneObj.m_strDeviceType;
		std::string l_strAgent = l_phoneObj.m_strAgent;
		std::string l_strPsw = l_phoneObj.m_strPsw;

		int l_iType = CTC_TYPE_UNKNOW;
		if (l_strDevicetype.compare(DEVICE_TYPE_DIGITAL) == 0)
			l_iType = CTC_TYPE_DIGITAL;
		else if (l_strDevicetype.compare(DEVICE_TYPE_IPPHONE) == 0)
			l_iType = CTC_TYPE_IPPHONE;
		else if (l_strDevicetype.compare(DEVICE_TYPE_ANALOG) == 0)
			l_iType = CTC_TYPE_ANALOG;
		else
			l_iType = CTC_TYPE_UNKNOW;

		boost::shared_ptr<CTapiLine> l_pDevice = boost::make_shared<CTapiLine>(l_strDeviceNum, l_iType, l_strAgent, l_strPsw);
		l_pDevice->SetIsAgent(true);
		l_pDevice->SetLineHandle(atoi(l_strDeviceNum.c_str()));
		//l_pDevice->setl
		l_pDevice->SetLogPtr(m_pLog);
		m_deviceQueue.push_back(l_pDevice);

		ICC_LOG_INFO(m_pLog, "LoadPhoneDevice DeviceNum: [%s], DeviceType: [%s]", 
			l_strDeviceNum.c_str(), l_strDevicetype.c_str());
	}
}
void CDeviceManager::LoadVirDn()
{
	std::list<CDeviceVir>	l_virList = CSysConfig::Instance()->m_DeviceVirList;

	for each (CDeviceVir l_virObj in l_virList)
	{
		std::string l_strDeviceNum = l_virObj.m_strDeviceNum;
		std::string l_strDevicetype = l_virObj.m_strDeviceType;

		int l_iType = CTC_TYPE_VIRDN;
		boost::shared_ptr<CTapiLine> pDevice = boost::make_shared<CTapiLine>(l_strDeviceNum, l_iType, "", "");

		pDevice->SetLogPtr(m_pLog);
		m_deviceQueue.push_back(pDevice);

		ICC_LOG_INFO(m_pLog, "LoadVirDn DeviceNum: [%s], DeviceType: [%s]",
			l_strDeviceNum.c_str(), l_strDevicetype.c_str());
	}
}
void CDeviceManager::LoadAllDevice()
{
	if(!m_bDeviceIsLoad)
	{
		m_bDeviceIsLoad = true;

		//装载ACD
		LoadACDDevice();
		//装载Phone
		LoadPhoneDevice();
	}
}

bool CDeviceManager::DeviceIsFree(const std::string& p_strDeviceNum)
{
	bool l_bRet = false;

	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pDevice in m_deviceQueue)
	{
		if (l_pDevice && l_pDevice->GetDeviceNum().compare(p_strDeviceNum) == 0)
		{
			long l_lCSTACallRefId = DEFAULT_CALLREFID;
			std::string l_strState = l_pDevice->GetDeviceState(l_lCSTACallRefId);

			if (l_strState.compare(CallStateString[STATE_FREE]) == 0)
			{
				l_bRet = true;

				break;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "LogicalDevice: [%s] Is Busy, LogicCall CSTACallRefId: [%u], DeviceState: [%s]",
					p_strDeviceNum.c_str(), l_lCSTACallRefId, l_strState.c_str());
			}
		}
	}

	return l_bRet;
}
std::string CDeviceManager::GetDeviceType(const std::string& p_strDeviceNum)
{
	std::string l_strDeviceType = DEVICE_TYPE_UNKNOW;

	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pDevice in m_deviceQueue)
	{
		if (l_pDevice)
		{
			if (l_pDevice->GetDeviceNum().compare(p_strDeviceNum) == 0)
			{
				l_strDeviceType = l_pDevice->GetDeviceTypeString();

				break;
			}
		}
	}

	return l_strDeviceType;
}

std::string CDeviceManager::GetDeviceState(const std::string& p_strDeviceNum)
{
	std::string l_strDeviceState = "";

	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pDevice in m_deviceQueue)
	{
		if (l_pDevice)
		{
			long l_lCSTACallRefId = DEFAULT_CALLREFID;
			if (l_pDevice->GetDeviceNum().compare(p_strDeviceNum) == 0)
			{
				l_strDeviceState = l_pDevice->GetDeviceState(l_lCSTACallRefId);

				break;
			}
		}
	}

	return l_strDeviceState;
}

std::string CDeviceManager::GetDnByLineHandle(HLINE p_dwLineId)
{
	std::string l_strDeviceNum = "";

	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pBaseDevice in m_deviceQueue)
	{
		boost::shared_ptr<CTapiLine> l_pTempDevice = boost::dynamic_pointer_cast<CTapiLine>(l_pBaseDevice);
		if (l_pTempDevice && l_pTempDevice->GetLineHandle() == p_dwLineId)
		{
			l_strDeviceNum = l_pTempDevice->GetDeviceNum();

			break;
		}
	}

	return l_strDeviceNum;
}
boost::shared_ptr<CDevice> CDeviceManager::FindDeviceByDn(const std::string& p_strDeviceNum)
{	
	boost::shared_ptr<CDevice> l_pDevice = nullptr;

	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pTempDevice in m_deviceQueue)
	{
		if (l_pTempDevice)
		{
			if (l_pTempDevice->GetDeviceNum().compare(p_strDeviceNum) == 0)
			{
				l_pDevice = l_pTempDevice;

				break;
			}
		}
	}
	
	return l_pDevice;
}

boost::shared_ptr<CDevice> CDeviceManager::FindDeviceByMonitorId(long p_lMonitorId)
{
	boost::shared_ptr<CDevice> l_pDevice = nullptr;

	/*std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (std::shared_ptr<CDevice> l_pTempDevice in m_deviceQueue)
	{
		if (l_pTempDevice)
		{
			if (l_pTempDevice->GetMonitorId() == p_lMonitorId)
			{
				l_pDevice = l_pTempDevice;

				break;
			}
		}
	}*/

	return l_pDevice;
}

boost::shared_ptr<CDevice> CDeviceManager::FindACDDevice(const std::string& p_strDevice)
{
	boost::shared_ptr<CDevice> l_pDevice = nullptr;

	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pTempDevice in m_deviceQueue)
	{
		if (l_pTempDevice && l_pTempDevice->GetDeviceNum().compare(p_strDevice) == 0)
		{
			l_pDevice = l_pTempDevice;

			break;
		}
	}

	return l_pDevice;
}
boost::shared_ptr<CTapiLine> CDeviceManager::FindLineByDn(const std::string& p_strDeviceNum)
{
	boost::shared_ptr<CTapiLine> l_pDevice = nullptr;

	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pBaseDevice in m_deviceQueue)
	{
		boost::shared_ptr<CTapiLine> l_pTempDevice = boost::dynamic_pointer_cast<CTapiLine>(l_pBaseDevice);
		if (l_pTempDevice && l_pTempDevice->GetDeviceNum().compare(p_strDeviceNum) == 0)
		{
			l_pDevice = l_pTempDevice;

			break;
		}
	}

	return l_pDevice;
}
boost::shared_ptr<CTapiLine> CDeviceManager::FindLineByHandle(HLINE p_dwLineId)
{
	boost::shared_ptr<CTapiLine> l_pDevice = nullptr;

	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pBaseDevice in m_deviceQueue)
	{
		boost::shared_ptr<CTapiLine> l_pTempDevice = boost::dynamic_pointer_cast<CTapiLine>(l_pBaseDevice);
		if (l_pTempDevice && l_pTempDevice->GetLineHandle() == p_dwLineId)
		{
			l_pDevice = l_pTempDevice;

			break;
		}
	}

	return l_pDevice;
}

IGetDeviceListResultNotifPtr CDeviceManager::GetDeviceList(long p_lRequestId)
{
	IGetDeviceListResultNotifPtr l_pResultNotif = boost::make_shared<CGetDeviceListResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
		for each (auto l_pDevice in m_deviceQueue)
		{
			if (l_pDevice && (l_pDevice->GetDeviceType() == CTC_TYPE_IPPHONE || l_pDevice->GetDeviceType() == CTC_TYPE_DIGITAL || l_pDevice->GetDeviceType() == CTC_TYPE_ANALOG))
			{
				boost::shared_ptr<CDeviceStateNotif> l_pDeviceNotif = boost::make_shared<CDeviceStateNotif>();
				if (l_pDeviceNotif)
				{
					std::string l_strDeviceState = CallStateString[STATE_FREE];
					std::string l_strStateTime = m_pDateTime->CurrentDateTimeStr();

					long l_lCSTACallRefId = DEFAULT_CALLREFID;
					bool Flag = l_pDevice->GetCurrentLogicalCall(l_lCSTACallRefId, l_strDeviceState, l_strStateTime);
					l_pDeviceNotif->SetCSTACallRefId(l_lCSTACallRefId);
					l_pDeviceNotif->SetDeviceNum(l_pDevice->GetDeviceNum());
					l_pDeviceNotif->SetDeviceType(l_pDevice->GetDeviceTypeString());
					l_pDeviceNotif->SetDeviceState(l_strDeviceState);
					l_pDeviceNotif->SetStateTime(l_strStateTime);

					ICC_LOG_DEBUG(m_pLog, "[%s]GetDeviceList->GetCurrentLogicalCall,l_lCSTACallRefId:[%d],state:[%s]",l_pDevice->GetDeviceNum().c_str(), l_lCSTACallRefId, l_strDeviceState.c_str());
					l_pResultNotif->m_DeviceList.push_back(l_pDeviceNotif);
				}
			}
		}
	}

	return l_pResultNotif;
}

void CDeviceManager::AddLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum, const std::string& p_strDeviceState)
{
	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pDevice in m_deviceQueue)
	{
		if (l_pDevice && p_strDeviceNum.compare(l_pDevice->GetDeviceNum()) == 0)
		{
			l_pDevice->AddLogicalCall(p_lCSTACallRefId, p_strDeviceState, m_pDateTime->CurrentDateTimeStr());

			ICC_LOG_DEBUG(m_pLog, "LogicalDevice: [%s], Add Logical Call State, CSTACallRefId: [%u], DeviceState: [%s]",
				 p_strDeviceState.c_str(), p_lCSTACallRefId, p_strDeviceNum.c_str());

			break;
		}
	}
}

void CDeviceManager::GetLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum, std::string& p_strDeviceState)
{
	p_strDeviceState = CallStateString[STATE_FREE];

	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pDevice in m_deviceQueue)
	{
		if (l_pDevice && p_strDeviceNum.compare(l_pDevice->GetDeviceNum()) == 0)
		{
			p_strDeviceState = l_pDevice->GetDeviceState(p_lCSTACallRefId);

			ICC_LOG_DEBUG(m_pLog, "GetLogicalCallState Device: [%s], CSTACallRefId: [%u], DeviceState: [%s]",
				p_strDeviceNum.c_str(), p_lCSTACallRefId, p_strDeviceState.c_str());
			break;
		}
	}
}

/*
void CDeviceManager::UpdateLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum, const std::string& p_strDeviceState)
{
	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pDevice in m_deviceQueue)
	{
		if (l_pDevice && p_strDeviceNum.compare(l_pDevice->GetDeviceNum()) == 0)
		{
			l_pDevice->SetDeviceState(p_lCSTACallRefId, p_strDeviceState, m_pDateTime->CurrentDateTimeStr());

			ICC_LOG_DEBUG(m_pLog, "LogicalDevice: [%s], Update Logical Call, CSTACallRefId: [%u], DeviceState: [%s]",
				p_strDeviceNum.c_str(), p_lCSTACallRefId, p_strDeviceState.c_str());

			break;
		}
	}
}*/

int CDeviceManager::UpdateLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum, const std::string& p_strDeviceState)
{
	int nRes = -1;
	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
		if (l_pDevice)
		{
			nRes = 0;
			if (p_strDeviceNum.compare(l_pDevice->GetDeviceNum()) == 0)
			{
				std::string l_lastState = l_pDevice->GetDeviceState(p_lCSTACallRefId);
				if (l_lastState.compare(p_strDeviceState) == 0)
				{
					nRes = 1;
				}
				else
				{
					nRes = 2;
				}
				l_pDevice->SetDeviceState(p_lCSTACallRefId, p_strDeviceState, m_pDateTime->CurrentDateTimeStr());

				ICC_LOG_DEBUG(m_pLog, "Update Logical Call, CSTACallRefId: [%u], LogicalDevice: [%s], DeviceState: [%s],lastState:[%s]",
					p_lCSTACallRefId, p_strDeviceNum.c_str(), p_strDeviceState.c_str(), l_lastState.c_str());

				break;
			}
		}
	}
	return nRes;
}

void CDeviceManager::CopyLogicalCallState(long p_lDestCSTACallRefId, long p_lSrcCSTACallRefId)
{
	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pDevice in m_deviceQueue)
	{
		if (l_pDevice && l_pDevice->CopyLogicalCallState(p_lDestCSTACallRefId, p_lSrcCSTACallRefId))
		{
			ICC_LOG_DEBUG(m_pLog, "LogicalDevice: [%s],, Copy Logical Call State, DestCSTACallRefId: [%u], SrcCSTACallRefId: [%u]",
				l_pDevice->GetDeviceNum().c_str(), p_lDestCSTACallRefId, p_lSrcCSTACallRefId);
		}
	}
}
void CDeviceManager::DeleteAllLogicalCallState(long p_lCSTACallRefId)
{
	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pDevice in m_deviceQueue)
	{
		if (l_pDevice && l_pDevice->DeleteLogicalCall(p_lCSTACallRefId))
		{
			ICC_LOG_DEBUG(m_pLog, "LogicalDevice: [%s], Delete All Logical Call State, CSTACallRefId: [%u]",
				l_pDevice->GetDeviceNum().c_str(), p_lCSTACallRefId);
		}
	}
}
void CDeviceManager::DeleteLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum)
{
	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pDevice in m_deviceQueue)
	{
		if (l_pDevice && p_strDeviceNum.compare(l_pDevice->GetDeviceNum()) == 0)
		{
			if (l_pDevice->DeleteLogicalCall(p_lCSTACallRefId))
			{
				ICC_LOG_DEBUG(m_pLog, "LogicalDevice: [%s], Delete Logical Call State, CSTACallRefId: [%u]",
					p_strDeviceNum.c_str(), p_lCSTACallRefId);
			}

			break;
		}
	}
}
std::string CDeviceManager::GetLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum)
{
	std::string l_strDeviceState = "";

	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pDevice in m_deviceQueue)
	{
		if (l_pDevice && p_strDeviceNum.compare(l_pDevice->GetDeviceNum()) == 0)
		{
			l_strDeviceState = l_pDevice->GetLogicalCallState(p_lCSTACallRefId);

			break;
		}
	}

	return l_strDeviceState;
}
bool CDeviceManager::FindLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum)
{
	bool l_bFind = false;

	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pDevice in m_deviceQueue)
	{
		if (l_pDevice && p_strDeviceNum.compare(l_pDevice->GetDeviceNum()) == 0)
		{
			l_bFind = l_pDevice->FindLogicalCall(p_lCSTACallRefId);

			break;
		}
	}

	return l_bFind;
}
bool CDeviceManager::FindLogicalCallExcludeDevice(long p_lCSTACallRefId, const std::string& p_strDeviceNum)
{
	bool l_bFind = false;

	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pDevice in m_deviceQueue)
	{
		if (l_pDevice)
		{
			std::string l_strDeviceNum = l_pDevice->GetDeviceNum();
			if (l_strDeviceNum.compare(p_strDeviceNum) != 0 && l_pDevice->FindLogicalCall(p_lCSTACallRefId))
			{
				l_bFind = true;

				break;
			}
		}
	}

	return l_bFind;
}
bool CDeviceManager::FindLogicalCall(long p_lCSTACallRefId)
{
	bool l_bFind = false;

	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pDevice in m_deviceQueue)
	{
		if (l_pDevice && l_pDevice->FindLogicalCall(p_lCSTACallRefId))
		{
			l_bFind = true;

			break;
		}
	}

	return l_bFind;
}

std::string CDeviceManager::GetDeptCodeByDeviceNum(const std::string& p_strDeviceNum)
{
	std::string l_strSeatInfo;
	std::string l_strDeptCode = "";
	if (m_pRedisClient->HGet("SeatInfo", p_strDeviceNum, l_strSeatInfo))
	{
		PROTOCOL::CSeatInfo cSeatInfo;
		if (cSeatInfo.Parse(l_strSeatInfo, m_pJsonFty->CreateJson()))
		{
			l_strDeptCode = cSeatInfo.m_oBody.m_strDeptCode;
		}
	}
	return l_strDeptCode;
}

bool CDeviceManager::FindOtherLogicalCall(long p_lCSTACallRefId, const std::string& p_strDeviceNum)
{
	bool l_bFind = false;

	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pDevice in m_deviceQueue)
	{
		if (l_pDevice->GetDeviceNum() == p_strDeviceNum)
		{
			continue;
		}

		if (l_pDevice && l_pDevice->FindLogicalCall(p_lCSTACallRefId))
		{
			l_bFind = true;

			break;
		}
	}

	return l_bFind;
}

void CDeviceManager::GetDeviceCall(const std::string& p_strDeviceNum, std::vector<long>& CallIDVec)
{
	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pDevice in m_deviceQueue)
	{
		if (l_pDevice->GetDeviceNum() == p_strDeviceNum)
		{
			l_pDevice->GetAllCSTACallRefId(CallIDVec);
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
// Initialise TAPI
bool CDeviceManager::InitTAPI()
{
	bool l_bRet = false;

	const DWORD TAPI_LOW_VERSION = 0x00020000;				 // TAPI v2.0
	const DWORD TAPI_HIGH_VERSION = TAPI_CURRENT_VERSION;
	HLINEAPP    l_hLineApp = 0;
	DWORD   l_dwAPIVersion = TAPI_HIGH_VERSION;
	LINEINITIALIZEEXPARAMS  l_pParams = { sizeof(LINEINITIALIZEEXPARAMS) };
	l_pParams.dwOptions = LINEINITIALIZEEXOPTION_USEHIDDENWINDOW;
//	LPSTR l_pszAppName = ("TAPIClient");

	HINSTANCE l_hInstance = ::GetModuleHandleA(NULL);
	if (l_hInstance == NULL)
	{
		// TODO: change error code to suit your needs
		ICC_LOG_ERROR(m_pLog, "GetModuleHandle Error, ErrorCode: [%u]", ::GetLastError());

		return false;
	}

	HRESULT l_hResult = ::lineInitializeExA(&l_hLineApp,
		l_hInstance,
		TapiLineCallback,
		"TAPIClient"/*l_pszAppName*/,
		&m_dwDevicesCount,
		&l_dwAPIVersion,
		&l_pParams);
	if (S_OK == l_hResult)
	{
		// Save the handle
		m_hLineApp = l_hLineApp;

		// Negotiate the API versions
		// (Necessary to get proper notifications)
		std::string l_strInfo = m_pStrUtil->Format("Get number of devices: [%u], LineApp: [%u]", m_dwDevicesCount, m_hLineApp);
		ICC_LOG_DEBUG(m_pLog, "%s", l_strInfo.c_str());
		ShowMsg(l_strInfo);

		if (m_dwDevicesCount > 0)
		{
			l_bRet = true;

			m_pApiVersions = new DWORD[m_dwDevicesCount];
			if (m_pApiVersions)
			{
				LINEEXTENSIONID l_extensionId;
				for (DWORD dwLineID = 0; dwLineID < m_dwDevicesCount; dwLineID++)
				{
					if (S_OK != ::lineNegotiateAPIVersion(m_hLineApp,
						dwLineID,
						TAPI_LOW_VERSION,
						TAPI_HIGH_VERSION,
						&m_pApiVersions[dwLineID],
						&l_extensionId))
					{
						m_pApiVersions[dwLineID] = 0;
					}
				}
			}
		}
	}

	return l_bRet;
}
void CDeviceManager::TestOpenAllAddresses()
{
	this->DeleteAllDevice();

	LoadACDDevice();
	LoadPhoneDevice();

	int nSize = m_deviceQueue.size();
	/*
	std::list<CDevicePhone> tmpPhoneList = CSysConfig::Instance()->m_DevicePhoneList;
	for each (auto l_deviceObj in tmpPhoneList)
	{
		boost::shared_ptr<CTapiLine> l_pTapiLine = boost::make_shared<CTapiLine>();
		if (l_pTapiLine)
		{
			std::string l_strDeviceNum = l_deviceObj.m_strDeviceNum;
			l_pTapiLine->SetDeviceNum(l_strDeviceNum);
			l_pTapiLine->SetExtension(atol(l_strDeviceNum.c_str()));
			l_pTapiLine->SetLineConnectState(true);

			l_pTapiLine->SetDeviceType(l_deviceObj.m_strDeviceType);


		}
	}*/
}

bool CDeviceManager::OpenAllAddresses()
{
	this->DeleteAllDevice();

	int l_nOpenedLines = 0;
	for (DWORD dwLineID = 0; dwLineID < m_dwDevicesCount; dwLineID++)
	{
		LINEDEVCAPS *l_pDevCaps = NULL;
		HRESULT l_hResult = LoopLineGetDevCaps(m_hLineApp, dwLineID, m_pApiVersions[dwLineID], 0, l_pDevCaps);

		// This is how you are supposed to select TAPI lines, 
		// i.e. according to the line's capabilities
		if ((S_OK == l_hResult) &&
			(l_pDevCaps->dwBearerModes & LINEBEARERMODE_VOICE) &&
			(l_pDevCaps->dwMediaModes & LINEMEDIAMODE_INTERACTIVEVOICE) &&
			(l_pDevCaps->dwLineFeatures & LINEFEATURE_MAKECALL))
		{
			// Retrieve the name of the TAPI service provider
			std::string l_strProviderName = "";
			GetTapiString(l_strProviderName, l_pDevCaps, l_pDevCaps->dwProviderInfoSize,
				l_pDevCaps->dwProviderInfoOffset);

			ICC_LOG_DEBUG(m_pLog, "API version: [%X], API Provider Name: [%s]", m_pApiVersions[dwLineID], l_strProviderName.c_str());

			// Skip TSPs that we are not interested in by looking for the
			// Provider name of the IP Office TAPI driver
			if (l_strProviderName.compare("Avaya") != 0)
			{
				delete[] l_pDevCaps;
				l_pDevCaps = NULL;

				continue;
			}
				

			std::string l_strLineName = "";
			GetTapiString(l_strLineName, l_pDevCaps, l_pDevCaps->dwLineNameSize, l_pDevCaps->dwLineNameOffset);
			ICC_LOG_DEBUG(m_pLog, "Get Line Name: [%s]", l_strLineName.c_str());

		//	std::string l_strDeviceNum = ReverFindString(l_strLineName, ":");
			boost::shared_ptr<CTapiLine> l_pTapiLine = boost::make_shared<CTapiLine>()/*this->FindLineByDn(l_strDeviceNum)*/;
			if (l_pTapiLine)
			{
				// Instantiate a TapiLine class for every line that we open
				bool l_bRet = l_pTapiLine->Open(m_hLineApp, dwLineID, m_pApiVersions[dwLineID],
					LINECALLPRIVILEGE_OWNER,
					LINEMEDIAMODE_INTERACTIVEVOICE /*| LINEMEDIAMODE_UNKNOWN*/);
				if (l_bRet)
				{
					l_nOpenedLines++;

					LINEADDRESSSTATUS* l_pAddressStatus = NULL;
					LoopLineGetAddressStatus(l_pTapiLine->GetLineHandle(), 0, l_pAddressStatus);
					delete[] l_pAddressStatus;
					l_pAddressStatus = NULL;

					std::string l_strDeviceNum = "";
					// Find out about each address
					for (DWORD dwAddressID = 0; dwAddressID < l_pDevCaps->dwNumAddresses; dwAddressID++)
					{
						// Find out its capabilities
						LINEADDRESSCAPS *l_pAddressCaps = NULL;
						l_hResult = LoopLineGetAddressCaps(m_hLineApp, dwLineID, dwAddressID, m_pApiVersions[dwLineID], 0, l_pAddressCaps);

						// Find and use the name of the extension we control
						std::string l_strAddress = "";
						GetTapiString(l_strAddress, l_pAddressCaps, l_pAddressCaps->dwAddressSize, l_pAddressCaps->dwAddressOffset);
						ICC_LOG_DEBUG(m_pLog, "Get Line: [%u] Extn ID: [%s]", dwLineID, l_strAddress.c_str());

						l_strDeviceNum = l_strAddress;
						l_pTapiLine->SetDeviceNum(l_strDeviceNum);
						l_pTapiLine->SetExtension(atol(l_strAddress.c_str()));
						l_pTapiLine->SetLineConnectState(true);

						l_pTapiLine->SetLogPtr(m_pLog);
						std::string l_strDeviceType = DEVICE_TYPE_UNKNOW;
						if (CSysConfig::Instance()->GetACDGrpTypeByDn(l_strDeviceNum, l_strDeviceType) || CSysConfig::Instance()->GetDeviceTypeByDn(l_strDeviceNum, l_strDeviceType))
						{
							l_pTapiLine->SetDeviceType(l_strDeviceType);
						}

						ICC_LOG_DEBUG(m_pLog, "OpenAllAddresses: DeviceNum:[%s] DeviceType:[%s]", l_strDeviceNum.c_str(), l_strDeviceType.c_str());

						delete[] l_pAddressCaps;
						l_pAddressCaps = NULL;
					}

					// Tell the user about the capabilities of this line
					std::string l_strDeviceStatus = "";
					std::string l_strInfo = m_pStrUtil->Format("Line [%s] Open Success. Supported Line States : [%s]",
						l_strDeviceNum.c_str(),
						CAvayaResult::Instance()->DescribeDeviceStatus(l_strDeviceStatus, l_pDevCaps->dwLineStates).c_str());
					ICC_LOG_DEBUG(m_pLog, "%s", l_strInfo.c_str());
					ShowMsg(l_strInfo);

					m_deviceQueue.push_back(l_pTapiLine);
				}
				else
				{
					ICC_LOG_WARNING(m_pLog, "Open Line [%d] failed !!!", dwLineID);
				}
			}
			else
			{
				ICC_LOG_WARNING(m_pLog, "Create CTapiLine Object Failed !!!");
			}
		}

		delete[] l_pDevCaps; 
		l_pDevCaps = NULL;
	}

	std::string l_strInfo = m_pStrUtil->Format("Open Lines Count [%d]", l_nOpenedLines);
	ICC_LOG_DEBUG(m_pLog, "%s", l_strInfo.c_str());
	ShowMsg(l_strInfo);

	return (l_nOpenedLines > 0 ? true : false);
}
// ----------------------------------------------------------------------------
// Shutdown our channel to TAPI
void CDeviceManager::ShutdownTAPI()
{
	if (m_hLineApp)
	{
		HRESULT l_lRet = ::lineShutdown(m_hLineApp);
		ICC_LOG_DEBUG(m_pLog, "Shutdown TAPI, LineApp:%u, Ret:%u", m_hLineApp, l_lRet);

		m_hLineApp = NULL;
	}
}
void CDeviceManager::CloseAllAddresses()
{
	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (auto l_pDeviceObj in m_deviceQueue)
	{
		boost::shared_ptr<CTapiLine> l_pTapiLine = boost::dynamic_pointer_cast<CTapiLine>(l_pDeviceObj);
		if (l_pTapiLine)
		{
			bool l_bRet = l_pTapiLine->Close();

			ICC_LOG_DEBUG(m_pLog, "Close TapiLine [%s] [%s]", l_pTapiLine->GetDeviceNum().c_str(), l_bRet ? "Success" : "Failed");
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
// Get a string from a TAPI structure
bool CDeviceManager::GetTapiString(std::string& p_strResult, void *p_pObj, DWORD p_dwSize, DWORD p_dwOffset)
{
	bool l_bRet = false;
	p_strResult = "";

	if (p_dwSize > 0)
	{
		char *l_pBuffer = new char[p_dwSize + 1];
		if (l_pBuffer)
		{
			l_bRet = true;

			memset(l_pBuffer, 0, p_dwSize + 1);
			memcpy(l_pBuffer, &((BYTE*)p_pObj)[p_dwOffset], p_dwSize);

			p_strResult = std::string(l_pBuffer);

			delete[] l_pBuffer;
			l_pBuffer = NULL;
		}
	}

	return l_bRet;
}

// ----------------------------------------------------------------------------
// Get device capabilities information from TAPI
// Note: The calling function must delete the info structure later on!
HRESULT CDeviceManager::LoopLineGetDevCaps(HLINEAPP p_hLineApp, DWORD p_dwDeviceID, DWORD p_dwAPIVersion, DWORD p_dwExtVersion, LINEDEVCAPS*& p_pLineDevCaps)
{
	std::size_t l_stCurrentSize = MAX_LINE_DEV_CAP_SIZE; // Starting value - usually big enough
	HRESULT l_hResult = -1;

	for (;;)
	{
		// Allocate some memory for the call
		p_pLineDevCaps = (LINEDEVCAPS *) new BYTE[l_stCurrentSize];
		ZeroMemory(&p_pLineDevCaps[0], l_stCurrentSize);
		p_pLineDevCaps->dwTotalSize = l_stCurrentSize;

		// Ask TAPI for some information
		l_hResult = ::lineGetDevCapsA(p_hLineApp, p_dwDeviceID, p_dwAPIVersion, p_dwExtVersion, p_pLineDevCaps);

		// Cope with variable length structures
		if (l_hResult == LINEERR_STRUCTURETOOSMALL)
		{
			if (p_pLineDevCaps->dwNeededSize <= 0)
				break;
			l_stCurrentSize = p_pLineDevCaps->dwNeededSize;

			delete[] p_pLineDevCaps;
			p_pLineDevCaps = NULL;
		}
		else
			break;
	}

	return l_hResult;
}

// ----------------------------------------------------------------------------
// Get call information from TAPI
// Note: The calling function must delete the info structure later on!
HRESULT CDeviceManager::LoopLineGetCallInfo(HCALL p_hCall, LINECALLINFO*& p_pCallInfo)
{
	std::size_t l_stCurrentSize = MAX_LINE_DEV_CAP_SIZE; // Starting value - usually big enough
	HRESULT l_hResult = S_OK;

	for (;;)
	{
		// Allocate some memory for the call
		p_pCallInfo = (LINECALLINFO *) new BYTE[l_stCurrentSize];
		ZeroMemory(&p_pCallInfo[0], l_stCurrentSize);
		p_pCallInfo->dwTotalSize = l_stCurrentSize;

		// Ask TAPI for some information
		l_hResult = ::lineGetCallInfoA(p_hCall, p_pCallInfo);

		// Cope with variable length structures
		if (l_hResult == LINEERR_STRUCTURETOOSMALL)
		{
			if (p_pCallInfo->dwNeededSize <= 0)
				break;
			l_stCurrentSize = p_pCallInfo->dwNeededSize;

			delete[] p_pCallInfo;
			p_pCallInfo = NULL;
		}
		else
			break;
	}

	return l_hResult;
}


// ----------------------------------------------------------------------------
// Get identification information from TAPI
HRESULT CDeviceManager::LoopLineGetID(DWORD& p_dwResult, HLINE p_hLine, DWORD p_dwAddressID, HCALL p_hCall, DWORD p_dwSelect, LPCSTR p_pszDeviceClass)
{
	VARSTRING* l_pVarString = NULL;
	p_dwResult = 0xffffffff;
	std::size_t l_stCurrentSize = MAX_LINE_DEV_CAP_SIZE; // Starting value - usually big enough
	HRESULT l_hResult = S_OK;

	for (;;)
	{
		// Allocate some memory for the call
		l_pVarString = (VARSTRING *) new BYTE[l_stCurrentSize];
		ZeroMemory(&l_pVarString[0], l_stCurrentSize);
		l_pVarString->dwTotalSize = l_stCurrentSize;

		// Ask TAPI for some information
		l_hResult = ::lineGetIDA(p_hLine, p_dwAddressID, p_hCall, p_dwSelect, l_pVarString, p_pszDeviceClass);

		// Cope with variable length structures
		if (l_hResult == LINEERR_STRUCTURETOOSMALL)
		{
			if (l_pVarString->dwNeededSize <= 0)
				break;
			l_stCurrentSize = l_pVarString->dwNeededSize;

			delete[] l_pVarString;
			l_pVarString = NULL;
		}
		else
			break;
	}
	if (l_hResult == S_OK)
		p_dwResult = ((DWORD *)(((BYTE *)l_pVarString) + l_pVarString->dwStringOffset))[0];

	delete[] l_pVarString;
	l_pVarString = NULL;

	return l_hResult;
}

// ----------------------------------------------------------------------------
// Get address capabilities information from TAPI
HRESULT CDeviceManager::LoopLineGetAddressCaps(HLINEAPP p_hLineApp, DWORD p_dwDeviceID, DWORD p_dwAddressID, DWORD p_dwAPIVersion, DWORD p_dwExtVersion, LINEADDRESSCAPS*& p_pAddressCaps)
{
	std::size_t l_stCurrentSize = MAX_LINE_DEV_CAP_SIZE; // Starting value - usually big enough
	HRESULT l_hResult = S_OK;

	for (;;)
	{
		// Allocate some memory for the call
		p_pAddressCaps = (LINEADDRESSCAPS *) new BYTE[l_stCurrentSize];
		ZeroMemory(&p_pAddressCaps[0], l_stCurrentSize);
		p_pAddressCaps->dwTotalSize = l_stCurrentSize;

		// Ask TAPI for some information
		l_hResult = ::lineGetAddressCapsA(p_hLineApp, p_dwDeviceID, p_dwAddressID, p_dwAPIVersion,
			p_dwExtVersion, p_pAddressCaps);

		// Cope with variable length structures
		if (l_hResult == LINEERR_STRUCTURETOOSMALL)
		{
			if (p_pAddressCaps->dwNeededSize <= 0)
				break;
			l_stCurrentSize = p_pAddressCaps->dwNeededSize;

			delete[] p_pAddressCaps;
			p_pAddressCaps = NULL;
		}
		else
			break;
	}

	return l_hResult;
}

// Get address capabilities information from TAPI
HRESULT CDeviceManager::LoopLineGetAddressStatus(HLINE p_hLine, DWORD p_dwAddressID, LINEADDRESSSTATUS*& p_pAddressStatus)
{
	std::size_t l_stCurrentSize = MAX_LINE_DEV_CAP_SIZE; // Starting value - usually big enough
	HRESULT l_hResult = S_OK;

	for (;;)
	{
		// Allocate some memory for the call
		p_pAddressStatus = (LINEADDRESSSTATUS *) new BYTE[l_stCurrentSize];
		ZeroMemory(&p_pAddressStatus[0], l_stCurrentSize);
		p_pAddressStatus->dwTotalSize = l_stCurrentSize;

		// Ask TAPI for some information
		l_hResult = ::lineGetAddressStatusA(p_hLine, p_dwAddressID, p_pAddressStatus);

		// Cope with variable length structures
		if (l_hResult == LINEERR_STRUCTURETOOSMALL)
		{
			if (p_pAddressStatus->dwNeededSize <= 0)
				break;
			l_stCurrentSize = p_pAddressStatus->dwNeededSize;

			delete[] p_pAddressStatus;
			p_pAddressStatus = NULL;
		}
		else
			break;
	}

	return l_hResult;
}

// ----------------------------------------------------------------------------
// Get call status information from TAPI
HRESULT CDeviceManager::LoopLineGetCallStatus(HCALL p_hCall, LINECALLSTATUS*& p_pCallStatus)
{
	std::size_t l_stCurrentSize = MAX_LINE_DEV_CAP_SIZE; // Starting value - usually big enough
	HRESULT l_hResult = S_OK;

	for (;;)
	{
		// Allocate some memory for the call
		p_pCallStatus = (LINECALLSTATUS *) new BYTE[l_stCurrentSize];
		ZeroMemory(&p_pCallStatus[0], l_stCurrentSize);
		p_pCallStatus->dwTotalSize = l_stCurrentSize;

		// Ask TAPI for some information
		l_hResult = ::lineGetCallStatus(p_hCall, p_pCallStatus);

		// Cope with variable length structures
		if (l_hResult == LINEERR_STRUCTURETOOSMALL)
		{
			if (p_pCallStatus->dwNeededSize <= 0)
				break;
			l_stCurrentSize = p_pCallStatus->dwNeededSize;

			delete[] p_pCallStatus;
			p_pCallStatus = NULL;
		}
		else
			break;
	}

	return l_hResult;
}

void CDeviceManager::ProcessCallHandle(HLINE p_dwLineId, HCALL p_hCall, E_CALL_HANDLE_TYPE p_nHandleType)
{
	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pTempDevice in m_deviceQueue)
	{
		boost::shared_ptr<CTapiLine> l_pTapiLine = boost::dynamic_pointer_cast<CTapiLine>(l_pTempDevice);
		if (l_pTapiLine)
		{
			if (l_pTapiLine->GetLineHandle() == p_dwLineId)
			{
				l_pTapiLine->ProcessCallHandle(p_hCall, p_nHandleType);
				ICC_LOG_DEBUG(m_pLog, "SetCallHandle, DeviceNum:[%s], LineId:[%u], Call:[%u], CallHandType:[%s]", 
					l_pTapiLine->GetDeviceNum().c_str(), p_dwLineId, p_hCall, CallHandleTypeString[p_nHandleType].c_str());

				break;
			}
		}
	}
}
void CDeviceManager::SetCallHandle(const std::string& p_strDeviceNum, HCALL p_hCall, E_CALL_HANDLE_TYPE p_nHandleType)
{
	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pTempDevice in m_deviceQueue)
	{
		boost::shared_ptr<CTapiLine> l_pTapiLine = boost::dynamic_pointer_cast<CTapiLine>(l_pTempDevice);
		if (l_pTapiLine)
		{
			if (l_pTapiLine->GetDeviceNum().compare(p_strDeviceNum) == 0)
			{
				l_pTapiLine->SetCallHandle(p_hCall, p_nHandleType);

				break;
			}
		}
	}
}
void CDeviceManager::ResetCallHandle(const std::string& p_strDeviceNum, HCALL p_hCall)
{
	std::lock_guard<std::mutex> guard(m_deviceQueueMutex);
	for each (boost::shared_ptr<CDevice> l_pTempDevice in m_deviceQueue)
	{
		boost::shared_ptr<CTapiLine> l_pTapiLine = boost::dynamic_pointer_cast<CTapiLine>(l_pTempDevice);
		if (l_pTapiLine)
		{
			if (l_pTapiLine->GetDeviceNum().compare(p_strDeviceNum) == 0)
			{
				l_pTapiLine->ResetCallHandle(p_hCall);

				break;
			}
		}
	}
}