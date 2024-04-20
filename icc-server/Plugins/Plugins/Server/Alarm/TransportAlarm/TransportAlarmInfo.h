#pragma once


#define MQTYPE_TOPIC							"1"				//mq接收或发送方式：1：主题，0：队列
#define MQTYPE_QUEUE							"0"

#define UPADATE_ALARM_SYNC						 2				//同步警单类型 2：代表修改	

#define TIMER_ONE_TIME							 0				//定时器启动类型，0：代表只启动一次
//#define ALARM_STATE								 "20"//"DIC019120"	//警单状态已转出
#define ALARM_STATE								 "07"//"DIC019120"	//警单状态已转出   2022/4/29 修改转出状态为07
#define CREATEUSER							     "ICC-ApplicationServer"

#define CALL_INFO								"CallInfo"

#define LOG_TRANSPORT_ALARM_SUCCESS				 "BS001005001"	//转警流水状态码	
#define LOG_TRANSPORT_ALARM_FAIL_TIMEOUT		 "BS001005002"
#define LOG_TRANSPORT_ALARM_FAIL_TRANSCALLFAIL	 "BS001005003"
#define LOG_TRANSPORT_ALARM_ACCEPT				 "BS001005006"
#define LOG_TRANSPORT_ALARM_APPLY				 "BS001005007"
#define LOG_TRANSPORT_ALARM_EX_APPLY			 "BS001005008"
#define LOG_TRANSPORT_ALARM_REFUSE				 "BS001005009"
#define LOG_TRANSPORT_ALARM_TAKEOVER_DEST		 "BS001005010"	//话务接管转警，目标坐席流水
#define LOG_TRANSPORT_ALARM_TAKEOVER_SRC		 "BS001005011"	//话务接管转警,源坐席流水
#define LOG_TRANSPORT_ALARM_TAKEOVER_DEST_EX	 "BS001005012"	//话务接管转警，目标坐席流水
#define LOG_TAKEOVER_CALL						 "BS001009004"	//话务接管流水
#define LOG_ALARM_RESOURCETYPE					 "8"			//转警资源类型
#define LOG_PHONE_RESOURCETYPE					 "7"			//资源类型:话务

#define ALARM_TRANSPORT_CTI_QUEUE				"queue_cti_control"
#define ALARM_TRANSPORT_ALARM_QUEUE				"queue_alarm"
#define ALARM_TRANSPORT_TOAPIC_NAME				"topic_alarm_sync"
#define ALARM_LOG_SYNC_TOPIC_NAME				"alarm_log_sync"
#define ALARM_SYNC								"alarm_sync"
#define ALARM_TRANSPORT_SYNC_CMD				"alarm_transport_sync"
#define ALARM_TRANSPORT_ACCEPT_SYNC_CMD			"alarm_transport_accept_sync"
#define TIMER_CMD_NAME							"alarm_transport_timer"
#define ALARM_TRANSPORT_REQUEST					"alarm_transport_request"
#define ALARM_TRANSPORT_ACCEPT_REQUEST			"alarm_transport_accept_request"
#define ALARM_TRANSPORT_REFUSE_REQUEST			"alarm_transport_refuse_request"
#define ALARM_TRANSPORT_ACCEPT_RESPOND_CMD		"alarm_transport_accept_respond"
#define ALARM_TRANSPORT_RECONNECT_REQUEST		"cancel_transfer_call_ex_request"
#define ALARM_TRANSPORT_TRANSFERCALL_REQUEST	"transfer_call_ex_request"
#define ALARM_TRANSPORT_ADDALARM_REQUEST		"add_alarm_request"
#define ALARM_TRANSPORT_CACHE_REQUEST			"transport_cache_request"
#define ALARM_TRANSPORT_TRANSFERCALL_SYNC		"transfer_call_ex_sync"
#define ALARM_TRANSPORT_ADDALARM_RESPOND		"add_alarm_respond"
#define ALARM_TRANSPORT_CACHE_RESPOND			"transport_cache_respond"
#define ALARM_TRANSPORT_REFUSE_RESPOND			"alarm_transport_refuse_respond"
#define ALARM_TRANSPORT_RESQOND					"alarm_transport_respond"

#define ALARM_TRANSPORT_RESULT_STATE_FAIL		"1"	//转警结果同步				
#define ALARM_TRANSPORT_RESULT_STATE_SUCCESS	"0"	

#define ASSIGNED								"1"	//是否指定转警席位 1：指定，0:不指定
#define NOT_ASSIGED								"0"

#define TRANSFER_CALL_TYPE_VDN					"1" //转电话类型：1：转部门，0：转指定坐席
#define TRANSFER_CALL_TYPE_ASSIGN				"0"

#define TRANSPORT_TYPE_NORMAL					"1"//主动转警
#define TRANSPORT_TYPE_TAKEOVER					"2"//话务接管转警


#define MSG_SOURCE_VCS	"{\"string\":[{\"key\":\"msg_source\",\"value\":\"vcs\"}]}"
#define MSG_SOURCE		"{\"string\":[{\"key\":\"msg_source\",\"value\":\"\"}]}"


#define DEVICE_TYPE							("inside")					//设备类型：inside：内部
#define LOGIN_USER_DEPT						("login_user_dept")			//已经登录客户端与部门code关系，该值代表在redis中的键值
#define MANGE_TRANSPORT_ALRM				("manage_transport_queue")  //转警缓存在redis中的键值
#define MANGE_TRANSPORT_BINDMEMBER			("dept_vdn_bind")			//部门VDN组在在redis中的键值

#define UPDATE_ICC_T_ALARM					("update_icc_t_jjdb")
#define UPDATE_ICC_T_ALARM_TRANSPORT		("update_icc_t_alarm_transport")
#define DELETE_ICC_T_ALARM_TRANSPORT		("delete_icc_t_alarm_transport")
#define INSERT_ICC_T_ALARM_TRANSPORT		("insert_icc_t_alarm_transport")
#define SELECT_ICC_T_ALARM_TRANSPORT		("select_icc_t_alarm_transport")

#define READY_STATE_IDLE					"idle"