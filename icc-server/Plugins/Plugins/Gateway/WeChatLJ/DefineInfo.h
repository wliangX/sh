#pragma once


#define TIMER_CMD_NAME				("WeChatLJ_Timer")
#define INSERT_ICC_T_THIRD_CHINFO	("insert_icc_t_third_changeinfo")
#define INSERT_ICC_T_THIRD_ALARM	("insert_icc_t_third_alarm")
#define SELECT_ICC_T_THIRD_ALARM	("select_icc_t_third_alarm")

#define CONTACT2POLICE							"0"
#define POLICE2CONTACT							"1"
#define MQTYPE_TOPIC							"1"				//mq接收或发送方式：1：主题，0：队列
#define MQTYPE_QUEUE							"0"

#define WECHAT_REQUEST_FILE_FLITER	(".req")
#define WECHAT_REQUEST_FILE_BAK		(".bak")
#define WECHAT_RESPEND_FILE_EXTEN	(".resp")
#define WECHAT_RESPEND_DIR			("out")
#define WECHAT_REQUEST_DIR			("in")

#define  ADD_WECHAT_ALARM			("ta_add_alarm_request")
#define  WECHART_ALARM_QUEUE		("icc_queue_third_alarm")

#define  WECHAT_TYPE_LJ				("DIC001040")//DIC001041
#define ALARM_STATE_PROCESSED		("DIC019040")//已处警

const unsigned int DELAYSTARTTIMER = 0;


