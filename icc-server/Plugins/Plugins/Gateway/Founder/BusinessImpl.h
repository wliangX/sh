#pragma once
#include "OracleClient.h"

namespace ICC
{
	enum CODEPAGE
	{
		CODE_ANSI,
		CODE_UTF7,
		CODE_UTF8,
		CODE_GB2312
	};
	//////////////////////////////////////////////////////////////////////////
	// Founder����ҵ������
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
		void OnReceiveAlarmSyncNotify(ObserverPattern::INotificationPtr p_pNotify);	
		void OnReceiveProcessSyncNotify(ObserverPattern::INotificationPtr p_pNotify);

	private:
		void _ReadDataThread();		
		void _Test();		

		/*****************************************************************
		 * 
		 ****************************************************************/
		bool _SyncDataFromICCToFounder(const std::string& strEndTime);
		
		//ͬ���Ӿ������ݵ�����
		bool _SyncAlarmDataFromICCToFounder(const std::string& strBeginTime,const std::string& strEndTime);
		bool _ReadAlarmDataFromICC(const std::string& strBeginTime, const std::string& strEndTime, std::vector<std::map<std::string, std::string>>& datas);
		bool _WriteAlarmDataToFounder(std::vector<std::map<std::string, std::string>>& datas);
		bool _ExistAlarmData(const std::string& strAlarmId, bool& bExist);		

		//ͬ���ɾ������ݵ�����
		bool _SyncProcessDataFromICCToFounder(const std::string& strBeginTime, const std::string& strEndTime);
		bool _ReadProcessDataFromICC(const std::string& strBeginTime, const std::string& strEndTime, std::vector<std::map<std::string, std::string>>& datas);
		bool _WriteProcessDataToFounder(std::vector<std::map<std::string, std::string>>& datas);
		bool _ExistProccessData(const std::string& strProcessId, bool& bExist);
	

		//ͬ��12345���ݵ�����
		bool _SyncFeedbackDataFromICCToFounder(const std::string& strBeginTime, const std::string& strEndTime);
		bool _ReadFeedbackDataFromICC(const std::string& strBeginTime, const std::string& strEndTime, std::vector<std::map<std::string, std::string>>& datas);
		bool _WriteFeedbackDataToFounder(std::vector<std::map<std::string, std::string>>& datas);
		bool _ExistFeedbackData(const std::string& strFeedbackId, bool& bExist);

		//�ݲ�ʹ��
		void _InsertProcessData(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& processData);
		void _UpdateProcessData(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& processData);
		void _AssignProcessSql(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& processData, DataBase::SQLRequest& sqlRequest);	

		void _InsertAlarmData(const PROTOCOL::CAlarmInfo alarmData);
		void _UpdateAlarmData(const PROTOCOL::CAlarmInfo alarmData);
		void _AssignAlarmSql(const PROTOCOL::CAlarmInfo alarmData, DataBase::SQLRequest& sqlRequest);
		bool _SyncDataFromFounderToICC(const std::string& strEndTime);

		//��ȡ�����ԽӾ����ݵ�ICC
		bool _ProcAlarmData(const std::string& strBeginTimes, const std::string& strEndTime);
		bool _ReadAlarmDataFromOracle(const std::string& strBeginTime, const std::string& strEndTime, std::vector<std::map<std::string, std::string>>& datas);
		bool _WriteAlarmDataToICC(std::vector<std::map<std::string, std::string>>& datas);
		bool _ExistAlarmDataByPostgres(const std::string& strAlarmId, bool& bExist);
		//bool SendAlarmUpdateRequest(std::vector<PROTOCOL::CAlarmInfo>& vecAlarmDatas);

		//���м���ȡ�����ɾ�����
		bool _ProcProcessData(const std::string& strBeginTimes, const std::string& strEndTime);
		bool _ReadProcessData(const std::string& strBeginTimes, const std::string& strEndTime, std::vector< PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas);
		bool _WriteProcessDataToICC(std::vector< PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas);
		bool _SendProcessData(std::vector< PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas);
		bool _IsNeedUpdate(const std::string& strAlarmId, bool& bNeedUpdate);
		bool UpdateOperateProcessLog(std::vector< PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas);
		bool InsertDBAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo);
		std::string BuildAlarmLogContent(std::vector<std::string> &p_vecParamList);


		//���м���ȡ����������
		bool _ProcFeedbackData(const std::string& strBeginTimes, const std::string& strEndTime);		
		bool _ReadFeedbackDataFromOracle(const std::string& strBeginTime, const std::string& strEndTime, std::vector<std::map<std::string, std::string>>& datas);
		bool _WriteFeedbackDataToICC(std::vector<std::map<std::string, std::string>>& datas);
		bool _FilterProcessData_FKDB(std::vector< PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas);
		bool _ExistFeedbackDataInICC(const std::string& strId, const std::string& strUpdateTime, bool& bExist, bool& bUpdate);
		bool _SendFeedbackData(std::vector< PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas);
		bool UpdateOperateFeedbackLog(std::vector< PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas);
		

		//�ݲ�ʹ��
		bool _ReadFeedbackData(const std::string& strBeginTimes, const std::string& strEndTime, std::vector<PROTOCOL::CFeedBackRequest>& vecFeedbackDatas);
		/*bool _SendFeedbackData(std::vector<PROTOCOL::CFeedBackRequest>& vecFeedbackDatas);*/

		/*****************************************************************
		 * 
		 ****************************************************************/
		bool _GetPreTimestamp(const std::string& strKey, std::string& strPreTimestamp);
		void _WriteTimestamp(const std::string& strKey, const std::string& strTimestamp);
		std::string _StandardDateTime(const std::string& strTimeStamp);		

		std::string FromLocalToUTC(const std::string& strLocalDateTime);
		std::string FromUTCToLocal(const std::string& strUTCDateTime);
		const DateTime::CDateTime ERROR_DATE_TIME = 0;

		bool _ReadProcessDataByProcessID(std::vector<std::map<std::string, std::string>>& datas,std::vector<PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas);
		std::string _ReadProcessIDByAlarmID(std::string &strAlarmId);
		/*bool _ReadProcessIDByAlarmIDandCode(std::string& strAlarmId, std::string &strProcessCode,std::string& strProcessId, std::string& strDispathcDeptName, std::string& strProcessDeptName);*/

	private:
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		DateTime::IDateTimePtr m_pDateTime;
		StringUtil::IStringUtilPtr m_pString;
		HelpTool::IHelpToolPtr m_pHelpTool;
		DataBase::ISqlBuilderPtr	m_pSQLBD;
		DataBase::ISqlRequestFactoryPtr m_pSqlReqeustFactory;
		Redis::IRedisClientPtr m_pRedisClient;
		DataBase::IDBConnPtr m_pDBConn;

		OracleClientPtr m_pOracleClient;

		boost::shared_ptr<boost::thread>		m_pReadDataThread;

		//������
		long long m_llCheckInterval;
		std::string m_strTableModeName;
		std::string m_strInvalidAlarm;                          //��Ч�����Ƿ����м�� 1���� 0:����
		std::string m_strSelfDispose;                           //���Դ��ľ����Ƿ����м��
		std::string m_strFpsIp;									//FPS��IP��ַ
		std::string m_strFpsPort;								//FPS�Ķ˿ں�
		std::string m_strDelayTime;							//ȡ���ݵ���ʱʱ�䣬һ��Ϊȡ����ʱ����෴��
	};
}