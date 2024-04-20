#pragma once
#include "CommonWorkThread.h"
#define THREADCOUNT 4

namespace ICC
{
	class CBusinessImpl;
	typedef void (CBusinessImpl::* ProcNotify)(ObserverPattern::INotificationPtr p_pNotifiRequest);

	class CBusinessImpl :
		public CBusinessBase,public CCommonThreadCore
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:

		void LoadPhoneDivision();

		PROTOCOL::CHeader CreateProtocolHeader(const std::string& p_strCmd, const std::string& p_strRequest = "",
			const std::string& p_strRequestType = SendType_Queue, const std::string& p_strRelatedId = "");

		//	���ڴ����ݿ����Ƿ���ڸû��� ID 
		bool CallRefIdIsExist(const std::string& p_strCTICallRefId);
		bool CallRefIdIsExistByCallTable(const std::string& p_strCTICallRefId);
		std::string GetRelAlarmIDByCallRefID(const std::string p_strCTICallRefId);
		void DeleteCallRefId(const std::string& p_strCTICallRefId);
		void CheckCSTCallrefId(std::string& p_strCTICallRefId, const std::string& p_strCstCallrefId);
		void AddCallInfo(const std::string& p_strCTICallRefId, const std::string& p_strCallerId, const std::string& p_strCalledId, 
			const std::string& p_strState, const std::string& p_strTime, const std::string& p_strCstCallrefId);

		//////////////////////////////////////////////////////////////////////////
		//	������¼�������ӱ�
		void InsertSubCallEvent(const std::string& p_strCallRefId, const std::string& p_strState, const std::string& p_strStateTime,
			const std::string& p_strSponsor, const std::string& p_strReveiver = "");
		//	����״̬ͬ��
		void OnNotifiCallStateSync(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	�������ͬ��
		void OnNotifiCallOverSync(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	�豸״̬ͬ��
		void OnNotifiDeviceStateSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	�ӻ������ͬ��
		void OnNotifiDetailCallOverSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	����ӹ�ͬ��
		void OnNotifiTakeOverCallSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//////////////////////////////////////////////////////////////////////////
		//	�ز���ˮ
		void SyncAlarmLogInfo(const std::string& p_strCTICallRefId, const std::string& p_strCaseId, const std::string& p_strLogContent, const std::string& p_strTime, const std::string& strGuid, const std::string& strReceivedTime);
		std::string GenAlarmLogAttach(std::string& type, std::string& id);
		void InsertCallBackLog(const std::string& p_strCTICallRefId, const std::string& p_strCaseId, const std::string& p_strCallerId,
			const std::string& p_strCalledId, const std::string& p_strTime);
		std::string QueryAlarmId(const std::string& p_strRelateCTICallRefId);
		//	�ز������뾯�������ϵ
		void InsertCallBackRelation(const std::string& p_strCTICallRefId, const std::string& p_strCaseId,
			const std::string& p_strReceiptCode, const std::string& p_strReceiptName, const std::string& p_strTime);
		//	�ز�����ͬ��
		void OnNotifiCallBackSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//////////////////////////////////////////////////////////////////////////
		void CR_RemoveReleaseCall(ObserverPattern::INotificationPtr p_pNotifiRequest, const std::string& p_strMsgId);
		void Sync_RemoveReleaseCall(std::vector<std::string>& p_vCallRefIdQueue);
		//	ɾ�����ͻ����¼����
		void OnNotifiRemoveReleaseCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//////////////////////////////////////////////////////////////////////////
		//	���������ʱ���غ������б����㲥�� CTI ���
		void LoadBlackList();
		//	��ȡ���к���������, CTI �������ʱ����
		void OnNotifiLoadAllBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	���ú�����ͬ��
		void OnNotifiSetBlackListSync(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	ɾ��������ͬ��
		void OnNotifiDeleteBlackListSync(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	ɾ�����к�����ͬ��
		void OnNotifiDeleteAllBlackListSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//////////////////////////////////////////////////////////////////////////
		//	���������ʱ���غ������б�
		void LoadRedList();
		//	���ú�����
		void OnNotifiSetRedListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	ɾ��������
		void OnNotifiDeleteRedListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	��ȡ���к�����
		void OnNotifiGetRedListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	���ú�����ͬ��
		void SetRedListSync(PROTOCOL::CSetRedListRequest& p_oRedListObj, /*const std::string& p_strGuid,*/ const std::string& p_strSynctype);

		//??????
		void OnNotifiGetViolationRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnGetViolationTimeoutCfgRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//////////////////////////////////////////////////////////////////////////
		//	��ʷ�����ѯ����
		void OnNotifiQueryCallEventRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	�������ܽӻ����ѯ����
		void OnNotifiQueryBlackCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//��ѯ�ӻ�����Ϣ
		void OnNotifiQueryDetailCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//////////////////////////////////////////////////////////////////////////
		void OnRedoSQLTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnViolationCheckTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnRecvAcdAgentStateSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//Nacos��ȡ����������Ϣ
		void OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	������������
		void OnReceiveAlarmRelationRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	�ɳ��������ѯ����
		void OnReceiveQueryProcessCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//¼��������¼���ɹ�,DBprocesд��
		void ReceiveRecordFileUpRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//¼������������ʷ����ʱ,DBproces��ѯ���ݿ����Ƿ���ڴ˻��񣬲���������û�����Ϣ��icc_t_callevent����
		void ReceiveHistoryCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

	private:
		bool _UpdateStatisticInfo(const std::string& hangup_type, const std::string& caller_id, const std::string& called_id, const std::string& call_direction);

		bool _QueryDeptNamePath(const std::string& strDeptCode, std::string& strDeptName, std::string& strDeptPath, std::string& strDeptIdentify);
		std::string _QueryAcdDept(const std::string& strAcd);
		std::string _QueryDeptByNumber(const std::string& strNumber, const std::string& strNumberType);

		std::string _QueryNumberType(const std::string& strNumber);
		bool        _QueryAgentNumber(const std::string& strNumber, std::string& strCount);
		bool        _QueryProcessNumber(const std::string& strNumber, std::string& strCount);

		bool        _QueryReveiverInfo(const std::string& strSeatNumber, std::string& strStaffCode, std::string& strStaffName);

		bool        _UpdateProcessSeatReleaseAcceptState(const std::string& strCallrefId, const std::string& strAcceptState);

		CCommonWorkThread& _GetThread(int iCallId);

		bool _IsProcessSeat(const std::string& strCalled, std::string& strDeptCode); 		
		void _SyncDeviceState(PROTOCOL::IRespondPtr pResponse, const std::string& strDeptCode);
		void _ProcDeviceStateChanged(PROTOCOL::IRespondPtr pResponse);
		void _RealDeviceState(const std::string& strCallrefId, std::string& strState);
		
		bool _AlarmRelation(const PROTOCOL::CVcsAlarmRelationRequest& request, std::string& strError);
		bool _CheckParam(const PROTOCOL::CVcsAlarmRelationRequest& request, std::string& strError);
		void _ProcAlarmRelation(ObserverPattern::INotificationPtr p_pNotifiRequest);
		
		std::string _BuildQueryCallInfoCondition(const PROTOCOL::CVcsQueryCallInfoRequest& request, std::string& strUnNeedPaging);
		bool _CheckQueryCallParam(const PROTOCOL::CVcsQueryCallInfoRequest& request, std::string& strError);
		bool _QueryCallInfo(const PROTOCOL::CVcsQueryCallInfoRequest& request, PROTOCOL::CVcsQueryCallInfoRespond& respond, std::string& strError);
		void _ProcQueryCallInfo(ObserverPattern::INotificationPtr p_pNotifiRequest);
		virtual void ProcMessage(CommonThread_data msg_data);
		void BuildManualAcceptConfig(std::string l_manualAcceptType, std::string& l_value);
		void SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
		std::string BuildTimeLen(std::string first_time, std::string second_time);
		std::string CalcRingLen(std::string l_strTalkTime, std::string l_strRingTime, std::string l_strHangupTime, std::string l_strRingBackTime);
		bool _IsAutoUrgeCallNum(std::string strCaller);

		//��ѯ����¼���ϴ��ɹ�֮ǰ���û����Ƿ����¼��
		bool SearchRecordID(PROTOCOL::CRecordFileUpSyncRequest l_oRecordFileUpSyncRequest, std::string& strRecordID);
		bool _UpdateCalleventWithRecord(const std::string& strRecordFileID, const std::string& strCallrefID);
		bool _IsExistsCallrefID(const std::string& strCallRefID,bool & bExist);
		bool _InsertCalleventWithHistoryCall(const PROTOCOL::CGetHistoryCallListRequest& request,int i);
		bool _GetAcdInfo(const std::string& strAcd, PROTOCOL::CAcdInfo& l_AcdInfo);
		bool _FindIfCreateAlarm(const std::string& strCallrefID,std::string &strAlarmID,std::string& strReceiverCode,std::string& strReceiverName);
		bool _GetDeptInfo(const std::string& strDeptCode, PROTOCOL::CDeptInfo& l_DeptInfo);
		bool _JudgeIsCallback(const std::string& strCallrefID,bool& isExist);
		bool _UpdateCalleventWithHistoryCall(const PROTOCOL::CGetHistoryCallListRequest& request, int i);
		bool _IsNeedUpdateCallevent(const std::string& strCallrefID, bool& isNeedUpdate);
		bool _GetSeatInfoInRedis(const std::string& strSeat,PROTOCOL::CSeatDataInfo& l_SeatDataInfo);
		std::string _StaffIdNoToStaffCode(const std::string& strStaffIdNo);
private:
		void    _OnReceiveNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		void    _DispatchNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust, const std::string& strCmdName);
		//virtual void ProcMessage(CommonThread_data msg_data);
		CommonWorkThreadPtr _GetThread();
		void    _InitProcNotifys();
		void    _CreateThreads();
		void    _DestoryThreads();

		unsigned int                        m_uCurrentThreadIndex;
		unsigned int                        m_uProcThreadCount;
		std::string                         m_strDispatchMode;

		std::mutex                          m_mutexDispatchThread;
		std::vector<CommonWorkThreadPtr>    m_vecProcThreads;
		std::map<std::string, ProcNotify>   m_mapFuncs;

		CCommonWorkThread                   m_threadViolation;
		std::atomic_bool                    m_bViolationIsRunning;

	private:
		Log::ILogPtr						m_pLog;
		Config::IConfigPtr					m_pConfig;
		DataBase::IDBConnPtr				m_pInitDBConn;		//	�����̵����ݿ����ӣ����ں��������绰����ǰ׺����
		DataBase::IDBConnPtr				m_pRequestDBConn;	//	���ݿ��¼����ɾ���ġ������ӣ�������ʷ����ͺ�������¼��ѯ�������¼���漰����
		StringUtil::IStringUtilPtr			m_pStringUtil;
		JsonParser::IJsonFactoryPtr			m_pJsonFty;
		DateTime::IDateTimePtr				m_pDateTime;
		Redis::IRedisClientPtr				m_pRedisClient;
		Timer::ITimerManagerPtr				m_pTimerManager;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		MsgCenter::IMessageCenterPtr		m_pMsgCenter;

		IHttpClientPtr 						m_pHttpClient;

		unsigned int                        m_uIndex;
		std::mutex                          m_mutexThread;
		CCommonWorkThread                   m_threadProc[THREADCOUNT];
		std::string                         m_strEnableProcessAlarm;

		std::string							m_strNacosServerIp;
		std::string							m_strNacosServerPort;
		std::string							m_strNacosServerNamespace;
		std::string							m_strNacosServerGroupName;
		boost::mutex						m_mutexNacosParams;

		std::string							m_strAutoUrgeCallNumber;
	};
}
