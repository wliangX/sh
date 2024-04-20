#pragma once
#include "ManagerKeyDept.h"

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
        //设置重点单位信息
		void OnCNotifiSetKeyDeptRequest(ObserverPattern::INotificationPtr p_pNotify);

        //获取所有重点单位信息
		void OnCNotifiGetAllKeyDeptRequest(ObserverPattern::INotificationPtr p_pNotify);

		//判断某单位是否为重点单位
		void OnCNotifiCheckDeptRequest(ObserverPattern::INotificationPtr p_pNotify);

		//删除重点单位信息
		void OnNotifiDeleteKeyDeptRequest(ObserverPattern::INotificationPtr p_pNotify);

		//同步重点单位
		void SendSyncKeyDept(CKeyDept& p_tKeyDept,eSyncType p_eSyncType);

		//从数据库中加载重点单位数据
		bool LoadKeyDeptFromDB();

		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);

		bool ExecSql(DataBase::SQLRequest p_oSQLReq, const std::string& strTransGuid);

	private:
		bool AddKeyDept(std::string p_strPhone, const CKeyDept& p_tKeyDept);
		bool DeleteKeyDept(std::string p_strPhone);
		bool GetAllKeyDept(std::map<std::string, std::string>& p_mapKeyDept);
		bool GetKeyDeptByPhone(std::string p_strPhone, CKeyDept& p_tKeyDept);
		std::string GetIDByPhone(std::string p_strPhone);
		bool ExistInCache(std::string p_strPhone);

	private:

		Log::ILogPtr				m_pLog;
		StringUtil::IStringUtilPtr  m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFac;
		DateTime::IDateTimePtr		m_pDateTime;
		DataBase::IDBConnPtr					m_pIDBConn;
		Config::IConfigPtr			m_pConfig;
		ObserverPattern::IObserverCenterPtr  m_pObserverCenter;

		Redis::IRedisClientPtr m_pRedisClient;
		// 重点单位缓存管理 [3/23/2018 w26326]
		ManagerKeyDeptPtr m_pManagerKeyDept;
	};
}