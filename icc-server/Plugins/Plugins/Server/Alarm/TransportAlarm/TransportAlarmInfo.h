#pragma once


#define MQTYPE_TOPIC							"1"				//mq���ջ��ͷ�ʽ��1�����⣬0������
#define MQTYPE_QUEUE							"0"

#define UPADATE_ALARM_SYNC						 2				//ͬ���������� 2�������޸�	

#define TIMER_ONE_TIME							 0				//��ʱ���������ͣ�0������ֻ����һ��
//#define ALARM_STATE								 "20"//"DIC019120"	//����״̬��ת��
#define ALARM_STATE								 "07"//"DIC019120"	//����״̬��ת��   2022/4/29 �޸�ת��״̬Ϊ07
#define CREATEUSER							     "ICC-ApplicationServer"

#define CALL_INFO								"CallInfo"

#define LOG_TRANSPORT_ALARM_SUCCESS				 "BS001005001"	//ת����ˮ״̬��	
#define LOG_TRANSPORT_ALARM_FAIL_TIMEOUT		 "BS001005002"
#define LOG_TRANSPORT_ALARM_FAIL_TRANSCALLFAIL	 "BS001005003"
#define LOG_TRANSPORT_ALARM_ACCEPT				 "BS001005006"
#define LOG_TRANSPORT_ALARM_APPLY				 "BS001005007"
#define LOG_TRANSPORT_ALARM_EX_APPLY			 "BS001005008"
#define LOG_TRANSPORT_ALARM_REFUSE				 "BS001005009"
#define LOG_TRANSPORT_ALARM_TAKEOVER_DEST		 "BS001005010"	//����ӹ�ת����Ŀ����ϯ��ˮ
#define LOG_TRANSPORT_ALARM_TAKEOVER_SRC		 "BS001005011"	//����ӹ�ת��,Դ��ϯ��ˮ
#define LOG_TRANSPORT_ALARM_TAKEOVER_DEST_EX	 "BS001005012"	//����ӹ�ת����Ŀ����ϯ��ˮ
#define LOG_TAKEOVER_CALL						 "BS001009004"	//����ӹ���ˮ
#define LOG_ALARM_RESOURCETYPE					 "8"			//ת����Դ����
#define LOG_PHONE_RESOURCETYPE					 "7"			//��Դ����:����

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

#define ALARM_TRANSPORT_RESULT_STATE_FAIL		"1"	//ת�����ͬ��				
#define ALARM_TRANSPORT_RESULT_STATE_SUCCESS	"0"	

#define ASSIGNED								"1"	//�Ƿ�ָ��ת��ϯλ 1��ָ����0:��ָ��
#define NOT_ASSIGED								"0"

#define TRANSFER_CALL_TYPE_VDN					"1" //ת�绰���ͣ�1��ת���ţ�0��תָ����ϯ
#define TRANSFER_CALL_TYPE_ASSIGN				"0"

#define TRANSPORT_TYPE_NORMAL					"1"//����ת��
#define TRANSPORT_TYPE_TAKEOVER					"2"//����ӹ�ת��


#define MSG_SOURCE_VCS	"{\"string\":[{\"key\":\"msg_source\",\"value\":\"vcs\"}]}"
#define MSG_SOURCE		"{\"string\":[{\"key\":\"msg_source\",\"value\":\"\"}]}"


#define DEVICE_TYPE							("inside")					//�豸���ͣ�inside���ڲ�
#define LOGIN_USER_DEPT						("login_user_dept")			//�Ѿ���¼�ͻ����벿��code��ϵ����ֵ������redis�еļ�ֵ
#define MANGE_TRANSPORT_ALRM				("manage_transport_queue")  //ת��������redis�еļ�ֵ
#define MANGE_TRANSPORT_BINDMEMBER			("dept_vdn_bind")			//����VDN������redis�еļ�ֵ

#define UPDATE_ICC_T_ALARM					("update_icc_t_jjdb")
#define UPDATE_ICC_T_ALARM_TRANSPORT		("update_icc_t_alarm_transport")
#define DELETE_ICC_T_ALARM_TRANSPORT		("delete_icc_t_alarm_transport")
#define INSERT_ICC_T_ALARM_TRANSPORT		("insert_icc_t_alarm_transport")
#define SELECT_ICC_T_ALARM_TRANSPORT		("select_icc_t_alarm_transport")

#define READY_STATE_IDLE					"idle"