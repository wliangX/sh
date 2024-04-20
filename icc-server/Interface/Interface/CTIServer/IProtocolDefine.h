#pragma once

/**
 @file		protocolDefine.h
 @created	2018/01/31
 @brief		Э�����õ��������ֶζ���

 @author	psy
*/

namespace ICC
{
//	============== ������ API ���� ==============
#define SWITCH_TYPE_TSAPI					("0")		//TSAPI S8300/S8800
#define SWITCH_TYPE_TAPI					("1")		//TAPI2 IPO
#define SWITCH_TYPE_TAPI3					("2")		//TAPI3 IPO
#define SWITCH_TYPE_FREESWITCH				("3")		//FreeSwitch

#define PLUGIN_IS_USING						("1")		//����Ƿ����ã�0����1����


#define	Server_Name_CTIServer				("ICC-CTIServer")
#define	Server_Name_DBProcess				("ICC-CTI-DBProcess")
#define CTI_OUTCALL_HEAD					("CTIOutCallHead")
#define CTI_REDO_SQL						("CTIRedoSQL")
#define CALL_INFO							("CallInfo")
#define RED_LIST							("RedList")

#define Result_Success						("0")
#define Result_Failed						("1")

#define Result_True							("0")
#define Result_False						("1")

#define Result_GetReadyAgent_Success		1

#define ReleaseCall_IsCallBack				("true")

//#define DEFAULT_CALLREFID					0x98967F	//	9,999,999 ���������� ID Ĭ��ֵ����������ʵ���� ID �� 0 ��ʼ��
#define DEFAULT_CALLREFID					0x10000006	//	268435462 TAPI Ĭ�ϻ���ID
#define DEFAULT_TASKID						0			//	Ĭ������ ID
#define DEFAULT_RING_TIMEOUT				20			//	Ĭ�����峬ʱʱ������λ���룩������ת���ͻ���


//	============== ���š��ֻ�����ǰ׺ ==============
#define MIN_LOCAL_PHONE_LEN					7
#define MAX_LOCAL_PHONE_LEN					8
#define MIN_ZONE_LEN						3
#define MAX_ZONE_LEN						4
#define MOBILE_PRE_LEN						7


//	============== ��������״̬ ==============
#define ConnectState_Connect				("0")
#define ConnectState_Disconnect				("1")


//	============== ��������˫����ɫ ==============
#define DoubleServerRole_Active				("master")
#define DoubleServerRole_Stanby				("stanby")


//	============== �豸���� ==============
#define TARGET_DEVICE_TYPE_INSIDE			("inside")		//	�ڲ�����
#define TARGET_DEVICE_TYPE_OUTSIDE			("outside")		//	�ⲿ�������ֻ�
#define TARGET_DEVICE_TYPE_WIRELESS			("wireless")	//	350 M ������̨
#define TARGET_DEVICE_TYPE_GROUPCALL		("groupcall")	//	


//	============== ��ϯ��¼���ǳ���æ��״̬ ==============
#define LoginMode_Login						("login")
#define LoginMode_Logout					("logout")
#define ReadyState_Ready					("ready")
#define ReadyState_NotReady					("notready")
#define ReadyState_Busy						("busy")
#define ReadyState_Idle						("idle")


//	============== ����״̬�����ֶ�ֵ ==============
#define Pr_CTICallRefId						("CTICallRefId")
#define Pr_RelatedCTICallRefId				("RelatedCTICallRefId")
#define Pr_RelatedCSTACallRefId				("RelatedCSTACallRefId")
#define Pr_AgentId							("AgentId")
#define Pr_ACDGrp							("ACDGrp")
#define Pr_CallerId							("CallerId")
#define Pr_CalledId							("CalledId")
#define Pr_CallDirection					("CallDirection")
#define Pr_CallState						("CallState")
#define Pr_StateTime						("StateTime")
#define Pr_IsBlackCall						("IsBlackCall")
#define Pr_IsAlarmCall						("IsAlarmCall")
#define Pr_HangupDevice						("HangupDevice")
#define Pr_HangupType						("HangupType")
#define Pr_OriginalCallerId					("OriginalCallerId")
#define Pr_OriginalCalledId					("OriginalCalledId")
#define Pr_AddCallMember					("AddCallMember")
#define Pr_IsSubCall						("IsSubCall")

//	============== �ز��绰�ֵ�ֵ ==============
#define Dic_ResouceType_Phone				("7")
#define Dic_CallBackLog						("BS001009003")	//�ز���ˮ
#define Dic_TakeOverCallLog					("BS001009004")	//�ӹ���ˮ


//	============== ������ʱ���ֵ� ==============
#define Dic_OneMinute						("DIC029001")	// 1 ����
#define Dic_FiveMinutes						("DIC029002")	// 5 ����
#define Dic_ThrityMinutes					("DIC029003")	// 30 ����
#define Dic_OneHour							("DIC029004")	// 1 Сʱ
#define Dic_TwoHours						("DIC029005")	// 2 Сʱ
#define Dic_SixHours						("DIC029006")	// 6 Сʱ
#define Dic_TwelveHours						("DIC029007")	// 12 Сʱ
#define Dic_OneDay							("DIC029008")	// 1 ��
#define Dic_SevenDays						("DIC029009")	// 7 ��
#define Dic_FifteenDays						("DIC029010")	// 15 ��
#define Dic_OneMonth						("DIC029011")	// 1 ����
#define Dic_SixMonthes						("DIC029012")	// 6 ����
#define Dic_OneYear							("DIC029013")	// 1 ��

#define Time_One							1
#define Time_Two							2
#define Time_Five							5
#define Time_Six							6
#define Time_Seven							7
#define Time_Twelve							12
#define Time_Fifteen						15
#define Time_Thrity							30

//	============== �������Ƿ�·��ָ�������� ==============
#define Disable_Black_Route					"0"
#define Enable_Black_Route                  "1"

//  ============== ת��ԭ�� ==============
#define Black_Transfer						"1"

//	============== ����ӹ�(TakeOverCall)���� ==============
#define Step_StopListenCall					("StopListenCall")
#define Step_BargeInCall					("BargeInCall")
#define Step_Hangup							("Hangup")


//	============== ActiveMQ Э���ֶ� ==============
#define SendType_Topic						("1")	// ����
#define SendType_Queue						("0")	// ����

#define SyncType_Add						("1")	//	����
#define SyncType_Modify						("2")	//	�޸�
#define SyncType_Delete						("3")	//	ɾ��

#define Topic_CTIConnectStateSync			("topic_cti_connect_state_sync")
#define Topic_ACDCallStateSync				("topic_acd_call_state_sync")
#define Topic_CallOverSync					("topic_call_over_sync")
#define Topic_ACDAgentStateSync				("topic_acd_agent_state_sync")
#define Topic_DeviceStateSync				("topic_device_state_sync")
#define Topic_RefuseCallSync				("topic_refuse_call_sync")
#define Topic_BlackListSync					("topic_blacklist_sync")
#define Topic_TransferCallExSync			("topic_transfer_call_ex_sync")
#define Topic_ConferenceSync				("topic_conference_sync")
#define Topic_RemoveReleaseCallSync			("topic_remove_release_call_sync")
#define Topic_Alarm							("topic_alarm")
#define Topic_TakeOverCallSync				("topic_takeover_call_sync")
#define Topic_CTIService					("topic_cti_service")
#define Topic_Alarm_Sync					"topic_alarm_sync"
#define Topic_RedList						("topic_redlist")
#define Topic_HistoryCallSync				("topic_history_call_sync")

#define Queue_CTIControl					("queue_cti_control")
#define Queue_CTIDBProcess					("queue_cti_db")
#define Queue_CTI_CallRefId_Mrcc			("queue_cti_mrcc")
#define Queue_ACDAgentStateSync				("queue_acd_agent_state_sync")
#define Queue_ACDCallStateSync				("queue_acd_call_state_sync")
#define Queue_CallOverSync					("queue_call_over_sync")
#define Queue_DeviceStateSync				("queue_device_state_sync")

#define Cmd_LoadAllBlackListSync			("load_all_blacklist_sync")
#define Cmd_ClientRegisterSync				("client_register_sync")
#define Cmd_CTIConnectStateSync				("cti_connect_state_sync")
#define Cmd_ACDCallStateSync				("acd_call_state_sync")
#define Cmd_CallRefIdSyncMrcc				("callRefId_sync_Mrcc")
#define Cmd_CallOverSync					("call_over_sync")
#define Cmd_DetailCallOverSync				("detail_call_over_sync")
#define Cmd_ACDAgentStateSync				("acd_agent_state_sync")
#define Cmd_DeviceStateSync					("device_state_sync")
#define Cmd_CallbackSync					("callback_sync")
#define Cmd_RemoveReleaseCallSync			("remove_release_call_sync")
#define Cmd_AlarmLogSync					("alarm_log_sync")
#define Cmd_DoubleServerSync				("sync_server_lock")
#define Cmd_HistoryCallSync					("history_call_sync")

#define Cmd_BlackListRefuseCallSync			("blacklist_refuse_call_sync")
#define Cmd_SetBlackListSync				("set_blacklist_sync")
#define Cmd_DeleteBlackListSync				("delete_blacklist_sync")
#define Cmd_DeleteAllBlackListSync			("delete_all_blacklist_sync")

#define Cmd_LoadAllRedListSync				("load_all_redlist_sync")
#define Cmd_SetRedListSync					("topic_redlist_sync")
#define Cmd_DeleteRedListSync				("delete_redlist_sync")
#define Cmd_DeleteAllRedListSync			("delete_all_redlist_sync")

#define Cmd_TransferCallExSync				("transfer_call_ex_sync")
#define Cmd_ConferenceSync					("conference_sync")

//////////////////////////////////////////////////////////////////////////
#define Cmd_AgentLoginRequest				("agent_login_request")
#define Cmd_AgentLoginRespond				("agent_login_respond")

#define Cmd_AgentLogoutRequest				("agent_logout_request")
#define Cmd_AgentLogoutRespond				("agent_logout_respond")

#define Cmd_SetAgentStateRequest			("set_agent_state_request")
#define Cmd_SetAgentStateRespond			("set_agent_state_respond")

#define Cmd_GetReadyAgentRequest			("get_ready_agent_request")
#define Cmd_GetReadyAgentRespond			("get_ready_agent_respond")

//////////////////////////////////////////////////////////////////////////
#define Cmd_MakeCallRequest					("make_call_request")
#define Cmd_MakeCallRespond					("make_call_respond")

#define Cmd_AnswerCallRequest				("answer_call_request")
#define Cmd_AnswerCallRespond				("answer_call_respond")

#define Cmd_RefuseAnswerRequest				("refuse_answer_request")
#define Cmd_RefuseAnswerRespond				("refuse_answer_respond")

#define Cmd_HangupRequest					("hangup_request")
#define Cmd_HangupRespond					("hangup_respond")

#define Cmd_ClearCallRequest				("clear_call_request")
#define Cmd_ClearCallRespond				("clear_call_respond")

#define Cmd_ListenCallRequest				("listen_call_request")
#define Cmd_ListenCallRespond				("listen_call_respond")

#define Cmd_PickupCallRequest				("pickup_call_request")
#define Cmd_PickupCallRespond				("pickup_call_respond")

#define Cmd_BargeInCallRequest				("bargein_call_request")
#define Cmd_BargeInCallRespond				("bargein_call_respond")

#define Cmd_ConsultationCallRequest			("consultation_call_request")
#define Cmd_ConsultationCallExRequest		("consultation_call_ex_request")
#define Cmd_ConsultationCallRespond			("consultation_call_respond")

#define Cmd_TransferCallRequest				("transfer_call_request")
#define Cmd_TransferCallRespond				("transfer_call_respond")

#define Cmd_TransferCallExRequest			("transfer_call_ex_request")
#define Cmd_TransferCallExRespond			("transfer_call_ex_respond")

#define Cmd_CancelTransferCallExRequest		("cancel_transfer_call_ex_request")
#define Cmd_CancelTransferCallExRespond		("cancel_transfer_call_ex_respond")

#define Cmd_ForcePopCallRequest				("forcepop_call_request")
#define Cmd_ForcePopCallRespond				("forcepop_call_respond")

#define Cmd_DeflectCallRequest				("deflect_call_request")
#define Cmd_DeflectCallRespond				("deflect_call_respond")

#define Cmd_HoldCallRequest					("hold_call_request")
#define Cmd_HoldCallRespond					("hold_call_respond")

#define Cmd_RetrieveCallRequest				("retrieve_call_request")
#define Cmd_RetrieveCallRespond				("retrieve_call_respond")

#define Cmd_ReconnectCallRequest			("reconnect_call_request")
#define Cmd_ReconnectCallRespond			("reconnect_call_respond")

#define Cmd_ConferenceCallRequest			("conference_call_request")
#define Cmd_ConferenceCallRespond			("conference_call_respond")

#define Cmd_MakeConferenceRequest			("make_conference_request")
#define Cmd_MakeConferenceRespond			("make_conference_respond")

#define Cmd_AddConferencePartyRequest		("add_conference_party_request")
#define Cmd_AddConferencePartyRespond		("add_conference_party_respond")

#define Cmd_GetConferencePartyRequest		("get_conference_party_request")
#define Cmd_GetConferencePartyRespond		("get_conference_party_respond")

#define Cmd_DeleteConferencePartyRequest	("delete_conference_party_request")
#define Cmd_DeleteConferencePartyRespond	("delete_conference_party_respond")

#define Cmd_TakeOverCallRequest				("take_over_call_request")
#define Cmd_TakeOverCallRespond				("take_over_call_respond")
#define Cmd_TakeOverCallSync				("take_over_call_sync")

//////////////////////////////////////////////////////////////////////////
#define Cmd_GetCTIConnStateRequest			("get_cti_connect_state_request")
#define Cmd_GetCTIConnStateRespond			("get_cti_connect_state_respond")

#define Cmd_GetDeviceListRequest			("get_device_list_request")
#define Cmd_GetDeviceListRespond			("get_device_list_respond")

#define Cmd_GetACDListRequest				("get_acd_list_request")
#define Cmd_GetACDListRespond				("get_acd_list_respond")

#define Cmd_GetAgentListRequest				("get_agent_list_request")
#define Cmd_GetAgentListRespond				("get_agent_list_respond")

#define Cmd_GetCallListRequest				("get_call_list_request")
#define Cmd_GetCallListRespond				("get_call_list_respond")

#define Cmd_CallEventQueryRequest			("callevent_query_request")
#define Cmd_CallEventQueryRespond			("callevent_query_respond")

#define Cmd_BlackCallQueryRequest			("black_call_query_request")
#define Cmd_BlackCallQueryRespond			("black_call_query_respond")

#define Cmd_DetailCallQueryRequest			("detail_call_query_request")
//////////////////////////////////////////////////////////////////////////
//������
#define Cmd_SetBlackListRequest				("set_blacklist_request")
#define Cmd_SetBlackListRespond				("set_blacklist_respond")

#define Cmd_DeleteBlackListRequest			("delete_blacklist_request")
#define Cmd_DeleteBlackListRespond			("delete_blacklist_respond")

#define Cmd_DeleteAllBlackListRequest		("delete_all_blacklist_request")
#define Cmd_DeleteAllBlackListRespond		("delete_all_blacklist_respond")

#define Cmd_GetAllBlackListRequest			("get_all_blacklist_request")
#define Cmd_GetAllBlackListRespond			("get_all_blacklist_respond")

#define Cmd_LoadAllBlackListRequest			("load_all_blacklist_request")
#define Cmd_LoadAllBlackListRespond			("load_all_blacklist_respond")

#define Cmd_GetCurrentTimeRequest			("get_current_time_request")
#define Cmd_GetCurrentTimeRespond			("get_current_time_respond")

//////////////////////////////////////////////////////////////////////////
//������
#define Cmd_SetRedListRequest				("set_redlist_request")
#define Cmd_SetRedListRespond				("set_redlist_respond")

#define Cmd_DeleteRedListRequest			("delete_redlist_request")
#define Cmd_DeleteRedListRespond			("delete_redlist_respond")

#define Cmd_DeleteAllRedListRequest			("delete_all_redlist_request")
#define Cmd_DeleteAllRedListRespond			("delete_all_redlist_respond")

#define Cmd_GetAllRedListRequest			("get_redlist_request")
#define Cmd_GetAllRedListRespond			("get_redlist_respond")

#define Cmd_LoadAllRedListRequest			("load_all_redlist_request")
#define Cmd_LoadAllRedListRespond			("load_all_redlist_respond")
//////////////////////////////////////////////////////////////////////////
//Υ����Ϣ
#define Cmd_GetViolationRequest				("get_violation_request")
#define Cmd_GetViolationRespond				("load_violation_respond")

//////////////////////////////////////////////////////////////////////////
#define Cmd_RemoveReleaseCallRequest		("remove_release_call_request")
#define Cmd_RemoveReleaseCallRespond		("remove_release_call_respond")

#define Cmd_InitOverTimer					("init_over_timer")
#define Cmd_BlackListManagerTimer			("blacklist_manager_timer")
#define Cmd_CallManagerTimer				("call_manager_timer")
#define Cmd_ConferenceManagerTimer			("conference_manager_timer")
#define Cmd_TransferCallManagerTimer		("transfer_call_manager_timer")
#define Cmd_RedoSQLTimer					("redo_sql_timer")

	//	============== ���з��� ==============
	typedef enum E_CALL_DIRECTION
	{					
		CALL_DIRECTION_IN = 0,		// PSTN ���� -> ��ϯ
		CALL_DIRECTION_OUT = 1,		//	���� -> PSTN,���ڲ���ϯ
		CALL_DIRECTION_UNKNOWN = 2,
	}ECallDirection;
	static std::string CallDirectionString[] =
	{
		("in"),
		("out"),
		("unknown")
	};
	
	//	============== ����ģʽ������������ ==============
	typedef enum E_CALL_MODE
	{		// call mode
		CALL_MODE_SINGLE = 0,		//��������
		CALL_MODE_CONFERENCE,		//����
		CALL_MODE_UNKNOWN,			//δ֪
	}ECallMode;
	static std::string CallModeString[] =
	{
		("singlecall"),
		("conference"),
		("unknown")
	};
	
	// ============== �����Ա ==============


	
	//	============== �һ����� ==============
	typedef enum E_HANGUP_TYPE{					//hangup type
		HANGUP_TYPE_TIMEOUT = 0,	//���峬ʱ�һ�
		HANGUP_TYPE_CALLER,			//���йһ�
		HANGUP_TYPE_CALLED,			//���йһ�
		HANGUP_TYPE_TRANSFER,		//����ת��
		HANGUP_TYPE_TP_TRANSFER,	//ת�Ʒ��𷽹һ�
		HANGUP_TYPE_OP_TRANSFER,	//ת��(����ƫת)
		HANGUP_TYPE_RELEASE,		//����
		HANGUP_TYPE_BLACKCALL,		//������
		HANGUP_TYPE_NORMAL,			//�豸�һ�
		HANGUP_TYPE_UNKNOWN
	}EHangupType;
	static std::string HangupTypeString[] =
	{
		("timeout"),
		("caller"),
		("called"),
		("transfer"),
		("tp_transfer"),
		("op_transfer"),
		("release"),
		("blackcall"),
		("normal"),
		("unknown"),
	};

//	============== ����״̬ ==============
	typedef enum E_CALL_STATE
	{
		STATE_FREE = 0,			//����״̬
		STATE_INCOMING,			//�м̺���
		STATE_QUEUE,			//�Ŷ�״̬
		STATE_ASSIGN,			//����״̬
		STATE_RING,				//����״̬
		STATE_RINGOPHOLD,		//����Է�����״̬
		STATE_SIGNALBACK,		//�Ż�����̬
		STATE_SIGNALBACKHOLD,	//�Ż��屣����̬
		STATE_SIGNALBACKOPHOLD,	//�Ż��屻������̬
		STATE_TALK,				//ͨ��״̬
		STATE_DIAL,				//����״̬		
		STATE_DIALERROR,		//�����	
		STATE_HOLD,				//����״̬
		STATE_CANCELHOLD,		//ȡ������(Hold -> Talk��WH Ҫ�����ӵ�״̬....)
		STATE_SILENTMONITOR,	//����̬
		STATE_CONSULT,			//��ѯ״̬
		STATE_OPHOLD,			//���Է�����̬
		STATE_HOLDONHOLD,		//�໥����
		STATE_CONFERENCE,		//����״̬
		STATE_PRESIDERCONF,		//���ֻ���̬
		STATE_HOLDCONF,			//��������̬
		STATE_OPHOLDCONF,		//�Է���������̬
		STATE_RELEASE,			//һ�����н���(����)
		STATE_CALLOVER,			//һ�����н���(��æ��)
		STATE_WAITHANGUP,		//�ȴ��һ�̬(��æ��)
		STATE_WAITHANGUPTIMEOUT,	//�ȴ��һ���ʱ
		STATE_HANGUP,				//�һ�̬(��æ��)
		STATE_ACDPLAYWELCOME,		//ACD���Ż�ӭ��̬
		STATE_ACDPLAYQUEUE,			//ACD�����Ŷ���̬
		STATE_UNKNOWN
	}ECallState;		//���ڷ�������CallInfo��Ϣ�ĺ���״̬
	static std::string CallStateString[] =
	{
		("freestate"),
		("incoming"),
		("waiting"),
		("assign"),
		("ringstate"),
		("ringopholdstate"),
		("ringbackstate"),
		("ringbackholdstate"),
		("ringbackopholdstate"),
		("talkstate"),
		("dialstate"),
		("dialerrorstate"),
		("holdstate"),
		("cancelholdstate"),
		("silentmonitorstate"),
		("consultstate"),
		("opholdstate"),
		("holdonholdstate"),
		("conferencestate"),
		("presiderconfstate"),
		("holdconfstate"),
		("opholdconfstate"),
		("release"),
		("callover"),
		("waithangupstate"),
		("waithanguptimeoutstate"),
		("hangupstate"),
		("acdplaywelcome"),
		("acdplayqueue"),
		("unknownstate")
	};
	
	
//	============== ������� ==============
	typedef enum E_ERROR_CODE
	{
		ERROR_CMD_SUCCESS = 0,					//	�ɹ�
		ERROR_CMD_FAILED = 1,					//	����ִ��ʧ��
		ERROR_CMD_TIMEOUT = 2,					//	����ִ�г�ʱ
		ERROR_NET_DISCONNECT = 4,				//	CTI �뽻�����������ӶϿ�
		ERROR_INVALID_DEVICE = 5,				//	�豸��Ч
		ERROR_INVALID_CALL = 6,					//	������Ч
		ERROR_MEMBER_LIMIT = 7,					//	�����Ա��������
		ERROR_DEVICE_BUSY = 8,					//	�豸æ
		ERROR_DEST_NOT_CONNECT = 10,			//	�����޷���ͨ
		ERROR_MIN_ONLINE_AGENT = 11,			//	��С��ϯ������
		ERROR_NO_READY_AGENT = 12,				//	�޿�����ϯ
		ERROR_RING_TIMEOUT = 13,				//	���峬ʱ
		ERROR_CALLER_HANGUP = 14,				//	�����˹һ�
		ERROR_CANCEL_TRANSFER_CALL = 15,		//	ȡ��ת��
		ERROR_CALL_HANGUP = 16,					//	ת�Ʒ��𷽻���Ŀ�귽�һ�
		ERROR_OPERATION_TRANSFER = 17,			//	ִ��ת�Ʋ���ʧ��
		ERROR_GENERIC_UNSPECIFIED = 100,
		ERROR_GENERIC_OPERATION = 101,
		ERROR_REQUEST_INCOMPATIBLE_WITH_OBJECT = 102,
		ERROR_VALUE_OUT_OF_RANGE = 103,
		ERROR_OBJECT_NOT_KNOWN = 104,
		ERROR_INVALID_CALLING_DEVICE = 105,
		ERROR_INVALID_CALLED_DEVICE = 106,
		ERROR_INVALID_FORWARDING_DESTINATION = 107,
		ERROR_PRIVILEGE_VIOLATION_ON_SPECIFIED_DEVICE = 108,
		ERROR_PRIVILEGE_VIOLATION_ON_CALLED_DEVICE = 109,
		ERROR_PRIVILEGE_VIOLATION_ON_CALLING_DEVICE = 110,
		ERROR_INVALID_CSTA_CALL_IDENTIFIER = 111,
		ERROR_INVALID_CSTA_DEVICE_IDENTIFIER = 112,
		ERROR_INVALID_CSTA_CONNECTION_IDENTIFIER = 113,
		ERROR_INVALID_DESTINATION = 114,
		ERROR_INVALID_FEATURE = 115,
		ERROR_INVALID_ALLOCATION_STATE = 116,
		ERROR_INVALID_CROSS_REF_ID = 117,
		ERROR_INVALID_OBJECT_TYPE = 118,
		ERROR_SECURITY_VIOLATION = 119,
		ERROR_GENERIC_STATE_INCOMPATIBILITY = 121,
		ERROR_INVALID_OBJECT_STATE = 122,
		ERROR_INVALID_CONNECTION_ID_FOR_ACTIVE_CALL = 123,
		ERROR_NO_ACTIVE_CALL = 124,
		ERROR_NO_HELD_CALL = 125,
		ERROR_NO_CALL_TO_CLEAR = 126,
		ERROR_NO_CONNECTION_TO_CLEAR = 127,
		ERROR_NO_CALL_TO_ANSWER = 128,
		ERROR_NO_CALL_TO_COMPLETE = 129,
		ERROR_RESOURCES_NOT_AVAILABLE = 130,
		ERROR_GENERIC_SYSTEM_RESOURCE_AVAILABILITY = 131,
		ERROR_SERVICE_BUSY = 132,
		ERROR_RESOURCE_BUSY = 133,
		ERROR_RESOURCE_OUT_OF_SERVICE = 134,
		ERROR_NET_WORK_BUSY = 135,
		ERROR_NETWORK_OUT_OF_SERVICE = 136,
		ERROR_OVERALL_MONITOR_LIMIT_EXCEEDED = 137,
		ERROR_CONFERENCE_MEMBER_LIMIT_EXCEEDED = 138,
		ERROR_GENERIC_SUBSCRIBED_RESOURCE_AVAILABILITY = 141,
		ERROR_OBJECT_MONITOR_LIMIT_EXCEEDED = 142,
		ERROR_EXTERNAL_TRUNK_LIMIT_EXCEEDED = 143,
		ERROR_OUTSTANDING_REQUEST_LIMIT_EXCEEDED = 144,
		ERROR_GENERIC_PERFORMANCE_MANAGEMENT = 151,
		ERROR_PERFORMANCE_LIMIT_EXCEEDED = 152,
		ERROR_UNSPECIFIED_SECURITY_ERROR = 160,
		ERROR_SEQUENCE_NUMBER_VIOLATED = 161,
		ERROR_TIME_STAMP_VIOLATED = 162,
		ERROR_PAC_VIOLATED = 163,
		ERROR_SEAL_VIOLATED = 164,
		ERROR_GENERIC_UNSPECIFIED_REJECTION = 170,
		ERROR_GENERIC_OPERATION_REJECTION = 171,
		ERROR_DUPLICATE_INVOCATION_REJECTION = 172,
		ERROR_UNRECOGNIZED_OPERATION_REJECTION = 173,
		ERROR_MISTYPED_ARGUMENT_REJECTION = 174,
		ERROR_RESOURCE_LIMITATION_REJECTION = 175,
		ERROR_ACS_HANDLE_TERMINATION_REJECTION = 176,
		ERROR_SERVICE_TERMINATION_REJECTION = 177,
		ERROR_REQUEST_TIMEOUT_REJECTION = 178,
		ERROR_REQUESON_DEVICE_EXCEEDED_REJECTION = 179,
		ERROR_UNRECOGNIZED_APDU_REJECTION = 180,
		ERROR_MISTYPED_APDU_REJECTION = 181,
		ERROR_BADLY_STRUCTURED_APDU_REJECTION = 182,
		ERROR_INITIATOR_RELEASING_REJECTION = 183,
		ERROR_UNRECOGNIZED_LINKEDID_REJECTION = 184,
		ERROR_LINKED_RESPONSE_UNEXPECTED_REJECTION = 185,
		ERROR_UNEXPECTED_CHILD_OPERATION_REJECTION = 186,
		ERROR_MISTYPED_RESULT_REJECTION = 187,
		ERROR_UNRECOGNIZED_ERROR_REJECTION = 188,
		ERROR_UNEXPECTED_ERROR_REJECTION = 189,
		ERROR_MISTYPED_PARAMETER_REJECTION = 190,
		ERROR_NON_STANDARD = 200,
		ERROR_UNKNOWN = 255,
	}EErrorCode;

}	// end ICC
