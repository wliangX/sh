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

		//	在内存数据库中是否存在该话务 ID 
		bool CallRefIdIsExist(const std::string& p_strCTICallRefId);
		bool CallRefIdIsExistByCallTable(const std::string& p_strCTICallRefId);
		std::string GetRelAlarmIDByCallRefID(const std::string p_strCTICallRefId);
		void DeleteCallRefId(const std::string& p_strCTICallRefId);
		void CheckCSTCallrefId(std::string& p_strCTICallRefId, const std::string& p_strCstCallrefId);
		void AddCallInfo(const std::string& p_strCTICallRefId, const std::string& p_strCallerId, const std::string& p_strCalledId, 
			const std::string& p_strState, const std::string& p_strTime, const std::string& p_strCstCallrefId);

		//////////////////////////////////////////////////////////////////////////
		//	新增记录到话务子表
		void InsertSubCallEvent(const std::string& p_strCallRefId, const std::string& p_strState, const std::string& p_strStateTime,
			const std::string& p_strSponsor, const std::string& p_strReveiver = "");
		//	话务状态同步
		void OnNotifiCallStateSync(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	话务结束同步
		void OnNotifiCallOverSync(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	设备状态同步
		void OnNotifiDeviceStateSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	子话务结束同步
		void OnNotifiDetailCallOverSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	话务接管同步
		void OnNotifiTakeOverCallSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//////////////////////////////////////////////////////////////////////////
		//	回拨流水
		void SyncAlarmLogInfo(const std::string& p_strCTICallRefId, const std::string& p_strCaseId, const std::string& p_strLogContent, const std::string& p_strTime, const std::string& strGuid, const std::string& strReceivedTime);
		std::string GenAlarmLogAttach(std::string& type, std::string& id);
		void InsertCallBackLog(const std::string& p_strCTICallRefId, const std::string& p_strCaseId, const std::string& p_strCallerId,
			const std::string& p_strCalledId, const std::string& p_strTime);
		std::string QueryAlarmId(const std::string& p_strRelateCTICallRefId);
		//	回拨话务与警情关联关系
		void InsertCallBackRelation(const std::string& p_strCTICallRefId, const std::string& p_strCaseId,
			const std::string& p_strReceiptCode, const std::string& p_strReceiptName, const std::string& p_strTime);
		//	回拨话务同步
		void OnNotifiCallBackSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//////////////////////////////////////////////////////////////////////////
		void CR_RemoveReleaseCall(ObserverPattern::INotificationPtr p_pNotifiRequest, const std::string& p_strMsgId);
		void Sync_RemoveReleaseCall(std::vector<std::string>& p_vCallRefIdQueue);
		//	删除早释话务记录请求
		void OnNotifiRemoveReleaseCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//////////////////////////////////////////////////////////////////////////
		//	本插件启动时加载黑名单列表，并广播给 CTI 插件
		void LoadBlackList();
		//	获取所有黑名单请求, CTI 插件启动时发起
		void OnNotifiLoadAllBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	设置黑名单同步
		void OnNotifiSetBlackListSync(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	删除黑名单同步
		void OnNotifiDeleteBlackListSync(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	删除所有黑名单同步
		void OnNotifiDeleteAllBlackListSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//////////////////////////////////////////////////////////////////////////
		//	本插件启动时加载红名单列表
		void LoadRedList();
		//	设置红名单
		void OnNotifiSetRedListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	删除红名单
		void OnNotifiDeleteRedListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	获取所有红名单
		void OnNotifiGetRedListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	设置红名单同步
		void SetRedListSync(PROTOCOL::CSetRedListRequest& p_oRedListObj, /*const std::string& p_strGuid,*/ const std::string& p_strSynctype);

		//??????
		void OnNotifiGetViolationRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnGetViolationTimeoutCfgRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//////////////////////////////////////////////////////////////////////////
		//	历史话务查询请求
		void OnNotifiQueryCallEventRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	黑名单拒接话务查询请求
		void OnNotifiQueryBlackCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//查询子话务信息
		void OnNotifiQueryDetailCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//////////////////////////////////////////////////////////////////////////
		void OnRedoSQLTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnViolationCheckTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnRecvAcdAgentStateSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//Nacos获取基本配置信息
		void OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	警单关联请求
		void OnReceiveAlarmRelationRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	派出所话务查询请求
		void OnReceiveQueryProcessCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//录音服务器录音成功,DBproces写库
		void ReceiveRecordFileUpRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//录音服务请求历史话务时,DBproces查询数据库中是否存在此话务，不存在则造该话务信息到icc_t_callevent表中
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

		//查询此条录音上传成功之前，该话务是否存在录音
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
		DataBase::IDBConnPtr				m_pInitDBConn;		//	主进程的数据库链接，用于黑名单、电话号码前缀加载
		DataBase::IDBConnPtr				m_pRequestDBConn;	//	数据库记录增、删、改、查链接，用于历史话务和黑名单记录查询、话务记录保存及更新
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
