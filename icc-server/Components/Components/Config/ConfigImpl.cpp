#include "Boost.h"
#include "ConfigImpl.h"
#include <boost/filesystem/operations.hpp>

std::string ICC::CConfigImpl::m_strServerFlag = "";

CConfigImpl::CConfigImpl(IResourceManagerPtr p_ResourceManagerPtr) :m_pResourceManager(p_ResourceManagerPtr)
{
	m_pString = ICCGetIStringFactory()->CreateString();

	IConfigPtr l_pConfigConfig = ICCGetIConfigFactory()->CreateConfigConfig();
	m_strFilePath = l_pConfigConfig->GetValue("ICC.ConfigPath", "./Config/Config.xml");

	m_XmlPtr = ICCGetIXmlFactory()->CreateXml();
	if (!m_XmlPtr->LoadFile(m_strFilePath))
	{
		std::cout << "load config file failed." << std::endl;
	}
	else
	{
		boost::filesystem::path p(m_strFilePath);
		m_tmLastUpdateTime = boost::filesystem::last_write_time(p);
	}	

	std::string l_strConfigFileDir = l_pConfigConfig->GetValue("ICC.ConfigFileDir", "./Config/");
	std::string l_strFilePath;

	//加载公共配置xml
	m_XmlRedisPtr = ICCGetIXmlFactory()->CreateXml();
	l_strFilePath = l_strConfigFileDir + "public-command-redis.xml";
	if (!m_XmlRedisPtr->LoadFile(l_strFilePath))
	{
		std::cout << "load redis config file failed." + l_strFilePath << std::endl;
	}

	m_XmlDBPtr = ICCGetIXmlFactory()->CreateXml();
	l_strFilePath = l_strConfigFileDir + "public-command-database.xml";
	if (!m_XmlDBPtr->LoadFile(l_strFilePath))
	{
		std::cout << "load DB config file failed." + l_strFilePath << std::endl;
	}

	m_XmlMQPtr = ICCGetIXmlFactory()->CreateXml();
	l_strFilePath = l_strConfigFileDir + "public-command-activemq.xml";
	if (!m_XmlMQPtr->LoadFile(l_strFilePath))
	{
		std::cout << "load DB config file failed." + l_strFilePath << std::endl;
	}

}

CConfigImpl::~CConfigImpl()
{
	
}

void CConfigImpl::_SetXmlPtr(Xml::IXmlPtr pXmlPtr)
{
	std::lock_guard<std::mutex> lock(m_mutexXml);
	m_XmlPtr = pXmlPtr;
}

Xml::IXmlPtr CConfigImpl::_GetXmlPtr()
{
	std::lock_guard<std::mutex> lock(m_mutexXml);
	return m_XmlPtr;
}

bool CConfigImpl::ReloadConfig()
{	
	boost::filesystem::path p(m_strFilePath);
	if (!boost::filesystem::exists(p))
	{
		std::cout << "not find config file." << std::endl;
		return false;
	}

	std::time_t t = boost::filesystem::last_write_time(p);
	if (m_tmLastUpdateTime == t)
	{
		std::cout << "config file update time is not modify, not neet reload." << std::endl;
		return false;
	}

	Xml::IXmlPtr  pXmlPtr = ICCGetIXmlFactory()->CreateXml();
	if (nullptr == pXmlPtr)
	{
		std::cout << "load config file failed." << std::endl;
		return false;
	}

	if (!pXmlPtr->LoadFile(m_strFilePath))
	{
		std::cout << "load config file failed." << std::endl;
		return false;
	}
	
	_SetXmlPtr(pXmlPtr);

	m_tmLastUpdateTime = t;

	return true;
}

void CConfigImpl::SetServerFlag(const std::string& strServerFlag)
{
	m_strServerFlag = strServerFlag;
}

std::string CConfigImpl::ServerFlag()
{
	return m_strServerFlag;
}

std::string CConfigImpl::GetValue(std::string p_strNodePath, std::string p_strDefault)
{
	Xml::IXmlPtr pXmlPtr = _GetXmlPtr();

	if (p_strNodePath == "ICC/Component/AmqClient/BrokerURI")
	{
		std::string l_strServers;
		int l_iPublicCount = 0;
		if (m_XmlMQPtr)
		{
			l_iPublicCount = m_XmlMQPtr->GetChildCount("PublicConfig/Activemq", "Servers");
		}
		if (l_iPublicCount > 0)
		{
			for (int i = 0; i < l_iPublicCount; i++)
			{
				std::string l_strPath = m_pString->Format("PublicConfig/Activemq/Servers[%d]", i);
				std::string l_strHost = m_XmlMQPtr->GetText(l_strPath + "/Host", "");
				std::string l_strPort = m_XmlMQPtr->GetText(l_strPath + "/Port", "");
				std::string l_strConnectionTimeout = "3000";
				std::string l_strMaxInactivityDuration = "9000";

				std::string l_strServer = m_pString->Format("tcp://%s:%s?transport.ConnectTimeout=%s&wireFormat.maxInactivityDuration=%s",
					l_strHost.c_str(), l_strPort.c_str(), l_strConnectionTimeout.c_str(), l_strMaxInactivityDuration.c_str());

				l_strServers += l_strServer;
				if (i < l_iPublicCount - 1)
				{
					l_strServers += ",";
				}
				printf("read MQ public Config l_strHost=%s,l_strPort=%s,l_iPublicCount=%d!\n", l_strHost.c_str(), l_strPort.c_str(), l_iPublicCount);
			}
			
		}
		else
		{
			int l_iCount = pXmlPtr->GetChildCount("ICC/Component/AmqClient/Servers", "Server");
			for (int i = 0; i < l_iCount; i++)
			{
				std::string l_strPath = m_pString->Format("ICC/Component/AmqClient/Servers/Server[%d]", i);

				std::string l_strHost = pXmlPtr->GetText(l_strPath + "/Host", "");
				std::string l_strPort = pXmlPtr->GetText(l_strPath + "/Port", "");
				std::string l_strConnectionTimeout = pXmlPtr->GetText(l_strPath + "/ConnectionTimeout", "");
				std::string l_strMaxInactivityDuration = pXmlPtr->GetText(l_strPath + "/MaxInactivityDuration", "");
				if (l_strConnectionTimeout.empty())
				{
					l_strConnectionTimeout = "3000";
				}
				if (l_strMaxInactivityDuration.empty())
				{
					l_strMaxInactivityDuration = "9000";
				}
				std::string l_strServer = m_pString->Format("tcp://%s:%s?transport.ConnectTimeout=%s&wireFormat.maxInactivityDuration=%s",
					l_strHost.c_str(), l_strPort.c_str(), l_strConnectionTimeout.c_str(), l_strMaxInactivityDuration.c_str());

				l_strServers += l_strServer;
				if (i < l_iCount - 1)
				{
					l_strServers += ",";
				}
			}
		}
		

		if (l_strServers.empty())
		{
			l_strServers = p_strDefault;
		}

		std::string l_strFailoverTimeout = pXmlPtr->GetText("ICC/Component/AmqClient/FailoverTimeout", "");
		std::string l_strTail = m_pString->Format("transport.timeout=%s", l_strFailoverTimeout.c_str());
		
		std::string l_strBrokerURI = m_pString->Format("failover:(%s)?%s", l_strServers.c_str(), l_strTail.c_str());

		printf("get MQ BrokerURI success! BrokerURI = %s\n", l_strBrokerURI.c_str());

		return l_strBrokerURI;
	}

	if (p_strNodePath == "ICC/Component/RedisClient/Servers")
	{		
		std::string l_strServers;
		int l_iPublicCount = 0;
		if (m_XmlRedisPtr)
		{
			l_iPublicCount = m_XmlRedisPtr->GetChildCount("PublicConfig/Redis", "Servers");
		}
		if (l_iPublicCount > 0)
		{
			for (int i = 0; i < l_iPublicCount; i++)
			{
				std::string l_strPath = m_pString->Format("PublicConfig/Redis/Servers[%d]", i);
				std::string l_strHost = m_XmlRedisPtr->GetText(l_strPath + "/Host", "");
				std::string l_strPort = m_XmlRedisPtr->GetText(l_strPath + "/Port", "");
				std::string l_strPsw = m_XmlRedisPtr->GetText(l_strPath + "/Password", "Command_123");

				std::string l_strServer = m_pString->Format("%s:%s#%s", l_strHost.c_str(), l_strPort.c_str(), l_strPsw.c_str());
				l_strServers += l_strServer;
				if (i < l_iPublicCount - 1)
				{
					l_strServers += ";";
				}
				printf("read Redis public Config l_strHost=%s,l_strPort=%s,l_strPsw=%s,l_iPublicCount=%d!\n", l_strHost.c_str(), l_strPort.c_str(), l_strPsw.c_str(), l_iPublicCount);
				//std::string l_strLocalPath = m_pString->Format("ICC/Component/RedisClient/Servers/Server[%d]", i);
				//if (m_XmlPtr)
				//{
				//	m_XmlPtr->SetText(l_strLocalPath + "/Host", l_strHost);
				//	m_XmlPtr->SetText(l_strLocalPath + "/Port", l_strPort);
				//  m_XmlPtr->SetText(l_strLocalPath + "/Password", l_strPsw);
				//}
			}
		}
		else
		{
			int l_iCount = 0;
			if (m_XmlPtr)
			{
				l_iCount = pXmlPtr->GetChildCount("ICC/Component/RedisClient/Servers", "Server");
			}
			else
			{
				printf("xml pointer is null!\n");
			}

			for (int i = 0; i < l_iCount; i++)
			{
				std::string l_strPath = m_pString->Format("ICC/Component/RedisClient/Servers/Server[%d]", i);

				std::string l_strHost = pXmlPtr->GetText(l_strPath + "/Host", "");
				std::string l_strPort = pXmlPtr->GetText(l_strPath + "/Port", "");
				std::string l_strPsw = pXmlPtr->GetText(l_strPath + "/Password", "");

				std::string l_strServer = m_pString->Format("%s:%s#%s", l_strHost.c_str(), l_strPort.c_str(), l_strPsw.c_str());

				l_strServers += l_strServer;
				if (i < l_iCount - 1)
				{
					l_strServers += ";";
				}
			}
		}
		

		if (l_strServers.empty())
		{
			l_strServers = p_strDefault;
		}

        printf("get redis client info success! server = %s\n", l_strServers.c_str());
		return l_strServers;
	}

	if (p_strNodePath == "ICC/Component/RedisClient/Cluster")
	{
		int l_iPublicCount = 0;
		if (m_XmlRedisPtr)
		{
			l_iPublicCount = m_XmlRedisPtr->GetChildCount("PublicConfig/Redis", "Servers");
		}

		if (l_iPublicCount > 0)
		{
			if (l_iPublicCount < 2)
			{
				return "0";
			}
			return "1";
		}
		else
		{
			int l_iCount = pXmlPtr->GetChildCount("ICC/Component/RedisClient/Servers", "Server");
			if (l_iCount < 2)
			{
				return "0";
			}
			return "1";
		}
	}

	if (p_strNodePath == "ICC/Plugin/SMT/SMTUrl")
	{
		std::string l_strHost = pXmlPtr->GetText("ICC/Plugin/SMT/Host", "127.0.0.1");
		std::string l_strPort = pXmlPtr->GetText("ICC/Plugin/SMT/Port", "26410");

		std::string l_strHostSlave = pXmlPtr->GetText("ICC/Plugin/SMT/HostSlave", "");
		std::string l_strPortSlave = pXmlPtr->GetText("ICC/Plugin/SMT/PortSlave", "");

		std::string l_strSMTUrl = m_pString->Format("http://%s:%s/smt-agent/systemMonitorToolAgentService/monitor", 
			l_strHost.c_str(), l_strPort.c_str());
		if (!l_strHostSlave.empty() && !l_strPortSlave.empty())
		{
			std::string l_strSlaveUrl = m_pString->Format("http://%s:%s/smt-agent/systemMonitorToolAgentService/monitor", 
				l_strHostSlave.c_str(), l_strPortSlave.c_str());

			l_strSMTUrl += ";" + l_strSlaveUrl;
		}

		return l_strSMTUrl;
	}

	if (p_strNodePath == "ICC/Plugin/LSP/LSPUrl")
	{
		std::string l_strHost = pXmlPtr->GetText("ICC/Plugin/LSP/Host", "127.0.0.1");
		std::string l_strPort = pXmlPtr->GetText("ICC/Plugin/LSP/Port", "22170");

		std::string l_strHostSlave = pXmlPtr->GetText("ICC/Plugin/LSP/HostSlave", "");
		std::string l_strPortSlave = pXmlPtr->GetText("ICC/Plugin/LSP/PortSlave", "");

		std::string l_strLSPUrl = m_pString->Format("http://%s:%s/api/v2/events",
			l_strHost.c_str(), l_strPort.c_str());

		if (!l_strHostSlave.empty() && !l_strPortSlave.empty())
		{
			std::string l_strSlaveUrl = m_pString->Format("http://%s:%s/api/v2/events",
				l_strHostSlave.c_str(), l_strPortSlave.c_str());

			l_strLSPUrl += ";" + l_strSlaveUrl;
		}

		return l_strLSPUrl;
	}

	if (p_strNodePath == "ICC/Plugin/Fast110/Members")
	{
		std::string l_strMembers;
		int l_iCount = pXmlPtr->GetChildCount("ICC/Plugin/Fast110/Members", "Member");
		for (int i = 0; i < l_iCount; i++)
		{
			std::string l_strPath = m_pString->Format("ICC/Plugin/Fast110/Members/Member[%d]", i);

			std::string l_strExtNo = pXmlPtr->GetText(l_strPath + "/ExtNo", "");
			std::string l_strOutNo = pXmlPtr->GetText(l_strPath + "/OutNo", "");
			std::string l_strSipNo = pXmlPtr->GetText(l_strPath + "/SipNo", "");

			std::string l_strMember = m_pString->Format("%s(%s%s)", l_strExtNo.c_str(), l_strOutNo.c_str(), l_strSipNo.c_str());

			l_strMembers += l_strMember;
			if (i < l_iCount - 1)
			{
				l_strMembers += ";";
			}
		}

		if (l_strMembers.empty())
		{
			l_strMembers = p_strDefault;
		}

		return l_strMembers;
	}

	if (p_strNodePath == "ICC/Plugin/SMP/DeptCodeMappings")
	{
		std::string l_strDeptCodeMappings;
		int l_iCount = pXmlPtr->GetChildCount("ICC/Plugin/SMP/DeptCodeMappings", "DeptCodeMapping");
		for (int i = 0; i < l_iCount; i++)
		{
			std::string l_strPath = m_pString->Format("ICC/Plugin/SMP/DeptCodeMappings/DeptCodeMapping[%d]", i);

			std::string l_strBeforeMap = pXmlPtr->GetText(l_strPath + "/BeforeMap", "");
			std::string l_strAfterMap = pXmlPtr->GetText(l_strPath + "/AfterMap", "");

			std::string l_strDeptCodeMapping = m_pString->Format("%s(%s)", l_strBeforeMap.c_str(), l_strAfterMap.c_str());

			l_strDeptCodeMappings += l_strDeptCodeMapping;
			if (i < l_iCount - 1)
			{
				l_strDeptCodeMappings += ";";
			}
		}

		if (l_strDeptCodeMappings.empty())
		{
			l_strDeptCodeMappings = p_strDefault;
		}

		return l_strDeptCodeMappings;
	}

	if (p_strNodePath == "ICC/Plugin/WeChatST/ProcessResults")
	{
		std::string l_strProcessResults;
		int l_iCount = pXmlPtr->GetChildCount("ICC/Plugin/WeChatST/ProcessResults", "ProcessResult");
		for (int i = 0; i < l_iCount; i++)
		{
			std::string l_strPath = m_pString->Format("ICC/Plugin/WeChatST/ProcessResults/ProcessResult[%d]", i);

			std::string l_strCode = pXmlPtr->GetText(l_strPath + "/Code", "");
			std::string l_strText = pXmlPtr->GetText(l_strPath + "/Text", "");

			std::string l_strProcessResult = m_pString->Format("%s(%s)", l_strCode.c_str(), l_strText.c_str());

			l_strProcessResults += l_strProcessResult;
			if (i < l_iCount - 1)
			{
				l_strProcessResults += ";";
			}
		}

		if (l_strProcessResults.empty())
		{
			l_strProcessResults = p_strDefault;
		}

		return l_strProcessResults;
	}

	if (p_strNodePath == "ICC/Plugin/TransportAlarm/DeptAcdMappings")
	{
		std::string l_strDeptAcdMappings;
		int l_iCount = pXmlPtr->GetChildCount("ICC/Plugin/TransportAlarm/DeptAcdMappings", "DeptAcdMapping");
		for (int i = 0; i < l_iCount; i++)
		{
			std::string l_strPath = m_pString->Format("ICC/Plugin/TransportAlarm/DeptAcdMappings/DeptAcdMapping[%d]", i);

			std::string l_strCode = pXmlPtr->GetText(l_strPath + "/Code", "");
			std::string l_strACD = pXmlPtr->GetText(l_strPath + "/ACD", "");

			std::string l_strDeptAcdMapping = m_pString->Format("%s(%s)", l_strCode.c_str(), l_strACD.c_str());

			l_strDeptAcdMappings += l_strDeptAcdMapping;
			if (i < l_iCount - 1)
			{
				l_strDeptAcdMappings += ";";
			}
		}

		if (l_strDeptAcdMappings.empty())
		{
			l_strDeptAcdMappings = p_strDefault;
		}

		return l_strDeptAcdMappings;
	}
		
	if (m_pString->StartsWith(p_strNodePath, "ICC/Plugin/CTI/Devices/Device["))
	{
		std::string l_strExtNo = pXmlPtr->GetText(p_strNodePath + "/ExtNo", "");
		std::string l_strType = pXmlPtr->GetText(p_strNodePath + "/Type", "");
		std::string l_strAgentId = pXmlPtr->GetText(p_strNodePath + "/AgentId", "");
		std::string l_strAgentPwd = pXmlPtr->GetText(p_strNodePath + "/AgentPwd", "");

		std::string l_strDevice = m_pString->Format("%s;%s;%s;%s",
			l_strExtNo.c_str(), l_strType.c_str(), l_strAgentId.c_str(), l_strAgentPwd.c_str());
		return l_strDevice;
	}

	if (m_pString->StartsWith(p_strNodePath, "ICC/Plugin/CTI/ACDGrpMembers/ACDGrpMember["))
	{
		std::string l_strACD = pXmlPtr->GetText(p_strNodePath + "/ACD", "");
		std::string l_strExt = pXmlPtr->GetText(p_strNodePath + "/Ext", "");

		std::string l_strACDGrpMember = m_pString->Format("%s#%s", l_strACD.c_str(), l_strExt.c_str());
		return l_strACDGrpMember;
	}

	if (p_strNodePath == "ICC/Plugin/CTI/ReadyAgents")
	{
		std::string l_strReadyAgents;
		int l_iCount = pXmlPtr->GetChildCount("ICC/Plugin/CTI/ReadyAgents", "ReadyAgent");
		for (int i = 0; i < l_iCount; i++)
		{
			std::string l_strPath = m_pString->Format("ICC/Plugin/CTI/ReadyAgents/ReadyAgent[%d]", i);
			std::string l_strReadyAgent = pXmlPtr->GetText(l_strPath, "");

			l_strReadyAgents += l_strReadyAgent;
			if (i < l_iCount - 1)
			{
				l_strReadyAgents += ";";
			}
		}

		if (l_strReadyAgents.empty())
		{
			l_strReadyAgents = p_strDefault;
		}

		return l_strReadyAgents;
	}

	if (p_strNodePath == "ICC/Plugin/CTI/DialRules")
	{
		std::string l_strDialRules;
		int l_iCount = pXmlPtr->GetChildCount("ICC/Plugin/CTI/DialRules", "DialRule");
		for (int i = 0; i < l_iCount; i++)
		{
			std::string l_strPath = m_pString->Format("ICC/Plugin/CTI/DialRules/DialRule[%d]", i);

			std::string l_strLen = pXmlPtr->GetText(l_strPath + "/Len", "");
			std::string l_strPrefix = pXmlPtr->GetText(l_strPath + "/Prefix", "");
			std::string l_strRemoveHeadLen = pXmlPtr->GetText(l_strPath + "/RemoveHeadLen", "");
			std::string l_strAddPrefix = pXmlPtr->GetText(l_strPath + "/AddPrefix", "");

			std::string l_strDialRule = m_pString->Format("%s,%s,%s,%s", 
				l_strLen.c_str(), l_strPrefix.c_str(), l_strRemoveHeadLen.c_str(), l_strAddPrefix.c_str());

			l_strDialRules += l_strDialRule;
			if (i < l_iCount - 1)
			{
				l_strDialRules += ";";
			}
		}

		if (l_strDialRules.empty())
		{
			l_strDialRules = p_strDefault;
		}

		return l_strDialRules;
	}

	if (p_strNodePath == "ICC/Plugin/CTI/ACDGrpConverList")
	{
		std::string l_strACDGrpConvers;
		int l_iCount = pXmlPtr->GetChildCount("ICC/Plugin/CTI/ACDGrpConvers", "ACDGrpConver");
		for (int i = 0; i < l_iCount; i++)
		{
			std::string l_strPath = m_pString->Format("ICC/Plugin/CTI/ACDGrpConvers/ACDGrpConver[%d]", i);

			std::string l_strACD = pXmlPtr->GetText(l_strPath + "/ACD", "");
			std::string l_strAlias = pXmlPtr->GetText(l_strPath + "/Alias", "");

			std::string l_strACDGrpConver = m_pString->Format("%s(%s)",
				l_strACD.c_str(), l_strAlias.c_str());

			l_strACDGrpConvers += l_strACDGrpConver;
			if (i < l_iCount - 1)
			{
				l_strACDGrpConvers += ";";
			}
		}

		if (l_strACDGrpConvers.empty())
		{
			l_strACDGrpConvers = p_strDefault;
		}

		return l_strACDGrpConvers;
	}

	//先从公共配置读MQ的用户名密码，读不到没有从confg.xml读取
	if (p_strNodePath == "ICC/Component/AmqClient/UserName")
	{
		std::string l_strTmpName;
		if (m_XmlMQPtr)
		{
			l_strTmpName = m_XmlMQPtr->GetText("PublicConfig/Activemq/Username", "");
			if (!l_strTmpName.empty())
			{
				printf("read MQ public Config Username=%s!\n", l_strTmpName.c_str());
				return l_strTmpName;
			}
		}
	}
	else if (p_strNodePath == "ICC/Component/AmqClient/Password")
	{
		std::string l_strTmpPswd;
		if (m_XmlMQPtr)
		{
			l_strTmpPswd = m_XmlMQPtr->GetText("PublicConfig/Activemq/Password", "");
			if (!l_strTmpPswd.empty())
			{
				printf("read MQ public Config Password=%s!\n", l_strTmpPswd.c_str());
				return l_strTmpPswd;
			}
		}
	}

	return pXmlPtr->GetText(p_strNodePath, p_strDefault);
}

bool CConfigImpl::SetValue(std::string p_strNodePath, std::string p_strValue)
{
	Xml::IXmlPtr pXmlPtr = _GetXmlPtr();
	pXmlPtr->SetText(p_strNodePath, p_strValue);
	return pXmlPtr->SaveAs(m_strFilePath);
}

unsigned int CConfigImpl::GetNodeCount(std::string p_strNodePath, std::string p_strNodeName)
{
	Xml::IXmlPtr pXmlPtr = _GetXmlPtr();
	return pXmlPtr->GetChildCount(p_strNodePath, p_strNodeName);
}

unsigned int CConfigImpl::GetPubicNodeCount(std::string p_strNodePath, std::string p_strNodeName)
{
	if (p_strNodePath == "ICC/Component/DBConn/Servers")
	{
		return m_XmlDBPtr->GetChildCount("PublicConfig/Database", "PG");
	}
	return 0;
}

std::string CConfigImpl::GetPublicValue(int p_nCfgType,std::string p_strNodePath, std::string p_strDefault)
{
	std::string l_strConfgInfo = "";
	if (p_nCfgType == 0)  //数据库公共配置
	{
		std::string l_strConfgInfo = m_XmlDBPtr->GetText(p_strNodePath, p_strDefault);
		printf("read DB public Config NodePath=%s,vaule=%s!\n", p_strNodePath.c_str(), l_strConfgInfo.c_str());
		return l_strConfgInfo;
	}
	return l_strConfgInfo;
}

IResourceManagerPtr CConfigImpl::GetResourceManager()
{
	return m_pResourceManager;
}