
#include "Boost.h"
#include "SysConfig.h"

#define DEFAULT_SWITCHCMD_TIMEOUT		6			
#define DEFAULT_POLLINGCALL_TIME		60
#define DEFAULT_MIN_ONLINEAGENT			1
#define DEFAULT_HEARTBEAT_TIME			6
#define DEFAULT_MONITOR_TIMESPAN		20
#define DEFAULT_MAX_MONITOR_COUNT		10
#define DEFAULT_CALLINVALID_TIME		24			//	24 小时
#define MIN_HAERTBEAT_TIMESPAN			6			//	查询 CTI 与 AES 网络状态时间间隔，单位，秒
#define MIN_QUERY_AGENTSTATE_TIMESPAN	6			//	查询 Agent 状态时间间隔，单位，秒
#define ADDR_ZERO_PRELEN				8


boost::shared_ptr<CSysConfig> CSysConfig::m_pInstance = nullptr;
CSysConfig::CSysConfig(void)
{
//	m_strDialRuleList = "";
	m_strInCallHead = "";
	m_strOutCallHead = "";

	m_nSwitchCmdTimeOut = DEFAULT_SWITCHCMD_TIMEOUT;
	m_nMinOnlineAgentNum = DEFAULT_MIN_ONLINEAGENT;
	m_nPollingCallTime = DEFAULT_POLLINGCALL_TIME;
	m_nHeartbeatTime = DEFAULT_HEARTBEAT_TIME;
	m_nCallInvalidTime = DEFAULT_CALLINVALID_TIME;
	m_nAgentStateTime = MIN_QUERY_AGENTSTATE_TIMESPAN;
	m_nMonitorTimeSpan = DEFAULT_MONITOR_TIMESPAN;
	m_nMaxMonitorCount = DEFAULT_MAX_MONITOR_COUNT;

	m_nMinLocalPhoneLen = MIN_LOCAL_PHONE_LEN;		//	本地号码最小长度
	m_nMaxLocalPhoneLen = MAX_LOCAL_PHONE_LEN;		//	本地号码最大长度
	m_nMinZoneLen = MIN_ZONE_LEN;				//	区号最小长度
	m_nMaxZoneLen = MAX_ZONE_LEN;				//	区号最大长度
	m_nMobilePreLen = MOBILE_PRE_LEN;			//	手机号码前缀

	m_nAddrZeroPreLen = ADDR_ZERO_PRELEN;

	m_bParamterIsLoad = false;
	m_pLog = nullptr;
	m_ConfigPtr = nullptr;
	m_StringUtilPtr = nullptr;

	m_bCTITestFlag = false; 
	m_nCTITestPort = 21050;

	m_strSlaveAEServerIP = "";

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

	if (m_pInstance)
	{
		m_pInstance.reset();
	}
}

//////////////////////////////////////////////////////////////////////////

void CSysConfig::OnLoadConfigSuccessNotify(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	ICC_LOG_DEBUG(m_pLog, "receive load config success!!");
	_LoadCheckNumList();
	_LoadDialRuleList();
}

void CSysConfig::OnInit(IResourceManagerPtr p_pResourceManager)
{
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_pResourceManager)->GetLogger(MODULE_NAME);
	m_ConfigPtr = ICCGetResourceEx(Config::IConfigFactory, ICCIConfigFactoryResourceName, p_pResourceManager)->CreateConfig();
	m_StringUtilPtr = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, p_pResourceManager)->CreateString();
	m_pRedisClient = ICCGetResourceEx(Redis::IRedisClientFactory, ICCIRedisClientFactoryResourceName, p_pResourceManager)->CreateRedisClient();

	m_pObserverCenter = ICCGetResourceEx(ObserverPattern::IObserverFactory, ICCIObserverFactoryResourceName, p_pResourceManager)->GetObserverCenter(BUSINESS_MESSAGE);

	MANUL_ADDOBSERVER(m_pObserverCenter, CSysConfig, "sync_load_config_success", OnLoadConfigSuccessNotify);

	LoadParamter();	
}

void CSysConfig::LoadParamter()
{
	if (!m_bParamterIsLoad)
	{
		m_bParamterIsLoad = true;

		m_strEnbaleConfigCalloutCheck = m_ConfigPtr->GetValue("ICC/Plugin/FSAESClient/EnableConfigCalloutCheck", "1");
		m_strOutCallHead = m_ConfigPtr->GetValue("ICC/Plugin/CTI/OutCallHead", "9");

		ICC_LOG_DEBUG(m_pLog, "callout check flag: [%s], callout head: [%s]", m_strEnbaleConfigCalloutCheck.c_str(), m_strOutCallHead.c_str());

		m_strAEServerIP = m_ConfigPtr->GetValue("ICC/Plugin/FSAESClient/FsServerIP", "127.0.0.1");
		m_strSlaveAEServerIP = m_ConfigPtr->GetValue("ICC/Plugin/FSAESClient/FsSlaveServerIP", "");
		if (m_strAEServerIP == m_strSlaveAEServerIP)
		{
			m_strSlaveAEServerIP = "";
		}

		m_strAEServerPort = m_ConfigPtr->GetValue("ICC/Plugin/FSAESClient/FsServerPort", "450");
		m_strAESUserName = m_ConfigPtr->GetValue("ICC/Plugin/FSAESClient/FsServerUserName", "cti");
		m_strAESPsw = m_ConfigPtr->GetValue("ICC/Plugin/FSAESClient/FsServerPassword", "123456");
		m_strAESNodeName = m_ConfigPtr->GetValue("ICC/Plugin/FSAESClient/NodeName", "hcp");
		m_strInCallHead = m_ConfigPtr->GetValue("ICC/Plugin/CTI/InCallHead", "");
		
		m_strWirelessCallHead = m_ConfigPtr->GetValue("ICC/Plugin/CTI/WirelessCallHead", "");
		m_strRouteDest = m_ConfigPtr->GetValue("ICC/Plugin/CTI/RouteDest", "108");
		/*m_strLocalPhoneCallHead = m_ConfigPtr->GetValue("ICC/Plugin/CTI/LocalPhoneCallHead", "0");
		m_strRemotePhoneCallHead = m_ConfigPtr->GetValue("ICC/Plugin/CTI/RemotePhoneCallHead", "00");
		m_strLocalMobileCallHead = m_ConfigPtr->GetValue("ICC/Plugin/CTI/LocalMobileCallHead", "0");
		m_strRemoteMobileCallHead = m_ConfigPtr->GetValue("ICC/Plugin/CTI/RemoteMobileCallHead", "00");
		m_strProvince = m_ConfigPtr->GetValue("ICC/Plugin/CTI/Province", "guangdong");*/

		std::string l_strMinLocalPhoneLen = m_ConfigPtr->GetValue("ICC/Plugin/CTI/MinLocalPhoneLen", "7");		//	本地号码最小长度
		std::string l_strMaxLocalPhoneLen = m_ConfigPtr->GetValue("ICC/Plugin/CTI/MaxLocalPhoneLen", "8");		//	本地号码最大长度
		std::string l_strMinZoneLen = m_ConfigPtr->GetValue("ICC/Plugin/CTI/MinZoneLen", "3");				//	区号最小长度
		std::string l_strMaxZoneLen = m_ConfigPtr->GetValue("ICC/Plugin/CTI/MaxZoneLen", "4");				//	区号最大长度
		std::string l_strMobilePreLen = m_ConfigPtr->GetValue("ICC/Plugin/CTI/MobilePreLen", "7");			//	手机号码前缀

		std::string l_stZeroPreLen = m_ConfigPtr->GetValue("ICC/Plugin/CTI/AddrZeroPreLen", "8");			//

		_LoadMergeHuntGrpConver();

		m_nMinLocalPhoneLen = atoi(l_strMinLocalPhoneLen.c_str());		
		m_nMaxLocalPhoneLen = atoi(l_strMaxLocalPhoneLen.c_str());		
		m_nMinZoneLen = atoi(l_strMinZoneLen.c_str());				
		m_nMaxZoneLen = atoi(l_strMaxZoneLen.c_str());					
		m_nMobilePreLen = atoi(l_strMobilePreLen.c_str());				

		m_nAddrZeroPreLen = atoi(l_stZeroPreLen.c_str());

		ICC_LOG_DEBUG(m_pLog, "CAddrZeroPreLen: [%d]", m_nAddrZeroPreLen);
		//最小在席数
		std::string l_strMinOnlineAgentNum = m_ConfigPtr->GetValue("ICC/Plugin/CTI/MinOnlineAgentNum", "1");
		m_nMinOnlineAgentNum = atoi(l_strMinOnlineAgentNum.c_str());

		std::string l_strCmdTimeout = m_ConfigPtr->GetValue("ICC/Plugin/CTI/SwitchCmdTimeout", "6");
		m_nSwitchCmdTimeOut = atoi(l_strCmdTimeout.c_str());

		std::string l_strMonitorTimeSpan = m_ConfigPtr->GetValue("ICC/Plugin/CTI/MonitorTimeSpan", "10");
		m_nMonitorTimeSpan = atoi(l_strMonitorTimeSpan.c_str());

		std::string l_strMaxMonitorCount = m_ConfigPtr->GetValue("ICC/Plugin/CTI/MaxMonitorCount", "10");
		m_nMaxMonitorCount = atoi(l_strMaxMonitorCount.c_str());

		std::string l_strHeartBeatTime = m_ConfigPtr->GetValue("ICC/Plugin/CTI/HeartBeatTimeSpan", "6");
		m_nHeartbeatTime = atoi(l_strHeartBeatTime.c_str());
		if (m_nHeartbeatTime < MIN_HAERTBEAT_TIMESPAN)
		{
			m_nHeartbeatTime = MIN_HAERTBEAT_TIMESPAN;
		}

		std::string l_strAgentStateTime = m_ConfigPtr->GetValue("ICC/Plugin/CTI/QueryAgentStateTimeSpan", "6");
		m_nAgentStateTime = atoi(l_strAgentStateTime.c_str());
		if (m_nAgentStateTime < MIN_QUERY_AGENTSTATE_TIMESPAN)
		{
			m_nAgentStateTime = MIN_QUERY_AGENTSTATE_TIMESPAN;
		}

		std::string l_strCallInvalidTime = m_ConfigPtr->GetValue("ICC/Plugin/CTI/CallInvalidTimeSpan", "8");	//	默认 8 小时
		m_nCallInvalidTime = atoi(l_strCallInvalidTime.c_str());

		std::string l_strWaitAnswerMaxTime = m_ConfigPtr->GetValue("ICC/Plugin/CTI/CallWaitAnswerMaxTime", "5");	//默认5分钟
		m_nWaitAnswerMaxTime = atoi(l_strWaitAnswerMaxTime.c_str());

		_LoadCheckNumList();

		/*int l_nCheckNumNodeCount = m_ConfigPtr->GetNodeCount("ICC/Plugin/CTI/CheckNumbers", "CheckNumber");
		for (int i = 0; i < l_nCheckNumNodeCount; ++i)
		{
			std::string l_stPrePath = "ICC/Plugin/CTI/CheckNumbers/CheckNumber[" + std::to_string(i) + "]";

			std::string l_strLen = m_ConfigPtr->GetValue(l_stPrePath + "/Len", "");
			std::string l_strPrefix = m_ConfigPtr->GetValue(l_stPrePath + "/Prefix", "");
			std::string l_strRemoveHeadLen = m_ConfigPtr->GetValue(l_stPrePath + "/RemoveHeadLen", "");
			std::string l_strAddPrefix = m_ConfigPtr->GetValue(l_stPrePath + "/AddPrefix", "");

			CDialRule l_tempRule;
			l_tempRule.m_nSrcNumLen = atoi(l_strLen.c_str());
			l_tempRule.m_strPreNum = l_strPrefix;
			l_tempRule.m_nRemoveLen = atoi(l_strRemoveHeadLen.c_str());
			l_tempRule.m_strAddPre = l_strAddPrefix;
			m_CheckNumList.push_back(l_tempRule);

			ICC_LOG_DEBUG(m_pLog, "Load CheckNums, SrcNumLen: [%d], PreNum: [%s], RemoveLen: [%d], AddPre: [%s]",
				l_tempRule.m_nSrcNumLen, l_tempRule.m_strPreNum.c_str(), l_tempRule.m_nRemoveLen, l_tempRule.m_strAddPre.c_str());
		}*/

		_LoadDialRuleList();

		/*std::string l_strDialRuleList = m_ConfigPtr->GetValue("ICC/Plugin/CTI/DialRules", "");
		std::vector<std::string> l_vDialRule;
		m_StringUtilPtr->Split(l_strDialRuleList, ";", l_vDialRule, false);
		for (auto l_oDialRule : l_vDialRule)
		{
			std::string l_strRule = l_oDialRule;
			std::vector<std::string> l_vRuleObj;
			m_StringUtilPtr->Split(l_strRule, ",", l_vRuleObj, false);
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
		}*/

		bool l_bFsDevice = false;
		int l_nFsDeviceNodeCount = m_ConfigPtr->GetNodeCount("ICC/Plugin/CTI/FsDevices", "Device");
		for (int i = 0; i < l_nFsDeviceNodeCount; ++i)
		{
			//分机号;设备类型;登录Agent;登录密码 (7001;ipphone;4003;1234)
			std::string l_strDeviceList = m_ConfigPtr->GetValue("ICC/Plugin/CTI/FsDevices/Device[" + std::to_string(i) + "]", "7001;ipphone;4003;");

			std::vector<std::string> l_vDevice;
			m_StringUtilPtr->Split(l_strDeviceList, ";", l_vDevice, false);
			const unsigned int l_iCorrectSize = 4;
			if (l_vDevice.size() == l_iCorrectSize)
			{
				CDevicePhone ipPhone;
				ipPhone.m_strDeviceNum = l_vDevice[0];
				ipPhone.m_strDeviceType = l_vDevice[1];
				ipPhone.m_strAgent = l_vDevice[2];
				ipPhone.m_strPsw = l_vDevice[3];
				m_DevicePhoneList.push_back(ipPhone);

				l_bFsDevice = true;
				_AddDeviceNumber(ipPhone.m_strDeviceNum);
			}
		}

		//if (l_bFsDevice)
		{
			int l_nDeviceNodeCount = m_ConfigPtr->GetNodeCount("ICC/Plugin/CTI/Devices", "Device");
			for (int i = 0; i < l_nDeviceNodeCount; ++i)
			{
				//分机号;设备类型;登录Agent;登录密码 (7001;ipphone;4003;1234)
				std::string l_strDeviceList = m_ConfigPtr->GetValue("ICC/Plugin/CTI/Devices/Device[" + std::to_string(i) + "]", "7001;ipphone;4003;");

				std::vector<std::string> l_vDevice;
				m_StringUtilPtr->Split(l_strDeviceList, ";", l_vDevice, false);
				const unsigned int l_iCorrectSize = 4;
				if (l_vDevice.size() == l_iCorrectSize)
				{
					CDevicePhone ipPhone;
					ipPhone.m_strDeviceNum = l_vDevice[0];
					ipPhone.m_strDeviceType = l_vDevice[1];
					ipPhone.m_strAgent = l_vDevice[2];
					ipPhone.m_strPsw = l_vDevice[3];
					m_DevicePhoneList.push_back(ipPhone);

					_AddDeviceNumber(ipPhone.m_strDeviceNum);
				}
			}
		}
		
		int l_nACDGrpNodeCount = m_ConfigPtr->GetNodeCount("ICC/Plugin/CTI/ACDGrpMembers", "ACDGrpMember");
		for (int i = 0; i < l_nACDGrpNodeCount; ++i)
		{
			//110#7001;7002;7003
			std::string l_strACDGrpList = m_ConfigPtr->GetValue("ICC/Plugin/CTI/ACDGrpMembers/ACDGrpMember[" + std::to_string(i) + "]", "");
			std::vector<std::string> l_vACDGrp;
			m_StringUtilPtr->Split(l_strACDGrpList, "#", l_vACDGrp, false);
			const unsigned int l_iCorrectSize = 2;
			if (l_vACDGrp.size() == l_iCorrectSize)
			{
				std::string l_strACDGrp = l_vACDGrp[0];
				std::string l_strAgentList = l_vACDGrp[1];

				_AddDeviceNumber(l_strACDGrp);

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
					m_StringUtilPtr->Split(l_strAgentList, ";", l_vAgentList, false);
					l_acdGrpObj.m_strACDNum = l_strACDGrp;
					l_acdGrpObj.m_strAgentList = l_vAgentList;
					m_ACDGroupList.push_back(l_acdGrpObj);
				}

				ICC_LOG_DEBUG(m_pLog, "Load ACDGrpLsit, ACDGrp: [%s], AgentList: [%s],l_strACDGrpList[%s]",
					l_strACDGrp.c_str(), l_strAgentList.c_str(), l_strACDGrpList.c_str());
			}
		}

		// 默认置闲的坐席列表
		std::string l_strAgentList = m_ConfigPtr->GetValue("ICC/Plugin/CTI/ReadyAgents", "");
		std::vector<std::string> l_vTempAgentList;
		m_StringUtilPtr->Split(l_strAgentList, ";", l_vTempAgentList, false);
		for (auto l_strAgent : l_vTempAgentList)
		{
			if (!l_strAgent.empty())
			{
				m_DefaultReadyAgentList.push_back(l_strAgent);
			}
		}

		m_bCTITestFlag = false;
		std::string l_strCTITestFlag = m_ConfigPtr->GetValue("ICC/Plugin/CTI/CTITestFlag", "0");
		if (!l_strCTITestFlag.empty())
		{
			if (atoi(l_strCTITestFlag.c_str()) > 0)
			{
				m_bCTITestFlag = true;
			}
		}
		std::string l_strCTITestPort = m_ConfigPtr->GetValue("ICC/Plugin/CTI/CTITestPort", "21050");
		if (!l_strCTITestPort.empty())
		{
			m_nCTITestPort = atoi(l_strCTITestPort.c_str());
		}

		std::map<std::string, std::string>::const_iterator itr;
		for ( itr = m_mapDevices.begin(); itr != m_mapDevices.end(); ++itr)
		{
			ICC_LOG_DEBUG(m_pLog, "mapdevice device: [%s]", itr->first.c_str());
		}

		ICC_LOG_DEBUG(m_pLog, "mapdevice size: [%d]", m_mapDevices.size());

		
	}	// End if m_bParamterIsLoad
}

void CSysConfig::_AddDeviceNumber(const std::string& strNumber)
{
	if (strNumber != "")
	{
		m_mapDevices[strNumber] = strNumber;
	}	
}

bool CSysConfig::_ExistDeviceNumber(const std::string& strNumber)
{
	if (m_mapDevices.find(strNumber) != m_mapDevices.end())
	{
		return true;
	}
	return false;
}

void CSysConfig::ClearParamter()
{
	m_DevicePhoneList.clear();
	m_DeviceVirList.clear();
	m_ACDGroupList.clear();

	m_bParamterIsLoad = false;
}



std::string CSysConfig::GetCheckNum(const std::string& p_strSrcNum)
{
	

	std::string l_strOutNum = p_strSrcNum;

	int l_nLen = l_strOutNum.length();
	for (auto l_oRule : m_CheckNumList)
	{
		// 1、输入号码的长度等于规则预设的长度（如果规则的长度为0，则匹配）
		// 2、输入号码的长度需大于规则预设的移除长度
		// 3、规则预设的字头匹配输入号码的字头
		// 4、输出号码 = 规则预设的添加的字头 + 移除字头后的输入号码
		if ((l_nLen == l_oRule.m_nSrcNumLen || 0 == l_oRule.m_nSrcNumLen) && l_nLen > l_oRule.m_nRemoveLen && l_strOutNum.find(l_oRule.m_strPreNum) == 0)
		{
			l_strOutNum = l_oRule.m_strAddPre + l_strOutNum.substr(l_oRule.m_nRemoveLen);
			ICC_LOG_DEBUG(m_pLog, "CheckNum, SrcNum: [%s] to DestNum: [%s]", p_strSrcNum.c_str(), l_strOutNum.c_str());

			break;
		}
	}

	return l_strOutNum;
}



std::string CSysConfig::GetOutCallNumEx(const std::string& p_strSrcNum)
{
	std::string l_strOutNum = p_strSrcNum;	

	ICC_LOG_DEBUG(m_pLog, "GetOutCallNumEx get call out number, SrcNum: [%s]", p_strSrcNum.c_str());

	int l_nLen = l_strOutNum.length();
	for (auto l_oRule : m_DialRuleList)
	{
		// 1、输入号码的长度等于规则预设的长度（如果规则的长度为0，则匹配）
		// 2、输入号码的长度需大于规则预设的移除长度
		// 3、规则预设的字头匹配输入号码的字头
		// 4、输出号码 = 规则预设的添加的字头 + 移除字头后的输入号码
		if ((l_nLen == l_oRule.m_nSrcNumLen || 0 == l_oRule.m_nSrcNumLen) && l_nLen > l_oRule.m_nRemoveLen && l_strOutNum.find(l_oRule.m_strPreNum) == 0)
		{
			l_strOutNum = l_oRule.m_strAddPre + l_strOutNum.substr(l_oRule.m_nRemoveLen);
			ICC_LOG_DEBUG(m_pLog, "DialRule, SrcNum: [%s] to DestNum: [%s]", p_strSrcNum.c_str(), l_strOutNum.c_str());

			break;
		}
	}

	if (l_strOutNum == p_strSrcNum)
	{
		if (m_strEnbaleConfigCalloutCheck == "1")
		{
			if (!_ExistDeviceNumber(l_strOutNum))
			{
				l_strOutNum = m_strOutCallHead + p_strSrcNum;
				ICC_LOG_DEBUG(m_pLog, "check call out number by devices, SrcNum: [%s] to DestNum: [%s]", p_strSrcNum.c_str(), l_strOutNum.c_str());
				return l_strOutNum;
			}
			else
			{
				return l_strOutNum;
			}
		}
	}

	return l_strOutNum;
}
bool CSysConfig::GetOutCallNum(const std::string& p_strSrcNum, std::string& p_strDestNum)
{
	std::string l_strOutCallHead = "";
	p_strDestNum = p_strSrcNum;

	if (p_strSrcNum == "119" || p_strSrcNum == "120" || p_strSrcNum == "12122" || p_strSrcNum == "12345")
	{
		if (m_pRedisClient->HGet(CTI_OUTCALL_HEAD, p_strSrcNum, l_strOutCallHead) && !l_strOutCallHead.empty())
		{
			p_strDestNum = l_strOutCallHead + p_strSrcNum;
			ICC_LOG_DEBUG(m_pLog, "Zone Add OutCallHead, SrcNum: [%s] to DestNum: [%s]", p_strSrcNum.c_str(), p_strDestNum.c_str());
			return true;
		}
	}

	if (p_strSrcNum.length() < m_nMinLocalPhoneLen)
	{
		// 非公网号码
		return false;
	}

	int nNumLen = p_strSrcNum.length();
	if (nNumLen == m_nAddrZeroPreLen)
	{
		//std::string l_strZero = p_strSrcNum.substr(0, 1);
		//if (l_strZero != "0")
		/*{
			p_strDestNum = "0" + p_strSrcNum;
			ICC_LOG_DEBUG(m_pLog, "Zone Add OutCallHead, SrcNum: [%s] to DestNum: [%s]", p_strSrcNum.c_str(), p_strDestNum.c_str());
			return true;
		}*/
	}
	

	{ // 3位区号,座机
		std::string l_strZone = p_strSrcNum.substr(0, m_nMinZoneLen);

		if (m_pRedisClient->HGet(CTI_OUTCALL_HEAD, l_strZone, l_strOutCallHead) && !l_strOutCallHead.empty())
		{
			if (m_nMinLocalPhoneLen + m_nMinZoneLen >= p_strSrcNum.length())
			{
				p_strDestNum = l_strOutCallHead + p_strSrcNum;
				ICC_LOG_DEBUG(m_pLog, "Zone Add OutCallHead, SrcNum: [%s] to DestNum: [%s]", p_strSrcNum.c_str(), p_strDestNum.c_str());

				return true;
			}
		}
	}

	{ // 4位区号，座机
		std::string l_strZone = p_strSrcNum.substr(0, m_nMaxZoneLen);

		if (m_pRedisClient->HGet(CTI_OUTCALL_HEAD, l_strZone, l_strOutCallHead) && !l_strOutCallHead.empty())
		{
			if (m_nMinLocalPhoneLen + m_nMaxZoneLen >= p_strSrcNum.length())
			{
				p_strDestNum = l_strOutCallHead + p_strSrcNum;
				ICC_LOG_DEBUG(m_pLog, "Zone Add OutCallHead, SrcNum: [%s] to DestNum: [%s]", p_strSrcNum.c_str(), p_strDestNum.c_str());

				return true;
			}
			
		}
	}

	{ // 7位前缀，手机
		std::string l_strMobilePre = p_strSrcNum.substr(0, m_nMobilePreLen);

		if (m_pRedisClient->HGet(CTI_OUTCALL_HEAD, l_strMobilePre, l_strOutCallHead) && !l_strOutCallHead.empty())
		{
			p_strDestNum = l_strOutCallHead + p_strSrcNum;
			ICC_LOG_DEBUG(m_pLog, "MobilePre Add OutCallHead, SrcNum: [%s] to DestNum: [%s]", p_strSrcNum.c_str(), p_strDestNum.c_str());

			return true;
		}
	}

	return false;
}

bool CSysConfig::FindDefaultReadyAgent(const std::string& p_strAgent)
{
	bool l_bRet = false;

	for (auto l_strAgent : m_DefaultReadyAgentList)
	{
		if (l_strAgent.compare(p_strAgent) == 0)
		{
			l_bRet = true;

			break;
		}
	}

	return l_bRet;
}

void CSysConfig::_LoadMergeHuntGrpConver()
{
	int l_nCount = m_ConfigPtr->GetNodeCount("ICC/Plugin/CTI/GrpMergeConvers", "GrpMergeConver");
	for (int i = 0; i < l_nCount; i++)
	{
		std::string l_strPath = m_StringUtilPtr->Format("ICC/Plugin/CTI/GrpMergeConvers/GrpMergeConver[%d]", i);

		std::string l_strNumber = m_ConfigPtr->GetValue(l_strPath + "/Number", "");
		std::string l_strConverACD = m_ConfigPtr->GetValue(l_strPath + "/ConverACD", "");
		if (!l_strConverACD.empty() && !l_strNumber.empty())
		{
			ICC_LOG_DEBUG(m_pLog, "Number:[%s]-->ConverACD:[%s]", l_strNumber.c_str(),l_strConverACD.c_str());
			m_mapMergeHuntGrpConvers[l_strNumber] = l_strConverACD;
		}
	}
}

bool CSysConfig::GetMergeHuntGrpConver(std::string& p_strDestAcd, const std::string& p_strSrcNum)
{
	bool l_bFind = false;
	auto it = m_mapMergeHuntGrpConvers.find(p_strSrcNum);
	if (it != m_mapMergeHuntGrpConvers.end())
	{
		l_bFind = true;
		p_strDestAcd = it->second;
	}
	return l_bFind;
}

void CSysConfig::_LoadCheckNumList()
{
	std::list<CDialRule>	lstCheckNumberList;

	int l_nCheckNumNodeCount = m_ConfigPtr->GetNodeCount("ICC/Plugin/CTI/CheckNumbers", "CheckNumber");
	for (int i = 0; i < l_nCheckNumNodeCount; ++i)
	{
		std::string l_stPrePath = "ICC/Plugin/CTI/CheckNumbers/CheckNumber[" + std::to_string(i) + "]";

		std::string l_strLen = m_ConfigPtr->GetValue(l_stPrePath + "/Len", "");
		std::string l_strPrefix = m_ConfigPtr->GetValue(l_stPrePath + "/Prefix", "");
		std::string l_strRemoveHeadLen = m_ConfigPtr->GetValue(l_stPrePath + "/RemoveHeadLen", "");
		std::string l_strAddPrefix = m_ConfigPtr->GetValue(l_stPrePath + "/AddPrefix", "");

		CDialRule l_tempRule;
		l_tempRule.m_nSrcNumLen = atoi(l_strLen.c_str());
		l_tempRule.m_strPreNum = l_strPrefix;
		l_tempRule.m_nRemoveLen = atoi(l_strRemoveHeadLen.c_str());
		l_tempRule.m_strAddPre = l_strAddPrefix;
		lstCheckNumberList.push_back(l_tempRule);

		ICC_LOG_DEBUG(m_pLog, "Load CheckNums, SrcNumLen: [%d], PreNum: [%s], RemoveLen: [%d], AddPre: [%s]",
			l_tempRule.m_nSrcNumLen, l_tempRule.m_strPreNum.c_str(), l_tempRule.m_nRemoveLen, l_tempRule.m_strAddPre.c_str());
	}

	{
		std::lock_guard<std::mutex> lock(m_mutexLoadCheckNumList);
		m_CheckNumList = lstCheckNumberList;
	}
}

void CSysConfig::_LoadDialRuleList()
{
	std::list<CDialRule>	lstDialRuleList;

	std::string l_strDialRuleList = m_ConfigPtr->GetValue("ICC/Plugin/CTI/DialRules", "");
	std::vector<std::string> l_vDialRule;
	m_StringUtilPtr->Split(l_strDialRuleList, ";", l_vDialRule, false);
	for (auto l_oDialRule : l_vDialRule)
	{
		std::string l_strRule = l_oDialRule;
		std::vector<std::string> l_vRuleObj;
		m_StringUtilPtr->Split(l_strRule, ",", l_vRuleObj, false);
		const unsigned int l_iCorrectSize = 4;
		if (l_vRuleObj.size() == l_iCorrectSize)
		{
			CDialRule l_tempRule;
			l_tempRule.m_nSrcNumLen = atoi(l_vRuleObj[0].c_str());
			l_tempRule.m_strPreNum = l_vRuleObj[1];
			l_tempRule.m_nRemoveLen = atoi(l_vRuleObj[2].c_str());
			l_tempRule.m_strAddPre = l_vRuleObj[3];
			lstDialRuleList.push_back(l_tempRule);

			ICC_LOG_DEBUG(m_pLog, "Load DialRule, SrcNumLen: [%d], PreNum: [%s], RemoveLen: [%d], AddPre: [%s]",
				l_tempRule.m_nSrcNumLen, l_tempRule.m_strPreNum.c_str(), l_tempRule.m_nRemoveLen, l_tempRule.m_strAddPre.c_str());
		}
	}

	{
		std::lock_guard<std::mutex> lock(m_mutexLoadCheckNumList);
		m_DialRuleList = lstDialRuleList;
	}
}

std::string CSysConfig::GetLocalIP()
{
	std::string l_strIpAddress = m_ConfigPtr->GetValue("ICC/Component/HttpServer/LocalAddress", "0.0.0.0");
	
	return l_strIpAddress;
}

std::string CSysConfig::GetHCPConnectLocalIP()
{
	std::string l_strIpAddress = m_ConfigPtr->GetValue("ICC/Plugin/FSAESClient/FsCTIServerIpAddress", "");
	return l_strIpAddress;
}

std::string CSysConfig::GetCTIServerPort()
{
	return m_ConfigPtr->GetValue("ICC/Plugin/FSAESClient/FsCTIServerPort", "21032");
}

bool CSysConfig::GetPhoneTransferEnable()
{
	std::string l_strEnable = m_ConfigPtr->GetValue("ICC/Plugin/FSAESClient/PhoneTransferEnable", "1");
	if (l_strEnable == "1")
	{
		return true;
	}

	return false;
}



