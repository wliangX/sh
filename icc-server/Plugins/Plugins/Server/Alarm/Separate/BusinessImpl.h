#pragma once

namespace ICC
{
	namespace Separate
	{
		//同步状态
		enum ESyncType
		{
			ADD = 1,
			SUBMIT,
			DEL,   //服务端不适用，同步状态禁止使用此状态  [1/29/2019 w16314]
			EDIT
		};

		class CBusinessImpl :
			public CBusinessBase
		{

		public:
			virtual void OnInit();
			virtual void OnStart();
			virtual void OnStop();
			virtual void OnDestroy();

		private:

		public:

			// 新增或更新接警信息
			void OnCNotifiAddOrUpdateAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);
			// 警情分配
			void OnNotifiAlarmAssignRequest(ObserverPattern::INotificationPtr p_pNotify);
			// 获取处警缓存
			void OnNotifiGetProcessCashRequest(ObserverPattern::INotificationPtr p_pNotify);
			// 处警完成
			void OnNotifiProcessDoneRequest(ObserverPattern::INotificationPtr p_pNotify);
			// 再处警
			void OnNotifiProcessAgainRequest(ObserverPattern::INotificationPtr p_pNotify);
			// 重新编辑无效警情
			void OnNotifiEditAgainRequest(ObserverPattern::INotificationPtr p_pNotify);

		private:
			void Init();
			void OnTimer(ObserverPattern::INotificationPtr p_pNotify);

			void BuildRespondHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);

			// 新增处警信息
			bool AddAlarmInfo(PROTOCOL::CAddOrUpdateAlarmRequest& l_oAddAlarmInfo, std::string p_strTransGuid = "");
			// 更新处警信息
			bool UpdateAlarmInfo(PROTOCOL::CAddOrUpdateAlarmRequest& l_oUpdateAlarmInfo, std::string p_strTransGuid = "");
			// 数据库操作：插入接警数据
			bool InsertDBAlarmInfo(PROTOCOL::CAddOrUpdateAlarmRequest& p_pAlarmInfo, std::string p_strTransGuid = "");
			// 数据库操作：更新接警数据
			bool UpdateDBAlarmInfo(PROTOCOL::CAddOrUpdateAlarmRequest& p_pAlarmInfo, std::string p_strTransGuid = "", bool p_isForce = false, bool p_isEditAgain = false);

			//新增警情流水
			bool AddAlarmLogInfo(PROTOCOL::CAlarmLogSync& p_pAlarmLogInfo, const std::string& strTransGuid = "");

			// 同步数据
			void SyncAlarmInfo(const PROTOCOL::CAddOrUpdateAlarmRequest& p_roAlarmSync, ESyncType p_iSyncType);

			//接警分配,接警完成，进入处警阶段
			bool AlarmAssign(const PROTOCOL::CAddOrUpdateAlarmRequest p_oAlarm, std::string p_strSrcSeat, std::string p_strDescSeat);
			//处警单分配
			void AssignProcessToSeat(const std::string& p_strDescSeatNo, std::string p_strAlarmID, std::string p_strProcessID);

			void AssignToSeat(const std::string& p_strDescSeatNo, std::string p_strAlarmID);

			//分配公共处警单
			bool AssignPubProcess(const std::string& p_strSeatNo, std::string& p_strAlarmID);

			//记录警情流水
			bool InsertDBAlarmLogInfo(const PROTOCOL::CAlarmLogSync& p_AlarmLogInfo);

			//同步警情流水
			void SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync& p_rAlarmLogToSync);

			//备份数据
			bool InsertAlarmBookBeforeUpdate(const PROTOCOL::CAddOrUpdateAlarmRequest& p_oAlarmInfo, std::string& p_strResourceID, std::string p_strTransGuid = "");
			bool InsertAlarmBookAfterUpdate(const PROTOCOL::CAddOrUpdateAlarmRequest &p_oAlarmInfo, std::string& p_strResourceID, std::string p_strTransGuid = "");

			// 新增处警信息
			bool AddProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_oAddProcessInfo, std::string p_strTransGuid = "");
			// 更新处警信息
			bool UpdateProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_oUpdateProcessInfo, std::string p_strTransGuid = "");

			bool _InsertBackReason(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_oUpdateProcessInfo);

			// 同步处警数据
			void SyncProcessInfo(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessToSync, int p_iSyncType);

			// 数据库操作：更新处警数据
			bool UpdateDBProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo);
			// 数据库操作：插入处警数据
			bool InsertDBProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo);

			//更新前备份数据库处警数据到insert_icc_t_pjdb_book
			bool InsertProcessBookBeforeUpdate(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo, std::string p_strResourceID, std::string p_strTransGuid = "");
			bool InsertProcessBookAfterUpdate(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo, std::string p_strResourceID, std::string p_strTransGuid = "");

			//模拟处警信息
			void AnalogProcessData(const PROTOCOL::CAddOrUpdateAlarmRequest& p_oAlarm, PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo);
		private:
			//
			std::string RealReceiptState(std::string p_strID, std::string p_strCurState, std::string p_strInputState, std::string strTransGuid = "");
			std::string RealProcessState(std::string p_strID, std::string p_strCurState, std::string p_strInputState, std::string strTransGuid = "");
			
			// 派警状态，如果上层有传以上层传的为准，没传数据库中有值取数据库原值，否则为01.
			std::string _GetCurrentProcessState(const std::string& p_strID, const std::string& p_strInputState, const std::string& strTransGuid = "");

			bool ExecSql(DataBase::SQLRequest p_oSQLReq);

			//根据输入的参数列表构造流水的Content值
			std::string BuildAlarmLogContent(std::vector<std::string> p_vecParamList);
			//获取警情信息
			bool GetAlarm(std::string p_strID, PROTOCOL::CAddOrUpdateAlarmRequest& p_oAlarm);
			bool GetProcess(std::string p_strID, PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo);

		private:
			DateTime::IDateTimePtr m_pDateTime;
			HelpTool::IHelpToolPtr m_pHelpTool;
			StringUtil::IStringUtilPtr m_pString;
			Config::IConfigPtr m_pConfig;
			Log::ILogPtr m_pLog;
			DataBase::IDBConnPtr m_pDBConn;
			JsonParser::IJsonFactoryPtr m_pJsonFty;
			ObserverPattern::IObserverCenterPtr m_pObserverCenter;
			Timer::ITimerManagerPtr m_pTimerMgr;
			Redis::IRedisClientPtr m_pRedisClient;

			//config
			std::string m_strAssistantPolice;	//配置项是否为辅警
			std::string m_strPolice;			//配置项是否为民警
		private:
			std::map<std::string, unsigned int> m_mapReceiptStateOrder;
			std::map<std::string, unsigned int> m_mapProcessStateOrder;

		private:
			unsigned int m_iMaxCacheCount;
			std::string strStaffType;     //警员类型
			std::string strStaffName;	  //警员姓名
			Data::CStaffInfo l_oStaffInfo;
			std::string l_strStaffInfo;

		};
	}
}