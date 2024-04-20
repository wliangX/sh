
#include "Boost.h"
#include "SysConfig.h"

#define DEFAULT_SWITCHCMD_TIMEOUT		6			
#define DEFAULT_POLLINGCALL_TIME		60
#define DEFAULT_MIN_ONLINEAGENT			1
#define DEFAULT_HEARTBEAT_TIME			6
#define DEFAULT_CALLINVALID_TIME		(60 * 24)	//	24 小时
#define MIN_HAERTBEAT_TIMESPAN			6			//	查询 CTI 与 AES 网络状态时间间隔，单位，秒
#define MIN_QUERY_AGENTSTATE_TIMESPAN	6			//	查询 Agent 状态时间间隔，单位，秒

boost::shared_ptr<CSysConfig> CSysConfig::m_pInstance = nullptr;
CSysConfig::CSysConfig(void)
{
	m_nSwitchCmdTimeOut = DEFAULT_SWITCHCMD_TIMEOUT;
	m_nMinOnlineAgentNum = DEFAULT_MIN_ONLINEAGENT;
	m_nPollingCallTime = DEFAULT_POLLINGCALL_TIME;
	m_nHeartbeatTime = DEFAULT_HEARTBEAT_TIME;
	m_nCallInvalidTime = DEFAULT_CALLINVALID_TIME;
	m_nAgentStateTime = MIN_QUERY_AGENTSTATE_TIMESPAN;
	m_nTransferRingTimeout = DEFAULT_RING_TIMEOUT;
	m_nConferenceRingTimeout = DEFAULT_RING_TIMEOUT;

	m_nMinLocalPhoneLen = MIN_LOCAL_PHONE_LEN;		//	本地号码最小长度
	m_nMaxLocalPhoneLen = MAX_LOCAL_PHONE_LEN;		//	本地号码最大长度
	m_nMinZoneLen = MIN_ZONE_LEN;				//	区号最小长度
	m_nMaxZoneLen = MAX_ZONE_LEN;				//	区号最大长度
	m_nMobilePreLen = MOBILE_PRE_LEN;			//	手机号码前缀

	m_bParamterIsLoad = false;
	m_bUseAnalogueTrunk = false;
	m_pConfig = nullptr;
	m_pStringUtil = nullptr;
}
CSysConfig::~CSysConfig(void)
{
	//
}

boost::shared_ptr<CSysConfig> CSysConfig::Instance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = boost::make_shared<CSysConfig>();
	}

	return m_pInstance;
}
void CSysConfig::ExitInstance()
{
	ClearParamter();
}

//////////////////////////////////////////////////////////////////////////
void CSysConfig::LoadParamter()
{
	if (!m_bParamterIsLoad)
	{
		m_bParamterIsLoad = true;

		std::string l_strMinLocalPhoneLen = m_pConfig->GetValue("ICC/Plugin/CTI/MinLocalPhoneLen", "7");		//	本地号码最小长度
		std::string l_strMaxLocalPhoneLen = m_pConfig->GetValue("ICC/Plugin/CTI/MaxLocalPhoneLen", "8");		//	本地号码最大长度
		std::string l_strMinZoneLen = m_pConfig->GetValue("ICC/Plugin/CTI/MinZoneLen", "3");				//	区号最小长度
		std::string l_strMaxZoneLen = m_pConfig->GetValue("ICC/Plugin/CTI/MaxZoneLen", "4");				//	区号最大长度
		std::string l_strMobilePreLen = m_pConfig->GetValue("ICC/Plugin/CTI/MobilePreLen", "7");			//	手机号码前缀
		m_nMinLocalPhoneLen = atoi(l_strMinLocalPhoneLen.c_str());
		m_nMaxLocalPhoneLen = atoi(l_strMaxLocalPhoneLen.c_str());
		m_nMinZoneLen = atoi(l_strMinZoneLen.c_str());
		m_nMaxZoneLen = atoi(l_strMaxZoneLen.c_str());
		m_nMobilePreLen = atoi(l_strMobilePreLen.c_str());

		 std::string l_strFlag= m_pConfig->GetValue("ICC/Plugin/CTI/UseAnalogueTrunk", "0");
		 m_bUseAnalogueTrunk = (l_strFlag.compare("1") == 0 ? true : false);

		m_strOutCallHead = m_pConfig->GetValue("ICC/Plugin/CTI/OutCallHead", "");
		m_strWirelessCallHead = m_pConfig->GetValue("ICC/Plugin/CTI/WirelessCallHead", "");
		m_strRouteDest = m_pConfig->GetValue("ICC/Plugin/CTI/RouteDest", "108");

		std::string l_strMinOnlineAgentNum = m_pConfig->GetValue("ICC/Plugin/CTI/MinOnlineAgentNum", "1");
		m_nMinOnlineAgentNum = atoi(l_strMinOnlineAgentNum.c_str());

		std::string l_strCmdTimeout = m_pConfig->GetValue("ICC/Plugin/CTI/SwitchCmdTimeout", "6");
		m_nSwitchCmdTimeOut = atoi(l_strCmdTimeout.c_str());

		std::string l_strHeartBeatTime = m_pConfig->GetValue("ICC/Plugin/CTI/HeartBeatTimeSpan", "6");
		m_nHeartbeatTime = atoi(l_strHeartBeatTime.c_str());
		if (m_nHeartbeatTime < MIN_HAERTBEAT_TIMESPAN)
		{
			m_nHeartbeatTime = MIN_HAERTBEAT_TIMESPAN;
		}

		std::string l_strAgentStateTime = m_pConfig->GetValue("ICC/Plugin/CTI/QueryAgentStateTimeSpan", "6");
		m_nAgentStateTime = atoi(l_strAgentStateTime.c_str());
		if (m_nAgentStateTime < MIN_QUERY_AGENTSTATE_TIMESPAN)
		{
			m_nAgentStateTime = MIN_QUERY_AGENTSTATE_TIMESPAN;
		}

		std::string l_strCallInvalidTime = m_pConfig->GetValue("ICC/Plugin/CTI/CallInvalidTimeSpan", "8");	//	默认 8 小时
		m_nCallInvalidTime = atoi(l_strCallInvalidTime.c_str());

		std::string l_strTransferRingTimeout = m_pConfig->GetValue("ICC/Plugin/CTI/TransferRingTimeout", "25");
		m_nTransferRingTimeout = atoi(l_strTransferRingTimeout.c_str());

		std::string l_strConferencerRingTimeout = m_pConfig->GetValue("ICC/Plugin/CTI/ConferenceRingTimeout", "25");
		m_nConferenceRingTimeout = atoi(l_strConferencerRingTimeout.c_str());

		const unsigned int l_iCorrectSize = 3;
		std::string l_strACDGrpConverList = m_pConfig->GetValue("ICC/Plugin/CTI/ACDGrpConverList", "");
		std::vector<std::string> l_vecACDGrpQueue;
		m_pStringUtil->Split(l_strACDGrpConverList, ";", l_vecACDGrpQueue, false);
		for each (std::string l_strACDGrpObj in l_vecACDGrpQueue)
		{
			if (!l_strACDGrpObj.empty())
			{
				std::vector<std::string> l_vecConverObj;
				m_pStringUtil->Split(l_strACDGrpObj, "()", l_vecConverObj, true);
				if (l_vecConverObj.size() == l_iCorrectSize)
				{
					m_mapACDGrpConversion[l_vecConverObj[0]] = l_vecConverObj[1];
				}
			}
		}

		int l_nDeviceNodeCount = m_pConfig->GetNodeCount("ICC/Plugin/CTI/Devices", "Device");
		for (int i = 0; i < l_nDeviceNodeCount; ++i)
		{
			//分机号;设备类型;登录Agent;登录密码 (7001;ipphone;4003;1234)
			std::string l_strDeviceList = m_pConfig->GetValue("ICC/Plugin/CTI/Devices/Device[" + std::to_string(i) + "]", "7001;ipphone;4001;");

			std::vector<std::string> l_vDevice;
			m_pStringUtil->Split(l_strDeviceList, ";", l_vDevice, false);
			if (l_vDevice.size() == 4)
			{
				CDevicePhone ipPhone;
				ipPhone.m_strDeviceNum = l_vDevice[0];
				ipPhone.m_strDeviceType = l_vDevice[1];
				ipPhone.m_strAgent = l_vDevice[2];
				ipPhone.m_strPsw = l_vDevice[3];
				m_DevicePhoneList.push_back(ipPhone);

				ICC_LOG_DEBUG(m_pLog, "Load Device, DeviceNum: [%s], DeviceType: [%s], AgentId: [%s], Psw: [%s]",
					l_vDevice[0].c_str(), l_vDevice[1].c_str(), l_vDevice[2].c_str(), l_vDevice[3].c_str());
			}
		}

		int l_nACDGrpNodeCount = m_pConfig->GetNodeCount("ICC/Plugin/CTI/ACDGrpMembers", "ACDGrpMember");
		for (int i = 0; i < l_nACDGrpNodeCount; ++i)
		{
			//110#7001;7002;7003
			std::string l_strACDGrpList = m_pConfig->GetValue("ICC/Plugin/CTI/ACDGrpMembers/ACDGrpMember[" + std::to_string(i) + "]", "");
			std::vector<std::string> l_vACDGrp;
			m_pStringUtil->Split(l_strACDGrpList, "#", l_vACDGrp, false);
			if (l_vACDGrp.size() == 2)
			{
				std::string l_strACDGrp = l_vACDGrp[0];
				std::string l_strAgentList = l_vACDGrp[1];

				bool l_bHas = false;
				for (auto it = m_ACDGroupList.begin(); it != m_ACDGroupList.end(); it++)
				{
					if (it->m_strACDNum == l_strACDGrp)
					{
						it->m_strAgentList.push_back(l_strAgentList);
						l_bHas = true;
						break;
					}
				}

				if (!l_bHas)
				{
					CACDGroup l_acdGrpObj;
					std::vector<std::string> l_vAgentList;
					m_pStringUtil->Split(l_strAgentList, ";", l_vAgentList, false);
					l_acdGrpObj.m_strACDNum = l_strACDGrp;
					l_acdGrpObj.m_strAgentList = l_vAgentList;
					m_ACDGroupList.push_back(l_acdGrpObj);
				}

				ICC_LOG_DEBUG(m_pLog, "Load ACDGrp, ACD: [%s], AgentList: [%s]",
					l_strACDGrp.c_str(), l_strAgentList.c_str());
			}
		}

		// 默认置闲的坐席列表
		/*
		std::string l_strAgentList = m_pConfig->GetValue("ICC/Plugin/CTI/ReadyAgents", "");
		std::vector<std::string> l_vTempAgentList;
		m_pStringUtil->Split(l_strAgentList, ";", l_vTempAgentList, false);
		for each (auto l_strAgent in l_vTempAgentList)
		{
			if (!l_strAgent.empty())
			{
				m_DefaultReadyAgentList.push_back(l_strAgent);
			}
		}
		*/
		std::string l_strDialRuleList = m_pConfig->GetValue("ICC/Plugin/CTI/DialRules", "");
		std::vector<std::string> l_vDialRule;
		m_pStringUtil->Split(l_strDialRuleList, ";", l_vDialRule, false);
		for each (auto l_oDialRule in l_vDialRule)
		{
			std::string l_strRule = l_oDialRule;
			std::vector<std::string> l_vRuleObj;
			m_pStringUtil->Split(l_strRule, ",", l_vRuleObj, false);
			const unsigned int l_iCorrectSize = 4;
			if (l_vRuleObj.size() == l_iCorrectSize)
			{
				CDialRule l_tempRule;
				l_tempRule.m_nSrcNumLen = atoi(l_vRuleObj[0].c_str());
				l_tempRule.m_strPreNum = l_vRuleObj[1];
				l_tempRule.m_nRemoveLen = atoi(l_vRuleObj[2].c_str());
				l_tempRule.m_strAddPre = l_vRuleObj[3];
				m_DialRuleList.push_back(l_tempRule);

				ICC_LOG_DEBUG(m_pLog, "Load DialRule, SrcNumLen: [%d], PreNum: [%s], RemoveLen: [%d], AddPre: [%s]",
					l_tempRule.m_nSrcNumLen, l_tempRule.m_strPreNum.c_str(), l_tempRule.m_nRemoveLen, l_tempRule.m_strAddPre.c_str());
			}
		}
	}	// End if m_bParamterIsLoad
}
void CSysConfig::ClearParamter()
{
	m_DevicePhoneList.clear();
	m_DeviceVirList.clear();
	m_ACDGroupList.clear();
	m_DialRuleList.clear();

	m_bParamterIsLoad = false;
}

bool CSysConfig::GetACDGrpConver(std::string& p_strDestACDGrp, const std::string& p_strSrcACDGrp)
{
	bool l_bFind = false;

	auto it = m_mapACDGrpConversion.find(p_strSrcACDGrp);
	if (it != m_mapACDGrpConversion.end())
	{
		l_bFind = true;
		p_strDestACDGrp = it->second;
	}

	return l_bFind;
}
std::string CSysConfig::GetCheckNum(const std::string& p_strSrcNum)
{
	std::string l_strOutNum = p_strSrcNum;

	int l_nLen = l_strOutNum.length();
	for each (auto l_oRule in m_DialRuleList)
	{
		// 1、输入号码的长度等于规则预设的长度（如果规则的长度为0，则匹配）
		// 2、输入号码的长度需大于规则预设的移除长度
		// 3、规则预设的字头匹配输入号码的字头
		// 4、输出号码 = 规则预设的添加的字头 + 移除字头后的输入号码
		if ((l_nLen == l_oRule.m_nSrcNumLen || 0 == l_oRule.m_nSrcNumLen) && l_nLen > l_oRule.m_nRemoveLen && l_strOutNum.find(l_oRule.m_strPreNum) == 0)
		{
			l_strOutNum = l_oRule.m_strAddPre + l_strOutNum.substr(l_oRule.m_nRemoveLen);
			ICC_LOG_DEBUG(m_pLog, "SrcNum: [%s] to DestNum: [%s]", p_strSrcNum.c_str(), l_strOutNum.c_str());

			break;
		}
	}

	return l_strOutNum;
}

bool CSysConfig::GetOutCallNum(const std::string& p_strSrcNum, std::string& p_strDestNum)
{
	std::string l_strOutCallHead = "";
	p_strDestNum = p_strSrcNum;

	if (p_strSrcNum.length() < m_nMinLocalPhoneLen)
	{
		// 非公网号码
		return false;
	}

	{ // 3位区号,座机
		std::string l_strZone = p_strSrcNum.substr(0, m_nMinZoneLen);

		if (m_pRedisClient->HGet(CTI_OUTCALL_HEAD, l_strZone, l_strOutCallHead) && !l_strOutCallHead.empty())
		{
			p_strDestNum = l_strOutCallHead + p_strSrcNum;
			ICC_LOG_DEBUG(m_pLog, "Zone Add OutCallHead，SrcNum: [%s] to DestNum: [%s]", p_strSrcNum.c_str(), p_strDestNum.c_str());

			return true;
		}
	}

	{ // 4位区号，座机
		std::string l_strZone = p_strSrcNum.substr(0, m_nMaxZoneLen);

		if (m_pRedisClient->HGet(CTI_OUTCALL_HEAD, l_strZone, l_strOutCallHead) && !l_strOutCallHead.empty())
		{
			p_strDestNum = l_strOutCallHead + p_strSrcNum;
			ICC_LOG_DEBUG(m_pLog, "Zone Add OutCallHead，SrcNum: [%s] to DestNum: [%s]", p_strSrcNum.c_str(), p_strDestNum.c_str());

			return true;
		}
	}

	{ // 7位前缀，手机
		std::string l_strMobilePre = p_strSrcNum.substr(0, m_nMobilePreLen);

		if (m_pRedisClient->HGet(CTI_OUTCALL_HEAD, l_strMobilePre, l_strOutCallHead) && !l_strOutCallHead.empty())
		{
			p_strDestNum = l_strOutCallHead + p_strSrcNum;
			ICC_LOG_DEBUG(m_pLog, "MobilePre Add OutCallHead，SrcNum: [%s] to DestNum: [%s]", p_strSrcNum.c_str(), p_strDestNum.c_str());

			return true;
		}
	}

	return false;
}

bool CSysConfig::GetACDGrpTypeByDn(const std::string& p_strInDeviceNum, std::string& p_strOutDeviceType)
{
	bool l_bRet = false;

	for each (auto l_acdGrpObj in m_ACDGroupList)
	{
		if (l_acdGrpObj.m_strACDNum.compare(p_strInDeviceNum) == 0)
		{
			l_bRet = true;
			p_strOutDeviceType = DEVICE_TYPE_ACDGROUP;

			break;
		}
	}

	return l_bRet;
}
bool CSysConfig::GetDeviceTypeByDn(const std::string& p_strInDeviceNum, std::string& p_strOutDeviceType)
{
	bool l_bRet = false;

	for each (auto l_deviceObj in m_DevicePhoneList)
	{
		if (l_deviceObj.m_strDeviceNum.compare(p_strInDeviceNum) == 0)
		{
			l_bRet = true;
			p_strOutDeviceType = l_deviceObj.m_strDeviceType;

			break;
		}
	}

	return l_bRet;
}

bool CSysConfig::FindDefaultReadyAgent(const std::string& p_strAgent)
{
	bool l_bRet = false;

	for each (auto l_strAgent in m_DefaultReadyAgentList)
	{
		if (l_strAgent.compare(p_strAgent) == 0)
		{
			l_bRet = true;
			
			break;
		}
	}

	return l_bRet;
}

