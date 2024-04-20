#pragma once

namespace ICC
{
#define RESULT_SUCCESS			0
#define RESULT_FAILED			1

#define MAX_BUFFER				255
#define MAX_LINE_DEV_CAP_SIZE	512

#define MAX_CONFERENCE_MEMBERS	6		//��������Ա��
#define MIN_CONFERENCE_MEMBERS	2		//������С��Ա��
#define MAX_CALLQUEUE_COUNT		2048	//�ڴ��б�����������

	//cti ͨ������
#define CTC_TYPE_UNKNOW			0	//δ֪����
#define CTC_TYPE_PROCESS		100 //�����ֻ�
#define CTC_TYPE_DUTYPROCESS	200 //ģ���ֻ�,�ж������ֻ�
#define CTC_TYPE_DIGITAL		300 //���ַֻ�
#define CTC_TYPE_ANALOG			400 //ģ��ֻ�
#define CTC_TYPE_ACDGROUP		500 //����ACD
#define CTC_TYPE_IVR			600 //ivrͨ��
#define CTC_TYPE_ACDSUPER		700	//ACD����Ա
#define CTC_TYPE_RECORDPLAY		800 //¼��¼ʱ������ͨ��
#define CTC_TYPE_ANNOYPLAY		900	//ɧ����������ͨ��
#define CTC_TYPE_ROUTE			1000//·��
#define CTC_TYPE_IPPHONE		1100//IP�ֻ�
#define CTC_TYPE_VIRDN			1200//�������

#define DEVICE_TYPE_UNKNOW		"unknow"	// δ֪
#define DEVICE_TYPE_DIGITAL		"digital"	// ���ַֻ�
#define DEVICE_TYPE_IPPHONE		"ipphone"	// IP�ֻ�
#define DEVICE_TYPE_ANALOG		"analog"	// ģ��ֻ�
#define DEVICE_TYPE_VIRDN		"virdn"		// �������
#define DEVICE_TYPE_ACDGROUP	"acdgroup"	// ����ACD
#define DEVICE_TYPE_ACDSUPER	"acdsuper"	// ACD����Ա
#define DEVICE_TYPE_ROUTE		"route"		// ·��

#define DEVICE_TYPE_OUTPHONE		"outphone"	// �ⲿ����
	// ����
	typedef struct S_DEVICE_PHONE
	{
		std::string	m_strDeviceNum	= "";
		std::string	m_strDeviceType = "";
		std::string	m_strAgent = "";
		std::string m_strPsw = "";
	}CDevicePhone;

	// �����豸
	typedef struct S_DEVICE_VIR
	{
		std::string	m_strDeviceNum = "";
		std::string	m_strDeviceType = "";
	}CDeviceVir;

	// ACD ��
	typedef struct S_ACD_GROUP
	{
		std::string					m_strACDNum = "";		// ACD ����
		std::string					m_strACDAlias = "";		// ����
		std::vector<std::string>	m_strAgentList;			// Agent Object
	}CACDGroup;

	typedef struct S_AGENT
	{
		std::string					m_strACDNum = "";		// ACD ����
		std::string					m_strACDAlias = "";		// ACD ����
		std::string					m_strAgentId = "";		// Agent Object
		std::string					m_strPsw = "";			// Agent Psw
		std::string					m_strDeviceNum = "";	// Device
	}CAgentObj;

	// ���й���
	typedef struct S_DIAL_RULE
	{
		int	m_nSrcNumLen = 0;				// ���볤��
		std::string	m_strPreNum = "";		// ��ͷ
		int	m_nRemoveLen = 0;				// �Ƴ�����ͷ����
		std::string m_strAddPre = "";		// ��ӵ���ͷ
	}CDialRule;

	typedef enum eAgentModeType
	{			//	Login / Logout
		AGENT_MODE_LOGIN = 0,
		AGENT_MODE_LOGOUT = 1,
	}E_AGENT_MODE_TYPE;
	static std::string AgentModeTypeString[] =
	{
		("login"),
		("logout"),
	};

	typedef enum eAgentReadyType
	{			// Ready / NoReady
		AGENT_READY		= 0,
		AGENT_NOTREADY	= 1,
		AGENT_OTHERWORK	= 2,
	}E_AGENT_READY_TYPE;
	static std::string AgentReadyTypeString[] =
	{
		("ready"),
		("notready"),
		("otherwork"),
	};

	typedef enum eSingleStepConfMode
	{
		CONF_MODE_ACTIVE = 0,	//	ConferenceCall
		CONF_MODE_SILENT = 1	//	ListenCall
	}E_SINGLE_STEP_CONF_MODE;

	//	����������
	typedef enum eCallHandleType
	{
		CONNECTED_CALL		= 0,	// My current call handle according to TAPI (0 if idle)
		WAITING_CALL		= 1,	// I am ringing this call (0 if none)
		HELD_CALL			= 2,	// My held call handle according to TAPI (0 if nothing on hold)
		PENDING_CALL		= 3,	// This call is waiting to be answered according to TAPI (0 if none)
		CONFERENCE_CALL		= 4,	// This is the conference call according to TAPI (0 if none)
		CONSULTATION_CALL	= 5,	// This is the call created by lineSetupTransfer
		LAST_CALL_INTO_CONF	= 6,	// This is the last call added to a conference	
		FREE_CALL			= 7,	// Call is free	
		DISCONNECT_CALL		= 8,	// Call is disconn
		QUEUE_CALL			= 9,	// Call is queue
	}E_CALL_HANDLE_TYPE;
	static std::string CallHandleTypeString[] =
	{
		("ConnectCall"),
		("WaitingCall"),
		("HeldCall"),
		("PendingCall"),
		("ConferenceCall"),
		("ConsultationCall"),
		("LastCallIntoConf"),
		("FreeCall"),
		("DisconnectCall"),
		("QueueCall")
	};

	typedef enum eTaskName
	{
		Task_NULL = 0,
		Task_ReOpenStream,
		Task_QueryDeviceInfo,
		Task_QueryDeviceInfoRet,
		Task_QueryAgentState,
		Task_QueryAgentStateRet,
		Task_AgentLogin,
		Task_AgentLogout,
		Task_SetAgentState,	
		Task_SetAgentStateConf,
		Task_ACDStateRequest,
		Task_MonitorDevice,
		Task_MonitorDeviceConf,
		Task_StopMonitorDevice,
		Task_MonitorCallVirDevice,
		Task_AnswerCall,
		Task_AnswerCallConf,
		Task_HoldCall,
		Task_HoldCallConf,
		Task_ReconnectCall,
		Task_ReconnectCallConf,
		Task_RetrieveCall,
		Task_RetrieveCallConf,
		Task_Hangup,
		Task_ClearConnectionConf,
		Task_ClearCall,
		Task_ClearCallConf,
		Task_RefuseAnswer,			//	Ӧ�ò�ܽ�
		Task_RefuseAnswerConf,
		Task_RefuseBlackCall,		//	����������
		Task_MakeCall,
		Task_MakeCallConf,
		Task_ConsultationCall,
		Task_ConsultationCallConf,
		Task_TransferCall,
		Task_TransferCallConf,
		Task_DeflectCall,	
		Task_DeflectCallConf,
		Task_PickupCall,
		Task_PickupCallConf,
		Task_ListenCall,
		Task_SingleStepConferenceConf,
		Task_BargeInCall,
		Task_BargeInCallConf,
		Task_ForcePopCall,
		Task_ForcePopCallConf,
		Task_ConferenceCall,
		Task_ConferenceCallConf,
		Task_AddConferenceParty,
		Task_DeleteConferenceParty,
		Task_AddConferencePartyConf,
		Task_DeleteConferencePartyConf,
		Task_TakeOverCall,
		Task_TakeOverCallConf,
		Task_RouteRequest,
		Task_RouteRegisterReq,
		Task_RouteRegisterConf,
		Task_AlternateCall,
		Task_ConnectAesEvent,
		Task_AgentStateEvent,
		Task_DeviceStateEvent,
		Task_CallStateEvent,
		Task_CallOverEvent,
		Task_RefuseCallEvent,				//�ܽӻ����������
		Task_ConferenceHangupEvent,
		Task_FailedEvent,
		Task_CallTimeoutEvent,
		Task_UniversalFailRet,
		Task_GetCTIConnState,
		Task_GetCTIConnStateRet,
		Task_GetDeviceList,
		Task_GetDeviceListRet,
		Task_GetACDList,
		Task_GetACDListRet,
		Task_GetAgentList,
		Task_GetAgentListRet,
		Task_GetCallList,
		Task_GetCallListRet,
		Task_GetReadyAgent,
		Task_GetReadyAgentRet,
		Task_SetBlackList,
		Task_DeleteBlackList,
		Task_DeleteAllBlackList,
		Task_GetFreeAgentList,
		Task_GetFreeAgentListRet,
	}E_TASK_NAME;
	static std::string TaskNameString[] =
	{
		("Task_NULL"),
		("Task_ReOpenStream"),
		("Task_QueryDeviceInfo"),
		("Task_QueryDeviceInfoRet"),
		("Task_QueryAgentState"),
		("Task_QueryAgentStateRet"),
		("Task_AgentLogin"),
		("Task_AgentLogout"),
		("Task_SetAgentState"),	// Ŀǰ����agent״̬Ҳ����AgentLogin����
		("Task_SetAgentStateConf"),
		("Task_ACDStateRequest"),
		("Task_MonitorDevice"),
		("Task_MonitorDeviceConf"),
		("Task_StopMonitorDevice"),
		("Task_MonitorCallVirDevice"),
		("Task_AnswerCall"),
		("Task_AnswerCallConf"),
		("Task_HoldCall"),
		("Task_HoldCallConf"),
		("Task_ReconnectCall"),
		("Task_ReconnectCallConf"),
		("Task_RetrieveCall"),
		("Task_RetrieveCallConf"),
		("Task_Hangup"),
		("Task_ClearConnectionConf"),
		("Task_ClearCall"),
		("Task_ClearCallConf"),
		("Task_RefuseAnswer"),
		("Task_RefuseAnswerConf"),
		("Task_RefuseBlackCall"),
		("Task_MakeCall"),
		("Task_MakeCallConf"),
		("Task_ConsultationCall"),
		("Task_ConsultationCallConf"),
		("Task_TransferCall"),
		("Task_TransferCallConf"),
		("Task_DeflectCall"),	// Transfer Call
		("Task_DeflectCallConf"),
		("Task_PickupCall"),
		("Task_PickupCallConf"),
		("Task_ListenCall"),
		("Task_SingleStepConferenceConf"),
		("Task_BargeInCall"),
		("Task_BargeInCallConf"),
		("Task_ForcePopCall"),
		("Task_ForcePopCallConf"),
		("Task_ConferenceCall"),
		("Task_ConferenceCallConf"),
		("Task_AddConferenceParty"),
		("Task_DeleteConferenceParty"),
		("Task_AddConferencePartyConf"),
		("Task_DeleteConferencePartyConf"),
		("Task_TakeOverCall"),
		("Task_TakeOverCallConf"),
		("Task_RouteRequest"),
		("Task_RouteRegisterReq"),
		("Task_RouteRegisterConf"),
		("Task_AlternateCall"),
		("Task_ConnectAesEvent"),
		("Task_AgentStateEvent"),
		("Task_DeviceStateEvent"),
		("Task_CallStateEvent"),
		("Task_CallOverEvent"),
		("Task_RefuseCallEvent"),
		("Task_ConferenceHangupEvent"),
		("Task_FailedEvent"),
		("Task_CallTimeoutEvent"),
		("Task_UniversalFailRet"),
		("Task_GetCTIConnState"),
		("Task_GetCTIConnStateRet"),
		("Task_GetDeviceList"),
		("Task_GetDeviceListRet"),
		("Task_GetACDList"),
		("Task_GetACDListRet"),
		("Task_GetAgentList"),
		("Task_GetAgentListRet"),
		("Task_GetCallList"),
		("Task_GetCallListRet"),
		("Task_GetReadyAgent"),
		("Task_GetReadyAgentRet"),
		("Task_SetBlackList"),
		("Task_DeleteBlackList"),
		("Task_DeleteAllBlackList"),
		("Task_GetFreeAgentList"),
		("Task_GetFreeAgentListRet"),
	};
}// end namespace ICC
