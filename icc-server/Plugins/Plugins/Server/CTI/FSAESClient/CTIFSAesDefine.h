////////////////////////////////////////////////////////////////////
//请求参数名 与CTI接口文档中保持一致
//
#define CTI_PARAM_msgid			"msgid"
#define CTI_PARAM_agentid		"agent_id"
#define CTI_PARAM_extension		"extension"  //分机号
#define CTI_PARAM_agentstatus	"status"	//坐席状态
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
#define CTI_PARAM_listener		"listener"	//监听发起者
#define CTI_PARAM_requester		"requester"	//强插、代答发起者
#define CTI_PARAM_moderator		"moderator"   //会议申请者
#define CTI_PARAM_IS_RELEASE	"is_release"

#define CTI_PARAM_extend_info		"extend_info" //

#define CTI_PARAM_conferencename	"conference_name"   //会议名
#define CTI_PARAM_member			"member"
#define CTI_PARAM_cti_conferenceid	"cti_conference_id"   //cti内部会议id

#define CTI_PARAM_member_type		"member_type"
//
#define CTI_PARAM_caseid		"caseid" //警单ID
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
//事件名 与CTI接口文档中保持一致
#define CTIEVT_AGENT_STATUS		"event_agent_status_report"
#define CTIEVT_CALL_INCOMMING	"event_call_incomming"  //呼叫进入
#define CTIEVT_CALL_WAITING		"event_call_waiting"	//呼叫等待
#define CTIEVT_CALL_ASSIGNED	"event_call_assigned"	//呼叫分配

#define CTIEVT_CALL_RING		"event_call_ring"		//振铃
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
//与CTI接口文档中保持一致
//CTI接口定义坐席登录状态
#define CTI_AGENT_STATE_LOGOUT	"0"  //坐席没有登录
#define CTI_AGENT_STATE_LOGIN	"1"  //坐席登录

//CTI接口定义的坐席忙闲状态
#define CTI_AGENT_STATUS_IDLE	"1"  //坐席空闲
#define CTI_AGENT_STATUS_BUSY	"2"  //坐席忙

//在AES定义
#define AES_CALL_DIRECTION_INSIDE	"3"  //内部呼叫
#define AES_CALL_DIRECTION_OUTSIDE	"2"	 //外部呼叫
//在AES定义
#define AES_MEMBER_TYPE_inline		"1"
#define AES_MEMBER_TYPE_outline		"4"
///////////////////////////////////////////////////////////////////
//HTTP返回码定义
#define	HTTP_SUCCESS_CODE	"200" 
#define	HTTP_BadRequest		"400"		//400  （错误请求）Bad Request
#define HTTP_Unauthorized	"401"		//当前请求需要用户验证
#define HTTP_Forbidden		"403"		//Forbidden服务器已理解请求，但拒绝执行
///////////////////////////////////////////////////////////////////
//请求
typedef enum E_FSAES_REQUEST {
	REQUEST_AES_LOGIN_CMD = 0,//AES用户登录
	REQUEST_AES_LOGOUT_CMD,//AES用户登出
	REQUEST_AES_HEART_BEAT_CMD,//

	REQUEST_MONITOR_AGENT_CMD,
	REQUEST_DISMONITOR_AGENT_CMD,
	REQUEST_MONITOR_DEVICE_CMD,
	REQUEST_DISMONITOR_DEVICE_CMD,
	REQUEST_MONITOR_ACD_CMD,
	REQUEST_DISMONITOR_ACD_CMD,

	REQUEST_AGENT_LOGIN_CMD,		//AgentLogin
	REQUEST_AGENT_LOGOUT_CMD,		//AgentLogout
	REQUEST_SET_AGENTIDLE_CMD,		//设置 Agent 空闲
	REQUEST_SET_AGENTBUSY_CMD,
	REQUEST_MAKE_CALL_CMD,			//呼叫
	REQUEST_HOLD_CALL_CMD,			//呼叫保留
	REQUEST_RETRIEVE_CALL_CMD,		//Call重拾,呼叫取回
	REQUEST_ANSWER_CALL_CMD,		//话务应答

	REQUEST_CONSULTATION_CALL_CMD,	//咨询
	REQUEST_RECONNECT_CALL_CMD,		//Call重连	

	REQUEST_HANGUP_CALL_CMD,			//挂断	
	REQUEST_REFUSE_CALL_CMD,			//拒接
	REQUEST_CLEAR_CALL_CMD,			//挂断整个话务
	REQUEST_SEND_DTMF_CMD,			//发送DTMF

	REQUEST_TRANSFER_CALL_CMD,		//呼叫转移 tzx??
	REQUEST_TRANSFER_EX_CALL_CMD,	//呼叫转移 咨询转
	REQUEST_TAKEOVER_CALL_CMD,		//接管话务

	REQUEST_PICKUP_CALL_CMD,			//代答
	REQUEST_THIRD_WAY_CMD,			//三方通话
	REQUEST_MONITOR_CALL_CMD,		//监听
	REQUEST_MONITOR_EX_CALL_CMD,
	REQUEST_BARGEIN_CALL_CMD,		//强插
	REQUEST_FORCEPOP_CALL_CMD,		//强拆

	//tzx??
	REQUEST_DEFLECT_CALL_CMD,		//偏转
	REQUEST_CONFERENCE_CALL_CMD,
	
	

	//REQUEST_SET_BLACK_LIST_CMD,
	//REQUEST_DEL_BLACK_LIST_CMD,
	//REQUEST_DEL_ALLBLACK_LIST_CMD,

	REQUEST_CONFERENCE_CREATE_CMD, //创建会议
	REQUEST_CREATE_ADD_CMD, //创建会议
	REQUEST_CONFERENCE_ADDPARTY_CMD, //添加会议成员
	REQUEST_CONFERENCE_DELPARTY_CMD, //从会议中删除成员
	REQUEST_CONFERENCE_RELEASE_CMD, //释放会议
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
	Task_ctiAsyncRquest_Cmd,  //异步请求
	Task_ctiSyncRquest_Cmd, //同步请求
	Task_ctiLocal_Query_cmd,
	Task_ctiLocal_Set_cmd,

	Task_HCP_Notify_Event,
	Task_ctiAsyncRequest_Resp, //CTI异步请求结果
	Task_ctiSyncRequest_Resp, //CTI同步请求结果
	Task_ctiLocal_Query_Resp
}EFsAesTask;

enum eTransferType
{
	TransferType_phoneOperate = -1,
	TransferType_null = 0,		//没有转移
	TransferType_outline = 1,	//转移到外线
	TransferType_inline = 2,	//转移到内线
	TransferType_TakeOver = 3,	//呼叫接管转移
	TransferType_pickup = 4
	
};


#define CTI_DEVICE_TYPE_ACD		0
#define CTI_DEVICE_TYPE_AGENT	1
#define CTI_DEVICE_TYPE_PHONE	2