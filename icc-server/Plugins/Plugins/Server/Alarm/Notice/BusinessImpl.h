#pragma once

namespace ICC
{
	class CBusinessImpl :
		public CBusinessBase
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();
	public:
		void LoadParams();

		// 通知/公告
		void LoadAllNotices();
		void OnNotifiAddNoticeRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetNoticeRequest(ObserverPattern::INotificationPtr p_pNotify);

		// 接警提示
		void LoadAllTips();
		void OnNotifiAddTipsRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiModifyTipsRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiDelTipsRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetTipsRequest(ObserverPattern::INotificationPtr p_pNotify);

		//催促督办
		void OnNotifiAddUrgesRequest(ObserverPattern::INotificationPtr p_pNotify);

		//根据话务结束同步，发出振铃超时、通话超时同步
		void LoadTimeOutEvents();
		void OnNotifiCallOverSync(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiDelTimeOutEventRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetTimeOutEventRequest(ObserverPattern::INotificationPtr p_pNotify);

	private:
		PROTOCOL::CHeader CreateProtocolHeader(const std::string& p_strCmd, const std::string& p_strRequest = "",
			const std::string& p_strRequestType = SENDTYPE_QUEUE, const std::string& p_strRelatedId = "");

		std::string GetNoticeType(const std::string& p_strInfo);
		std::string GetNoticeInfo(const PROTOCOL::CAddNoticeRequest& p_oRequest, const std::string& p_strNoticeId);
		bool InsertNotice(const PROTOCOL::CAddNoticeRequest& p_oRequest, const std::string& p_strNoticeId);
		bool UpdateNotice(const std::string& p_strNoticeId);
		bool DeleteNotice(const std::string& p_strNoticeType);
		void SendAddNoticeSync(const PROTOCOL::CAddNoticeRequest& p_oRequest);

		//////////////////////////////////////////////////////////////////////////
		std::string GetTipsInfo(const PROTOCOL::CSetTipsRequest& p_oRequest, const std::string& p_strTipsId);
		bool InsertTips(const PROTOCOL::CSetTipsRequest& p_oRequest, const std::string& p_strTipsId);
		bool UpdateTips(const PROTOCOL::CSetTipsRequest& p_oRequest, const std::string& p_strTipsId);
		bool DeleteTips(const std::string& p_strTipsId);
		void SendTipsSync(const PROTOCOL::CSetTipsRequest& p_oRequest, const std::string& p_strTipsId, const std::string& p_strSynType);

		//////////////////////////////////////////////////////////////////////////
		bool SelectProcessAlarmByAlarmID(std::vector<PROTOCOL::CUrgesRequest::CBody> &p_outVecData, const std::string& p_strInAlarmId);
		void SendUrgesSync(std::vector<PROTOCOL::CUrgesRequest::CBody> &p_outVecData, const std::string& p_strContent);
		void AddUrgesAlarmLog(std::vector<PROTOCOL::CUrgesRequest::CBody> &p_outVecData, const std::string& p_strContent);

		std::string GenAlarmLogContent(std::vector<std::string> p_vecParamList);
		std::string GenAlarmLogAttach(std::string& type, std::string& id);
		bool InsertDBAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo);
		void SendAlarmLogInfoSync(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync);

		//////////////////////////////////////////////////////////////////////////
		bool SelectAlarmByCallRefId(PROTOCOL::CTimeOutEvent::CBody& p_oOutAlarm, const std::string& p_strInCallRefId);
		bool SelectAlarmByCallRefId(PROTOCOL::CTimeOutEvent::CBody& p_oOutAlarm, const std::string& p_strInCallRefId, std::string p_strTalkTime);
		std::string GetTimeOutEventInfo(const PROTOCOL::CTimeOutEvent::CBody& p_oTimeOutEvent);
		bool InsertTimeOutEvent(const PROTOCOL::CTimeOutEvent::CBody& p_oTimeOutEvent);
		bool DeleteTimeOutEvent(const std::string& p_strTimeOutEventId);
		void SendTimeOutEventSync(const PROTOCOL::CTimeOutEvent::CBody& p_oTimeOutEvent, const std::string& p_strSynType);
		void ProcessTimeOutEvent(const std::string& p_strCallRefId, const std::string& p_strStartTime, const std::string& p_strEndTime, const std::string& p_strAlarmType = "0");

	private:
		unsigned int				m_nRingTimeOut;	//通话超时（单位：秒）
		unsigned int				m_nTalkTimeOut;	//通话超时（单位：分钟）

		Log::ILogPtr				m_pLog;
        DataBase::IDBConnPtr		m_pDBConn;
		Config::IConfigPtr			m_pConfig;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
        StringUtil::IStringUtilPtr	m_pString;
		DateTime::IDateTimePtr		m_pDateTime;
		Redis::IRedisClientPtr		m_pRedisClient;			//redis客户端
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
	};
}




