
#include "DeviceManager.h"
#include "SysConfig.h"
#include "TaskManager.h"
#include "CTIFSAesDefine.h"
#include "CAgentPhoneState.h"
#include "AgentManager.h"
#include "CallManager.h"
#include "ConferenceManager.h"
#include "FSAesSwitchManager.h"
#include <Protocol/CSeatInfo.h>
#define SLEEP_TIMESPAN				300			//	单位：微秒

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
	_DeleteAllDevice();
	if (m_pInstance)
	{
		m_pInstance.reset();
	}
}

CDeviceManager::CDeviceManager()
{
	m_pLog = nullptr;
	m_pDateTimePtr = nullptr;
	m_pStrUtil = nullptr;

	m_pRedisClient = nullptr;
	m_bDeviceIsLoad = false;
	m_bLoadAesExtension = false;
}

CDeviceManager::~CDeviceManager()
{
	//
}

void CDeviceManager::OnInit(IResourceManagerPtr p_pResourceManager)
{
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_pResourceManager)->GetLogger(MODULE_NAME);
	m_pDateTimePtr = ICCGetResourceEx(DateTime::IDateTimeFactory, ICCIDateTimeFactoryResourceName, p_pResourceManager)->CreateDateTime();


	m_pRedisClient = ICCGetResourceEx(Redis::IRedisClientFactory, ICCIRedisClientFactoryResourceName, p_pResourceManager)->CreateRedisClient();
	m_pJsonFty = ICCGetResourceEx(JsonParser::IJsonFactory, ICCIJsonFactoryResourceName, p_pResourceManager);
	//装载所有设备
	_LoadAllDevice();
}

void CDeviceManager::OnStart()
{
	
}

void CDeviceManager::OnStop()
{
	m_bLoadAesExtension = false;
	m_aesExtensionList.clear();
}
//////////////////////////////////////////////////////////////////////////
void CDeviceManager::AddDevice(std::string &p_strDeviceNum, std::string &p_strDevicetype, std::string &p_strAgent, std::string &p_strPsw)
{
	if (p_strDevicetype != "sipphone")
	{
		ICC_LOG_ERROR(m_pLog, "AddDevice Type Error,DeviceNum:[%s], DeviceType:[%s]", p_strDeviceNum.c_str(), p_strDevicetype.c_str());
		return;
	}

	boost::shared_ptr<CDevice> l_pDevice = boost::make_shared<CDevice>(p_strDeviceNum, CTI_DEVICE_TYPE_PHONE, p_strAgent, p_strPsw);
	l_pDevice->SetIsAgent(true);

	CAgentPhoneState l_CAgentPhoneState;
	l_CAgentPhoneState.m_strPhoneNum = p_strDeviceNum;

	std::string l_strSeatInfo;
	if (m_pRedisClient->HGet("SeatInfo", p_strDeviceNum, l_strSeatInfo))
	{
		PROTOCOL::CSeatInfo cSeatInfo;
		cSeatInfo.Parse(l_strSeatInfo, m_pJsonFty->CreateJson());
		if (!cSeatInfo.m_oBody.m_strDeptCode.empty() && cSeatInfo.m_oBody.m_strDeptCode.size() >= 6)
		{
			//资阳现场部门编码前6位是行政编码，默认取编码前6位。
			l_CAgentPhoneState.m_strZZJGDM = cSeatInfo.m_oBody.m_strDeptCode;
			l_CAgentPhoneState.m_strDistrictCode = cSeatInfo.m_oBody.m_strDeptCode.substr(0, 6);
			l_CAgentPhoneState.m_strSID = l_CAgentPhoneState.m_strDistrictCode + p_strDeviceNum;
		}
		else
		{
			l_CAgentPhoneState.m_strSID = p_strDeviceNum;
		}
		l_CAgentPhoneState.m_strBMMC = cSeatInfo.m_oBody.m_strDeptName;
		l_CAgentPhoneState.m_strBMDM = cSeatInfo.m_oBody.m_strDeptCode;
	}
	std::string l_strVal = l_CAgentPhoneState.ToString(m_pJsonFty->CreateJson());
	m_pRedisClient->HSet("AgentPhoneState", p_strDeviceNum, l_strVal);

	if (m_mapDevicesTB.find(p_strDeviceNum) == m_mapDevicesTB.end())
	{
		SAFE_LOCK(m_deviceTBMutex);
		m_mapDevicesTB[p_strDeviceNum] = l_pDevice;
		ICC_LOG_DEBUG(m_pLog, "AddDevice DeviceNum:[%s], DeviceType:[%s]", p_strDeviceNum.c_str(), p_strDevicetype.c_str());
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "AddDevice Error,DeviceNum:[%s], DeviceType:[%s]", p_strDeviceNum.c_str(), p_strDevicetype.c_str());
	}
}

std::string CDeviceManager::GetAgentID(const std::string& p_strDeviceNum)
{
	SAFE_LOCK(m_deviceTBMutex);
	auto l_iter = m_mapDevicesTB.find(p_strDeviceNum);
	if (l_iter != m_mapDevicesTB.end())
	{
		return l_iter->second->GetAgentId();
	}
	return "";
}

std::string CDeviceManager::GetAgentPswd(const std::string& p_strDeviceNum)
{
	SAFE_LOCK(m_deviceTBMutex);
	auto l_iter = m_mapDevicesTB.find(p_strDeviceNum);
	if (l_iter != m_mapDevicesTB.end())
	{
		return l_iter->second->GetAgentPsw();
	}
	return "";
}
std::string CDeviceManager::GetDeviceCallState(const std::string& p_strDeviceNum, const std::string& p_strCTICallId)
{
	SAFE_LOCK(m_deviceTBMutex);
	auto l_iter = m_mapDevicesTB.find(p_strDeviceNum);
	if (l_iter != m_mapDevicesTB.end())
	{
		return l_iter->second->GetDeviceCallState(p_strCTICallId);
	}
	return "";
}


std::string CDeviceManager::GetDeviceCurrState(const std::string& p_strDeviceNum)
{
	SAFE_LOCK(m_deviceTBMutex);
	auto l_iter = m_mapDevicesTB.find(p_strDeviceNum);
	if (l_iter != m_mapDevicesTB.end())
	{
		return l_iter->second->GetCurrCallState();
	}
	return "";
}

std::string CDeviceManager::GetDeviceCurrCallId(const std::string& p_strDeviceNum)
{
	SAFE_LOCK(m_deviceTBMutex);
	auto l_iter = m_mapDevicesTB.find(p_strDeviceNum);
	if (l_iter != m_mapDevicesTB.end())
	{
		return l_iter->second->GetCurrCallId();
	}
	return "";
}


void CDeviceManager::SaveAESExtensionInfo(const std::vector<std::string>& p_aesExtensions)
{
	if (p_aesExtensions.size() > 0)
	{
		m_bLoadAesExtension = true;
		for (size_t i = 0; i < p_aesExtensions.size(); i++)
		{
			ICC_LOG_DEBUG(m_pLog, "add aes extension, Extension:[%s]", p_aesExtensions[i].c_str());
			m_aesExtensionList[p_aesExtensions[i]] = "1";
		}
	}
}

bool CDeviceManager::IsExtension(const std::string& p_strDevice,bool p_bFindAesExtension)
{
	if (m_bLoadAesExtension && p_bFindAesExtension)
	{
		if (m_aesExtensionList.find(p_strDevice) != m_aesExtensionList.end())
		{
			ICC_LOG_DEBUG(m_pLog, "it's an Extension(aes), DeviceNum:[%s]", p_strDevice.c_str());
			return true;
		}
	}

	{
		SAFE_LOCK(m_deviceTBMutex);
		auto l_iter = m_mapDevicesTB.find(p_strDevice);
		if (l_iter != m_mapDevicesTB.end())
		{
			if (l_iter->second->GetDeviceType() == CTI_DEVICE_TYPE_PHONE)
			{
				ICC_LOG_DEBUG(m_pLog, "it's an Extension(cti), DeviceNum:[%s]", p_strDevice.c_str());
				return true;
			}
		}
	}
	
	return false;
}

std::string CDeviceManager::GetCurrTalkTime(const std::string& p_strDevice)
{
	SAFE_LOCK(m_deviceTBMutex);
	auto l_iter = m_mapDevicesTB.find(p_strDevice);
	if (l_iter != m_mapDevicesTB.end())
	{
		return l_iter->second->GetCurrCallTalkTime();
	}
	return "";
}

boost::shared_ptr<CDevice> CDeviceManager::FindACDDevice(const std::string& p_strDevice)
{
	boost::shared_ptr<CDevice> l_pDevice = nullptr;
	{
		SAFE_LOCK(m_deviceTBMutex);
		auto l_iter = m_mapDevicesTB.find(p_strDevice);
		if (l_iter != m_mapDevicesTB.end())
		{
			l_pDevice = l_iter->second;
		}
	}
	return l_pDevice;
}

std::string CDeviceManager::GetACDListString()
{
	std::map<std::string, boost::shared_ptr<CDevice>> l_mapDevices;
	{
		SAFE_LOCK(m_deviceTBMutex);
		l_mapDevices = m_mapDevicesTB;
	}
	std::string l_strACDs = "";
	for (auto l_deviceObj : l_mapDevices)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj.second;
		if (l_pDevice && !l_deviceObj.first.empty())
		{
			if (l_pDevice->GetDeviceType() != CTI_DEVICE_TYPE_ACD)
			{
				continue;
			}

			if (l_strACDs.empty())
			{
				l_strACDs = l_deviceObj.first;
			}
			else
			{
				l_strACDs = l_strACDs + ";" + l_deviceObj.first;
			}
		}
	}
	return l_strACDs;
}

bool CDeviceManager::IsACDDevice(const std::string& p_strNum)
{
	std::map<std::string, boost::shared_ptr<CDevice>> l_mapDevices;
	{
		SAFE_LOCK(m_deviceTBMutex);
		l_mapDevices = m_mapDevicesTB;
	}

	for (auto l_deviceObj : l_mapDevices)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj.second;
		if (l_pDevice && !l_deviceObj.first.empty())
		{
			if (l_pDevice->GetDeviceType() != CTI_DEVICE_TYPE_ACD)
			{
				continue;
			}

			if (p_strNum == l_deviceObj.first)
			{
				ICC_LOG_DEBUG(m_pLog, "Is acd number, Num:[%s]", p_strNum.c_str());
				return true;
			}
		}
	}
	return false;
}

std::string CDeviceManager::GetAgentListString()
{
	std::map<std::string, boost::shared_ptr<CDevice>> l_mapDevices;
	{
		SAFE_LOCK(m_deviceTBMutex);
		l_mapDevices = m_mapDevicesTB;
	}

	std::string l_strAgents;
	for (auto l_deviceObj : l_mapDevices)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj.second;
		if (l_pDevice)
		{
			if (l_strAgents.empty())
			{
				l_strAgents = l_pDevice->GetAgentId();
			}
			else
			{
				if (!l_pDevice->GetAgentId().empty())
				{
					l_strAgents = l_strAgents + ";" + l_pDevice->GetAgentId();
				}
			}
		}
	}
	return l_strAgents;
}

std::string CDeviceManager::GetDeviceListString()
{
	std::map<std::string, boost::shared_ptr<CDevice>> l_mapDevices;
	{
		SAFE_LOCK(m_deviceTBMutex);
		l_mapDevices = m_mapDevicesTB;
	}

	std::string l_strDevices;
	for (auto l_deviceObj : l_mapDevices)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj.second;
		if (l_pDevice)
		{
			if (l_pDevice->GetDeviceType() == CTI_DEVICE_TYPE_ACD)
			{
				continue;
			}

			if (l_strDevices.empty())
			{
				l_strDevices = l_pDevice->GetDeviceNum();
			}
			else
			{
				if (!l_pDevice->GetAgentId().empty())
				{
					l_strDevices = l_strDevices + ";" + l_pDevice->GetDeviceNum();
				}
			}
		}
	}
	return l_strDevices;
}

bool CDeviceManager::GetDeviceList(IGetDeviceListResultNotifPtr p_pGetResultNotify)
{
	if (p_pGetResultNotify == nullptr)
	{
		return false;
	}
	std::map<std::string, boost::shared_ptr<CDevice>> l_mapDevices;
	{
		SAFE_LOCK(m_deviceTBMutex);
		l_mapDevices = m_mapDevicesTB;
	}

	std::string l_strDevices;
	for (auto l_deviceObj : l_mapDevices)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj.second;
		if (l_pDevice == nullptr || l_pDevice->GetDeviceType() != CTI_DEVICE_TYPE_PHONE)
		{
			continue;
		}

		boost::shared_ptr<CDeviceStateNotif> l_pDeviceNotif = boost::make_shared<CDeviceStateNotif>();
		if (l_pDeviceNotif)
		{
			std::string l_strDeviceState = CallStateString[STATE_FREE];
			std::string l_strStateTime = m_pDateTimePtr->CurrentDateTimeStr();
			l_pDeviceNotif->SetDeviceState(l_strDeviceState);
			l_pDeviceNotif->SetStateTime(l_strStateTime);
			long l_lCSTACallRefId = DEFAULT_CALLREFID;
			//l_pDevice->GetCurrentLogicalCall(l_lCSTACallRefId, l_strDeviceState, l_strStateTime);
			l_pDeviceNotif->SetCSTACallRefId(l_lCSTACallRefId);
			std::string l_strDeviceNum = l_pDevice->GetDeviceNum();

			std::string l_strCurrCTICallId = l_pDevice->GetCurrCallId();
			ICTICallPtr l_pCTICall = CCallManager::Instance()->GetCallByHcpCallId(l_strCurrCTICallId);
			if (l_pCTICall)
			{
				l_pDeviceNotif->SetDeviceState(l_pDevice->GetCurrCallState());
				
				l_pDeviceNotif->SetStateTime(l_pDevice->GetCurrCallStateTime());
				l_pDeviceNotif->SetCSTACallRefId(l_pCTICall->GetCallIndex());

				l_pDeviceNotif->SetCTICallRefId(l_pCTICall->GetCTICallRefId());

				l_pDeviceNotif->SetCallerId(l_pCTICall->GetCallerId());
				if (!l_pCTICall->GetConferenceCompere().empty() && 
					l_pCTICall->GetConferenceCompere() != l_strDeviceNum && 
					l_pDevice->GetCurrCallState() != CallStateString[STATE_SILENTMONITOR])
				{
					IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceBySrcCallId(l_pCTICall->GetConferenceCompere());
					if (l_pConferencePtr)
					{
						if (!l_pConferencePtr->IsBargeInCreate() && !l_pConferencePtr->IsListenCreate() && !l_pConferencePtr->IsTakeoverCreate())
						{
							l_pDeviceNotif->SetCallerId(l_pCTICall->GetConferenceCompere());
						}
					}
				}
				
				std::string l_strCompere;
				std::string l_strBargeInSponsor;
				std::string l_strConferenceId = CConferenceManager::Instance()->QueryConferenceByCallid(l_strCurrCTICallId, l_strCompere, l_strBargeInSponsor);
				if (!l_strConferenceId.empty() && l_pDevice->GetCurrCallState() != CallStateString[STATE_SILENTMONITOR])
				{
					l_pDeviceNotif->SetConferenceId(l_strConferenceId);
					l_pDeviceNotif->SetConfCompere(l_strCompere);
					if (!l_strBargeInSponsor.empty())
					{
						l_pDeviceNotif->SetConfBargeinSponsor(l_strBargeInSponsor);
					}

					if (l_pDevice->GetCurrCallState() == CallStateString[STATE_TALK] && l_strBargeInSponsor.empty())
					{
						l_pDeviceNotif->SetDeviceState(CallStateString[STATE_CONFERENCE]);
					}
				}

				//l_pDeviceNotif->SetTalkTime(l_pCTICall->GetTalkTime());
				if (l_pDevice->GetCurrCallTalkTime().empty())
				{
					l_pDeviceNotif->SetTalkTime(l_pCTICall->GetTalkTime());
				}
				else
				{
					l_pDeviceNotif->SetTalkTime(l_pDevice->GetCurrCallTalkTime());
				}
				
				l_pDeviceNotif->SetCalledId(l_pCTICall->GetCalledId());
				l_pDeviceNotif->SetCallDirection(l_pCTICall->GetCallDirection());
				l_pDeviceNotif->SetOriginalCalledId(l_pCTICall->GetOriginalCalled());
				
				std::string l_strAcdGrp = l_pCTICall->GetAcdNum();
				if (!l_strAcdGrp.empty())
				{
					l_pDeviceNotif->SetACDGrp(l_strAcdGrp);
				}
			}
			else
			{
				if (l_pDevice->GetCurrCallState() == CallStateString[STATE_DIAL])
				{
					l_pDeviceNotif->SetDeviceState(CallStateString[STATE_DIAL]);
					l_pDeviceNotif->SetCallerId(l_strDeviceNum);
				}
				
				if (!l_strCurrCTICallId.empty())
				{
					l_pDevice->DeleteCallInfo(l_strCurrCTICallId);
				}
			}

			l_pDeviceNotif->SetDeviceNum(l_strDeviceNum);
			l_pDeviceNotif->SetDeviceType(l_pDevice->GetDeviceTypeString());
			l_pDeviceNotif->SetDeptCode(_GetDeptCodeByDeviceNum(l_pDevice->GetDeviceNum()));

			std::string l_strLoginMode;
			std::string l_strReadyState;
			CAgentManager::Instance()->GetAgentState(l_strDeviceNum, l_strLoginMode, l_strReadyState);
			l_pDeviceNotif->SetLoginMode(l_strLoginMode);
			l_pDeviceNotif->SetReadyState(l_strReadyState);

			p_pGetResultNotify->m_DeviceList.push_back(l_pDeviceNotif);
		}
	}
	return true;
}

bool CDeviceManager::UpdateDeviceCallState(const std::string& p_strDeviceNum, const std::string& p_strHcpCallId, const std::string& p_strState,bool p_bUpdateCurrCallId)
{
	boost::shared_ptr<CDevice> l_pDevice = nullptr;
	{
		SAFE_LOCK(m_deviceTBMutex);
		auto l_iter = m_mapDevicesTB.find(p_strDeviceNum);
		if (l_iter != m_mapDevicesTB.end())
		{
			l_pDevice = l_iter->second;
		}
	}
	if (l_pDevice != nullptr)
	{
		std::string l_strCurrCallId = p_strHcpCallId;
		l_pDevice->SetDeviceCallState(l_strCurrCallId, p_strState, m_pDateTimePtr->CurrentDateTimeStr(), p_bUpdateCurrCallId);

		ICC_LOG_DEBUG(m_pLog, "set device call state, DeviceNum:[%s], callState:[%s],callId:[%s],currCallId:[%s],callListSize:%d",
			p_strDeviceNum.c_str(), p_strState.c_str(), p_strHcpCallId.c_str(), l_strCurrCallId.c_str(), l_pDevice->GetCallListSize());

		return true;
	}
	
	return false;
}

std::string CDeviceManager::GetDeviceNextCallState(const std::string& p_strDeviceNum, std::string& p_strState)
{
	boost::shared_ptr<CDevice> l_pDevice = nullptr;
	{
		SAFE_LOCK(m_deviceTBMutex);
		auto l_iter = m_mapDevicesTB.find(p_strDeviceNum);
		if (l_iter != m_mapDevicesTB.end())
		{
			l_pDevice = l_iter->second;
		}
	}
	if (l_pDevice != nullptr)
	{
		return l_pDevice->GetNextCallState(p_strState);
	}

	return "";
}


bool CDeviceManager::SetDeviceRecordInfo(const std::string& p_strDeviceNum,const std::string & p_strHcpCallId, const std::string& p_strSwitchAddr, const std::string& p_strRecordFile)
{
	boost::shared_ptr<CDevice> l_pDevice = nullptr;
	{
		SAFE_LOCK(m_deviceTBMutex);
		auto l_iter = m_mapDevicesTB.find(p_strDeviceNum);
		if (l_iter != m_mapDevicesTB.end())
		{
			l_pDevice = l_iter->second;
		}
	}

	if (l_pDevice != nullptr)
	{
		return l_pDevice->SaveCurrRecordInfo(p_strHcpCallId,p_strSwitchAddr, p_strRecordFile);
	}

	return false;
}

bool CDeviceManager::GetDeviceRecordInfo(const std::string& p_strDeviceNum, const std::string& p_strHcpCallId, std::string& p_strSwitchAddr, std::string& p_strRecordFile)
{
	boost::shared_ptr<CDevice> l_pDevice = nullptr;
	{
		SAFE_LOCK(m_deviceTBMutex);
		auto l_iter = m_mapDevicesTB.find(p_strDeviceNum);
		if (l_iter != m_mapDevicesTB.end())
		{
			l_pDevice = l_iter->second;
		}
	}

	if (l_pDevice != nullptr)
	{
		return l_pDevice->GetCurrRecordInfo(p_strHcpCallId,p_strSwitchAddr, p_strRecordFile);
	}

	return false;
}

void CDeviceManager::ClearAllDeviceCallInfo()
{
	std::map<std::string, boost::shared_ptr<CDevice>> l_mapDevices;
	{
		SAFE_LOCK(m_deviceTBMutex);
		l_mapDevices = m_mapDevicesTB;
	}

	std::string l_strDevices;
	for (auto l_deviceObj : l_mapDevices)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj.second;
		if (l_pDevice == nullptr)
		{
			continue;
		}
		std::string l_strCurrCallId = l_pDevice->GetCurrCallId();
		std::string l_strDeviceNum = l_pDevice->GetDeviceNum();
		if (!l_strCurrCallId.empty())
		{
			IDeviceStateNotifPtr l_pDeviceNotify = boost::make_shared<CDeviceStateNotif>();
			if (l_pDeviceNotify)
			{
				//std::string l_strDeviceNum = p_HcpDeviceStateEvent.m_oBody.m_strAgentExtension;
				l_pDeviceNotify->SetCTICallRefId(l_strCurrCallId);
				l_pDeviceNotify->SetTalkTime(l_pDevice->GetCurrCallTalkTime());
				l_pDeviceNotify->SetDeviceNum(l_strDeviceNum);
				l_pDeviceNotify->SetDeviceType(DEVICE_TYPE_IPPHONE);
				l_pDeviceNotify->SetDeviceState(CallStateString[STATE_FREE]);
				l_pDeviceNotify->SetStateTime(m_pDateTimePtr->CurrentDateTimeStr());
				
				ICC_LOG_DEBUG(m_pLog, "report event DeviceState(freestate),CTICallRefId:[%s],device:[%s],state:[%s]", l_strCurrCallId.c_str(), l_strDeviceNum.c_str());
				//l_pSwitchEventCallback->Event_DeviceState(l_pDeviceNotify);
				CFSAesSwitchManager::Instance()->DispatchCTIEvent("Event_DeviceState", l_pDeviceNotify);
			}
		}
		l_pDevice->ClearCallInfo();
	}
}
//////////////////////////////////////////////////////////////////////
//
std::string CDeviceManager::_GetDeptCodeByDeviceNum(const std::string& p_strDeviceNum)
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

void CDeviceManager::_LoadAllDevice()
{
	if (!m_bDeviceIsLoad)
	{
		std::list<CACDGroup> l_ACDGroupList = CSysConfig::Instance()->m_ACDGroupList;
		for (auto l_acdGrpObj : l_ACDGroupList)
		{
			std::string l_strACDNum = l_acdGrpObj.m_strACDNum;
			std::string l_strACDAlias = l_acdGrpObj.m_strACDAlias;

			if (!l_strACDNum.empty())
			{
				boost::shared_ptr<CDevice> l_pDevice = boost::make_shared<CDevice>(l_strACDNum, CTI_DEVICE_TYPE_ACD, "", "");

				if (m_mapDevicesTB.find(l_strACDNum) == m_mapDevicesTB.end())
				{
					SAFE_LOCK(m_deviceTBMutex);
					m_mapDevicesTB[l_strACDNum] = l_pDevice;
					ICC_LOG_DEBUG(m_pLog, "AddDevice DeviceNum:[%s], DeviceType:ACD", l_strACDNum.c_str());
				}
				else
				{
					ICC_LOG_WARNING(m_pLog, "AddDevice error, DeviceNum:[%s], DeviceType:ACD", l_strACDNum.c_str());
				}
			}
		}
	}

	if (!m_bDeviceIsLoad)
	{
		m_pRedisClient->Del("AgentPhoneState");
		std::list<CDevicePhone>	l_phoneList = CSysConfig::Instance()->m_DevicePhoneList;
		for (auto l_phoneObj : l_phoneList)
		{
			std::string l_strDeviceNum = l_phoneObj.m_strDeviceNum;
			std::string l_strDevicetype = l_phoneObj.m_strDeviceType;
			std::string l_strAgent = l_phoneObj.m_strAgent;
			std::string l_strPsw = l_phoneObj.m_strPsw;

			if (l_strDevicetype != "ipphone")
			{
				ICC_LOG_ERROR(m_pLog, "AddDevice Type Error,DeviceNum:[%s], DeviceType:[%s]", l_strDeviceNum.c_str(), l_strDevicetype.c_str());
				continue;
			}
			boost::shared_ptr<CDevice> l_pDevice = boost::make_shared<CDevice>(l_strDeviceNum, CTI_DEVICE_TYPE_PHONE, l_strAgent, l_strPsw);
			l_pDevice->SetIsAgent(true);

			CAgentPhoneState l_CAgentPhoneState;
			l_CAgentPhoneState.m_strPhoneNum = l_strDeviceNum;

			std::string l_strSeatInfo;
			if (m_pRedisClient->HGet("SeatInfo", l_strDeviceNum, l_strSeatInfo))
			{
				PROTOCOL::CSeatInfo cSeatInfo;
				cSeatInfo.Parse(l_strSeatInfo, m_pJsonFty->CreateJson());
				if (!cSeatInfo.m_oBody.m_strDeptCode.empty() && cSeatInfo.m_oBody.m_strDeptCode.size() >= 6)
				{
					// 资阳现场部门编码前6位是行政编码，默认取编码前6位。
					l_CAgentPhoneState.m_strZZJGDM = cSeatInfo.m_oBody.m_strDeptCode;
					l_CAgentPhoneState.m_strDistrictCode = cSeatInfo.m_oBody.m_strDeptCode.substr(0, 6);
					l_CAgentPhoneState.m_strSID = l_CAgentPhoneState.m_strDistrictCode + l_strDeviceNum;
				}
				l_CAgentPhoneState.m_strBMMC = cSeatInfo.m_oBody.m_strDeptName;
				l_CAgentPhoneState.m_strBMDM = cSeatInfo.m_oBody.m_strDeptCode;
			}
			std::string l_strVal = l_CAgentPhoneState.ToString(m_pJsonFty->CreateJson());
			m_pRedisClient->HSet("AgentPhoneState", l_strDeviceNum, l_strVal);

			if (m_mapDevicesTB.find(l_strDeviceNum) == m_mapDevicesTB.end())
			{
				SAFE_LOCK(m_deviceTBMutex);
				m_mapDevicesTB[l_strDeviceNum] = l_pDevice;
				ICC_LOG_DEBUG(m_pLog, "AddDevice DeviceNum:[%s], DeviceType:[%s]", l_strDeviceNum.c_str(), l_strDevicetype.c_str());
			}
			else
			{
				ICC_LOG_WARNING(m_pLog, "AddDevice Error,DeviceNum:[%s], DeviceType:[%s]", l_strDeviceNum.c_str(), l_strDevicetype.c_str());
			}
		}
	}
	m_bDeviceIsLoad = true;
}

void CDeviceManager::_DeleteAllDevice()
{
	m_bDeviceIsLoad = false;
	{
		SAFE_LOCK(m_deviceTBMutex);
		m_mapDevicesTB.clear();
	}

}
