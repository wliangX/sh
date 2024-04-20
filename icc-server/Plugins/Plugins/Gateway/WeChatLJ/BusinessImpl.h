#pragma once

namespace ICC
{
    class CBusinessImpl :
        public CBusinessBase
    {
	public:
		enum eThird_ChangeType //聊天附件类型
		{
			ChType_Text=1,//1、文字；2、图片；3、视频；4、语音
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

		bool WeChaReadRequestFile(std::string p_strFileName);				// 读取请求文件内容
		bool WeChatChangeFileExtension(std::string p_strFileName);			// 处理req请求文件后改变文件拓展名（.req-->.bak） [11/1/2018 w26326]
		bool WeChatGenerateRespFile(std::string p_strRespMsg);				// 处理完微信报警，生产（.resp）回复文件 [11/1/2018 w26326]
		bool WeChatScanFilesWithFilter(const std::string& p_strPath, 
									   const std::string& p_strFilter, 
								       std::vector<std::string>& p_vFileList);	//按后缀名扫描请求文件（*.req）

		bool SelectThirdAlarmByGuid(std::string p_strGuid);

	private:
		std::string				m_strFilePath;	//微信报警请求文件路径
		unsigned int	        m_timerInterval;//定时器启动间隔时间
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