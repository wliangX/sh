#pragma once

namespace ICC
{
	//////////////////////////////////////////////////////////////////////////
	// GPS网关业务处理类
	class CBusinessImpl :
		public CBusinessBase
	{
	public:
		CBusinessImpl();
		~CBusinessImpl();
	
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();
	public:
		void OnNotifiGetGpsRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetPhoneInfoRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiSetPhoneInfoRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetDivisionRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiSetDivisionRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetBJRDHGpsRequest(ObserverPattern::INotificationPtr p_pNotify);
		//获取Nacos服务参数
		void OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotify);
	public:
		void BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, 
								const PROTOCOL::CHeader& p_oRequestHeader,
								std::string p_strCmd);

		void BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, 
								std::string p_strCmd);
	
		bool BuildGetGpsRespondAndSync(PROTOCOL::CGPSGetGpsRespond& p_oRespond,
									   PROTOCOL::CGpsSync& p_oSync, 
									   const PROTOCOL::CGPSGetGpsRequest& p_oRequest);

		bool BuildGetPhoneInfoRespondAndSync(PROTOCOL::CGPSGetPhoneInfoRespond& p_oRespond,
											 PROTOCOL::CGPSPhoneInfoSync& p_oSync,
											 const PROTOCOL::CGPSGetPhoneInfoRequest& p_oRequest);

		bool BuildSetPhoneInfoRespondAndSync(PROTOCOL::CGPSSetPhoneInfoRespond& p_oRespond,
											 PROTOCOL::CGPSPhoneInfoSync& p_oSync,
											 const PROTOCOL::CGPSSetPhoneInfoRequest& p_oRequest);

		bool BuildGetDivisionRespondAndSync(PROTOCOL::CGPSGetDivisionRespond& p_oRespond,
											PROTOCOL::CGPSDivisionSync& p_oSync,
											const PROTOCOL::CGPSGetDivisionRequest& p_oRequest);

		bool BuildSetDivisionRespondAndSync(PROTOCOL::CGPSSetDivisionRespond& p_oRespond,
											PROTOCOL::CGPSDivisionSync& p_oSync,
											const PROTOCOL::CGPSSetDivisionRequest& p_oRequest);

		bool BuildGetBJRDHGpsRespond(PROTOCOL::CGPSGetBJRDHGpsRespond& p_oRespond, const PROTOCOL::CGPSGetBJRDHGpsRequest& p_oRequest);
		bool BuildGetGpsRespond(PROTOCOL::CGPSGetBJRDHGpsRespond& p_oRespond, const PROTOCOL::CGPSGetBJRDHGpsRequest& p_oRequest);

		std::string ParsePhonePre(std::string p_strPhone);

	private:
		bool GetServiceInfo(std::string& p_strServiceName, std::string& p_strIp, std::string& p_strPort);
		bool queryLonLoatByAddr(std::string& p_strLon, std::string& p_strLoat, PROTOCOL::CLonLoatByAddrInfo& p_pLonLoatByAddrInfo);
		void SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
	private:
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig;
		DataBase::IDBConnPtr m_pDBConn;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		DateTime::IDateTimePtr m_pDateTime;
		StringUtil::IStringUtilPtr m_pString;
		IHttpClientPtr m_pHttpClient;

	private:
		std::string m_strIsUsing;
		std::string m_strServerIp;
		std::string m_strServerPort;
		std::string m_strAppID;
		std::string m_strPassword;

		int m_intTimeOut;

		std::string m_strGisSdkServiceName;  //gis-sdk-server

	private://Nacos
		std::string					m_strAiaAlarmServerName;
		std::string					m_strNacosServerIp;
		std::string					m_strNacosServerPort;
		std::string					m_strNacosServerNamespace;
		std::string					m_strNacosServerGroupName;
		boost::mutex			    m_mutexNacosParams;
	};
}