#pragma once

#define  SELECT_ICC_T_BLACKLIST		("select_icc_t_blacklist")
#define	 SELECT_ICC_T_THIRD_ALARM	("select_icc_t_third_alarm")
#define  BLACKLIST_DELETE_FLAG		("false")
#define  SYSTEMID					("ICC")
#define  SUBSYSTEMID				("WeChatST")
#define  WECHART_CHANGE_INFO		("OS_AcceptChangeInfo")
#define  WECHART_ACCEPT_INFO		("OS_AcceptWeChatAlarmResult")
#define  WECHART_PROCESS_RESULT		("OS_AcceptProcessedResult")
#define  WECHART_TOPIC				("Topic_WebService")
#define  ADD_WECHAT_QUEUE			("icc_queue_third_alarm")

#define  ADD_WECHAT_ALARM			("ta_add_alarm_request")
#define  ADD_CHANGE_INFO			("ta_set_changeinfo_request")

#define WECHAT_MQTYPE_TOPIC			("1")				//mq接收或发送方式：1：主题，0：队列
#define WECHAT_MQTYPE_QUEUE			("0")

#define NOT_READ					("0")
#define CONTACT2POLICE				("0")
#define POLICE2CONTACT				("1")
#define WECHAT_TYPE_ST				("DIC001041")


#define ALARM_STATE_PROCESING		("DIC019010")
#define ALARM_STATE_PROCESSED		("DIC019040")
#define ALARM_STATE_INVALID			("DIC003011")
#define ALARM_STATE_REPEAT			("DIC002003")
#define ALARM_STATE_FEEDBACK		("DIC019090")

#define WECAHT_ACCEPT				("1")
#define WECAHT_NOT_ACCEPT			("0")
const unsigned int CORRECTSIZE = 3;
