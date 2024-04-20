#pragma once

/**
 @Copyright Copyright (C), 2018
 @file		HytProtocol.h
 @created	2018/01/23
 @brief		协议解析类
            1, 负责解析所有来自通信类的命令,将 JSON 协议解析内部命令
			2, 负责将内部事件，按 JSON 协议封装,交给通信类发送

 @author	psy
*/

namespace ICC
{

class CHytProtocol
{
public:
	CHytProtocol(void);
	virtual~CHytProtocol(void);

	static boost::shared_ptr<CHytProtocol> Instance();
	void ExitInstance();
	IResourceManagerPtr GetResourceManager();

	void SetFSAesMode(bool p_bFsAesMode) { m_bFSAesMode = p_bFsAesMode; };

private:
	static boost::shared_ptr<CHytProtocol> m_pInstance;

public:
	void SetObserverCenterPtr(ObserverPattern::IObserverCenterPtr p_observerCenterPtr){ m_pObserverCenter = p_observerCenterPtr; }
	void SetSwitchClientPtr(ISwitchClientPtr p_switchClientPtr){ m_SwitchClientPtr = p_switchClientPtr; }
	void SetStringUtilPtr(StringUtil::IStringUtilPtr p_stringUtilPtr){ m_StringUtilPtr = p_stringUtilPtr; }
	void SetDateTimePtr(DateTime::IDateTimePtr p_dateTimePtr){ m_DateTimePtr = p_dateTimePtr; }
	void SetJsonFactoryPtr(JsonParser::IJsonFactoryPtr p_jsonFactoryPtr){ m_JsonFactoryPtr = p_jsonFactoryPtr; }
	void SetLogPtr(Log::ILogPtr p_logPtr){ m_LogPtr = p_logPtr; }
	void SetRedisClientPtr(Redis::IRedisClientPtr p_pRedisClient) { m_pRedisClient = p_pRedisClient; }
	void SetDBConnPtr(DataBase::IDBConnPtr	m_pDBConn) { m_pDBConnPtr = m_pDBConn; }
	void SetHelpToolPtr(HelpTool::IHelpToolPtr HelpToolPtr) { m_pHelpTool = HelpToolPtr; }
	void SetConfiglPtr(Config::IConfigPtr ConfigPtr) { m_pConfig = ConfigPtr; }
public:
	PROTOCOL::CHeaderEx CreateProtocolHeader(const std::string& p_strCmd,const std::string& p_strRequest = "", 
		const std::string& p_strRequestType = SendType_Queue, const std::string& p_strRelatedId = "");

	//	向 CTI-DBProcess 请求加载所有的黑名单
	void LoadAllBlackList();

	// 具体命令的解析函数 CC--Call Control command
	void CC_ClientRegisterSync(const std::string& p_strNetMsg);

	void CC_AgentLogin(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_AgentLogout(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_SetAgentState(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);

	void CC_MakeCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_Hangup(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_AnswerCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_RefuseAnswer(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_ClearCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_ListenCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_PickupCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_ForcePopCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_BargeInCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest); 
	void CC_ConsultationCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_TransferCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_TransferCallEx(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_CancelTransferCallEx(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_DeflectCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_HoldCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_RetrieveCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_ReconnectCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_MakeConference(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_MakeConferenceEx(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_AddConferenceParty(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_GetConferenceParty(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_QueryConferenceByCallid(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_DisenableConferenceParty(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);

	void CC_DeleteConferenceParty(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_TakeOverCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);

	void CC_GetCTIConnState(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_GetDeviceList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_GetACDList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_GetAgentList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);

	void CC_GetFreeAgentList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);

	void CC_GetCallList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	/*void CC_QueryCallEvent(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_QueryBlackCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);*/
	void CC_GetReadyAgent(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);

	void CC_SetBlackList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_DeleteBlackList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_DeleteAllBlackList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CC_GetAllBlackList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);

	//void CC_RemoveReleaseCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);

	void CC_GetCurrentTime(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);

	void CC_AgentHeartBeatTimeout(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);

	//
	void CC_GetHistoryCallList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);

	void CR_GetHistoryCallList(IFSAesExEventNotifPtr p_pSwitchNotif);

	void CC_RecordFileUp(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest);
	//////////////////////////////////////////////////////////////////////////
	// 黑名单状态同步
	void Sync_SetBlackList(const PROTOCOL::CSetBlackListRequest l_oSetBlackListRequest, bool p_bIsUpdate);
	void Sync_DeleteBlackList(const std::string& p_strLimitType, const std::string& p_strLimitNum);
	void Sync_DeleteAllBlackList();
	// 发送消息给MRCC
	void SyncCallRefIdToMrcc(const std::string& p_strCallRefId, const std::string& p_strOldCallRefId);
	
	// 转移结果同步
	void Sync_TransferCall(const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId, const std::string& p_strSponsor,
		const std::string& p_strTarget, const std::string& p_strRelatedId, const int p_nResult);

	// 会议结果同步
	void Sync_MakeConference(const std::string& p_strConferenceId, const std::string& p_strCallRefId, const std::string& p_strCompere, 
		const std::string& p_strTarget, const std::string& p_strState, const std::string& p_strResult);

	void Sync_MakeConference(const std::string& p_strSyncBodyData);
	//	移除早释话务同步
	//void Sync_RemoveReleaseCall(std::vector<std::string>& p_vCallRefIdQueue);

	//	回拨话务同步
	void Sync_Callback(const std::string& p_strCTICallRefId, const std::string& p_strCallerId, const std::string& p_strCalledId, const std::string& p_strCaseId,
		const std::string& p_strRelateCallRefId, const std::string& p_strReceiptCode, const std::string& p_strReceiptName);

	void Sync_AgentState(IAgentStateNotifPtr p_pSwitchNotif);

	void Sync_ConferenceDetailCallOver(const std::string& p_strDeviceNum,
		const std::string& p_strTargetDeviceType,
		const std::string& p_strCompere,
		const std::string& p_strCallId,
		const std::string& p_strActiveCallRefId,
		const std::string& p_strRingTime,
		const std::string& p_strTalkTime,
		const std::string& p_strJoinTime,
		const std::string& p_strHangupTime);
	//////////////////////////////////////////////////////////////////////////
	// Switch 主动上报事件
	void Event_CTIConnState(ICTIConnStateNotifPtr p_pSwitchNotif);
	void Event_AgentState(IAgentStateNotifPtr p_pSwitchNotif);
	void Event_CallState(ICallStateNotifPtr p_pSwitchNotif);
	void Event_CallOver(ICallOverNotifPtr p_pSwitchNotif);
	void Event_DeviceState(IDeviceStateNotifPtr p_pSwitchNotif, const std::string& p_strConferenceId, const std::string& p_strConferenceCompere, const std::string& l_ConfBargeinSp);
	void Event_RefuseCall(IRefuseCallNotifPtr p_pSwitchNotif);
	void Event_Failed(IFailedEventNotifPtr p_pSwitchNotif);
	void Event_BlackTransfer(IBlackTransferEventNotifPtr p_pSwitchNotif);

	//////////////////////////////////////////////////////////////////////////
	// 具体命令执行结果反馈( CR-- Command Result)
	std::string ErrorCodeToString(int p_nErrorCode);

	void CR_AgentLogin(ISetAgentStateResultNotifPtr p_pSwitchNotif);
	void CR_AgentLogout(ISetAgentStateResultNotifPtr p_pSwitchNotif);
	void CR_SetAgentState(ISetAgentStateResultNotifPtr p_pSwitchNotif);
	void CR_MakeCall(IMakeCallResultNotifPtr p_pSwitchNotif);
	void CR_AnswerCall(ISwitchResultNotifPtr p_pSwitchNotif);
	void CR_RefuseAnswer(ISwitchResultNotifPtr p_pSwitchNotif);
	void CR_Hangup(ISwitchResultNotifPtr p_pSwitchNotif);
	void CR_ClearCall(ISwitchResultNotifPtr p_pSwitchNotif);
	void CR_ListenCall(ISwitchResultNotifPtr p_pSwitchNotif);
	void CR_PickupCall(ISwitchResultNotifPtr p_pSwitchNotif);
	void CR_BargeInCall(ISwitchResultNotifPtr p_pSwitchNotif);
	void CR_ForcePopCall(ISwitchResultNotifPtr p_pSwitchNotif);
	void CR_ConsultationCall(IConsultationCallResultNotifPtr p_pRespondNotif);
	void CR_TransferCall(ITransferCallResultNotifPtr p_pRespondNotif);
	void CR_TransferCallEx(long p_lRequestId, const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId,
		const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strResult);
	void CR_TransferCallEx(ObserverPattern::INotificationPtr p_pNotifiRequest, const std::string& p_strHeldCallRefId, 
		const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strMsgId, bool p_bResult);
	void CR_CancelTransferCallEx(ObserverPattern::INotificationPtr p_pNotifiRequest, const std::string& p_strHeldCallRefId,
		const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strMsgId);
	void CR_DeflectCall(ISwitchResultNotifPtr p_pSwitchNotif);
	void CR_HoldCall(ISwitchResultNotifPtr p_pSwitchNotif);
	void CR_RetrieveCall(ISwitchResultNotifPtr p_pSwitchNotif);
	void CR_ReconnectCall(ISwitchResultNotifPtr p_pSwitchNotif);
	void CR_ConferenceCall(ISwitchResultNotifPtr p_pSwitchNotif);
	void CR_MakeConference(const std::string& p_strConferenceId, ObserverPattern::INotificationPtr p_pNotifiRequest,bool p_bIsSuccess = true);
	void CR_AddConferenceParty(const std::string& p_strConferenceId, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CR_DeleteConferenceParty(const std::string& p_strConferenceId, ObserverPattern::INotificationPtr p_pNotifiRequest);
	void CR_TakeOverCall(ITakeOverCallResultNotifPtr p_pSwitchNotif);
	void CR_TakeOverCallEx(ObserverPattern::INotificationPtr p_pNotifiRequest,
		const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strMsgId, bool p_bResult);
	//void CR_RemoveReleaseCall(ObserverPattern::INotificationPtr p_pNotifiRequest);

	void CR_GetCTIConnState(IGetCTIConnStateResultNotifPtr p_pSwitchNotif);
	void CR_GetDeviceList(IGetDeviceListResultNotifPtr p_pSwitchNotif);
	void CR_GetACDList(IGetACDListResultNotifPtr p_pSwitchNotif);
	void CR_GetAgentList(IGetAgentListResultNotifPtr p_pSwitchNotif);
	void CR_GetFreeAgentList(IGetFreeAgentResultNotifPtr p_pSwitchNotif);
	void CR_GetCallList(IGetCallListResultNotifPtr p_pSwitchNotif);
	void CR_GetReadyAgent(IGetReadyAgentResultNotifPtr p_pSwitchNotif);


	void _SendSetAgentStateFailedSync(const PROTOCOL::CSetAgentStateRespond& p_oRespondObj, const std::string& p_strResult);

	bool AddSyntInfo(PROTOCOL::CMakeCallRequest &l_oRequestObj, const std::string& strTransGuid = "");
	std::string BuildUrgeId();
	std::string BuildAlarmLogContent(std::vector<std::string> p_vecParamList);
	bool AddAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_pAlarmLogInfo, const std::string& strTransGuid = "");
	bool InsertAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo, const std::string& strTransGuid="");
	//bool InsertAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo);
	void SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync);

	std::string _QueryAcdDept(const std::string& strAcd);

	void        _SyncAgentState(IDeviceStateNotifPtr p_pSwitchNotif);
	bool		_IsAutoUrgeCallNum(std::string strCaller);

	bool	_GetStaffInfo(const std::string& strStaffCode, Data::CStaffInfo& l_oStaffInfo);

	void _AddGeneralConsultationInfo(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId);
	std::string _GetGeneralConsultationInfo(const std::string& p_strActiveCallRefId);
	void DeleteGeneralConsultationInfo(const std::string& p_strActiveCallRefId);
private:
	ISwitchClientPtr					m_SwitchClientPtr;
	ObserverPattern::IObserverCenterPtr	m_pObserverCenter;
	StringUtil::IStringUtilPtr			m_StringUtilPtr;
	JsonParser::IJsonFactoryPtr			m_JsonFactoryPtr;
	DateTime::IDateTimePtr				m_DateTimePtr;
	Log::ILogPtr						m_LogPtr;

	Redis::IRedisClientPtr				m_pRedisClient;

	DataBase::IDBConnPtr		        m_pDBConnPtr;
	HelpTool::IHelpToolPtr				m_pHelpTool;
	IResourceManagerPtr					m_pResourceManager;
	Config::IConfigPtr					m_pConfig;

	std::string							m_strAutoUrgeCallNumber;

	std::map<std::string, std::string>	m_mapGeneralConsultCalls;

	bool m_bFSAesMode;
	std::string m_strCodeMode;			//警员号码模式：1、警员身份证号码 2、警员编号
};	//end class CHytProtocol

 }	// end namespace

