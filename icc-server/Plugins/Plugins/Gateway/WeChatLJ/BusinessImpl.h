#pragma once

namespace ICC
{
    class CBusinessImpl :
        public CBusinessBase
    {
	public:
		enum eThird_ChangeType //���츽������
		{
			ChType_Text=1,//1�����֣�2��ͼƬ��3����Ƶ��4������
			ChType_Image,
			ChType_Video,
			ChType_Record
		};
    public:
        virtual void OnInit();
        virtual void OnStart();
		virtual void OnStop();
        virtual void OnDestroy();

	public:
		void LoadConfig();
		void OnTimerHandleRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnWeChatAlarmRespond(ObserverPattern::INotificationPtr p_pNotifiRequest);
		
		void SendAddWeChatRequest(const PROTOCOL::CWeChatRequest& p_oWeChatInfo);

		bool WeChaReadRequestFile(std::string p_strFileName);				// ��ȡ�����ļ�����
		bool WeChatChangeFileExtension(std::string p_strFileName);			// ����req�����ļ���ı��ļ���չ����.req-->.bak�� [11/1/2018 w26326]
		bool WeChatGenerateRespFile(std::string p_strRespMsg);				// ������΢�ű�����������.resp���ظ��ļ� [11/1/2018 w26326]
		bool WeChatScanFilesWithFilter(const std::string& p_strPath, 
									   const std::string& p_strFilter, 
								       std::vector<std::string>& p_vFileList);	//����׺��ɨ�������ļ���*.req��

		bool SelectThirdAlarmByGuid(std::string p_strGuid);

	private:
		std::string				m_strFilePath;	//΢�ű��������ļ�·��
		unsigned int	        m_timerInterval;//��ʱ���������ʱ��
		std::string				m_strType;
		Xml::IXmlFactoryPtr		m_pXmlFty;
        Log::ILogPtr			m_pLog;
        Config::IConfigPtr      m_pConfig;
        Timer::ITimerManagerPtr	m_pTimerMgr;
		DataBase::IDBConnPtr	m_pDBConn;
		StringUtil::IStringUtilPtr  m_pString;
		StringUtil::IStringFactoryPtr m_pFact;
        DateTime::IDateTimePtr      m_pDateTime;  
		JsonParser::IJsonFactoryPtr	m_pJsonFty;
        ObserverPattern::IObserverCenterPtr m_pObserverCenter;
    };
}