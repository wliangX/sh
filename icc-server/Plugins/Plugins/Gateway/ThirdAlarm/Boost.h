#pragma once

// icc
#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <AmqClient/IAmqClient.h>
#include <Config/IConfigFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <Json/IJsonFactory.h>
#include <Lock/ILockFactory.h>
#include <Log/ILogFactory.h>
#include <Observer/IObserverFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Lock/ILockFactory.h>
#include <Timer/ITimerFactory.h>
#include <TcpClient/ITcpClientFactory.h>

// protocol
#include <Protocol/CTAGetAlarmByGuidRequest.h>
#include <Protocol/CTAGetAlarmByGuidRespond.h>
#include <Protocol/CTAGetAlarmRequest.h>
#include <Protocol/CTAGetAlarmRespond.h>
#include <Protocol/CTAAddAlarmRequest.h>
#include <Protocol/CTAAddAlarmRespond.h>
#include <Protocol/CTAGetNotReadCountRequest.h>
#include <Protocol/CTAGetNotReadCountRespond.h>
#include <Protocol/CTASetStateRequest.h>
#include <Protocol/CTASetStateRespond.h>
#include <Protocol/CTASetChangeInfoStateRequest.h>
#include <Protocol/CTASetChangeInfoStateRespond.h>
#include <Protocol/CTAGetChangeInfoRequest.h>
#include <Protocol/CTAGetChangeInfoRespond.h>
#include <Protocol/CTASetChangeInfoRequest.h>
#include <Protocol/CTASetChangeInfoRespond.h>
#include <Protocol/CTAGetLocationRequest.h>
#include <Protocol/CTAGetLocationRespond.h>

#include <Protocol/CTAAlarmSync.h>
#include <Protocol/CTANotReadCountSync.h>
#include <Protocol/CTAStateSync.h>
#include <Protocol/CTAChangeInfoSync.h>
#include <Protocol/CTAChangeInfoNotReadCountSync.h>
#include <Protocol/CTAChangeInfoStateSync.h>

// define
#define SYSTEMID							"ICC"
#define SUBSYSTEMID							"ICC-ThirdAlarm"

#define ORIENTATION_FIRST					"2"
#define ORIENTATION_POLICE_TO_CONTECT		"1"
#define ORIENTATION_CONTECT_TO_POLICE		"0"
#define STATE_READ							"1"
#define STATE_UNREAD						"0"
#define BLACKLIST_DELETE_FLAG				"false"

#define ICC_QUEUE_SMS						"icc_queue_sms"
#define ICC_TOPIC_THIRD_ALARM				"icc_topic_third_alarm"

#define SELECT_ICC_T_BLACKLIST				"select_icc_t_blacklist"
#define SELECT_THIRD_ALARM					"select_icc_t_third_alarm"
#define SELECT_THIRD_ALARM_EX				"select_icc_t_third_alarm_ex"
#define SELECT_THIRD_ALARM_COUNT			"select_icc_t_third_alarm_count"
#define SELECT_THIRD_ALARM_NCOUNT			"select_icc_t_third_alarm_ncount"
#define SELECT_THIRD_CHANGEINFO				"select_icc_t_third_changeinfo"
#define SELECT_THIRD_CHANGEINFO_NCOUNT		"select_icc_t_third_changeinfo_ncount"
#define UPDATE_THIRD_ALARM					"update_icc_t_third_alarm"
#define UPDATE_THIRD_CHANGEINFO				"update_icc_t_third_changeinfo"
#define INSERT_THIRD_CHANGEINFO				"insert_icc_t_third_changeinfo"
#define INSERT_THIRD_ALARM					"insert_icc_t_third_alarm"

#define TA_GET_ALARM_BY_GUID_REQUEST		"ta_get_alarm_by_guid_request"
#define TA_GET_ALARM_BY_GUID_RESPOND		"ta_get_alarm_by_guid_respond"
#define TA_GET_ALARM_REQUEST				"ta_get_alarm_request"
#define TA_GET_ALARM_RESPOND				"ta_get_alarm_respond"
#define TA_ADD_ALARM_REQUEST				"ta_add_alarm_request"
#define TA_ADD_ALARM_RESPOND				"ta_add_alarm_respond"
#define TA_GET_NOT_READ_COUNT_REQUEST		"ta_get_not_read_count_request"
#define TA_GET_NOT_READ_COUNT_RESPOND		"ta_get_not_read_count_respond"
#define TA_SET_STATE_REQUEST				"ta_set_state_request"
#define TA_SET_STATE_RESPOND				"ta_set_state_respond"
#define TA_SET_CHANGEINFO_STATE_REQUEST		"ta_set_changeinfo_state_request"
#define TA_SET_CHANGEINFO_STATE_RESPOND		"ta_set_changeinfo_state_respond"
#define TA_GET_CHANGEINFO_REQUEST			"ta_get_changeinfo_request"
#define TA_GET_CHANGEINFO_RESPOND			"ta_get_changeinfo_respond"
#define TA_SET_CHANGEINFO_REQUEST			"ta_set_changeinfo_request"
#define TA_SET_CHANGEINFO_RESPOND			"ta_set_changeinfo_respond"
#define TA_GET_LOCATION_REQUEST				"ta_get_location_request"
#define TA_GET_LOCATION_RESPOND				"ta_get_location_respond"

#define TA_ALARM_SYNC						"ta_alarm_sync"
#define TA_NOT_READ_COUNT_SYNC				"ta_not_read_count_sync"
#define TA_STATE_SYNC						"ta_state_sync"
#define TA_CHANGEINFO_SYNC					"ta_changeinfo_sync"
#define TA_CHANGEINFO_NOT_READ_COUNT_SYNC	"ta_changeinfo_not_read_count_sync"
#define TA_CHANGEINFO_STATE_SYNC			"ta_changeinfo_state_sync"

#define SMS_GET_LOCATION_REQUEST			"sms_get_location_request"

#define MODULE_NAME "ThirdAlarm"

using namespace ICC;