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
		//获取所有文本模板信息
		void OnNotifiGetAllTextTemplate(ObserverPattern::INotificationPtr p_pNotify);
		//设置某一文本模板信息
		void OnNotifiSetOneTextTemplate(ObserverPattern::INotificationPtr p_pNotify);
		//删除某一文本模板信息
		void OnNotifiDelTextTemplate(ObserverPattern::INotificationPtr p_pNotify);
    public:    
		//加载文本模板信息
		bool LoadTextTemplateInfo();

		//功能: 根据输入的cmd及请求消息体生成响应消息体的Header
		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);
    private:
		DataBase::IDBConnPtr m_pDBConn;

		Config::IConfigPtr m_pConfig;
        Log::ILogPtr m_pLog;

		Redis::IRedisClientPtr		m_pRedisClient;		//redis客户端
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