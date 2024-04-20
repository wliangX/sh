#pragma once

namespace ICC
{
    class CBusinessImpl :
        public CBusinessBase
    {
		enum eWeChatResult
		{
			Pending = 0,		//待处理
			Processing,			//正在处置
			InvalidandEnd=6,	//作为无效警处理并终结
			RepeatandEnd,		//作为重报警处理并终结
			Processed,			//受理完毕并终结（处理状态的值大于5均为微信报警交互结束）
			LimitedService		//服务受限
		};

    public:
        virtual void OnInit();
        virtual void OnStart();
		virtual void OnStop();
        virtual void OnDestroy();

    public:
		void OnNotifiWeChatDSAcceptAlarmInfo(ObserverPattern::INotificationPtr  p_pDSAcceptWeChatAlarm);//省厅微信报警
		void OnNotifiWeChatDSAcceptChangeInfo(ObserverPattern::INotificationPtr p_pDSAcceptChangeInfo);	//省厅微信报警--报警人发消息
		void OnNotifiWeChatOSAcceptChangeInfo(ObserverPattern::INotificationPtr p_pOSAcceptChangeInfo); //省厅微信报警--警员发消息
		void OnNotifiWeChatProcessResult(ObserverPattern::INotificationPtr p_pOSProcessResultInfo);		//发送警情处理结果
	
	public:
		void SendAddWeChatAlarm(const PROTOCOL::CWeChatAlarmInfo::COMPLAINTLIST& p_oWeChatInfo);
		void SendAddChangeInfo(PROTOCOL::CDSWeChatChangeInfo& p_oWeChatChangeInfo);
		void SendAcceptWeChatAlarm(std::string p_strGuid);
		void SendWeChatReturnMsg(PROTOCOL::CRetureMsg& p_oReturnMsg);
		bool SelectThirdAlarm(std::string p_strGuid);
		bool SelectThirdAlarm(std::string p_strGuid,std::string& p_strOpenID);
		void SetBindMember();
		std::string GetResMsg(eWeChatResult p_eResType);

	private:
		std::string				m_BlackMsg;
        Log::ILogPtr			m_pLog;
        Config::IConfigPtr      m_pConfig;
        Timer::ITimerManagerPtr	m_pTimerMgr;
		DataBase::IDBConnPtr	m_pDBConn;
		StringUtil::IStringUtilPtr  m_pString;
        DateTime::IDateTimePtr      m_pDateTime;  
		JsonParser::IJsonFactoryPtr	m_pJsonFty;
        ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		std::map<eWeChatResult, std::string> m_mProcessRes;
    };
}