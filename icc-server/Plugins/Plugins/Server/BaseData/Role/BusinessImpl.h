#pragma once
#include "RoleInfo.h"

namespace ICC
{
	typedef std::map<std::string, PROTOCOL::tRole> mapRoleInfo;
    
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
       	//获取角色信息
		void OnNotifiGetRoleRequest(ObserverPattern::INotificationPtr p_pNotify);

		//设置角色信息
		void OnNotifiSetRoleRequest(ObserverPattern::INotificationPtr p_pNotify);

		//删除角色信息
		void OnNotifiDeleteRoleRequest(ObserverPattern::INotificationPtr p_pNotify);

    public:
		//加载角色信息
		bool LoadRoleInfo();

		//功能: 根据输入的cmd及请求消息体生成响应消息体的Header
		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);

		//同步角色信息
		bool SyncRoleInfo(const CRoleInfo& p_tRole,std::string SyncType);

	private:
		DataBase::IDBConnPtr m_pDBConn;
		IAmqClientPtr m_pAmqClient;
		Config::IConfigPtr m_pConfig;
		Log::ILogPtr m_pLog;
		JsonParser::IJsonPtr m_pIJson;
		StringUtil::IStringUtilPtr m_pString;
		DateTime::IDateTimePtr m_pDateTime;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		Redis::IRedisClientPtr m_pRedisClient;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;

	private:
		std::string m_strDeptInfoTime;
		std::string m_strStaffInfoTime;
		std::string m_strDictInfoTime;
		std::string m_strUserInfoTime;
		std::string m_strLanguageInfoTime;
		std::string m_strSystemInfoTime;
	};
}