#include "Boost.h"
#include "UdpHelper.h"
#include "BusinessImpl.h"

#define TIMER_CMD_NAME "mrps_timer"

namespace ICC
{
	CUdpHelper::CUdpHelper(Log::ILogPtr	pLogPtr, HelpTool::IHelpToolPtr pHelpTool)
	{
		m_pHelpTool = pHelpTool;
		m_pLog = pLogPtr;		
		m_pParent = nullptr;
		m_bMaster = false;
		m_bBindSuccess = false;
		m_iSendErrorCount = 0;
		m_bIsRebindSendFailed = false;
		m_iSendFailedNum = 10;

		m_SendTimes = 3;
		m_IntervalTime = 3;
		m_CurrentTimes = 0;
		m_LoginTime = 0;
		


		m_pBuffer = new char[UDP_RECV_BUF_SIZE];
		memset(m_pBuffer, 0, UDP_RECV_BUF_SIZE);
	}

	CUdpHelper::~CUdpHelper()
	{
		if(m_pBuffer)
		{
			delete[] m_pBuffer;
			m_pBuffer = nullptr;
		}
	}


	void CUdpHelper::SetSendTimes(unsigned int iSendTimes, unsigned int iIntervalTime)
	{ 
		m_SendTimes = iSendTimes;
		m_IntervalTime = iIntervalTime;
		m_CurrentTimes = 0;
	}
	void CUdpHelper::SetDateTimePtr(DateTime::IDateTimePtr pDateTimePtr)
	{ 
		m_pDateTime = pDateTimePtr; 
	}
	void CUdpHelper::SetStringUtilPtr(StringUtil::IStringUtilPtr pStringUtilPtr)
	{ 
		m_pString = pStringUtilPtr; 
	}
	void CUdpHelper::SetXmlFactoryPtr(Xml::IXmlFactoryPtr pXmlFactoryPtr)
	{ 
		m_pXmlFty = pXmlFactoryPtr; 
	}
	void CUdpHelper::SetJsonFactoryPtr(JsonParser::IJsonFactoryPtr pJsonFty)
	{ 
		m_pJsonFty = pJsonFty; 
	}
	void CUdpHelper::SetObserverCenterPtr(ObserverPattern::IObserverCenterPtr pObserverCenter)
	{ 
		m_pObserverCenter = pObserverCenter; 
	}

	void CUdpHelper::SetLockFtyPtr(Lock::ILockFactoryPtr pLockFty)
	{
		m_pLockFty = pLockFty;
	}

	void CUdpHelper::OnStart()
	{
		//ICC_DICTCODE表相关协议
		_func_map["cti_login"] = &CUdpHelper::Login;
		_func_map["cti_logout"] = &CUdpHelper::Logout;
		_func_map["cti_heartbeat"] = &CUdpHelper::Heartbeat;
		_func_map["cti_callmember_notice_ack"] = &CUdpHelper::NoticeAck;
		_func_map["cti_callinfo_notice_ack"] = &CUdpHelper::NoticeAck;
		_func_map["cti_caseinfo_notice_ack"] = &CUdpHelper::NoticeAck;

		m_pClientLock = m_pLockFty->CreateLock(Lock::TypeRecursiveMutex);
		m_pMsgLock = m_pLockFty->CreateLock(Lock::TypeRecursiveMutex);
	}

	void CUdpHelper::OnStop()
	{

	}

	void CUdpHelper::OnReport(int iCode, const std::string& strMessage)
	{
		ICC_LOG_DEBUG(m_pLog, "receive udp report message :code:[%d],msg:[%s]", iCode, strMessage.c_str());

		if (iCode == Udp::UDP_RESULT_SUCCESS)
		{
			m_bBindSuccess = true;
		}
		else
		{
			m_bBindSuccess = false;
		}

		if (m_pParent)
		{
			m_pParent->OnReport(iCode, strMessage);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "parent is null!!! not send report message");
		}		
	}

	void CUdpHelper::OnReceived(const char* p_pData, unsigned int p_iLength, std::string p_strRemoteIP, size_t p_iPort)
	{
		memset(m_pBuffer, 0, UDP_RECV_BUF_SIZE);
		memcpy(m_pBuffer, p_pData + sizeof(unsigned short), p_iLength - sizeof(unsigned short));

		//short l_sNetLen = 0;
		//char Msgbuf[UDP_RECV_BUF_SIZE] = {0};
		//memcpy(&l_sNetLen, p_pData, sizeof(short));
		//short l_sHostLen = m_pHelpTool->NetworkToHostShort(l_sNetLen);
		//memcpy(Msgbuf, p_pData + sizeof(short), l_sHostLen);

		ICC_LOG_DEBUG(m_pLog, "Receive Message:[%s]", m_pBuffer);

		if (!m_bMaster)
		{			
			ICC_LOG_WARNING(m_pLog, "is slave, not process message");
			return;
		}

		Xml::IXmlPtr l_pXml = m_pXmlFty->CreateXml();
		if(!l_pXml->LoadXml(m_pBuffer))
		{
			ICC_LOG_ERROR(m_pLog, "load receive Xml failed.");
			return;
		}

		std::string l_strCmd = l_pXml->GetText("hytera/cmd_name", STRING_EMPTY);
		ICC_LOG_ERROR(m_pLog, "load receive Xml l_strCmd. %s", l_strCmd.c_str());
		/*if (l_strCmd.compare("cti_heartbeat") == 0)
		{
			std::string l_strClinetName = l_pXml->GetText("hytera/name", STRING_EMPTY);
			CheckAuthhearttime(l_strClinetName);
			ICC_LOG_ERROR(m_pLog, "Receive user cti_heartbeat, update heart time %s ", l_strCmd.data());
		}*/

		if(l_strCmd.compare("cti_login") != 0 && !CheckAuthenticate(p_strRemoteIP, p_iPort))
		{
			return;
		}

		if(_func_map.find(l_strCmd) != _func_map.end())
		{  //如果在_func_map处理函数中找到处理方法，则进行处理,找到命令 则返回错。
			(this->*(_func_map[l_strCmd]))(l_pXml, p_strRemoteIP, p_iPort);
			return;
		}

		ICC_LOG_ERROR(m_pLog, "Not Handle Message[%s].", l_strCmd.data());
	}

	bool CUdpHelper::SendLoginResult(int i_result)
	{
		if(m_CurrentUser.empty())
		{
			ICC_LOG_DEBUG(m_pLog, "MRPS login exception. Not have current user.");
			return false;
		}

		Lock::AutoLock l_lk(m_pClientLock);
		auto l_it = m_UDPClientMap.find(m_CurrentUser);
		if(l_it == m_UDPClientMap.end())
		{
			ICC_LOG_DEBUG(m_pLog, "MRPS login exception. Can not find current user.");
			return false;
		}
		if(LOGIN_RESULT_SUCESS == i_result)	
		{
			l_it->second.auth = true;
			ICC_LOG_DEBUG(m_pLog, "MRPS Login Sucess, UserName[%s].", m_CurrentUser.data());
		}

		m_CurrentUser = STRING_EMPTY;
		std::string l_str = m_pString->Format(CTI_LOGIN_OUT_ACK, "cti_login_ack", l_it->second.guid.data(), 
			l_it->second.name.data(), i_result, UDPCurrentTime());
		ResponseXml(l_str, l_it->second.ip, l_it->second.port);
		return true;
	}

	bool CUdpHelper::Login(Xml::IXmlPtr l_pXml, std::string str_remoteIP, size_t iPort)
	{
		if(nullptr == l_pXml)
		{
			ICC_LOG_ERROR(m_pLog, "l_pXml is null");
			return false;
		}

		std::string l_guid = l_pXml->GetText("hytera/cmd_guid", STRING_EMPTY);
		std::string l_name = l_pXml->GetText("hytera/name", STRING_EMPTY);
		if(l_guid.empty() || l_name.empty())
		{//-------By_ZengW_2018/05/29 16:29  !m_CurrentUser.empty() 由于鉴权模块返回协议不带username参数，所以上次登录未返回认为无法登录。
			ICC_LOG_ERROR(m_pLog, "MRPS login user name or GUID is NULL.");
			std::string l_str = m_pString->Format(CTI_LOGIN_OUT_ACK, "cti_login_ack", l_guid.data(),
				l_name.data(), LOGIN_RESULT_UNKNOW, UDPCurrentTime());
			ResponseXml(l_str, str_remoteIP, iPort);
			return false;
		}

		Lock::AutoLock l_lk(m_pClientLock);
		auto l_it = m_UDPClientMap.find(l_name);
		if(l_it != m_UDPClientMap.end())
		{
			if(l_it->second.auth && UDPCurrentTime() - l_it->second.time < 60)
			{//假如已经登录，并且60秒有活动
				std::string l_password = l_pXml->GetText("hytera/password", STRING_EMPTY);
				if(l_it->second.ip.compare(str_remoteIP) == 0 && l_it->second.port == iPort && l_it->second.password.compare(l_password) == 0)
				{//允许重复登陆情况：相同IP、端口、用户名密码(断线重连)
					ICC_LOG_DEBUG(m_pLog, "MRPS Reconnection Sucess. UserName[%s], ip[%s] port[%d].", l_name.data(), str_remoteIP.data(), iPort);

					l_it->second.guid = l_guid;
					l_it->second.time = UDPCurrentTime();

					std::string l_str = m_pString->Format(CTI_LOGIN_OUT_ACK, "cti_login_ack", l_guid.data(),
						l_name.data(), LOGIN_RESULT_SUCESS, l_it->second.time);
					ResponseXml(l_str, str_remoteIP, iPort);
					return true;
				}

				ICC_LOG_ERROR(m_pLog, "MRPS Login UserName is already login, ip[%s] port[%d].", l_it->second.ip.data(), l_it->second.port);

				//--By z13060 2018/05/31 15:35 重复登陆，回复协议到发送的IP
				std::string l_str = m_pString->Format(CTI_LOGIN_OUT_ACK, "cti_login_ack", l_guid.data(),
					l_name.data(), LOGIN_RESULT_REPEAT, UDPCurrentTime());
				ResponseXml(l_str, str_remoteIP, iPort);
				return false;
			}
		}

		if(!m_CurrentUser.empty() && UDPCurrentTime() - m_LoginTime < 3)
		{//登陆3秒没有发送回复，不允许其他用户登陆。
			ICC_LOG_ERROR(m_pLog, "MRPS Login User[%s] Login not finish.", m_CurrentUser.data());
			std::string l_str = m_pString->Format(CTI_LOGIN_OUT_ACK, "cti_login_ack", l_guid.data(),
				l_name.data(), LOGIN_RESULT_UNKNOW, UDPCurrentTime());
			ResponseXml(l_str, str_remoteIP, iPort);
			return false;
		}


		m_CurrentUser = l_name;	//正在登录的用户
		m_LoginTime = UDPCurrentTime();

		UDPClient& l_UDPClient = m_UDPClientMap[l_name];
		l_UDPClient.name = l_name;
		l_UDPClient.guid = l_guid;
		l_UDPClient.password = l_pXml->GetText("hytera/password", STRING_EMPTY);
		l_UDPClient.ip = str_remoteIP;
		l_UDPClient.port = iPort;
		l_UDPClient.time = UDPCurrentTime();
		l_UDPClient.auth = false;

		//直接返回成功，不到 SMP 验证了
		return SendLoginResult(LOGIN_RESULT_SUCESS);

		/*PROTOCOL::CAuthCheckUserRequest p_pNotifiRequest;
		p_pNotifiRequest.m_oHeader.m_strSystemID = "icc_server";
		p_pNotifiRequest.m_oHeader.m_strSubsystemID = "MRPS";
		p_pNotifiRequest.m_oHeader.m_strMsgid = l_UDPClient.guid;
		p_pNotifiRequest.m_oHeader.m_strRelatedID = STRING_EMPTY;
		p_pNotifiRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		p_pNotifiRequest.m_oHeader.m_strCmd = "check_user_request";
		p_pNotifiRequest.m_oHeader.m_strRequest = "queue_auth";
		p_pNotifiRequest.m_oHeader.m_strRequestType = "0";
		p_pNotifiRequest.m_oHeader.m_strResponse = STRING_EMPTY;
		p_pNotifiRequest.m_oHeader.m_strResponseType = STRING_EMPTY;
		p_pNotifiRequest.m_oBody.m_strUser_Code = l_UDPClient.name;
		p_pNotifiRequest.m_oBody.m_strUser_Pwd = l_UDPClient.password;

		std::string l_strMessage = p_pNotifiRequest.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage, ObserverPattern::ERequestMode::Request_Respond));
		ICC_LOG_DEBUG(m_pLog, "MRPS Login Auth Request Send Content:[%s]", l_strMessage.c_str());
		return true;*/
	}

	bool CUdpHelper::Logout(Xml::IXmlPtr l_pXml, std::string str_remoteIP, size_t iPort)
	{
		if(nullptr == l_pXml)
		{
			ICC_LOG_ERROR(m_pLog, "l_pXml is null");
			return false;
		}

		std::string l_name = l_pXml->GetText("hytera/name", STRING_EMPTY);
		uint64 l_time = m_pString->ToUInt64(l_pXml->GetText("hytera/timestamp", "0"));
		unsigned int l_result = 1;

		auto l_it = m_UDPClientMap.find(l_name);
		if(l_it != m_UDPClientMap.end())
		{
			if(l_it->second.auth)
			{
				ICC_LOG_DEBUG(m_pLog, "MRPS Logout, User[%s] Online time:[%lld]s.", l_it->first.data(), l_time - l_it->second.time);
				m_UDPClientMap.erase(l_it);
				l_result = 0;
			}
		}

		std::string l_guid = l_pXml->GetText("hytera/cmd_guid", STRING_EMPTY);
		std::string l_str = m_pString->Format(CTI_LOGIN_OUT_ACK, "cti_logout_ack", l_guid.data(), l_name.data(), l_result, UDPCurrentTime());
		ResponseXml(l_str, str_remoteIP, iPort);
		return true;
	}

	bool CUdpHelper::Heartbeat(Xml::IXmlPtr l_pXml, std::string str_remoteIP, size_t iPort)
	{
		if(nullptr == l_pXml)
		{
			ICC_LOG_ERROR(m_pLog, "l_pXml is null");
			return false;
		}

		std::string l_guid = l_pXml->GetText("hytera/cmd_guid", STRING_EMPTY);
		std::string l_str = m_pString->Format(UDP_MESSAGE_ACK, "cti_heartbeat_ack", l_guid.data(), UDPCurrentTime());
		ResponseXml(l_str, str_remoteIP, iPort);
		return true;
	}

	bool CUdpHelper::NoticeAck(Xml::IXmlPtr l_pXml, std::string str_remoteIP, size_t iPort)
	{
		if(nullptr == l_pXml)
		{
			ICC_LOG_ERROR(m_pLog, "l_pXml is null");
			return false;
		}

		std::string l_guid = l_pXml->GetText("hytera/cmd_guid", STRING_EMPTY);
		if(!l_guid.empty())
		{
			Lock::AutoLock l_lk(m_pMsgLock);
			auto l_it = m_SendMsgMap.find(SENDMSG_KEY(str_remoteIP, iPort, l_guid));
			if(l_it != m_SendMsgMap.end())
			{
				m_SendMsgMap.erase(l_it);
			}
		}
	    return true;
	}

	void CUdpHelper::ResponseXml(const std::string &str_data, std::string str_remoteIP, size_t iPort)
	{
		if(str_remoteIP.empty() || 0 == iPort)
		{
			return;
		}
		ICC_LOG_DEBUG(m_pLog, "Send Message to UDP Client:ip[%s] port[%d], protocol:%s", str_remoteIP.data(), iPort, str_data.data());
		char szBuf[UDP_RECV_BUF_SIZE] = {0};
		unsigned short l_sHostLen = str_data.size();
		unsigned short l_sNetLen = m_pHelpTool->HostToNetworkShort(l_sHostLen);
		memcpy(szBuf, &l_sNetLen, sizeof(unsigned short));
		memcpy(szBuf + sizeof(unsigned short), str_data.c_str(), l_sHostLen);

		if (!m_bMaster)
		{
			ICC_LOG_WARNING(m_pLog, "is not master, not send message!!");
			return;
		}

		if (!m_bBindSuccess)
		{
			ICC_LOG_WARNING(m_pLog, "is not bind success, not send message!!");
			return;
		}

		if (m_pUdp == nullptr)
		{
			ICC_LOG_WARNING(m_pLog, "udp is null, not send message!!");
			return;
		}

		int iRs = m_pUdp->Send(szBuf, l_sHostLen + sizeof(unsigned short), str_remoteIP, iPort);
		if (iRs == 0)
		{
			m_iSendErrorCount++;
			if (m_iSendErrorCount >= m_iSendFailedNum)
			{
				if (m_pParent && m_bIsRebindSendFailed)
				{
					m_pParent->OnReport(Udp::UDP_RESULT_UDPERROR, "send msg failed!!!");
					m_iSendErrorCount = 0;
				}
				else
				{
					ICC_LOG_WARNING(m_pLog, "parent is null or flag is false, not rebind!!");
				}
			}
		}
		else
		{
			m_iSendErrorCount = 0;
		}
	}

	void CUdpHelper::NoticeXml(const std::string &str_guid, const std::string &str_data)
	{
		bool l_send = false;
		Lock::AutoLock l_lk(m_pClientLock);
		for(auto l_it = m_UDPClientMap.begin(); l_it != m_UDPClientMap.end(); l_it++)
		{
			if(l_it->second.auth)
			{
				Lock::AutoLock l_lk(m_pMsgLock);
				SendMsg& l_SendMsg = m_SendMsgMap[SENDMSG_KEY(l_it->second.ip, l_it->second.port, str_guid)];
				l_SendMsg.content = str_data;
				l_SendMsg.sendTimes = m_SendTimes - 1;
				l_SendMsg.ip = l_it->second.ip;
				l_SendMsg.port = l_it->second.port;
				l_SendMsg.time = UDPCurrentTime();
				l_send = true;
				ResponseXml(str_data, l_it->second.ip, l_it->second.port);
			}
		}
		if(!l_send)
		{
			ICC_LOG_DEBUG(m_pLog, "Message no receiver, protocol:%s", str_data.data());
		}
	}

	bool CUdpHelper::CheckAuthhearttime(std::string l_strClinetName)
	{
		Lock::AutoLock l_lk(m_pClientLock);
		auto l_it = m_UDPClientMap.find(l_strClinetName);
		if (l_it != m_UDPClientMap.end())
		{
			ICC_LOG_DEBUG(m_pLog, " MRPS update before [%s - %s]  time  ", l_it->first.c_str(), l_strClinetName.c_str());
			if (l_it->first == l_strClinetName && l_it->second.auth) {
				l_it->second.time = UDPCurrentTime();
				ICC_LOG_DEBUG(m_pLog, "  MRPS update [%s - %s]  time ", l_it->first.c_str(), l_strClinetName.c_str());
				return true;
			}
		}
		return false;
	}

	bool CUdpHelper::CheckAuthenticate(std::string str_remoteIP, size_t iPort)
	{
		Lock::AutoLock l_lk(m_pClientLock);
		for(auto l_it = m_UDPClientMap.begin(); l_it != m_UDPClientMap.end(); l_it++)
		{
			ICC_LOG_DEBUG(m_pLog, "MRPS Authenticate Client [%s]-[%s] ip[%s] port[%d].", 
				l_it->first.c_str(), l_it->second.name.c_str(), l_it->second.ip.c_str(), l_it->second.port);
			if(iPort == l_it->second.port && l_it->second.ip.compare(str_remoteIP) == 0)
			{
				if(l_it->second.auth)
				{
					l_it->second.time = UDPCurrentTime();
					ICC_LOG_DEBUG(m_pLog, "  MRPS update [%s - %s]  time ", l_it->first.c_str(), l_it->second.name.c_str());
					return true;
				}
				break;
			}
		}
		ICC_LOG_DEBUG(m_pLog, "MRPS Authenticate fail, ClientMapSize[%d] ip[%s] port[%d].", m_UDPClientMap.size(), str_remoteIP.data(), iPort);
		return false;
	}

	void CUdpHelper::CheckClientHeart()
	{
		uint64 l_time = UDPCurrentTime();
		ICC_LOG_DEBUG(m_pLog, " check heart time  client  [%d]  ", l_time);
		Lock::AutoLock l_lk(m_pClientLock);
		for (auto l_it = m_UDPClientMap.begin(); l_it != m_UDPClientMap.end(); l_it++)
		{
			ICC_LOG_DEBUG(m_pLog, "MRPS CheckClientHeart Client [%s]-[%s] ip[%s] port[%d].",
				l_it->first.c_str(), l_it->second.name.c_str(), l_it->second.ip.c_str(), l_it->second.port);
			if (l_time - l_it->second.time > 30 )
			{
				ICC_LOG_DEBUG(m_pLog, " heart time > 30 , delete client  [%s]-[%d-%d]  ", l_it->first.c_str(), l_time, l_it->second.time);
				m_UDPClientMap.erase(l_it);
			}
		}
	}

	bool CUdpHelper::OnTimer()
	{
		m_CurrentTimes++;
		if(m_CurrentTimes == m_IntervalTime)
		{
			m_CurrentTimes = 0;
			uint64 l_time = UDPCurrentTime();
			Lock::AutoLock l_lk(m_pMsgLock);
			for(auto l_it = m_SendMsgMap.begin(); l_it != m_SendMsgMap.end();)
			{
				if(l_it->second.sendTimes > 0 && l_time - l_it->second.time > m_IntervalTime - 1)
				{
					ResponseXml(l_it->second.content, l_it->second.ip, l_it->second.port);
					l_it->second.sendTimes--;
					l_it++;
				}
				else
				{
					l_it = m_SendMsgMap.erase(l_it);
				}
			}
		}
		return true;
	}

	uint64 CUdpHelper::UDPCurrentTime()
	{
		return m_pDateTime->CurrentDateTime() / 1000 / 1000 - 28800;
	}
}
