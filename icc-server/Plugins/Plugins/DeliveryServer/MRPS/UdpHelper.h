#pragma once

#include <atomic>

#define STRING_EMPTY ""
#define TELEPHONECALL  "01"//"DIC001001"

#define LOGIN_RESULT_SUCESS	0
#define LOGIN_RESULT_FAIL	1
#define LOGIN_RESULT_REPEAT	2
#define LOGIN_RESULT_UNKNOW	9

#define COMMAND_GATEWAY_UDPBINDFAILED       "gateway_udp_bindfailed"
#define COMMAND_GATEWAY_UDPBINDSUCCESS      "gateway_udp_bindsuccess"

#define CTI_CALLMEMBER_NOTICE "<hytera>"\
"<cmd_name>cti_callmember_notice</cmd_name>"\
"<cmd_guid>%s</cmd_guid>"\
"<callrefid>%s</callrefid>"\
"<realcallrefid>%s</realcallrefid>"\
"<phone>%s</phone>"\
"<phonetype>%d</phonetype>"\
"<type>%d</type>"\
"<timestamp>%lld</timestamp>"\
"</hytera>"

#define CTI_CALLINFO_NOTICE "<hytera>"\
"<cmd_name>cti_callinfo_notice</cmd_name>"\
"<cmd_guid>%s</cmd_guid>"\
"<callrefid>%s</callrefid>"\
"<realcallrefid>%s</realcallrefid>"\
"<phone>%s</phone>"\
"<caller>%s</caller>"\
"<called>%s</called>"\
"<callstart>%lld</callstart>"\
"<callstop>%lld</callstop>"\
"<timestamp>%lld</timestamp>"\
"</hytera>"

#define CTI_CASEINFO_NOTICE "<hytera>"\
"<cmd_name>cti_caseinfo_notice</cmd_name>"\
"<cmd_guid>%s</cmd_guid>"\
"<caseid>%s</caseid>"\
"<callrefid>%s</callrefid>"\
"<realcallrefid>%s</realcallrefid>"\
"<timestamp>%lld</timestamp>"\
"</hytera>"

#define CTI_CALLREFID_NOTICE "<hytera>"\
"<cmd_name>cti_callrefid_notice</cmd_name>"\
"<cmd_guid>%s</cmd_guid>"\
"<callrefid>%s</callrefid>"\
"<callrefid_old>%s</callrefid_old>"\
"<timestamp>%lld</timestamp>"\
"</hytera>"

#define CTI_LOGIN_OUT_ACK "<hytera><cmd_name>%s</cmd_name><cmd_guid>%s</cmd_guid><name>%s</name><result>%d</result><timestamp>%lld</timestamp></hytera>"
#define UDP_MESSAGE_ACK "<hytera><cmd_name>%s</cmd_name><cmd_guid>%s</cmd_guid><timestamp>%lld</timestamp></hytera>"

#define SENDMSG_KEY(ip, port, guid) m_pString->Format("%s:%d-%s", ip.data(), port, guid.data())

namespace ICC
{
	class CBusinessImpl;
	
	typedef struct tagUDPClient
	{
		tagUDPClient() { time = 0; port = 0; auth = false; };
		std::string		ip;			//IP
		unsigned int	port;		//端口
		uint64			time;		//最后一次请求的时间
		std::string		guid;		//登录协议GUID
		std::string		name;		//登录用户名
		std::string		password;	//登录密码
		bool			auth;		//是否鉴权成功
	}UDPClient;
	typedef std::map<std::string, UDPClient> UDPClientMap;	//主键为登录用户名，相同用户名已经登录无法再次登录

	typedef struct tagCallInfo
	{
		tagCallInfo() { callstart = 0; callstop = 0; };
		std::string		callrefid;
		std::string		device;
		std::string		realcallrefid;
		std::string		caller;
		std::string		called;
		uint64			callstart;
		uint64			callstop;
		std::string		alarmid;
	}CallInfo;
	typedef std::map<std::string, CallInfo> CallInfoMap;	//主键为realcallrefid，在同一个realcallrefid中，主被叫只有一个

	typedef struct tagSendMsg
	{
		tagSendMsg() { time = 0; sendTimes = 0; port = 0; };
		std::string		content;	  //要重发的内容
		unsigned int	sendTimes;    //待重发的次数
		std::string		ip;           //IP
		unsigned int	port;         //端口
		uint64			time;		  //最后一次发送的时间
	}SendMsg;
	typedef std::map<std::string, SendMsg> SendMsgMap;        //保存要重发的话务、警情信息（key为guid）

	class CUdpHelper : public Udp::IUdpCallBack
	{
	public:
		CUdpHelper(Log::ILogPtr	pLogPtr, HelpTool::IHelpToolPtr pHelpTool);
		~CUdpHelper();

		void SetSendTimes(unsigned int iSendTimes, unsigned int iIntervalTime);
		void SetDateTimePtr(DateTime::IDateTimePtr pDateTimePtr);
		void SetStringUtilPtr(StringUtil::IStringUtilPtr pStringUtilPtr);
		void SetXmlFactoryPtr(Xml::IXmlFactoryPtr pXmlFactoryPtr);
		void SetJsonFactoryPtr(JsonParser::IJsonFactoryPtr pJsonFty);
		void SetObserverCenterPtr(ObserverPattern::IObserverCenterPtr pObserverCenter);
		void SetLockFtyPtr(Lock::ILockFactoryPtr m_LockFty);
		void SetBusinessImp(CBusinessImpl* pParent) { m_pParent = pParent; };
		void SetUdpPtr(Udp::IUdpPtr pUdpServer) { m_pUdp = pUdpServer; }

		void SetMasterSlaveFlag(bool bMaster) { m_bMaster = bMaster; };	
		void SetRebindFailedFlag(bool bFlag) { m_bIsRebindSendFailed = bFlag; }
		void SetFailedNum(int iNum) { m_iSendFailedNum = iNum; }

		void OnStart();
		void OnStop();

		/********************************************************
		*  @function :  CUdpHelper::OnReceived
		*  @brief    :  UDP服务回调函数,处理所有收到的消息
		*  @input    :  p_pData, p_iLength, p_strRemoteIP, p_iPort
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 17:08
		********************************************************/
		virtual void OnReceived(const char* p_pData, unsigned int p_iLength, std::string p_strRemoteIP, size_t p_iPort);
		virtual void OnReport(int iCode, const std::string& strMessage);

		/********************************************************
		*  @function :  CUdpHelper::SendLoginResult
		*  @brief    :  处理鉴权服务返回的登陆回复
		*  @input    :  i_result
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 17:09
		********************************************************/
		bool SendLoginResult(int i_result);

		/********************************************************
		*  @function :  CUdpHelper::NoticeXml
		*  @brief    :  发送广播消息，保存消息内容待重发
		*  @input    :  str_guid, str_data
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 17:12
		********************************************************/
		void NoticeXml(const std::string &str_guid, const std::string &str_data);

		/********************************************************
		*  @function :  CUdpHelper::OnTimer
		*  @brief    :  定时器，处理重发列表中消息
		*  @input    :
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 17:13
		********************************************************/
		bool OnTimer();

		void CheckClientHeart();

		uint64 UDPCurrentTime();
	private:
		/********************************************************
		*  @function :  CUdpHelper::ResponseXml
		*  @brief    :  回复消息到UDP客户端
		*  @input    :  str_data, str_remoteIP, iPort
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 17:12
		********************************************************/
		inline void ResponseXml(const std::string &str_data, std::string str_remoteIP, size_t iPort);

		/********************************************************
		*  @function :  CUdpHelper::CheckAuthenticate
		*  @brief    :  检查收到的消息是否来自已鉴权的UDP客户端
		*  @input    :  str_remoteIP, iPort
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 17:13
		********************************************************/
		bool CheckAuthenticate(std::string str_remoteIP, size_t iPort);

		/********************************************************
		*  @function :  CUdpHelper::CheckAuthenticate
		*  @brief    :  每次心跳更新每次心跳的时间
		*  @input    :  l_strClinetName
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 17:13
		********************************************************/
		bool CheckAuthhearttime(std::string l_strClinetName);

		/********************************************************
		*  @function :  CUdpHelper::CheckAuthenticate
		*  @brief    :  删除没有心跳的用户
		*  @input    :  
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 17:13
		********************************************************/
		void DeleteNoHeartClient();

		/********************************************************
		*  @function :  CUdpHelper::Login
		*  @brief    :  处理MRPS系统发送的登录消息,转发登陆到鉴权服务鉴权
		*  @input    :  l_pXml, str_remoteIP, iPort
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 17:10
		********************************************************/
		bool Login(Xml::IXmlPtr l_pXml, std::string str_remoteIP, size_t iPort);

		/********************************************************
		*  @function :  CUdpHelper::Logout
		*  @brief    :  处理MRPS系统发送的登出消息
		*  @input    :  l_pXml, str_remoteIP, iPort
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 17:11
		********************************************************/
		bool Logout(Xml::IXmlPtr l_pXml, std::string str_remoteIP, size_t iPort);

		/********************************************************
		*  @function :  CUdpHelper::Heartbeat
		*  @brief    :  处理心跳消息，返回本机的时间撮
		*  @input    :  l_pXml, str_remoteIP, iPort
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 17:11
		********************************************************/
		bool Heartbeat(Xml::IXmlPtr l_pXml, std::string str_remoteIP, size_t iPort);

		/********************************************************
		*  @function :  CUdpHelper::CallMemberNoticeAck
		*  @brief    :  处理话务成员、话务信息、警情信息回复
		*  @input    :
		*  @output   :
		*  @return   :  成功返回true，失败返回false
		*  @author   :  z13060  2018/05/30 16:15
		********************************************************/
		bool NoticeAck(Xml::IXmlPtr l_pXml, std::string str_remoteIP, size_t iPort);
		//bool CallMemberNoticeAck(Xml::IXmlPtr l_pXml, std::string str_remoteIP, size_t iPort);
		//bool CallInfoNoticeAck(Xml::IXmlPtr l_pXml, std::string str_remoteIP, size_t iPort);
		//bool CaseInfoNoticeAck(Xml::IXmlPtr l_pXml, std::string str_remoteIP, size_t iPort);

		typedef bool (CUdpHelper::*HandFuncPointer)(Xml::IXmlPtr l_pXml, std::string str_remoteIP, size_t iPort);
		std::map<std::string, HandFuncPointer> _func_map;

		unsigned int m_SendTimes;
		unsigned int m_IntervalTime;
		unsigned int m_CurrentTimes;

		char* m_pBuffer;

		UDPClientMap m_UDPClientMap;
		Lock::ILockPtr m_pClientLock;

		std::string  m_CurrentUser;
		uint64		 m_LoginTime;

		SendMsgMap   m_SendMsgMap;
		Lock::ILockPtr m_pMsgLock;
	private:
		Log::ILogPtr                        m_pLog;
		HelpTool::IHelpToolPtr				m_pHelpTool;
		Udp::IUdpPtr						m_pUdp;
		DateTime::IDateTimePtr		        m_pDateTime;
		StringUtil::IStringUtilPtr          m_pString;
		Xml::IXmlFactoryPtr                 m_pXmlFty;
		JsonParser::IJsonFactoryPtr         m_pJsonFty;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		Lock::ILockFactoryPtr				m_pLockFty;

		CBusinessImpl* m_pParent;
		std::atomic_bool m_bMaster;
		std::atomic_bool m_bBindSuccess;
		int                m_iSendErrorCount;
		int                m_iSendFailedNum;
		std::atomic_bool                   m_bIsRebindSendFailed;
	};
	typedef boost::shared_ptr<CUdpHelper> UdpHelperPtr;

}
