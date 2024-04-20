#pragma once
#include <algorithm> 
#include <Timer/ITimerFactory.h> 
#include "CommonTimer.h"

using namespace std;

//消息分包数

#define SELECT_TODAY_ALARM					"select_today_jjdb"
#define SELECT_TODAY_ALARM_CNT				"select_today_jjdb_count"
#define SELECT_SIMILAR_ALARM				"select_similar_alarm"
#define GET_TODAY_ALARM_RESPOND				"get_today_alarm_respond"
#define TODAY_ALARM_SYNC					"today_alarm_sync"
#define TOPIC_ALARM							"topic_alarm"

namespace ICC
{
	class CBusinessImpl : public CBusinessBase, public CCommonTimerCore
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();
	public:
		void OnNotifiAddAlarmAttention(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiUpdateAlarmAttention(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiDeleteAlarmAttention(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiQueryAlarmAttention(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiReceiveAlarmSync(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiReceiveProcessAlarmSync(ObserverPattern::INotificationPtr p_pNotify);		

		//判断警情是否关注
		void OnNotifiJudgeAlarmAttention(ObserverPattern::INotificationPtr p_pNotify);


	private:
		virtual void OnTimer(unsigned long uMsgId);
		void _CheckExpired();
		bool _DeleteAttention(const std::string& strAlarmId, const std::string& strStaffCode, std::string& strMsg);

		std::string _AlarmLogContent(std::vector<std::string> p_vecParamList);
		bool _InsertAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo, const std::string& strTransGuid = "");
		void _SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync);
		void _QueryStaffName(const std::string& strStaffCode, PROTOCOL::AttentionNameAndDeptName& response, const std::string& strTransGuid = "");
		bool	_GetStaffInfo(const std::string& strStaffCode, Data::CStaffInfo& l_oStaffInfo);
		std::string	_GetPoliceTypeName(const std::string& strStaffType, const std::string& strStaffName);
	private:
		Lock::ILockPtr m_pAlarmLock;
		Lock::ILockPtr m_pTimeLock;

		Log::ILogPtr			m_pLog;
		Config::IConfigPtr		m_pConfig;
		DataBase::IDBConnPtr		m_pDBConn;
		StringUtil::IStringUtilPtr	m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		Lock::ILockFactoryPtr m_pLockFty;
		DateTime::IDateTimePtr	m_pDateTime;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		Timer::ITimerManagerPtr m_pTimerMgr;
		HelpTool::IHelpToolPtr      m_pHelpTool;
		Redis::IRedisClientPtr		m_pRedisClient;		//redis客户端
	private:
		static string m_strCurTime;
		int64 m_IntervalTime;

		uint64 m_lTerminalExpiredTime;
		uint64 m_lExpiredTime;
		CCommonTimer m_timerCheckExpired;

		//config
		std::string m_strAssistantPolice;	//配置项是否为辅警
		std::string m_strPolice;			//配置项是否为民警
		std::string m_strCodeMode;			//警员号码模式：1、警员身份证号码 2、警员编号
	
		std::string m_strBeginTime;         //查询开始时间
		
	};
}