#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

// boost
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
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
#include <Redisclient/IRedisClientFactory.h>
#include <HelpTool/HelpToolFactory.h>
#include <Lock/ILockFactory.h>
#include <Timer/ITimerFactory.h>

// Protocol
#include <Protocol/CWebCheckLinkRequest.h>
#include <Protocol/CWebCheckLinkRespond.h>
#include <Protocol/CWebLoginRequest.h>
#include <Protocol/CWebLoginRespond.h>
#include <Protocol/CSetClientRegisterRequest.h>
#include <Protocol/CSetClientRegisterRespond.h>
#include <Protocol/CWebLogoutRequest.h>
#include <Protocol/CWebLogoutRespond.h>
#include <Protocol/CDeleteClientRegisterRequest.h>
#include <Protocol/CDeleteClientRegisterRespond.h>
#include <Protocol/CWebServiceCloseNotify.h>
#include <Protocol/CWebGetDictRequest.h>
#include <Protocol/CWebGetDictRespond.h>
#include <Protocol/CWebGetAlarmRequest.h>
#include <Protocol/CWebGetAlarmRespond.h>
#include <Protocol/WebGetAlarm.h>
#include <Protocol/CWebGetAlarmDetailRequest.h>
#include <Protocol/CWebGetAlarmDetailRespond.h>
#include <Protocol/CWebAddAlarmRequest.h>
#include <Protocol/CWebAddAlarmRespond.h>
#include <Protocol/CWebSignRequest.h>
#include <Protocol/CWebSignRespond.h>
#include <Protocol/CWebAddAlarmRemarkRequest.h>
#include <Protocol/CWebAddAlarmRemarkRespond.h>
#include <Protocol/CWebGetAlarmFeedbackRequest.h>
#include <Protocol/CWebGetAlarmFeedbackRespond.h>
#include <Protocol/CWebAddAlarmFeedbackRequest.h>
#include <Protocol/CWebAddAlarmFeedBackRespond.h>
#include <Protocol/CWebBackAlarmRequest.h>
#include <Protocol/CWebBackAlarmRespond.h>

#include <Protocol/CWebAlarmSync.h>
#include <Protocol/CWebAlarmProcessSync.h>
#include <Protocol/CWebAlarmRemarkSync.h>
#include <Protocol/CWebAlarmFeedbackSync.h>
#include <Protocol/CWebBackAlarmSync.h>
#include <Protocol/CWebGetUserInfo.h>

#include <Protocol/CAlarmLogSync.h>

#include <Protocol/CBaseRequest.h>
#include <Protocol/CAddNoticeSync.h>
#include <Protocol/CNoticeInfo.h>
#include <Protocol/CGetNoticesRespond.h>
#include <Protocol/CUrgesRequest.h>

#include "Protocol/CGetDeptRequest.h"
#include "Protocol/CGetDeptRespond.h"
#include "Protocol/CDeptSync.h"
#include <Protocol/CGetStaffRequest.h>
#include <Protocol/CGetStaffRespond.h>
#include <Protocol/CSetStaffRequest.h>

#include <Protocol/SyncServerLock.h>

#define MODULE_NAME				"web"
#define USER_INFO				"UserInfo"
#define WEB_LOGIN_INFO			"WebLoginInfo"
#define TIMER_CMD_NAME			"web_check_link_timer"
#define SELECT_WEB_ALARM		"select_web_alarm"
#define SELECT_WEB_ALARM_COUNT	"select_web_alarm_count"
#define DIC001003				("DIC001003")	//报警方式-短信
#define DIC001040				("DIC001040")	//报警方式-微信
#define DIC020030				("DIC020030")	//处警单状态-已签收
#define DIC020060				("DIC020060")	//处警单状态-已反馈
#define DIC020073				("DIC020073")	//处警单状态-已退单
#define BS001002028				("BS001002028")	//流水状态-已退单

#define WEB_SUCCESS				"0"
#define WEB_ERROR_UNKNOWN		"1"				//未知错误
#define WEB_ERROR_REPEATLOGIN	"2"				//该账户已在别处登录
#define WEB_ERROR_IPCHECK		"3"				//IP校验失败
#define WEB_ERROR_PWD			"4"				//账号密码错误
#define WEB_ERROR_PURVIEW		"5"				//账号所属单位权限错误
#define WEB_ERROR_DB			"6"				//数据库操作失败
#define WEB_ERROR_TOKEN			"7"				//Token 不匹配
#define WEB_ERROR_ALARMID		"8"				//警情 id 不存在
#define WEB_ERROR_PROCESSID		"9"				//处警 id 不存在
#define WEB_ERROR_FEEDBACKID	"10"			//反馈 id 不存在
#define WEB_ERROR_SIGN			"11"			//警单已签收

using namespace ICC;