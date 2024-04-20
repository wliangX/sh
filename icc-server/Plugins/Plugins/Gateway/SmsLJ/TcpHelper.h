#pragma once
#include "SMS.hpp"

namespace ICC
{
	enum RequestType
	{
		QUEUE = 0,
		TOPIC
	};

	class CTcpHelper
	{
	public:
		CTcpHelper();
		virtual ~CTcpHelper();
	
	public:
		void SetLogPtr(Log::ILogPtr p_pLog){ m_pLog = p_pLog; }				
		void SetDBConnPtr(DataBase::IDBConnPtr p_pDBConnPtr){ m_pDBConn = p_pDBConnPtr; }
		void SetDateTimePtr(DateTime::IDateTimePtr p_pDateTime){ m_pDateTime = p_pDateTime; }
		void SetStringUtilPtr(StringUtil::IStringUtilPtr p_pString){ m_pString = p_pString; }
		void SetTimerManagerPtr(Timer::ITimerManagerPtr p_pTimerMgr){ m_pTimerMgr = p_pTimerMgr; }
		void SetJsonFactoryPtr(JsonParser::IJsonFactoryPtr p_pJsonFty){ m_pJsonFty = p_pJsonFty; }
		void SetObserverCenterPtr(ObserverPattern::IObserverCenterPtr p_pObserverCenter){ m_pObserverCenter = p_pObserverCenter; }
		void SetLockPtr(Lock::ILockPtr p_pMutex){ m_Mutex = p_pMutex; }
		void SetSmsLockPtr(Lock::ILockPtr p_pMutex){ m_SmsMutex = p_pMutex; }
		void SetTcpClientPtr(Tcp::ITcpClientPtr p_pTcpClient){ m_TcpClient = p_pTcpClient; }
		void SetConnParam(std::string& p_strDeptCode, std::string& p_strDeptName, std::string& p_strIp, unsigned short p_ushPort, 
			std::string& p_strUserName, std::string& p_strPassword);
		void SetProcessParam(std::string& p_strRespContent, unsigned int p_uiCheckLinkTime, unsigned int p_uiProcessTime, unsigned int p_uiIsChangeInfoTime);

		void LoadUnReadSms();
		void LoadOwnerSms();
		bool IsOwnerSms(const std::string& p_strGuid);
		bool HasSmsByPhone(const std::string& p_strPhone);

		bool Connect();
		void DisConnect();

	public:
		// ThirdAlarm 2 SmsLJ
		void OnNotifiSetReadSync(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiSendSmsSync(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetLocationRequest(ObserverPattern::INotificationPtr p_pNotify);

		// SmsLJ 2 ThirdAlarm
		void OnNotifiServerStatusSync(const SmsBodyServerStatusArrival& p_oServerStatus);
		void OnNotifiUserStatusSync(const SmsBodyUserStatusArrival& p_oUserStatus);
		void OnNotifiLocationSync(const SmsBodyGetLocationResp& p_oGetLocation, std::string p_strTelNum);

		void Transmit2ThirdAlarm(const SmsBodySmsArrival& p_oSmsAlarm);
		void TransmitSmsAlarm(const SmsBodySmsArrival& p_oSmsAlarm);
		void TransmitChangeInfo(const SmsBodySmsArrival& p_oSmsAlarm, std::string p_strTAGuid);

	private:
		
		bool BuildReceipted(std::string& p_strTelNum, std::string& p_strRespContent, std::string p_strSeqID);
		bool IsSmsRead(std::string p_strSeqID);
		bool IsChangeInfo(std::string p_strTelNum, std::string& p_strTAGuid);
		bool SelectDeptNameByDeptCode(std::string p_strDeptCode, std::string& p_strDeptName);

	private:
		void BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, const PROTOCOL::CHeader& p_oRequestHeader);
		void BuildRespondHeader(PROTOCOL::CHeader& p_oHeader, std::string p_strCmd, RequestType p_Type);

	private:
		static void S_RunCheckLink(CTcpHelper* p_pThread, unsigned int p_uiCheckLinkTime);
		static void S_RunProcess(CTcpHelper* p_pThread, unsigned int p_uiProcessTime);

		void CheckLink(unsigned int p_uiCheckLinkTime);
		void Process(unsigned int p_uiCheckLinkTime);
		bool ProcessSmsRecvBuf();

	public:
		bool GetSmsLoginStatus();
		void SetSmsLoginStatus(bool p_status);

		void BeginProcessThread();
		void EndProcessThread();

	public:
		// send: SmsLJ 2 SmsService
		bool SendCheckLinkReq();

		bool SendLogin();
		bool SendSetReaded(std::string p_szTypeid);
		void SendReceipted(std::string p_strSeqID);
		bool SendGetLocation(std::string p_strTelNum);
		bool SendSmsInfo(std::string p_strTelNum, std::string p_strContent);

		bool SendServerStatusResp(int seqID);
		bool SendUserStatusResp(int seqID);
		bool SendSmsAlarmResp(int seqID);

		// recv: SmsService 2 SmsLJ
		void OnCmdCheckAliveResp(const char* buf, int nLen, int seqID);

		void OnCmdLoginResp(const char* buf, int nLen, int seqID);
		void OnCmdSetReadedResp(const char* buf, int nLen, int seqID);
		void OnCmdGetLocationResp(const char* buf, int nLen, int seqID);
		void OnCmdSendMSGResp(const char* buf, int nLen, int seqID);

		void OnCmdServerStatusArrival(const char* buf, int nLen, int seqID);
		void OnCmdUserStatusArrival(const char* buf, int nLen, int seqID);
		void OnCmdSmsAlarmArrival(const char* buf, int nLen, int seqID);

	private:
		std::string _TimeStandard(const std::string& strTmpTime);
		void AddOwerSms(const std::string& strGuid, const std::string& strPhone);
		void RemoveOwerSms(const std::string& strGuid);
		void ClearOwerSms();

	private:
		std::string							m_strDeptCode;
		std::string							m_strDeptName;

		std::string							m_strUserName;
		std::string							m_strPassword;
		std::string							m_strServiceIP;
		unsigned int						m_uiServicePort;

		unsigned int						m_uiCheckLinkTime;
		unsigned int						m_uiProcessTime;
		unsigned int						m_uiIsChangeInfoTime;

		std::string							m_strRespContent;

		bool								m_bSmsLoginStatus;

		bool								m_bIsStopCheckLinkThread;
		bool								m_bIsStopProcessThread;

		boost::shared_ptr<boost::thread>	m_pThreadCheckLink;
		boost::shared_ptr<boost::thread>	m_pThreadProcess;

		std::map<std::string, std::string>					m_mapSmsResInfo;
		std::map<int, std::map<std::string, std::string>>	m_mapSmsReqInfo;
		std::map<std::string, std::string>					m_mapOwnerSms;

	public:
		SmsBuffer							m_SmsBuffer;

	public:
		Log::ILogPtr						m_pLog;
		DataBase::IDBConnPtr				m_pDBConn;
		DateTime::IDateTimePtr				m_pDateTime;
		StringUtil::IStringUtilPtr			m_pString;
		Timer::ITimerManagerPtr				m_pTimerMgr;
		JsonParser::IJsonFactoryPtr			m_pJsonFty;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		Lock::ILockPtr						m_Mutex;
		Lock::ILockPtr						m_SmsMutex;

		Tcp::ITcpClientPtr					m_TcpClient;
		Tcp::ITcpClientCallBackPtr			m_ITcpClientCallbackPtr;

	private:
		std::map<std::string, std::string>	m_mapCmdName;
	};

	//////////////////////////////////////////////////////////////////////////
	class TcpClientImpl : public Tcp::ITcpClientCallBack
	{
	public:
		TcpClientImpl(CTcpHelper* p_pTcpHelper);
		virtual ~TcpClientImpl();

		void OnReceived(const char* p_pData, unsigned int p_iLength);
		void OnConnected(std::string p_strError);
		void OnDisconnected(std::string p_strError);

	private:
		CTcpHelper* m_pTcpHelper;
	};
}