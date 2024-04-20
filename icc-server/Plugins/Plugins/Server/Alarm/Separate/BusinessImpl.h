#pragma once

namespace ICC
{
	namespace Separate
	{
		//ͬ��״̬
		enum ESyncType
		{
			ADD = 1,
			SUBMIT,
			DEL,   //����˲����ã�ͬ��״̬��ֹʹ�ô�״̬  [1/29/2019 w16314]
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

			// ��������½Ӿ���Ϣ
			void OnCNotifiAddOrUpdateAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);
			// �������
			void OnNotifiAlarmAssignRequest(ObserverPattern::INotificationPtr p_pNotify);
			// ��ȡ��������
			void OnNotifiGetProcessCashRequest(ObserverPattern::INotificationPtr p_pNotify);
			// �������
			void OnNotifiProcessDoneRequest(ObserverPattern::INotificationPtr p_pNotify);
			// �ٴ���
			void OnNotifiProcessAgainRequest(ObserverPattern::INotificationPtr p_pNotify);
			// ���±༭��Ч����
			void OnNotifiEditAgainRequest(ObserverPattern::INotificationPtr p_pNotify);

		private:
			void Init();
			void OnTimer(ObserverPattern::INotificationPtr p_pNotify);

			void BuildRespondHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);

			// ����������Ϣ
			bool AddAlarmInfo(PROTOCOL::CAddOrUpdateAlarmRequest& l_oAddAlarmInfo, std::string p_strTransGuid = "");
			// ���´�����Ϣ
			bool UpdateAlarmInfo(PROTOCOL::CAddOrUpdateAlarmRequest& l_oUpdateAlarmInfo, std::string p_strTransGuid = "");
			// ���ݿ����������Ӿ�����
			bool InsertDBAlarmInfo(PROTOCOL::CAddOrUpdateAlarmRequest& p_pAlarmInfo, std::string p_strTransGuid = "");
			// ���ݿ���������½Ӿ�����
			bool UpdateDBAlarmInfo(PROTOCOL::CAddOrUpdateAlarmRequest& p_pAlarmInfo, std::string p_strTransGuid = "", bool p_isForce = false, bool p_isEditAgain = false);

			//����������ˮ
			bool AddAlarmLogInfo(PROTOCOL::CAlarmLogSync& p_pAlarmLogInfo, const std::string& strTransGuid = "");

			// ͬ������
			void SyncAlarmInfo(const PROTOCOL::CAddOrUpdateAlarmRequest& p_roAlarmSync, ESyncType p_iSyncType);

			//�Ӿ�����,�Ӿ���ɣ����봦���׶�
			bool AlarmAssign(const PROTOCOL::CAddOrUpdateAlarmRequest p_oAlarm, std::string p_strSrcSeat, std::string p_strDescSeat);
			//����������
			void AssignProcessToSeat(const std::string& p_strDescSeatNo, std::string p_strAlarmID, std::string p_strProcessID);

			void AssignToSeat(const std::string& p_strDescSeatNo, std::string p_strAlarmID);

			//���乫��������
			bool AssignPubProcess(const std::string& p_strSeatNo, std::string& p_strAlarmID);

			//��¼������ˮ
			bool InsertDBAlarmLogInfo(const PROTOCOL::CAlarmLogSync& p_AlarmLogInfo);

			//ͬ��������ˮ
			void SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync& p_rAlarmLogToSync);

			//��������
			bool InsertAlarmBookBeforeUpdate(const PROTOCOL::CAddOrUpdateAlarmRequest& p_oAlarmInfo, std::string& p_strResourceID, std::string p_strTransGuid = "");
			bool InsertAlarmBookAfterUpdate(const PROTOCOL::CAddOrUpdateAlarmRequest &p_oAlarmInfo, std::string& p_strResourceID, std::string p_strTransGuid = "");

			// ����������Ϣ
			bool AddProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_oAddProcessInfo, std::string p_strTransGuid = "");
			// ���´�����Ϣ
			bool UpdateProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_oUpdateProcessInfo, std::string p_strTransGuid = "");

			bool _InsertBackReason(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_oUpdateProcessInfo);

			// ͬ����������
			void SyncProcessInfo(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessToSync, int p_iSyncType);

			// ���ݿ���������´�������
			bool UpdateDBProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo);
			// ���ݿ���������봦������
			bool InsertDBProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo);

			//����ǰ�������ݿ⴦�����ݵ�insert_icc_t_pjdb_book
			bool InsertProcessBookBeforeUpdate(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo, std::string p_strResourceID, std::string p_strTransGuid = "");
			bool InsertProcessBookAfterUpdate(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo, std::string p_strResourceID, std::string p_strTransGuid = "");

			//ģ�⴦����Ϣ
			void AnalogProcessData(const PROTOCOL::CAddOrUpdateAlarmRequest& p_oAlarm, PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo);
		private:
			//
			std::string RealReceiptState(std::string p_strID, std::string p_strCurState, std::string p_strInputState, std::string strTransGuid = "");
			std::string RealProcessState(std::string p_strID, std::string p_strCurState, std::string p_strInputState, std::string strTransGuid = "");
			
			// �ɾ�״̬������ϲ��д����ϲ㴫��Ϊ׼��û�����ݿ�����ֵȡ���ݿ�ԭֵ������Ϊ01.
			std::string _GetCurrentProcessState(const std::string& p_strID, const std::string& p_strInputState, const std::string& strTransGuid = "");

			bool ExecSql(DataBase::SQLRequest p_oSQLReq);

			//��������Ĳ����б�����ˮ��Contentֵ
			std::string BuildAlarmLogContent(std::vector<std::string> p_vecParamList);
			//��ȡ������Ϣ
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
			std::string m_strAssistantPolice;	//�������Ƿ�Ϊ����
			std::string m_strPolice;			//�������Ƿ�Ϊ��
		private:
			std::map<std::string, unsigned int> m_mapReceiptStateOrder;
			std::map<std::string, unsigned int> m_mapProcessStateOrder;

		private:
			unsigned int m_iMaxCacheCount;
			std::string strStaffType;     //��Ա����
			std::string strStaffName;	  //��Ա����
			Data::CStaffInfo l_oStaffInfo;
			std::string l_strStaffInfo;

		};
	}
}