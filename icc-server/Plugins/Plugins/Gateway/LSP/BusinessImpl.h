#pragma once

namespace ICC
{
	//////////////////////////////////////////////////////////////////////////
	// ͳһ��־ƽ̨�Խ�
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
		std::string m_strSelfDir; //������־���·��
		std::string m_strScanDir; //������־���·��
		std::string m_strLSPUrl;  //��־ƽ̨URL
		std::string m_strSystem;  //ϵͳ����
		int m_iPackageSize;//ÿ������
	};
}