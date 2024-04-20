////////////////////////////////////////////////////////////////////
//��������� ��CTI�ӿ��ĵ��б���һ��
//
#define CTI_PARAM_msgid			"msgid"
#define CTI_PARAM_agentid		"agent_id"
#define CTI_PARAM_extension		"extension"  //�ֻ���
#define CTI_PARAM_agentstatus	"status"	//��ϯ״̬
#define CTI_PARAM_clientid		"clientid"
#define CTI_PARAM_pswd			"pwd"
#define CTI_PARAM_skill			"skill"
#define CTI_PARAM_user_name		"user_name"
#define CTI_PARAM_user_pwd		"user_pwd"
#define CTI_PARAM_callback_url	"callback_url"
#define CTI_PARAM_agent			"agent"


#define CTI_PARAM_callid		"call_id" //
#define CTI_PARAM_direction		"call_direction"
#define CTI_PARAM_caller		"caller"
#define CTI_PARAM_called		"called"
#define CTI_PARAM_dest			"dest"
#define CTI_PARAM_listener		"listener"	//����������
#define CTI_PARAM_requester		"requester"	//ǿ�塢��������
#define CTI_PARAM_moderator		"moderator"   //����������
#define CTI_PARAM_IS_RELEASE	"is_release"

#define CTI_PARAM_extend_info		"extend_info" //

#define CTI_PARAM_conferencename	"conference_name"   //������
#define CTI_PARAM_member			"member"
#define CTI_PARAM_cti_conferenceid	"cti_conference_id"   //cti�ڲ�����id

#define CTI_PARAM_member_type		"member_type"
//
#define CTI_PARAM_caseid		"caseid" //����ID
#define CTI_PARAM_sponsor		"sponsor"  //
#define CTI_PARAM_channelnumber	"channel_number" 
#define CTI_PARAM_number		"number" 
#define CTI_PARAM_DEPT_CODE		"dept_code"

#define CTI_PRAM_src_number		"src_number"
#define CTI_PRAM_dest_number	"dest_number"
#define CTI_PRAM_dest_type		"dest_number_type"

#define CTI_PARAM_calloutPre	"call_out_pre"

#define CTI_PARAM_page_size		"page_size"
#define CTI_PARAM_page_index	"page_index"
#define CTI_PARAM_recordfile	"record_file"

#define CTI_PARAM_isListen		"listen_flag"
////////////////////////////////////////////////////////////////////
//�¼��� ��CTI�ӿ��ĵ��б���һ��
#define CTIEVT_AGENT_STATUS		"event_agent_status_report"
#define CTIEVT_CALL_INCOMMING	"event_call_incomming"  //���н���
#define CTIEVT_CALL_WAITING		"event_call_waiting"	//���еȴ�
#define CTIEVT_CALL_ASSIGNED	"event_call_assigned"	//���з���

#define CTIEVT_CALL_RING		"event_call_ring"		//����
#define CTIEVT_CALL_CONNECTED	"event_call_connected"
#define CTIEVT_CALL_HANGUP		"event_call_hangup"
#define CTIEVT_CALL_RINGBACK	"event_call_ringback"


#define CTIEVT_CALL_HOLD		"event_call_hold"
#define CTIEVT_CALL_UNHOLD		"event_call_unhold"

#define CTIEVT_CALL_MONITOR		"event_call_monitor"

#define CTIEVT_DEVICE_DIAL		"event_device_dial"
#define CTIEVT_DEVICE_FREE		"event_device_free"	

#define CTIEVT_CONFERENCE_CREATE			"event_conference_create"
#define CTIEVT_CONFERENCE_RELEASE			"event_conference_release"
#define CTIEVT_CONFERENCE_MEMBER_ADD		"event_conference_member_add"
#define CTIEVT_CONFERENCE_MEMBER_DEL		"event_conference_member_del"
////////////////////////////////////////////////////////////////////
//��CTI�ӿ��ĵ��б���һ��
//CTI�ӿڶ�����ϯ��¼״̬
#define CTI_AGENT_STATE_LOGOUT	"0"  //��ϯû�е�¼
#define CTI_AGENT_STATE_LOGIN	"1"  //��ϯ��¼

//CTI�ӿڶ������ϯæ��״̬
#define CTI_AGENT_STATUS_IDLE	"1"  //��ϯ����
#define CTI_AGENT_STATUS_BUSY	"2"  //��ϯæ

//��AES����
#define AES_CALL_DIRECTION_INSIDE	"3"  //�ڲ�����
#define AES_CALL_DIRECTION_OUTSIDE	"2"	 //�ⲿ����
//��AES����
#define AES_MEMBER_TYPE_inline		"1"
#define AES_MEMBER_TYPE_outline		"4"
///////////////////////////////////////////////////////////////////
//HTTP�����붨��
#define	HTTP_SUCCESS_CODE	"200" 
#define	HTTP_BadRequest		"400"		//400  ����������Bad Request
#define HTTP_Unauthorized	"401"		//��ǰ������Ҫ�û���֤
#define HTTP_Forbidden		"403"		//Forbidden��������������󣬵��ܾ�ִ��
///////////////////////////////////////////////////////////////////
//����
typedef enum E_FSAES_REQUEST {
	REQUEST_AES_LOGIN_CMD = 0,//AES�û���¼
	REQUEST_AES_LOGOUT_CMD,//AES�û��ǳ�
	REQUEST_AES_HEART_BEAT_CMD,//

	REQUEST_MONITOR_AGENT_CMD,
	REQUEST_DISMONITOR_AGENT_CMD,
	REQUEST_MONITOR_DEVICE_CMD,
	REQUEST_DISMONITOR_DEVICE_CMD,
	REQUEST_MONITOR_ACD_CMD,
	REQUEST_DISMONITOR_ACD_CMD,

	REQUEST_AGENT_LOGIN_CMD,		//AgentLogin
	REQUEST_AGENT_LOGOUT_CMD,		//AgentLogout
	REQUEST_SET_AGENTIDLE_CMD,		//���� Agent ����
	REQUEST_SET_AGENTBUSY_CMD,
	REQUEST_MAKE_CALL_CMD,			//����
	REQUEST_HOLD_CALL_CMD,			//���б���
	REQUEST_RETRIEVE_CALL_CMD,		//Call��ʰ,����ȡ��
	REQUEST_ANSWER_CALL_CMD,		//����Ӧ��

	REQUEST_CONSULTATION_CALL_CMD,	//��ѯ
	REQUEST_RECONNECT_CALL_CMD,		//Call����	

	REQUEST_HANGUP_CALL_CMD,			//�Ҷ�	
	REQUEST_REFUSE_CALL_CMD,			//�ܽ�
	REQUEST_CLEAR_CALL_CMD,			//�Ҷ���������
	REQUEST_SEND_DTMF_CMD,			//����DTMF

	REQUEST_TRANSFER_CALL_CMD,		//����ת�� tzx??
	REQUEST_TRANSFER_EX_CALL_CMD,	//����ת�� ��ѯת
	REQUEST_TAKEOVER_CALL_CMD,		//�ӹܻ���

	REQUEST_PICKUP_CALL_CMD,			//����
	REQUEST_THIRD_WAY_CMD,			//����ͨ��
	REQUEST_MONITOR_CALL_CMD,		//����
	REQUEST_MONITOR_EX_CALL_CMD,
	REQUEST_BARGEIN_CALL_CMD,		//ǿ��
	REQUEST_FORCEPOP_CALL_CMD,		//ǿ��

	//tzx??
	REQUEST_DEFLECT_CALL_CMD,		//ƫת
	REQUEST_CONFERENCE_CALL_CMD,
	
	

	//REQUEST_SET_BLACK_LIST_CMD,
	//REQUEST_DEL_BLACK_LIST_CMD,
	//REQUEST_DEL_ALLBLACK_LIST_CMD,

	REQUEST_CONFERENCE_CREATE_CMD, //��������
	REQUEST_CREATE_ADD_CMD, //��������
	REQUEST_CONFERENCE_ADDPARTY_CMD, //��ӻ����Ա
	REQUEST_CONFERENCE_DELPARTY_CMD, //�ӻ�����ɾ����Ա
	REQUEST_CONFERENCE_RELEASE_CMD, //�ͷŻ���
	REQUEST_CONFMEMBER_MUTE_CMD,
	REQUEST_CONFMEMBER_UNMUTE_CMD,

	GET_CTICONNECT_STATE_CMD,
	GET_DEVICE_LIST_CMD,
	GET_ACD_LIST_CMD,

	GET_CALL_LIST_CMD,
	GET_READY_AGENT_CMD,
	GET_AGENT_LIST_CMD,
	GET_EXTENSION_LIST_CMD,
	GET_FREE_AGENT_LIST_CMD,
	REQUEST_GET_HISTORY_CALL_LIST_CMD,
	REQUEST_DEL_HISTORY_RECORD_CMD

}EFSAesRequest;

typedef enum E_FSAES_TASK {
	Task_ctiAsyncRquest_Cmd,  //�첽����
	Task_ctiSyncRquest_Cmd, //ͬ������
	Task_ctiLocal_Query_cmd,
	Task_ctiLocal_Set_cmd,

	Task_HCP_Notify_Event,
	Task_ctiAsyncRequest_Resp, //CTI�첽������
	Task_ctiSyncRequest_Resp, //CTIͬ��������
	Task_ctiLocal_Query_Resp
}EFsAesTask;

enum eTransferType
{
	TransferType_phoneOperate = -1,
	TransferType_null = 0,		//û��ת��
	TransferType_outline = 1,	//ת�Ƶ�����
	TransferType_inline = 2,	//ת�Ƶ�����
	TransferType_TakeOver = 3,	//���нӹ�ת��
	TransferType_pickup = 4
	
};


#define CTI_DEVICE_TYPE_ACD		0
#define CTI_DEVICE_TYPE_AGENT	1
#define CTI_DEVICE_TYPE_PHONE	2