#pragma once
#include "TextTemplate.h"

namespace ICC
{    
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
		//��ȡ�����ı�ģ����Ϣ
		void OnNotifiGetAllTextTemplate(ObserverPattern::INotificationPtr p_pNotify);
		//����ĳһ�ı�ģ����Ϣ
		void OnNotifiSetOneTextTemplate(ObserverPattern::INotificationPtr p_pNotify);
		//ɾ��ĳһ�ı�ģ����Ϣ
		void OnNotifiDelTextTemplate(ObserverPattern::INotificationPtr p_pNotify);
    public:    
		//�����ı�ģ����Ϣ
		bool LoadTextTemplateInfo();

		//����: ���������cmd��������Ϣ��������Ӧ��Ϣ���Header
		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);
    private:
		DataBase::IDBConnPtr m_pDBConn;

		Config::IConfigPtr m_pConfig;
        Log::ILogPtr m_pLog;

		Redis::IRedisClientPtr		m_pRedisClient;		//redis�ͻ���
		JsonParser::IJsonFactoryPtr m_pJsonFty;
        StringUtil::IStringUtilPtr m_pString;
		DateTime::IDateTimePtr m_pDateTime;

        boost::shared_ptr<boost::thread> m_DBThread;
        bool m_isConnectDB;
        bool m_isConnectMQ;

		static const std::string TextTemplate;

	private:
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
	};
}