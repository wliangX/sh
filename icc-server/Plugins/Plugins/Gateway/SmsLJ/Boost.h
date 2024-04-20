#pragma once

// c++
#include <map>
#include <string>

// boost
#include <boost/asio.hpp>
#include <boost/locale.hpp>
#include <boost/log/detail/snprintf.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>

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
#include <Protocol/CSmsGetLocationRequest.h>
#include <Protocol/CSmsGetLocationRespond.h>

#include <Protocol/CSmsTransmitAlarmRequest.h>
#include <Protocol/CSmsTransmitAlarmRespond.h>

#include <Protocol/CSmsTransmitChangeRequest.h>
#include <Protocol/CSmsTransmitChangeRespond.h>

#include <Protocol/CSmsSetReadSync.h>
#include <Protocol/CSmsSendSmsSync.h>

#include <Protocol/CSmsServerStatusSync.h>
#include <Protocol/CSmsUserStatusSync.h>
#include <Protocol/CSmsLocationSync.h>

// #define
#define SYSTEMID							"ICC"
#define SUBSYSTEMID							"ICC-Sms"

#define ICC_QUEUE_THIRD_ALARM				"icc_queue_third_alarm"
#define ICC_TOPIC_THIRD_ALARM				"icc_topic_third_alarm"

#define ORIENTATION_POLICE_TO_CONTECT		"1"
#define ORIENTATION_CONTECT_TO_POLICE		"0"
#define STATE_READ							"1"
#define STATE_UNREAD						"0"

#define THIRD_TYPE_SMS						"DIC001003"

#define SELECT_THIRD_ALARM					"select_icc_t_third_alarm"
#define SELECT_IS_CHANGEINFO				"select_is_changeinfo"
#define SELECT_ICC_T_DEPT					"select_icc_t_dept"

#define SMS_GET_LOCATION_REQUEST			"sms_get_location_request"
#define SMS_GET_LOCATION_RESPOND			"sms_get_location_respond"

#define TA_ADD_ALARM_REQUEST				"ta_add_alarm_request"
#define TA_SET_CHANGEINFO_REQUEST			"ta_set_changeinfo_request"

#define TA_STATE_SYNC						"ta_state_sync"
#define TA_CHANGEINFO_SYNC					"ta_changeinfo_sync"

#define SMS_SERVER_STATUS_SYNC				"sms_server_status_sync"
#define SMS_USER_STATUS_SYNC				"sms_user_status_sync"
#define SMS_LOCATION_SYNC					"sms_location_sync"

#define MODULE_NAME "SmsLJ"

using namespace ICC;