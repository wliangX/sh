#pragma once

namespace ICC
{
    class CBusinessImpl :
        public CBusinessBase
    {
		enum eWeChatResult
		{
			Pending = 0,		//������
			Processing,			//���ڴ���
			InvalidandEnd=6,	//��Ϊ��Ч�������ս�
			RepeatandEnd,		//��Ϊ�ر��������ս�
			Processed,			//������ϲ��սᣨ����״̬��ֵ����5��Ϊ΢�ű�������������
			LimitedService		//��������
		};

    public:
        virtual void OnInit();
        virtual void OnStart();
		virtual void OnStop();
        virtual void OnDestroy();

    public:
		void OnNotifiWeChatDSAcceptAlarmInfo(ObserverPattern::INotificationPtr  p_pDSAcceptWeChatAlarm);//ʡ��΢�ű���
		void OnNotifiWeChatDSAcceptChangeInfo(ObserverPattern::INotificationPtr p_pDSAcceptChangeInfo);	//ʡ��΢�ű���--�����˷���Ϣ
		void OnNotifiWeChatOSAcceptChangeInfo(ObserverPattern::INotificationPtr p_pOSAcceptChangeInfo); //ʡ��΢�ű���--��Ա����Ϣ
		void OnNotifiWeChatProcessResult(ObserverPattern::INotificationPtr p_pOSProcessResultInfo);		//���;��鴦����
	
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