#pragma once

//#define ALARM_STATUS_HANDLING	 "00"    //"DIC019010"		// 受理中
//#define ALARM_STATUS_HANDLED	 "01"    //"DIC019020"		// 已受理
//#define ALARM_STATUS_PROCESSING	 "02"    //"DIC019030"		// 处警中
//#define ALARM_STATUS_PROCESSED	 "02"    //"DIC019040"		// 已处警
//#define ALARM_STATUS_FEEDBACK	 "05"    //"DIC019090"		// 已反馈
//#define ALARM_STATUS_TERMINAL	 "06"    //"DIC019100"		// 已终结
//
//#define PROCESS_STATUS_ASSGIN	 "02"	 //DIC020010	// 已下达
//#define PROCESS_STATUS_SIGIN	"DIC020030"	 //DIC020030	// 已签收
//#define PROCESS_STATUS_CANCEL	"DIC020071"    //处警单已取消
//#define PROCESS_STATUS_RETURN	"DIC020073"    //处警单已退单
//#define INVALID_ALARM			"DIC003011"		// 无效警情

#define ALARM_STATUS_HANDLING	 "00"    //"DIC019010"		// 受理中
#define ALARM_STATUS_HANDLED	 "01"    //"DIC019020"		// 已受理
//#define ALARM_STATUS_PROCESSING	 "02"    //"DIC019030"		// 处警中
#define ALARM_STATUS_PROCESSED	 "02"    //"DIC019040"		// 已处警，已派警
#define ALARM_STATUS_DISPATCHED	 "03"    //"DIC019040"		// 已出警
#define ALARM_STATUS_ARRIVED	 "04"    //"DIC019040"		// 已到达现场
#define ALARM_STATUS_FEEDBACK	 "05"    //"DIC019090"		// 已反馈
#define ALARM_STATUS_CLOSED	  "06"    //"DIC019090"		// 现场处置完毕
#define ALARM_STATUS_TERMINAL	 "07"    //"DIC019100"		// 已终结
#define ALARM_STATUS_PUTFILED	 "19"    //"DIC019110"		// 已存档
#define ALARM_STATUS_TRANS	     "07"    //"DIC019120"		// 已转出 // 已转出 2022/4/29  ICC 代表黄元兵让修改



#define PROCESS_STATUS_HANDLING	 "00"		// 受理中
#define PROCESS_STATUS_TAKEN	 "01"		// 已接警
#define PROCESS_STATUS_ASSGIN	 "02"		// 已下达，已派警
#define PROCESS_STATUS_SIGIN	 "03"		// 已签收, 已出警
//04      不管
#define PROCESS_STATUS_ARRIVED	 "05"		// 已到达现场  原值04 
#define PROCESS_STATUS_CLOSED	 "06"		// 现场处置完毕  原值05 
#define PROCESS_STATUS_ARCHIVED	 "07"		// 警情处置完毕  原值06 
#define PROCESS_STATUS_CANCEL	 "17"       //"DIC020071"    //处警单已取消 原值07
#define PROCESS_STATUS_RETURN	 "08"       //"DIC020073"    //处警单已退单
//#define PROCESS_STATUS_HANDLING	 "00"		// 受理中
//#define PROCESS_STATUS_TAKEN	 "01"		// 已接警
//#define PROCESS_STATUS_ASSGIN	 "02"		// 已下达，已派警
//#define PROCESS_STATUS_SIGIN	 "03"		// 已签收, 已出警
//#define PROCESS_STATUS_ARRIVED	 "04"		// 已到达现场
//#define PROCESS_STATUS_CLOSED	 "05"		// 现场处置完毕
//#define PROCESS_STATUS_ARCHIVED	 "06"		// 警情处置完毕
//#define PROCESS_STATUS_CANCEL	 "07"       //"DIC020071"    //处警单已取消
//#define PROCESS_STATUS_RETURN	 "08"       //"DIC020073"    //处警单已退单
#define INVALID_ALARM			"1"		// 无效警情

#define LOG_RECEIPT_ACCEPTED					"BS001001001"
#define LOG_RECEIPT_RECORDED					"BS001001002"
#define LOG_MANUALLY_ALARM_CREATED				"BS001001003"
#define LOG_PROCESS_ASSIGNED					"BS001002001"
#define LOG_PROCESS_COMPLETED					"BS001002002"
#define LOG_PROCESS_AGAIN						"BS001002003"
#define LOG_PROCESS_DISPATCHED					"BS001002004"
#define LOG_DISPATCH_NOTICE						"BS001002005"
#define LOG_DISPATCH_ACCEPTED					"BS001002006"
#define LOG_DISPATCH_ACKNOWLEDGED				"BS001002007"
#define LOG_DISPATCH_TIMEOUT					"BS001002008"
#define LOG_DISPATCH_FAILED_NO_OPERATOR			"BS001002009"
#define LOG_DISPATCH_SENT						"BS001002010"
#define LOG_DISPATCH_CANCELLED					"BS001002011"
#define LOG_DISPATCH_FAILED_HAS_BOUND			"BS001002012"
#define LOG_DISPATCH_FAILED_UNREPORTED			"BS001002013"
#define LOG_DISPATCH_APPLIED_REBACK				"BS001002014"
#define LOG_DISPATCH_CONFIRM_RECEIVE			"BS001002015"
#define LOG_DISPATCH_CONFIRM_RECEIVE_CANCELLED	"BS001002016"
#define LOG_DISPATCH_BACK_SUCCESS				"BS001002017"
#define LOG_DISPATCH_TIMEOUT_NO_CONFIRM			"BS001002018"
#define LOG_DISPATCH_TIMEOUT_NO_HANDLE			"BS001002019"
#define LOG_DISPATCH_FAILED_NO_ONLINE_SEATS		"BS001002020"
#define LOG_DISPATCH_REFUSED					"BS001002021"
#define LOG_DISPATCH_REFUSED_CANCELLED			"BS001002022"
#define LOG_DISPATCH_REFUSED_BACK				"BS001002023"
#define LOG_ALARM_EDIT							"BS001002024"
#define LOG_ALARM_SUBMIT						"BS001002026"
#define LOG_FEEDBACK_COMMON						"BS001002025"
#define LOG_ALARM_EDIT_AGAIN					"BS001002027"
#define LOG_FEEDBACK							"BS001003001"
#define LOG_FEEDBACK_ARRIVED					"BS001003002"
#define LOG_FEEDBACK_CLOSED						"BS001003003"
#define LOG_FEEDBACK_ACCEPTED					"BS001003004"
#define LOG_FEEDBACK_ADD						"BS001003005"
#define LOG_CLOSED								"BS001004001"
#define LOG_CLOSED_PDT							"BS001004002"
#define LOG_TRANSFERRED_SUCCESS					"BS001005001"
#define LOG_TRANSFERRED_FAILED_TIMEOUT			"BS001005002"
#define LOG_TRANSFERRED_FAILED_CALL				"BS001005003"
#define LOG_TRANSFERRED_FAILED_REJECTED			"BS001005004"
#define LOG_CALL_TRANSFER_WATING				"BS001005006"
#define LOG_ALARM_TRANSFER_APPLIED				"BS001005007"
#define LOG_ALARM_MERGED						"BS001006001"
#define LOG_ALARM_CANCELLED_MERGED				"BS001006002"
#define LOG_URGE_ALARM							"BS001007001"
#define LOG_ALARM_REMARK						"BS001008001"
#define LOG_CALL_INCOMMING						"BS001009002"


#define TIMER_CMD_NAME							"alarm_separate_timer"
#define SEAT_MANAGER_TIMER_CMD_NAME				"seat_manager_timer"
#define ALARM_QUEUE_TIMER_CMD_NAME				"alarm_queue_timer"

//REDIS
#define CLIENT_REGISTER_INFO_KEY	"ClientRegisterInfo"
#define ALARM_QUEUE_KEY				"alarm_queue"
#define PUB_ALARM_CASH_KEY			"pub_alarm_cash"

#define ALARM_CASH_KEY				"alarm_cash"

//部门用户关系
#define LOGIN_USER_DEPT				"login_user_dept"

#define MSG_SOURCE_VCS	"{\"string\":[{\"key\":\"msg_source\",\"value\":\"vcs\"}]}"
#define MSG_SOURCE		"{\"string\":[{\"key\":\"msg_source\",\"value\":\"\"}]}"

namespace ICC
{
	namespace Separate
	{

		enum SeatType
		{
			SEAT_UNKNOW = 0,	//未知类型		
			SEAT_RECEIPT,		//接警席
			SEAT_PROCESS,		//处警席
			SEAT_SYNTHETICAL,	//接处警席
			SEAT_MONITOR,		//班长席
			SeatTypeCount
		};

		enum PhoneState
		{
			PHONE_LOGOUT = 0,
			PHONE_LOGIN_BUSY,
			PHONE_LOGIN_IDLE,
			PHONE_LOGIN_UNKNOW
		};

		enum ActionType
		{
			ACTION_RECEIPT = 1,
			ACTION_PROCESS,
			ACTION_CACHE_RECEIPT,
			ACTION_CACHE_PROCESS
		};

		//案件状态
		enum CaseState
		{
			CASE_RECEIPT = 0,
			CASE_PROCESS,
			CASE_FEEDBACK,
			CASE_DISPATCH,
			CASE_FINISH
		};

		typedef ICC::PROTOCOL::CAddOrUpdateAlarmRequest Alarm;
		typedef std::list<Alarm> Alarms;
		typedef std::map<std::string, Alarms> AlarmMap;

		typedef std::list<std::string> PubAlarmList;

		//席位信息
		struct SeatInfo
		{
			SeatInfo() : m_nSeatType(SEAT_UNKNOW), m_nPhoneState(PHONE_LOGOUT){}

			bool Parse(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}

				m_strSeatNo = p_pJson->GetNodeValue("/client_id", "");
				m_strUserCode = p_pJson->GetNodeValue("/staff_code", "");
				m_strUserName = p_pJson->GetNodeValue("/user_name", "");
				m_strClientIP = p_pJson->GetNodeValue("/client_ip", "");
				m_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
				m_strDeptName = p_pJson->GetNodeValue("/dept_name", "");
				m_strStaffCode = p_pJson->GetNodeValue("/staff_code", "");

				std::string l_strSeatType = p_pJson->GetNodeValue("/seat_type", "");
				if (!l_strSeatType.empty())
				{
					m_nSeatType = (SeatType)std::stoi(l_strSeatType);
				}
				std::string l_strPhoneState = p_pJson->GetNodeValue("/phone_state", "");
				if (!l_strPhoneState.empty())
				{
					m_nPhoneState = (PhoneState)std::stoi(l_strPhoneState);
				}

				return true;
			}

			std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (p_pJson)
				{
					p_pJson->SetNodeValue("/client_id", m_strSeatNo);
					p_pJson->SetNodeValue("/client_name", m_strUserCode);
					p_pJson->SetNodeValue("/user_name", m_strUserName);
					p_pJson->SetNodeValue("/client_ip", m_strClientIP);
					p_pJson->SetNodeValue("/staff_code", m_strStaffCode);

					p_pJson->SetNodeValue("/dept_code", m_strDeptCode);
					p_pJson->SetNodeValue("/dept_name", m_strDeptName);
					p_pJson->SetNodeValue("/seat_type", std::to_string(m_nSeatType));
					p_pJson->SetNodeValue("/phone_state", std::to_string(m_nPhoneState));
				}

				return p_pJson->ToString();
			}

			std::string	m_strSeatNo;	//席位号，关键字
			std::string	m_strUserCode;	//用户ID
			std::string	m_strUserName;	//用户名称
			std::string m_strStaffCode;	//警员编码
			std::string	m_strClientIP;	//IP
			std::string m_strDeptCode;	//所属单位ID
			std::string m_strDeptName;	//部门名称
			SeatType	m_nSeatType;	//席位类型
			PhoneState	m_nPhoneState;  //分机状态					
		};

		//警情缓存
		struct AlarmCache
		{
			AlarmCache() : m_isDone(false), m_isSend(false){}

			bool Parse(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				m_strAlarmID = p_pJson->GetNodeValue("/alarm_id", "");
				m_strSeatNo = p_pJson->GetNodeValue("/seat_no", "");
				m_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
				m_isDone = p_pJson->GetNodeValue("/is_done", "") == "1" ? true :  false;
				m_isSend = p_pJson->GetNodeValue("/is_send", "") == "1" ? true : false;
				return true;
			}

			std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (p_pJson)
				{
					p_pJson->SetNodeValue("/alarm_id", m_strAlarmID);
					p_pJson->SetNodeValue("/seat_no", m_strSeatNo);
					p_pJson->SetNodeValue("/dept_code", m_strDeptCode);
					p_pJson->SetNodeValue("/is_done", m_isDone ? "1" : "0");
					p_pJson->SetNodeValue("/is_send", m_isSend ? "1" : "0");
				}

				return p_pJson->ToString();
			}

			std::string		m_strAlarmID;		//警单号
			std::string		m_strSeatNo;		//席位号
			std::string		m_strDeptCode;		//处理单位
			bool			m_isDone;			//是否已完成处警
			bool			m_isSend;			//是否已发送给处警席位
		};

		typedef std::list<AlarmCache> AlarmCaches;
				
		struct AlarmCacheList
		{
			bool Parse(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}

				int l_iCount = p_pJson->GetCount("/data");
				for (int i = 0; i < l_iCount; i++)
				{
					std::string l_strValueNum = std::to_string(i);
					AlarmCache l_value;
					l_value.m_strAlarmID = p_pJson->GetNodeValue("/data/" + l_strValueNum + "/alarm_id", "");
					l_value.m_strSeatNo = p_pJson->GetNodeValue("/data/" + l_strValueNum + "/seat_no", "");
					l_value.m_strDeptCode = p_pJson->GetNodeValue("/data/" + l_strValueNum + "/dept_code", "");
					l_value.m_isDone = p_pJson->GetNodeValue("/data/" + l_strValueNum + "/is_done", "") == "1" ? true : false;
					l_value.m_isSend = p_pJson->GetNodeValue("/data/" + l_strValueNum + "/is_send", "") == "1" ? true : false;

					m_alarmCaches.push_back(l_value);
				}

				return true;
			}

			std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson)
				{
					return "";
				}

				size_t i = 0;

				for (auto var : m_alarmCaches)
				{
					std::string l_strPath = "/data/";
					std::string l_strNum = std::to_string(i);
					p_pJson->SetNodeValue(l_strPath + l_strNum + "/alarm_id", var.m_strAlarmID);
					p_pJson->SetNodeValue(l_strPath + l_strNum + "/seat_no", var.m_strSeatNo);
					p_pJson->SetNodeValue(l_strPath + l_strNum + "/dept_code", var.m_strDeptCode);
					p_pJson->SetNodeValue(l_strPath + l_strNum + "/is_done", var.m_isDone ? "1" : "0");
					p_pJson->SetNodeValue(l_strPath + l_strNum + "/is_send", var.m_isSend ? "1" : "0");

					++i;
				}

				return p_pJson->ToString();
			}

			AlarmCaches m_alarmCaches;
		};
	}
}