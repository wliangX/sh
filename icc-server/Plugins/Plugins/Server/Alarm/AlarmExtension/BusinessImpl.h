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
		// �������޸ĽӾ�Ԥ��
		void OnCNotifiAddOrUpdateAlarmPlanRequest(ObserverPattern::INotificationPtr p_pNotify);
		// ɾ���Ӿ�Ԥ��
		void OnCNotifiDeleteAlarmPlanRequest(ObserverPattern::INotificationPtr p_pNotify);
		// ����-ͣ�ýӾ�Ԥ��
		void OnCNotifiEnableAlarmPlanRequest(ObserverPattern::INotificationPtr p_pNotify);
		// ���澯���Ԥ����Ϣ
		void OnCNotifiSaveAlarmPlanRequest(ObserverPattern::INotificationPtr p_pNotify);
		// ��ѯ�����Ԥ����Ϣ
		void OnCNotifiSearchAlarmPlanRequest(ObserverPattern::INotificationPtr p_pNotify);
		// ��ѯ����Ԥ��
		void OnCNotifiSearchCommonUsePlanRequest(ObserverPattern::INotificationPtr p_pNotify);
		// ��ѯԤ���б�
		void OnCNotifiSearchPlanListRequest(ObserverPattern::INotificationPtr p_pNotify);
		// ��ѯ�Ƽ�Ԥ��
		void OnCNotifiSearchRecommendPlanRequest(ObserverPattern::INotificationPtr p_pNotify);
		// ��ѯԤ������
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

		// ���ݿ����
		bool InsertPlanBaseInfo(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, const std::string& strbaseId, const std::string& strTransGuid);
		bool InsertPlanDetail(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, const std::string& strbaseId, const std::string& strTransGuid);
		bool InsertPlandata(const PROTOCOL::CNotifiSaveAlarmPlanRequest& p_pRequest, const std::string& strbaseId, const std::string& strTransGuid);
		bool UpdatePlanBaseInfo(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, const std::string& strTransGuid);
		bool UpdatePlanDetail(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, const std::string& strTransGuid);
		bool UpdatePlandata(const PROTOCOL::CNotifiSaveAlarmPlanRequest& p_pRequest, const std::string& strTransGuid);

		bool DeletePlanBaseInfo(const PROTOCOL::CNotifiDeleteAlarmPlanRequest& p_pRequest, const std::string& strTransGuid);
		bool DeletePlanDetail(const PROTOCOL::CNotifiDeleteAlarmPlanRequest& p_pRequest, const std::string& strTransGuid);

		// ��ѯ�����Ԥ����Ϣ
		bool SearchSaveAlarmPlan(const PROTOCOL::CNotifiSaveAlarmPlanRequest& p_pRequest, PROTOCOL::CNotifiSaveAlarmPlanResponse& p_pResponse);
		bool SearchPlanList(const PROTOCOL::CNotifiSearchPlanListRequest& p_pRequest, std::string p_pSql_id, DataBase::SQLRequest p_oSQLRequest, DataBase::IResultSetPtr& p_pRSet);
		bool SearchPlanInfo(const PROTOCOL::CNotifiSearchPlanInfoRequest& p_pRequest, std::string p_pSql_id, DataBase::SQLRequest p_oSQLRequest, DataBase::IResultSetPtr& p_pRSet);

	private:
		// ��ֵ��ѯ�������캯��
		std::string BuildMultiConditions(std::string p_strColumnName, std::string p_strOperator, std::vector<std::string> p_vecCondition);
		// ����ת�ַ�������,�ָ�
		std::string ArrayToStr(std::vector<PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest::KeyValuePair> p_pKeyValuePair,std::string s);
		// �ַ���ת����
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