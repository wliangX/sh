#pragma once
#include "DeptInfo.h"

#define SET_DEPT_RESPOND "set_dept_respond";
#define GET_DEPT_RESPOND "get_dept_respond";
#define DELETE_DEPT_RESPOND "delete_dept_respond";
#define DEPT_SYNC "dept_sync";

namespace ICC
{

    typedef std::map<std::string, PROTOCOL::CSetDeptRequest::CBody> mapDept;

	class CBusinessImpl :
        public CBusinessBase
    {
		enum eSyncType
		{
			ICC_ADD = 1,
			ICC_MODIFY,
			ICC_DELETE
		};
    public:
        CBusinessImpl();
        ~CBusinessImpl();

	public:
		
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:        
        //获取单位信息
		void OnNotifiGetDeptRequest(ObserverPattern::INotificationPtr p_pNotify);
		//获取下级单位信息
		void OnNotifiGetSubDeptRequest(ObserverPattern::INotificationPtr p_pNotify);

        //设置单位信息
		void OnNotifiSetDeptRequest(ObserverPattern::INotificationPtr p_pNotify);

        //删除单位信息
		void OnNotifiDeleteDeptRequest(ObserverPattern::INotificationPtr p_pNotify);

	private:
		bool LoadDept();
		bool LoadDeptByUser(const std::string& p_strInUserCode, const std::string& p_strInPageSize, const std::string& p_strInPageIndex, 
			std::string& p_strAllCount, std::map<std::string, std::string>& p_mapOutDeptInfo);
		void SetCommandDept();
		bool SetRedisDetpInfo(CDeptInfo& p_oDeptInfo);

		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);
		bool SyncDeptInfo(const CDeptInfo& p_oDept, eSyncType p_eSyncType);

	private:

		static const std::string DeptInfoKey;

		DateTime::IDateTimePtr m_pDateTime;
        StringUtil::IStringUtilPtr m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
        Config::IConfigPtr m_pConfig;
        Log::ILogPtr m_pLog;
		DataBase::IDBConnPtr m_pDBConn;
		Redis::IRedisClientPtr m_pRedisClient;
        ObserverPattern::IObserverCenterPtr m_pObserverCenter;
    private:
		std::list<std::string>m_lstCommCode;		//发送处警超时部门类型，默认市局、分局指挥中心
	};
}