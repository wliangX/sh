#pragma once
#include <algorithm> 
#include <Timer/ITimerFactory.h> 

using namespace std;

namespace ICC
{
	class CBusinessImpl : public CBusinessBase
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();
	public:
		// 新增与修改接警预案
		void OnCNotifiAddOrUpdateAlarmPlanRequest(ObserverPattern::INotificationPtr p_pNotify);
		// 删除接警预案
		void OnCNotifiDeleteAlarmPlanRequest(ObserverPattern::INotificationPtr p_pNotify);
		// 启用-停用接警预案
		void OnCNotifiEnableAlarmPlanRequest(ObserverPattern::INotificationPtr p_pNotify);
		// 保存警情的预案信息
		void OnCNotifiSaveAlarmPlanRequest(ObserverPattern::INotificationPtr p_pNotify);
		// 查询警情的预案信息
		void OnCNotifiSearchAlarmPlanRequest(ObserverPattern::INotificationPtr p_pNotify);
		// 查询常用预案
		void OnCNotifiSearchCommonUsePlanRequest(ObserverPattern::INotificationPtr p_pNotify);
		// 查询预案列表
		void OnCNotifiSearchPlanListRequest(ObserverPattern::INotificationPtr p_pNotify);
		// 查询推荐预案
		void OnCNotifiSearchRecommendPlanRequest(ObserverPattern::INotificationPtr p_pNotify);
		// 查询预案详情
		void OnCNotifiSearchPlanInfoRequest(ObserverPattern::INotificationPtr p_pNotify);
	private:

		void AddAlarmPlan(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, PROTOCOL::CNotifiAlarmPlanResponse& p_pResponse);
		void UpdateAlarmPlan(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, PROTOCOL::CNotifiAlarmPlanResponse& p_pResponse);
		void DeleteAlarmPlan(const PROTOCOL::CNotifiDeleteAlarmPlanRequest& p_pRequest, PROTOCOL::CNotifiAlarmPlanResponse& p_pResponse);
		
		bool SaveAddAlarmPlan(const PROTOCOL::CNotifiSaveAlarmPlanRequest& p_pRequest, PROTOCOL::CNotifiSaveAlarmPlanResponse& p_pResponse);
		bool SaveUpdateAddAlarmPlan(const PROTOCOL::CNotifiSaveAlarmPlanRequest& p_pRequest, PROTOCOL::CNotifiSaveAlarmPlanResponse& p_pResponse);
		
		void SearchAlarmPlan(const PROTOCOL::CNotifiSearchAlarmPlanRequest& p_pBaseInfo, PROTOCOL::CNotifiSearchAlarmPlanResponse& p_pResponse);
		
		void SearchAlarmPlanList(const PROTOCOL::CNotifiSearchPlanListRequest& p_pRequest, PROTOCOL::CNotifiSearchPlanListResponse& p_pResponse);
		void SearchAlarmPlanInfo(const PROTOCOL::CNotifiSearchPlanInfoRequest& p_pRequest, PROTOCOL::CNotifiSearchPlanInfoResponse& p_pResponse);
		void SearchAlarmCommonUsePlan(const PROTOCOL::CNotifiSearchCommonUsePlanRequest& p_pRequest, PROTOCOL::CNotifiSearchCommonUsePlanResponse& p_pResponse);
		void SearchAlarmRecommendPlan(const PROTOCOL::CNotifiSearchPlanListRequest& p_pRequest, PROTOCOL::CNotifiSearchCommonUsePlanResponse& p_pResponse);

		// 数据库操作
		bool InsertPlanBaseInfo(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, const std::string& strbaseId, const std::string& strTransGuid);
		bool InsertPlanDetail(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, const std::string& strbaseId, const std::string& strTransGuid);
		bool InsertPlandata(const PROTOCOL::CNotifiSaveAlarmPlanRequest& p_pRequest, const std::string& strbaseId, const std::string& strTransGuid);
		bool UpdatePlanBaseInfo(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, const std::string& strTransGuid);
		bool UpdatePlanDetail(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, const std::string& strTransGuid);
		bool UpdatePlandata(const PROTOCOL::CNotifiSaveAlarmPlanRequest& p_pRequest, const std::string& strTransGuid);

		bool DeletePlanBaseInfo(const PROTOCOL::CNotifiDeleteAlarmPlanRequest& p_pRequest, const std::string& strTransGuid);
		bool DeletePlanDetail(const PROTOCOL::CNotifiDeleteAlarmPlanRequest& p_pRequest, const std::string& strTransGuid);

		// 查询警情的预案信息
		bool SearchSaveAlarmPlan(const PROTOCOL::CNotifiSaveAlarmPlanRequest& p_pRequest, PROTOCOL::CNotifiSaveAlarmPlanResponse& p_pResponse);
		bool SearchPlanList(const PROTOCOL::CNotifiSearchPlanListRequest& p_pRequest, std::string p_pSql_id, DataBase::SQLRequest p_oSQLRequest, DataBase::IResultSetPtr& p_pRSet);
		bool SearchPlanInfo(const PROTOCOL::CNotifiSearchPlanInfoRequest& p_pRequest, std::string p_pSql_id, DataBase::SQLRequest p_oSQLRequest, DataBase::IResultSetPtr& p_pRSet);

	private:
		// 多值查询条件构造函数
		std::string BuildMultiConditions(std::string p_strColumnName, std::string p_strOperator, std::vector<std::string> p_vecCondition);
		// 数组转字符串，用,分隔
		std::string ArrayToStr(std::vector<PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest::KeyValuePair> p_pKeyValuePair,std::string s);
		// 字符串转数组
		std::vector<std::string> StrToArray(std::string s, std::string p_pSplit = ",");
	private:
		Log::ILogPtr			            m_pLog;
		Config::IConfigPtr		            m_pConfig;
		DataBase::IDBConnPtr				m_pDBConn;
		StringUtil::IStringUtilPtr	        m_pString;
		JsonParser::IJsonFactoryPtr         m_pJsonFty;
		Lock::ILockFactoryPtr               m_pLockFty;
		DateTime::IDateTimePtr	            m_pDateTime;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		HelpTool::IHelpToolPtr              m_pHelpTool;
	};
}