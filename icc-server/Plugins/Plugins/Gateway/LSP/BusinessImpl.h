#pragma once

namespace ICC
{
	//////////////////////////////////////////////////////////////////////////
	// 统一日志平台对接
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
		void OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);

	private:
		bool IsUsed(std::string p_strFile);
		bool RenameFile(std::string p_strFile);
		void GetLogRecords(std::string p_strFile, std::vector<std::string>& p_vecRecords);
		unsigned int GetLogRecordsIncrement(std::string p_strFile, unsigned int p_startPos, std::vector<std::string>& p_vecRecords);
		std::string BuildBody(const std::vector<std::string>& p_logVec);
		void SendLog(const std::vector<std::string>& p_vecRecords);

	public:
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig;
		Config::IConfigPtr m_pStaticConfig;
		HelpTool::IHelpToolPtr m_pHelpTool;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		DateTime::IDateTimePtr m_pDateTime;
		StringUtil::IStringUtilPtr m_pString;
		Timer::ITimerManagerPtr	m_pTimerMgr;
		IHttpClientPtr m_pHttpClient;
	private:
		std::map<std::string, unsigned int> m_readCachMap;
		std::string m_strSelfDir; //自身日志存放路径
		std::string m_strScanDir; //本地日志存放路径
		std::string m_strLSPUrl;  //日志平台URL
		std::string m_strSystem;  //系统名称
		int m_iPackageSize;//每包数量
	};
}