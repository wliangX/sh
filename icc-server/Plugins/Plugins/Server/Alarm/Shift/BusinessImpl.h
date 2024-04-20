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
		void OnReceiveUpdateShiftNotify(ObserverPattern::INotificationPtr p_pNotify);
		void OnReceiveQueryShiftNotify(ObserverPattern::INotificationPtr p_pNotify);
		void OnReceiveQueryShiftRelationNotify(ObserverPattern::INotificationPtr p_pNotify);

		void OnCNotifiUpdateShift(ObserverPattern::INotificationPtr p_pNotify);
		void OnCNotifiGetShift(ObserverPattern::INotificationPtr p_pNotify);
	public:
		void OnTimer();

	private:
		bool _InsertShift(const PROTOCOL::CShiftAddOrUpdateReqeust& request, const std::string& strShiftId, const std::string& strTransGuid);
		bool _InsertReleation(const PROTOCOL::CShiftAddOrUpdateReqeust& request, const std::string& strShiftId, const std::string& strTransGuid);
		bool _ProcessAddShift(const PROTOCOL::CShiftAddOrUpdateReqeust& request, PROTOCOL::CShiftAddOrUpdateRespond& response);
		
		bool _DeleteShiftRelation(const std::string& strShiftId, const std::string& strTransGuid);
		bool _UpdateShift(const PROTOCOL::CShiftAddOrUpdateReqeust& request, const std::string& strTransGuid);
		bool _ProcessUpdateShift(const PROTOCOL::CShiftAddOrUpdateReqeust& request, PROTOCOL::CShiftAddOrUpdateRespond& response);

		bool _CheckQueryShiftParam(const PROTOCOL::CShiftQueryReqeust& request, PROTOCOL::CShiftQueryRespond& response);
		bool _QueryShiftCount(const PROTOCOL::CShiftQueryReqeust& request, PROTOCOL::CShiftQueryRespond& response);
		bool _QueryShift(const PROTOCOL::CShiftQueryReqeust& request, PROTOCOL::CShiftQueryRespond& response);

		bool _QueryShiftRelation(const PROTOCOL::CShiftQueryRelationReqeust& request, PROTOCOL::CShiftQueryRelationRespond& response);

		void _SynShift(const PROTOCOL::CShiftAddOrUpdateReqeust& request, const std::string& strShiftId, const std::string& strTransGuid);

		std::string _QueryDeptPath(const std::string& strDeptCode, const std::string& strTransGuid, bool name_path = false);

		bool _AlarmLogSync(const PROTOCOL::CShiftAddOrUpdateReqeust& request, const std::string& strTransGuid);

		std::string _AlarmLogContent(std::vector<std::string> p_vecParamList);
		bool _InsertAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo, const std::string& strTransGuid = "");
		void _SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync);

		bool		_GetStaffInfo(const std::string& strStaffCode, Data::CStaffInfo& l_oStaffInfo);
		std::string	_GetPoliceTypeName(const std::string& strStaffType, const std::string& strStaffName);
	private:
		Log::ILogPtr			m_pLog;
		Config::IConfigPtr		m_pConfig;
		DataBase::IDBConnPtr				m_pDBConn;
		StringUtil::IStringUtilPtr	m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		Lock::ILockFactoryPtr m_pLockFty;
		DateTime::IDateTimePtr	m_pDateTime;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		HelpTool::IHelpToolPtr m_pHelpTool;
		Redis::IRedisClientPtr	m_pRedisClient;

		//config
		std::string m_strAssistantPolice;	//配置项是否为辅警
		std::string m_strPolice;			//配置项是否为民警
		std::string m_strCodeMode;			//警员号码模式：1、警员身份证号码 2、警员编号
	};
}