#include "Boost.h"
#include "BusinessImpl.h"
#include <thread>


#define CLIENT_REGISTER_INFO "ClientRegisterInfo"
#define SEAT_DATA_INFO ("SeatDataInfo")
#define SEAT_INFO_KEY ("SeatInfo")
#define STAFF_DATA_INFO ("StaffDataInfo")
#define DEPT_CODE_INFO_KEY ("DeptCodeInfoKey")
#define SYSTEMID	("ICC")
#define SUBSYSTEMID ("Seat-Sync-data")
#define AGENT_PHONE_STATE ("AgentPhoneState")
#define PARAMINFO	("ParamInfo")
#define SEATIDLECOUNT	("SeatIdleCount")
#define CALL_INFO		("CallInfo")
#define CTI_HANGUPSTATE ("hangupstate")		//Device （挂机态）
#define CTI_FREESTATE	"freestate"			//Device （空闲态）
const std::string SEATINFO_ADD = "1";
const std::string SEATINFO_UPDATE = "2";
const std::string SEATINFO_DELETE = "3";
const std::string SEAT_SYNC_MSGID = "syncdata";
const std::string TOMORROW_MSGID = "TomorrowTime";
const std::string INIT_SEAT_DEVICE_STATE = "InitSeatDeviceState";
const std::string COUNT_ALARM_CONFIG = "AlarmCountConfig";

namespace SET_TYPE
{
	const std::string ADD_TYPE = "1";   //新增
	const std::string UPDATE_TYPE = "2";  //更新
	const std::string DELETE_TYPE = "3";  //删除
}

CBusinessImpl::CBusinessImpl()
{
}

CBusinessImpl::~CBusinessImpl()
{
}

void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s", MODULE_NAME);

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(BASEDATA_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pIDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pJsonFac = ICCGetIJsonFactory();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();
	m_pMsgCenter = ICCGetIMessageCenterFactory()->CreateMessageCenter();
	printf("OnInit complete! plugin = %s", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s", MODULE_NAME);

	std::string strAlarmCountTimeConfig = m_pConfig->GetValue("ICC/Plugin/SEAT/IsUsing", "1");
	if (strAlarmCountTimeConfig != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "SEAT plugin no need to load!!!");
		return;
	}
	m_strAlarmCountConfig = m_pConfig->GetValue("ICC/Plugin/SEAT/AlarmCountTime", "60");
	m_strCheckDeviceState = m_pConfig->GetValue("ICC/Plugin/SEAT/CheckDeviceState", "0");
	m_strMinFreeCount = m_pConfig->GetValue("ICC/Plugin/SEAT/MinFreeCount", "5");


	m_strCodeMode = m_pConfig->GetValue("ICC/Plugin/Synthetical/CodeMode", "1");

	ICC_LOG_INFO(m_pLog, "plugin p_startTime1-2. [%s]", m_strAlarmCountConfig.c_str());
	ICC_LOG_INFO(m_pLog, "plugin config [%s] [%s]", m_strCheckDeviceState.c_str(), m_strMinFreeCount.c_str());
	if (!_LoadSeatDataFromDB())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.SeatData failed to load SeatData info");
	}

	// 上下线同步坐席号数据信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "client_register_sync", OnNotifiSeatDataSync);
	// 话机状态同步坐席号数据信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "device_state_sync", OnNotifiSeatDeviceStateSync);
	// 通话状态同步坐席号数据信息 置忙，置闲
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "acd_agent_state_sync", OnNotifiSeatLoginStateSync);
	// CTI 连接更新数据
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "cti_connect_state_sync", OnNotifiCtiConnectStateSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_sync", OnNotifiAlarmCountSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "seat_sync", OnNotifiSeatInfoManagerSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_all_seat_device_info_request", OnCNotifGetSeatInfoRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "syn_nacos_params", OnReceiveSynNacosParams);
	std::string p_startTime = GetYestertodayTime();
	std::string p_strTomorrow = m_pDateTime->GetAfterTime(p_startTime);
	ICC_LOG_INFO(m_pLog, "plugin p_startTime. p_strTomorrow [%s-%s]", p_startTime.c_str(), p_strTomorrow.c_str());
	DateTime::CDateTime timeStart = m_pDateTime->SecondsDifference(m_pDateTime->FromString(p_strTomorrow), m_pDateTime->CurrentDateTime());
	int iIncTime = m_pString->ToInt(m_strAlarmCountConfig);
	m_pTimer.AddTimer(this, SEAT_SYNC_MSGID, 24 * 60 * 60 * 1000, timeStart * 1000);
	m_pTimer.AddTimer(this, TOMORROW_MSGID, 5 * 1000, (timeStart) * 1000);
	m_pTimer.AddTimer(this, COUNT_ALARM_CONFIG, iIncTime * 60 * 1000, 5 * 1000);
	m_pTimer.AddTimer(this, INIT_SEAT_DEVICE_STATE, 5 * 1000, 1 * 1000);
	ICC_LOG_INFO(m_pLog, "plugin p_startTime1-2. [%d]", iIncTime);
	ICC_LOG_INFO(m_pLog, "plugin p_startTime1-2. [%d]", timeStart);
	ICC_LOG_INFO(m_pLog, "plugin  seatdata start success");
	m_pMsgCenter->Start();
	printf("OnStart complete! plugin = %s", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "plugin seat stop success");
}

void CBusinessImpl::OnDestroy()
{
}


bool CBusinessImpl::_LoadSeatDataFromDB()
{
	// 初始化之前先清空一下数据
	m_pRedisClient->Del(SEAT_DATA_INFO);
	m_pRedisClient->Del(STAFF_DATA_INFO);
	bool b_result = true;
	// 初始化 班长席注册信息
	if (!InitClientRegisterInfo())
	{
		ICC_LOG_ERROR(m_pLog, " start InitClientRegisterInfo failed !");
		b_result = false;
	}
	// 班长注册信息
	if (!InitSeatInfoFromRedis()) 
	{
		ICC_LOG_ERROR(m_pLog, " start InitSeatInfoFromRedis failed !");
		b_result = false;
	}
	// 初始化 话机状态
	if (!InitClientDevicesStateInfo()) 
	{
		ICC_LOG_ERROR(m_pLog, " start InitClientDevicesStateInfo failed !");
		b_result = false;
	}
	// 初始化今日接警数量
	if (!InitSeatAlarmCount()) 
	{
		ICC_LOG_ERROR(m_pLog, " start InitSeatAlarmCount failed !");
		b_result = false;
	}
	
	return b_result;
}

bool CBusinessImpl::InitClientRegisterInfo()
{
	std::map<std::string, std::string> l_mapClientRegisterInfo;
	m_pRedisClient->HGetAll(CLIENT_REGISTER_INFO, l_mapClientRegisterInfo);
	PROTOCOL::CSeatDataInfo SeatData;
	std::string l_strSeatInfo = "";
	for (auto it = l_mapClientRegisterInfo.begin(); it != l_mapClientRegisterInfo.end(); it++) {
		ICC_LOG_INFO(m_pLog, " start InitClientRegisterInfo %s-%s", it->first.c_str(), it->second.c_str());
		if (SeatData.ParseStringRegist(it->second, m_pJsonFac->CreateJson())) {
			SeatData.m_oBody.m_strSeatNo = it->first;
			SeatData.m_oBody.m_strSeatType = "0";
			l_strSeatInfo = SeatData.ToString(m_pJsonFac->CreateJson());
			ICC_LOG_INFO(m_pLog, "new InitClientRegisterInfo  Sync: %s-%s", SeatData.m_oBody.m_strSeatNo.c_str(), l_strSeatInfo.c_str());
			if (!it->first.empty()) {
				m_pRedisClient->HSet(SEAT_DATA_INFO, it->first, l_strSeatInfo);
			}
		}
	}
	return true;
}

bool CBusinessImpl::InitSeatInfoFromRedis()
{
	std::map<std::string, std::string> l_mapClientRegisterInfo;
	m_pRedisClient->HGetAll(SEAT_INFO_KEY, l_mapClientRegisterInfo);
	PROTOCOL::CSeatDataInfo SeatInfo;
	PROTOCOL::CSeatDataInfo oldSeatInfo;
	std::string strOldData = "";
	std::string l_strSeatInfo;
	ICC_LOG_INFO(m_pLog, "start init seatinfo size %d", l_mapClientRegisterInfo.size());
	for (auto it = l_mapClientRegisterInfo.begin(); it != l_mapClientRegisterInfo.end(); it++) {
		ICC_LOG_INFO(m_pLog, "start init seatinfo old value %s-%s", it->first.c_str(), it->second.c_str());
		if (!SeatInfo.ParseStringSeatInfo(it->second, m_pJsonFac->CreateJson())) {
			ICC_LOG_INFO(m_pLog, "Parse seatinfo failed %s-%s", it->first.c_str(), it->second.c_str());
			continue;
		}
		SeatInfo.m_oBody.m_strSeatNo = it->first;
		if (!m_pRedisClient->HExists(SEAT_DATA_INFO, it->first)) {
			SeatInfo.m_oBody.m_strSeatType = "1";
			l_strSeatInfo = SeatInfo.ToString(m_pJsonFac->CreateJson());
		}
		else {
			m_pRedisClient->HGet(SEAT_DATA_INFO, it->first, strOldData);
			ICC_LOG_INFO(m_pLog, "seatinfo Exists value %s-%s", it->first.c_str(), strOldData.c_str());
			if (oldSeatInfo.ParseString(strOldData, m_pJsonFac->CreateJson())) {
				oldSeatInfo.m_oBody.m_strDeptCode = SeatInfo.m_oBody.m_strDeptCode;
				oldSeatInfo.m_oBody.m_strSeatType = "1";
				l_strSeatInfo = oldSeatInfo.ToString(m_pJsonFac->CreateJson());
			}
			else {
				ICC_LOG_ERROR(m_pLog, "Invalid ParseString request  [%s]", l_strSeatInfo.c_str());
			}
		}
		ICC_LOG_INFO(m_pLog, "new seatinfo Sync: %s-%s", it->first.c_str(), l_strSeatInfo.c_str());
		if (!it->first.empty()) {
			m_pRedisClient->HSet(SEAT_DATA_INFO, it->first, l_strSeatInfo);
		}
		
	}
	return true;
}

// 初始化今日接警数量
bool  CBusinessImpl::InitSeatAlarmCount()
{
	std::vector<ICC::PROTOCOL::CSeatDataInfo> tmp_vecSeatAlarmCount;
	if (!_QueryTodayAlarmCount(tmp_vecSeatAlarmCount))
	{
		ICC_LOG_ERROR(m_pLog, "_QueryAll Seat TodayAlarmCount failed!!!");
		return false;
	}

	std::vector<ICC::PROTOCOL::CStaffDataInfo::CStaffInfo> tmp_vecStaffCount;
	if (_QueryStaffTodayAlarmCount(tmp_vecStaffCount))
	{
		ICC_LOG_ERROR(m_pLog, "_Query Staff Today AlarmCount failed!!!");
		return false;
	}

	for (size_t i = 0; i < tmp_vecStaffCount.size(); ++i)
	{
		{
			PROTOCOL::CStaffDataInfo l_StaffDataInfo;
			l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCode = tmp_vecStaffCount[i].m_strStaffCode;
			l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffName = tmp_vecStaffCount[i].m_strStaffName;
			l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCount = "0";
			if (!tmp_vecStaffCount[i].m_strStaffCount.empty())
			{
				l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCount = tmp_vecStaffCount[i].m_strStaffCount;
			}
			
			std::string l_strStaffInfo = l_StaffDataInfo.ToString(m_pJsonFac->CreateJson());
			ICC_LOG_INFO(m_pLog, "SetStaffAlarmCountByStaff Sync: %s-%s", l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCode.c_str(), l_strStaffInfo.c_str());
			if (!l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCode.empty())
			{
				m_pRedisClient->HSet(STAFF_DATA_INFO, l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCode, l_strStaffInfo);
			}
		}
	}

	std::map<std::string, std::string> l_mapClientRegisterInfo;
	m_pRedisClient->HGetAll(SEAT_DATA_INFO, l_mapClientRegisterInfo);
	ICC_LOG_INFO(m_pLog, " all alarm count clear to 0 ");
	for (auto it = l_mapClientRegisterInfo.begin(); it != l_mapClientRegisterInfo.end(); it++) 
	{
		SetSeatAlarmCountBySeatNo(it->first, "0");
	}

	int iCount = tmp_vecSeatAlarmCount.size();
	
	for (int i = 0; i < iCount; ++i)
	{
		if (m_pRedisClient->HExists(SEAT_DATA_INFO, tmp_vecSeatAlarmCount[i].m_oBody.m_strSeatNo)) 
		{
			SetSeatAlarmCountBySeatNo(tmp_vecSeatAlarmCount[i].m_oBody.m_strSeatNo, tmp_vecSeatAlarmCount[i].m_oBody.m_strAlarmCount);
		}
	}
	return true;
}

bool CBusinessImpl::InitClientDevicesStateInfo()
{
	std::string strCTIServerIp;
	std::string strCTIServerPort;
	std::string stdCTIName = "icc-cti-service";
	
	bool b_Timer = true;
	do
	{
		// https://192.168.0.168:26000/icc-cti-service/icc/cti/get_agent_list_request
		std::string strTarget = "/icc-cti-service/icc/cti/get_device_list_request";
		std::string strErrorMessage;
		std::string strContent = "{}";
		std::map<std::string, std::string> mapHeaders;
		JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
		mapHeaders.insert(std::make_pair("Content-Type", "application/json"));
		
		if (!GetServiceInfo(stdCTIName, strCTIServerIp, strCTIServerPort))
		{
			ICC_LOG_ERROR(m_pLog, "faile seatinfo Receive empty strErrorMessage : %s", strErrorMessage.c_str());
			return false;
		}

		//std::string strSeatInfoReceive = m_pHttpClient->PostEx(strWechatServerIp, strWechatServerPort, strTarget, mapHeaders, strContent, strErrorMessage);
		std::string strSeatInfoReceive = m_pHttpClient->PostWithTimeout(strCTIServerIp, strCTIServerPort, strTarget, mapHeaders, strContent, strErrorMessage, 2);

		ICC_LOG_DEBUG(m_pLog, "get_device_list_request to strReceive: %s", strSeatInfoReceive.c_str());

		if (strSeatInfoReceive.empty())
		{
			ICC_LOG_ERROR(m_pLog, "faile seatinfo empty strErrorMessage : %s", strErrorMessage.c_str());
		}
		else {
			if (!InitSeatDeviceState(strSeatInfoReceive)) {
				ICC_LOG_ERROR(m_pLog, "InitSeatDeviceState faile seatinfo ");
				b_Timer = false;
			}
		}

		strTarget = "/icc-cti-service/icc/cti/get_agent_list_request";
		strSeatInfoReceive = m_pHttpClient->PostWithTimeout(strCTIServerIp, strCTIServerPort, strTarget, mapHeaders, strContent, strErrorMessage, 2);

		ICC_LOG_DEBUG(m_pLog, "get_agent_list_request to strReceive: %s", strSeatInfoReceive.c_str());

		if (strSeatInfoReceive.empty())
		{
			ICC_LOG_ERROR(m_pLog, "faile seatinfo empty strErrorMessage : %s", strErrorMessage.c_str());
			return false;
		}
		else {
			if (!InitSeatAgentState(strSeatInfoReceive)) {
				ICC_LOG_ERROR(m_pLog, "InitSeatAgentState faile seatinfo ");
				b_Timer = false;
			}
		}
		if (b_Timer) {
			return true;
		}
		else {
			return false;
		}

	} while (false);
	return true;
}

bool CBusinessImpl::InitSeatDeviceState(std::string& strSeatInfoReceive)
{
	PROTOCOL::CGetSeatDeviceListRespond l_oRequestObj;
	if (!l_oRequestObj.ParseString(strSeatInfoReceive, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Invalid GetDeviceList request protocol: [%s]", strSeatInfoReceive.c_str());
		return false;
	}
	int size = l_oRequestObj.m_oBody.m_vecData.size();
	std::string l_strSeatInfo;
	PROTOCOL::CSeatDataInfo oldSeatData;
	PROTOCOL::CSeatDataInfo SeatDataClear;
	for (int i = 0; i < size; i++) {
		if (!m_pRedisClient->HExists(SEAT_DATA_INFO, l_oRequestObj.m_oBody.m_vecData[i].m_strDevice))
		{
			ICC_LOG_DEBUG(m_pLog, "init  seat devices redis no HExists request  [%s]", l_oRequestObj.m_oBody.m_vecData[i].m_strDevice.c_str());
			oldSeatData.m_oBody.m_strSeatNo = l_oRequestObj.m_oBody.m_vecData[i].m_strDevice;
			oldSeatData.m_oBody.m_strLoginMode = l_oRequestObj.m_oBody.m_vecData[i].m_strLoginMode;
			oldSeatData.m_oBody.m_strDeptCode = l_oRequestObj.m_oBody.m_vecData[i].m_strDeptCode;
			oldSeatData.m_oBody.m_strDeviceState = l_oRequestObj.m_oBody.m_vecData[i].m_strState;
			oldSeatData.m_oBody.m_strReadyState = l_oRequestObj.m_oBody.m_vecData[i].m_strReadyState;
			oldSeatData.m_oBody.m_strDeviceType = l_oRequestObj.m_oBody.m_vecData[i].m_strDeviceType;
			oldSeatData.m_oBody.m_strTime = l_oRequestObj.m_oBody.m_vecData[i].m_strTime;
			oldSeatData.m_oBody.m_strBeginTalkTime = l_oRequestObj.m_oBody.m_vecData[i].m_strBeginTalkTime;
			oldSeatData.m_oBody.m_strCallDirection = l_oRequestObj.m_oBody.m_vecData[i].m_strCallDirection;
			oldSeatData.m_oBody.m_strCallrefId = l_oRequestObj.m_oBody.m_vecData[i].m_strCallrefId;
			oldSeatData.m_oBody.m_strCallerId = l_oRequestObj.m_oBody.m_vecData[i].m_strCallerId;
			oldSeatData.m_oBody.m_strCalledId = l_oRequestObj.m_oBody.m_vecData[i].m_strCalledId;
			oldSeatData.m_oBody.m_strOriginalCallId = l_oRequestObj.m_oBody.m_vecData[i].m_strOriginalCallId;
		}
		else {
			m_pRedisClient->HGet(SEAT_DATA_INFO, l_oRequestObj.m_oBody.m_vecData[i].m_strDevice, l_strSeatInfo);
			ICC_LOG_DEBUG(m_pLog, "Invalid HExists request %s  [%s]", l_oRequestObj.m_oBody.m_vecData[i].m_strDevice.c_str(), l_strSeatInfo.c_str());
			if (oldSeatData.ParseString(l_strSeatInfo, m_pJsonFac->CreateJson())) {
				if (oldSeatData.m_oBody.m_strSeatNo == l_oRequestObj.m_oBody.m_vecData[i].m_strDevice) {
					oldSeatData.m_oBody.m_strLoginMode = l_oRequestObj.m_oBody.m_vecData[i].m_strLoginMode;
					oldSeatData.m_oBody.m_strDeviceState = l_oRequestObj.m_oBody.m_vecData[i].m_strState;
					oldSeatData.m_oBody.m_strReadyState = l_oRequestObj.m_oBody.m_vecData[i].m_strReadyState;
					oldSeatData.m_oBody.m_strDeviceType = l_oRequestObj.m_oBody.m_vecData[i].m_strDeviceType;
					oldSeatData.m_oBody.m_strTime = l_oRequestObj.m_oBody.m_vecData[i].m_strTime;
					oldSeatData.m_oBody.m_strBeginTalkTime = l_oRequestObj.m_oBody.m_vecData[i].m_strBeginTalkTime;
					oldSeatData.m_oBody.m_strCallDirection = l_oRequestObj.m_oBody.m_vecData[i].m_strCallDirection;
					oldSeatData.m_oBody.m_strCallrefId = l_oRequestObj.m_oBody.m_vecData[i].m_strCallrefId;
					oldSeatData.m_oBody.m_strCallerId = l_oRequestObj.m_oBody.m_vecData[i].m_strCallerId;
					oldSeatData.m_oBody.m_strCalledId = l_oRequestObj.m_oBody.m_vecData[i].m_strCalledId;
					oldSeatData.m_oBody.m_strOriginalCallId = l_oRequestObj.m_oBody.m_vecData[i].m_strOriginalCallId;
				}
			}
			else {
				ICC_LOG_DEBUG(m_pLog, "Invalid ParseString request  [%s]", strSeatInfoReceive.c_str());
				// return false;
			}
		}
		if (oldSeatData.m_oBody.m_strLoginMode == "logout") {
			oldSeatData.m_oBody.m_strReadyState = "busy";
		}
		oldSeatData.m_oBody.m_strSeatType = "3";
		l_strSeatInfo = oldSeatData.ToString(m_pJsonFac->CreateJson());
		ICC_LOG_INFO(m_pLog, "new - old data Sync: %s-%s", oldSeatData.m_oBody.m_strSeatNo.c_str(), l_strSeatInfo.c_str());
		if (!oldSeatData.m_oBody.m_strSeatNo.empty()) {
			m_pRedisClient->HSet(SEAT_DATA_INFO, oldSeatData.m_oBody.m_strSeatNo, l_strSeatInfo);
		}
		// 清空一次数据
		oldSeatData = SeatDataClear;
	}
	return true;
}

bool CBusinessImpl::InitSeatAgentState(std::string& strSeatInfoReceive)
{
	PROTOCOL::CGetSeatAgentRespond l_oSeatAgent;
	if (!l_oSeatAgent.ParseString(strSeatInfoReceive, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Invalid GetDeviceList request protocol: [%s]", strSeatInfoReceive.c_str());
		return false;
	}

	int size = l_oSeatAgent.m_oBody.m_vecData.size();
	std::string l_strSeatInfo;
	PROTOCOL::CSeatDataInfo oldSeatData;
	for (int i = 0; i < size; i++) 
	{
		if (!m_pRedisClient->HExists(SEAT_DATA_INFO, l_oSeatAgent.m_oBody.m_vecData[i].m_strAgent))
		{
			ICC_LOG_DEBUG(m_pLog, "init  l_oSeatAgent seat devices redis no HExists request  [%s]", l_oSeatAgent.m_oBody.m_vecData[i].m_strAgent.c_str());
			oldSeatData.m_oBody.m_strSeatNo = l_oSeatAgent.m_oBody.m_vecData[i].m_strAgent;
			oldSeatData.m_oBody.m_strLoginMode = l_oSeatAgent.m_oBody.m_vecData[i].m_strLoginMode;
			oldSeatData.m_oBody.m_strReadyState = l_oSeatAgent.m_oBody.m_vecData[i].m_strReadyState;
		}
		else 
		{
			m_pRedisClient->HGet(SEAT_DATA_INFO, l_oSeatAgent.m_oBody.m_vecData[i].m_strAgent, l_strSeatInfo);
			ICC_LOG_DEBUG(m_pLog, "Invalid l_oSeatAgent HExists request %s  [%s]", l_oSeatAgent.m_oBody.m_vecData[i].m_strAgent.c_str(), l_strSeatInfo.c_str());
			if (oldSeatData.ParseString(l_strSeatInfo, m_pJsonFac->CreateJson())) {
				if (oldSeatData.m_oBody.m_strSeatNo == l_oSeatAgent.m_oBody.m_vecData[i].m_strAgent)
				{
					oldSeatData.m_oBody.m_strLoginMode = l_oSeatAgent.m_oBody.m_vecData[i].m_strLoginMode;
					oldSeatData.m_oBody.m_strReadyState = l_oSeatAgent.m_oBody.m_vecData[i].m_strReadyState;
				}
			}
			else 
			{
				ICC_LOG_DEBUG(m_pLog, "Invalid l_oSeatAgent ParseString request  [%s]", strSeatInfoReceive.c_str());
				// return false;
			}
		}
		if (oldSeatData.m_oBody.m_strLoginMode == "logout")
		{
			oldSeatData.m_oBody.m_strReadyState = "busy";
		}
		oldSeatData.m_oBody.m_strSeatType = "3";
		l_strSeatInfo = oldSeatData.ToString(m_pJsonFac->CreateJson());
		ICC_LOG_INFO(m_pLog, "new - old data Sync: %s-%s", oldSeatData.m_oBody.m_strSeatNo.c_str(), l_strSeatInfo.c_str());
		if (!oldSeatData.m_oBody.m_strSeatNo.empty()) 
		{
			m_pRedisClient->HSet(SEAT_DATA_INFO, oldSeatData.m_oBody.m_strSeatNo, l_strSeatInfo);
		}
	}
	return true;
}

bool CBusinessImpl::GetServiceInfo(std::string& p_strServiceName, std::string& p_strIp, std::string& p_strPort)
{
	if (m_strNacosServerIp.empty() || m_strNacosServerPort.empty()) {
		return false;
	}
	std::string strNacosServerIp = m_strNacosServerIp;
	std::string strNacosServerPort = m_strNacosServerPort;
	std::string strNacosNamespace = m_strNacosServerNamespace;
	std::string strServiceName = p_strServiceName;
	std::string strNacosGroupName = m_strNacosServerGroupName;
	std::string strNacosQueryUrl = m_pConfig->GetValue("ICC/Component/HttpServer/queryurl", "/nacos/v1/ns/instance/list");
	std::string strVcsServiceHealthyFlag = m_pConfig->GetValue("ICC/Plugin/Synthetical/servicehealthyflag", "1");

	std::string strTarget = m_pString->Format("%s?namespaceId=%s&serviceName=%s@@%s", strNacosQueryUrl.c_str(), strNacosNamespace.c_str(),
		strNacosGroupName.c_str(), strServiceName.c_str());
	std::string strContent;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;

	std::string strReceive = m_pHttpClient->GetWithTimeout(strNacosServerIp, strNacosServerPort, strTarget, mapHeaders, strContent, strErrorMessage, 2);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "not receive nacos server response.NacosServerIp=%s:%s,strTarget=%s", strNacosServerIp.c_str(), strNacosServerPort.c_str(), strTarget.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "receive nacos response : [%s]. ", strReceive.c_str());

	JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
	if (!pJson->LoadJson(strReceive))
	{
		ICC_LOG_ERROR(m_pLog, "analyze nacos response failed.[%s] ", strReceive.c_str());
		return false;
	}

	int iCount = pJson->GetCount("/hosts");
	for (int i = 0; i < iCount; i++)
	{
		std::string l_strPrefixPath("/hosts/" + std::to_string(i) + "/");
		std::string strHealthy = pJson->GetNodeValue(l_strPrefixPath + "healthy", "");
		if (strHealthy == strVcsServiceHealthyFlag)
		{
			p_strIp = pJson->GetNodeValue(l_strPrefixPath + "ip", "");
			p_strPort = pJson->GetNodeValue(l_strPrefixPath + "port", "");
			ICC_LOG_DEBUG(m_pLog, "find healthy sms send service : [%s:%s]. ", p_strIp.c_str(), p_strPort.c_str());
			return true;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "not find healthy sms send service,iCount=%d,strNacosServerIp=%s:%s!!!", iCount, strNacosServerIp.c_str(), strNacosServerPort.c_str());
	return false;
}

bool CBusinessImpl::SetSeatAlarmCountBySeatNo(const std::string strSeatNo, const std::string strAlarmCount)
{
	PROTOCOL::CSeatDataInfo SeatData;

	std::string strClientInfo;
	m_pRedisClient->HGet(SEAT_DATA_INFO, strSeatNo, strClientInfo);
	ICC_LOG_INFO(m_pLog, "SetSeatAlarmCount beform : %s-%s-%s", strSeatNo.c_str(), strAlarmCount.c_str(), strClientInfo.c_str());
	if (!SeatData.ParseString(strClientInfo, m_pJsonFac->CreateJson())) 
	{
		ICC_LOG_ERROR(m_pLog, "SetSeatAlarmCount beform :error");
		return false;
	}

	SeatData.m_oBody.m_strAlarmCount = strAlarmCount;
	SeatData.m_oBody.m_strSeatNo = strSeatNo;

	{
		PROTOCOL::CStaffDataInfo l_StaffDataInfo;
		std::string strStaffInfo;
		m_pRedisClient->HGet(STAFF_DATA_INFO, SeatData.m_oBody.m_strStaffCode, strStaffInfo);
		ICC_LOG_INFO(m_pLog, "SetStaffAlarmCount beform : %s-%s", SeatData.m_oBody.m_strStaffCode.c_str(), strStaffInfo.c_str());
		if (!l_StaffDataInfo.ParseString(strStaffInfo, m_pJsonFac->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "SetStaffAlarmCount beform :error");
			return false;
		}

		SeatData.m_oBody.m_strStaffCount = l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCount;
	}


	std::string l_strSeatInfo = SeatData.ToString(m_pJsonFac->CreateJson());
	ICC_LOG_INFO(m_pLog, "SetSeatAlarmCountBySeatNo Sync: %s-%s", strSeatNo.c_str(), l_strSeatInfo.c_str());
	if (!strSeatNo.empty()) 
	{
		m_pRedisClient->HSet(SEAT_DATA_INFO, strSeatNo, l_strSeatInfo);
	}
	return true;
}

void CBusinessImpl::OnCNotifGetSeatInfoRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	PROTOCOL::CSeatDataInfoClient SeatDataRespond;
	std::string l_strRequestMsg = p_pNotify->GetMessages();
	std::string l_strMsg = "";
	PROTOCOL::CSeatDataInfo SeatData;
	ICC_LOG_DEBUG(m_pLog, "OnCNotifGetSeatInfoRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	if (!SeatData.ParseStringClient(l_strRequestMsg, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Invalid OnCNotifGetSeatInfoRequest protocol: [%s]", l_strRequestMsg.c_str());
		SeatDataRespond.m_oHeader.m_strCode = "500";
		SeatDataRespond.m_oHeader.m_strResult = "1";
		SeatDataRespond.m_oHeader.m_strMsgId = m_pString->CreateGuid();
		SeatDataRespond.m_oHeader.m_strMsg = "SeatData ParseString failed";
		l_strMsg = SeatDataRespond.ToString(m_pJsonFac->CreateJson());
		p_pNotify->Response(l_strMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
		return;
	}
	std::map<std::string, std::string> l_mapClientRegisterInfo;
	m_pRedisClient->HGetAll(SEAT_DATA_INFO, l_mapClientRegisterInfo);
	PROTOCOL::CSeatDataInfo ClientSeatData;
	for (auto it = l_mapClientRegisterInfo.begin(); it != l_mapClientRegisterInfo.end(); it++) 
	{
		ICC_LOG_INFO(m_pLog, " seatdata start success %s-%s", it->first.c_str(), it->second.c_str());
		PROTOCOL::CSeatDataInfoClient::CBody::CList vectData;
		if (ClientSeatData.ParseString(it->second, m_pJsonFac->CreateJson()))
		{
			if (CheckDeptCodeData(ClientSeatData.m_oBody.m_strDeptCode, SeatData))
			{
				vectData.m_strSeatNo = ClientSeatData.m_oBody.m_strSeatNo;
				vectData.m_strClientName = ClientSeatData.m_oBody.m_strClientName;
				vectData.m_strClientIp = ClientSeatData.m_oBody.m_strClientIp;
				vectData.m_strStaffCode = ClientSeatData.m_oBody.m_strStaffCode;
				vectData.m_strStaffName = ClientSeatData.m_oBody.m_strStaffName;
				vectData.m_strDeptCode = ClientSeatData.m_oBody.m_strDeptCode;
				vectData.m_strDeptName = ClientSeatData.m_oBody.m_strDeptName;
				vectData.m_strLoginMode = ClientSeatData.m_oBody.m_strLoginMode;
				vectData.m_strReadyState = ClientSeatData.m_oBody.m_strReadyState;
				vectData.m_strDeviceState = ClientSeatData.m_oBody.m_strDeviceState;
				vectData.m_strAlarmCount = ClientSeatData.m_oBody.m_strAlarmCount;
				{
					if (!vectData.m_strStaffCode.empty())
					{
						PROTOCOL::CStaffDataInfo l_StaffDataInfo;
						std::string strStaffInfo;
						m_pRedisClient->HGet(STAFF_DATA_INFO, vectData.m_strStaffCode, strStaffInfo);
						ICC_LOG_INFO(m_pLog, "SetStaffAlarmCount beform : %s-%s", vectData.m_strStaffCode.c_str(), strStaffInfo.c_str());
						if (!l_StaffDataInfo.ParseString(strStaffInfo, m_pJsonFac->CreateJson()))
						{
							ICC_LOG_ERROR(m_pLog, "SetStaffAlarmCount beform :error");
						}

						vectData.m_strSatffCount = l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCount;
					}
				}
				vectData.m_strSeatType = ClientSeatData.m_oBody.m_strSeatType;
				vectData.m_strAcd = ClientSeatData.m_oBody.m_strAcd;
				vectData.m_strBeginTalkTime = ClientSeatData.m_oBody.m_strBeginTalkTime;
				vectData.m_strCallDirection = ClientSeatData.m_oBody.m_strCallDirection;
				vectData.m_strCalledId = ClientSeatData.m_oBody.m_strCalledId;
				vectData.m_strCallerId = ClientSeatData.m_oBody.m_strCallerId;
				vectData.m_strCallrefId = ClientSeatData.m_oBody.m_strCallrefId;
				vectData.m_strOriginalCallId = ClientSeatData.m_oBody.m_strOriginalCallId;
				vectData.m_strDeviceType = ClientSeatData.m_oBody.m_strDeviceType;
				vectData.m_strTime = ClientSeatData.m_oBody.m_strTime;
				SeatDataRespond.m_oBody.m_vecLists.push_back(vectData);
			}
		}
	}

	// 回复前端的消息
	SeatDataRespond.m_oHeader.m_strResult = "0";
	SeatDataRespond.m_oHeader.m_strMsg = "success";
	SeatDataRespond.m_oHeader.m_strMsgId = m_pString->CreateGuid();
	SeatDataRespond.m_oBody.m_strCount = std::to_string(SeatDataRespond.m_oBody.m_vecLists.size());
	l_strMsg = SeatDataRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotify->Response(l_strMsg);
	ICC_LOG_INFO(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

bool CBusinessImpl::CheckDeptCodeData(const std::string strDeptCode, PROTOCOL::CSeatDataInfo SeatData)
{
	if (strDeptCode.empty())
	{
		return false;
	}
	if (SeatData.m_oBody.m_strIsRecursive == "1")
	{
		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = "select_icc_t_dept_recursive_by_deptcode";
		l_SqlRequest.param["dept_code"] = SeatData.m_oBody.m_strDeptCode;
		l_SqlRequest.param["code"] = strDeptCode;

		DataBase::IResultSetPtr l_pResult = m_pIDBConn->Exec(l_SqlRequest, true);
		ICC_LOG_DEBUG(m_pLog, "select_icc_t_dept_recursive_by_deptcode sql: %s", l_pResult->GetSQL().c_str());

		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "select_icc_t_dept_recursive_by_deptcode sql error: %s", l_pResult->GetErrorMsg().c_str());
			return false;
		}

		unsigned int tmp_uiCount = 0;
		while (l_pResult->Next())
		{
			tmp_uiCount = m_pString->ToUInt(l_pResult->GetValue("count"));
			if (tmp_uiCount > 0)
			{
				return true;
			}
		}
	}
	else
	{
		if (strDeptCode == SeatData.m_oBody.m_strDeptCode)
		{
			return true;
		}
	}
	return false;
}

void CBusinessImpl::_LoadSeatDataToTimer()
{
	// PROTOCOL::CSeatDataInfoClient SeatDataRespond;
	{
		std::map<std::string, std::string> l_mapStaffDataInfo;
		PROTOCOL::CStaffDataInfo l_StaffDataInfo;
		std::string strStaffInfo;
		m_pRedisClient->HGetAll(STAFF_DATA_INFO, l_mapStaffDataInfo);
		
		for (auto it = l_mapStaffDataInfo.begin(); it != l_mapStaffDataInfo.end(); it++)
		{
			ICC_LOG_INFO(m_pLog, " staffdata start success %s-%s", it->first.c_str(), it->second.c_str());
			if (l_StaffDataInfo.ParseString(it->second, m_pJsonFac->CreateJson()))
			{
				l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCount = "0";
				strStaffInfo = l_StaffDataInfo.ToString(m_pJsonFac->CreateJson());
				if (!l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCode.empty())
				{
					m_pRedisClient->HSet(STAFF_DATA_INFO, l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCode, strStaffInfo);
				}
			}
		}
	}

	std::map<std::string, std::string> l_mapClientRegisterInfo;
	m_pRedisClient->HGetAll(SEAT_DATA_INFO, l_mapClientRegisterInfo);

	PROTOCOL::CSeatDataInfo ClientSeatData;
	std::string l_strSeatInfo;
	for (auto it = l_mapClientRegisterInfo.begin(); it != l_mapClientRegisterInfo.end(); it++) 
	{
		ICC_LOG_INFO(m_pLog, " seatdata start success %s-%s", it->first.c_str(), it->second.c_str());
		// PROTOCOL::CSeatDataInfoClient::CBody::CList vectData;
		if (ClientSeatData.ParseString(it->second, m_pJsonFac->CreateJson())) 
		{
			ClientSeatData.m_oBody.m_strAlarmCount = "0";
			ClientSeatData.m_oBody.m_strStaffCount = "0";
			l_strSeatInfo = ClientSeatData.ToString(m_pJsonFac->CreateJson());
			if (!ClientSeatData.m_oBody.m_strSeatNo.empty()) 
			{
				m_pRedisClient->HSet(SEAT_DATA_INFO, ClientSeatData.m_oBody.m_strSeatNo, l_strSeatInfo);
				SendSeatDataInfoSync(ClientSeatData);
			}
		}
	}
}

void CBusinessImpl::OnNotifiSeatDataSync(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strRequestMsg = p_pNotify->GetMessages();
	ICC_LOG_INFO(m_pLog, "Receive OnNotifiSeatDataSync Sync: %s", l_strRequestMsg.c_str());
	PROTOCOL::CClientRegisterSync l_oSyncObj;
	if (!l_oSyncObj.ParseString(l_strRequestMsg, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Invalid ClientRegisterSync protocol: [%s]", l_strRequestMsg.c_str());
		return;
	}

	PROTOCOL::CSeatDataInfo SeatData;
	std::string strSeatInfo;
	if (m_pRedisClient->HExists(SEAT_DATA_INFO, l_oSyncObj.m_oBody.m_strClientID)) 
	{
		m_pRedisClient->HGet(SEAT_DATA_INFO, l_oSyncObj.m_oBody.m_strClientID, strSeatInfo);
		SeatData.ParseString(strSeatInfo, m_pJsonFac->CreateJson());
		ICC_LOG_INFO(m_pLog, "Receive SEAT_DATA_INFO Sync: %s", strSeatInfo.c_str());
	
	}
	else 
	{
		SeatData.m_oBody.m_strAlarmCount = "0";
		SeatData.m_oBody.m_strStaffCount = "0";
		SeatData.m_oBody.m_strSeatType = "0";
	}

	if(l_oSyncObj.m_oBody.m_strClientStatus == "1")
	{
		if (!l_oSyncObj.m_oBody.m_strStaffCode.empty())
		{
			PROTOCOL::CStaffDataInfo l_StaffDataInfo;
			std::string strStaffInfo;
			m_pRedisClient->HGet(STAFF_DATA_INFO, l_oSyncObj.m_oBody.m_strStaffCode, strStaffInfo);
			ICC_LOG_INFO(m_pLog, "SetStaffAlarmCount beform : %s-%s", l_oSyncObj.m_oBody.m_strStaffCode.c_str(), strStaffInfo.c_str());
			if (!l_StaffDataInfo.ParseString(strStaffInfo, m_pJsonFac->CreateJson()))
			{
				ICC_LOG_ERROR(m_pLog, "SetStaffAlarmCount beform :error");
			}

			SeatData.m_oBody.m_strStaffCount = l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCount;
		}
	}
	
	BuildSeatDataFromClientRegister(l_oSyncObj, SeatData);
	if (SeatData.m_oBody.m_strSeatNo.empty()) 
	{
		ICC_LOG_ERROR(m_pLog, "  OnNotifiSeatDataSync seat num is empty: %s", SeatData.m_oBody.m_strSeatNo.c_str());
		return;
	}

	std::string l_strSeatInfo = SeatData.ToString(m_pJsonFac->CreateJson());
	ICC_LOG_INFO(m_pLog, "Receive SEAT_DATA_INFO Sync: %s", l_strSeatInfo.c_str());
	// 0 表示下线 1 表示上线 
	if (l_oSyncObj.m_oBody.m_strClientStatus == "0" && SeatData.m_oBody.m_strSeatType == "0") 
	{
		ICC_LOG_INFO(m_pLog, "delete  SEAT_DATA_INFO Sync: %s", l_strSeatInfo.c_str());
		m_pRedisClient->HDel(SEAT_DATA_INFO, SeatData.m_oBody.m_strSeatNo);
	}
	else 
	{
		ICC_LOG_INFO(m_pLog, "update  SEAT_DATA_INFO Sync: %s", l_strSeatInfo.c_str());

		m_pRedisClient->HSet(SEAT_DATA_INFO, SeatData.m_oBody.m_strSeatNo, l_strSeatInfo);
	}
	// 发送同步消息消息
	SendSeatDataInfoSync(SeatData);
}

void CBusinessImpl::OnNotifiAlarmCountSync(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strRequestMsg = p_pNotify->GetMessages();
	ICC_LOG_INFO(m_pLog, "Receive OnNotifiAlarmCountSync Sync: %s", l_strRequestMsg.c_str());
	PROTOCOL::CAlarmSync l_oSyncObj;
	if (!l_oSyncObj.ParseString(l_strRequestMsg, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Invalid ClientRegisterSync protocol: [%s]", l_strRequestMsg.c_str());
		return;
	}
	
	if ((l_oSyncObj.m_oBody.m_strSyncType == SEATINFO_ADD || l_oSyncObj.m_oBody.m_strSyncType == SEATINFO_DELETE) && (!l_oSyncObj.m_oBody.m_oAlarmInfo.m_strSeatNo.empty()))
	{

		PROTOCOL::CSeatDataInfo SeatData;
		std::string strClientInfo;
		PROTOCOL::CStaffDataInfo l_StaffDataInfo;
		std::string strStaffInfo;
		int alarmCount = 0;
		int staffCount = 0;
		m_pRedisClient->HGet(SEAT_DATA_INFO, l_oSyncObj.m_oBody.m_oAlarmInfo.m_strSeatNo, strClientInfo);
		if (SeatData.ParseString(strClientInfo, m_pJsonFac->CreateJson())) 
		{
			alarmCount = std::atoi(SeatData.m_oBody.m_strAlarmCount.c_str());
		}
		else 
		{
			ICC_LOG_ERROR(m_pLog, "Get redis alarm count err: %d", alarmCount);
			return;
		}

		{
			m_pRedisClient->HGet(STAFF_DATA_INFO, SeatData.m_oBody.m_strStaffCode, strStaffInfo);
			if (l_StaffDataInfo.ParseString(strStaffInfo, m_pJsonFac->CreateJson()))
			{
				staffCount = std::atoi(l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCount.c_str());
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "Get redis alarm staff count err: %d", staffCount);
				//return;
			}
		}

		if (l_oSyncObj.m_oBody.m_strSyncType == SEATINFO_ADD) 
		{
			alarmCount += 1;
			staffCount += 1;
		}
		else if (l_oSyncObj.m_oBody.m_strSyncType == SEATINFO_DELETE) 
		{
			ICC_LOG_INFO(m_pLog, "Get request alarm time: %s - %s", l_oSyncObj.m_oBody.m_oAlarmInfo.m_strReceivedTime.c_str(), GetYestertodayTime().c_str());
			DateTime::CDateTime startTime = m_pDateTime->FromString(GetYestertodayTime());
			DateTime::CDateTime strReceivedTime = m_pDateTime->FromString(l_oSyncObj.m_oBody.m_oAlarmInfo.m_strReceivedTime);
			if (strReceivedTime >= startTime && alarmCount > 0)
			{
				alarmCount -= 1;
				
			}
			if (staffCount > 0)
			{
				staffCount -= 1;
			}
		}
		else 
		{
			ICC_LOG_INFO(m_pLog, "Get request alarm type: %s", l_oSyncObj.m_oBody.m_strSyncType.c_str());
			return;
		}
		SeatData.m_oBody.m_strAlarmCount = std::to_string(alarmCount);
		std::string l_strSeatInfo = SeatData.ToString(m_pJsonFac->CreateJson());
		ICC_LOG_INFO(m_pLog, "send query alam count Sync: %s-%s", SeatData.m_oBody.m_strSeatNo.c_str(), l_strSeatInfo.c_str());
		if (!SeatData.m_oBody.m_strSeatNo.empty()) 
		{
			m_pRedisClient->HSet(SEAT_DATA_INFO, SeatData.m_oBody.m_strSeatNo, l_strSeatInfo);
			// 发送同步消息消息
			SendSeatDataInfoSync(SeatData);
		}

		{
			l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCount = std::to_string(staffCount);
			std::string l_strStaffInfo = l_StaffDataInfo.ToString(m_pJsonFac->CreateJson());

			if (!l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCode.empty())
			{
				if (!m_pRedisClient->HSet(STAFF_DATA_INFO, l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCode, l_strStaffInfo))
				{
					ICC_LOG_ERROR(m_pLog, "STAFF_DATA_INFO err: %s", l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCode.c_str());
				}
			}
		}
	}
	else
	{
		ICC_LOG_INFO(m_pLog, "Invalid ClientRegisterSync protocol: [%s]", l_oSyncObj.m_oBody.m_strSyncType.c_str());
		return;
	}
}

void CBusinessImpl::OnNotifiSeatInfoManagerSync(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strRequestMsg = p_pNotify->GetMessages();
	ICC_LOG_INFO(m_pLog, "Receive OnNotifiSeatInfoManagerSync Sync: %s", l_strRequestMsg.c_str());
	PROTOCOL::CSeatInfoManagerSync l_oSyncObj;
	if (!l_oSyncObj.ParseString(l_strRequestMsg, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Invalid OnNotifiSeatInfoManagerSync protocol: [%s]", l_strRequestMsg.c_str());
		return;
	}
	PROTOCOL::CSeatDataInfo ClientSeatData;
	std::map<std::string, std::string> l_mapClientRegisterInfo;
	std::string l_strSeatInfo;
	m_pRedisClient->HGetAll(SEAT_DATA_INFO, l_mapClientRegisterInfo);
	if (!l_oSyncObj.m_oBody.m_strSeatNo.empty())
	{
		auto it = l_mapClientRegisterInfo.find(l_oSyncObj.m_oBody.m_strSeatNo);
		if (it != l_mapClientRegisterInfo.end())
		{
			if (!ClientSeatData.ParseString(it->second, m_pJsonFac->CreateJson()))
			{
				ICC_LOG_DEBUG(m_pLog, "Invalid OnNotifiSeatInfoManagerSync protocol: [%s]", it->second.c_str());
				return;
			}
			// 1 新增
			if (l_oSyncObj.m_oBody.m_strSyncType == "1" && ClientSeatData.m_oBody.m_strDeptCode != l_oSyncObj.m_oBody.m_strDeptCode) {
				SyncSeatDataInfo(ClientSeatData, l_oSyncObj);
				return;
			}
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "no find SeatDataInfoSync [%s]", l_oSyncObj.m_oBody.m_strSeatNo.c_str());		
			if (l_oSyncObj.m_oBody.m_strSyncType == "1") 
			{
				SyncSeatDataInfo(ClientSeatData, l_oSyncObj);
				return;
			}
		}
	}
	if (ClientSeatData.m_oBody.m_strSeatNo.empty()) 
	{
		ICC_LOG_ERROR(m_pLog, " ClientSeatData seatinfo empty");
		return;
	}
	// 2 是更新
	if (l_oSyncObj.m_oBody.m_strSyncType == "2") 
	{
		if (ClientSeatData.m_oBody.m_strSeatNo == l_oSyncObj.m_oBody.m_strSeatNo && ClientSeatData.m_oBody.m_strDeptCode != l_oSyncObj.m_oBody.m_strDeptCode) 
		{
			SyncSeatDataInfo(ClientSeatData, l_oSyncObj);
			return;
		}
	}
	// 3 删除说明该坐席不是电话坐席 需要同步一次CTI的数据
	else if (l_oSyncObj.m_oBody.m_strSyncType == "3") 
	{
		m_pRedisClient->HExists(SEAT_INFO_KEY, l_oSyncObj.m_oBody.m_strSeatNo);
		if (!m_pRedisClient->HExists(SEAT_INFO_KEY, l_oSyncObj.m_oBody.m_strSeatNo) && !m_pRedisClient->HExists(CLIENT_REGISTER_INFO, l_oSyncObj.m_oBody.m_strSeatNo)
			&& m_pRedisClient->HExists(SEAT_DATA_INFO, l_oSyncObj.m_oBody.m_strSeatNo))
		{
			m_pRedisClient->HDel(SEAT_DATA_INFO, l_oSyncObj.m_oBody.m_strSeatNo);
		}
		// 同步一次CTI的数据
		if (!InitClientDevicesStateInfo()) 
		{
			ICC_LOG_ERROR(m_pLog, " init InitClientDevicesStateInfo failed !");
		}
	}
	else 
	{
		ICC_LOG_INFO(m_pLog, "over OnNotifiSeatInfoManagerSync Sync: %s", l_oSyncObj.m_oBody.m_strSyncType.c_str());
	}
}

void CBusinessImpl::SyncSeatDataInfo(PROTOCOL::CSeatDataInfo ClientSeatData, PROTOCOL::CSeatInfoManagerSync l_oSyncObj)
{
	std::string l_strSeatInfo;
	ClientSeatData.m_oBody.m_strSeatNo = l_oSyncObj.m_oBody.m_strSeatNo;
	ClientSeatData.m_oBody.m_strClientName = l_oSyncObj.m_oBody.m_strClientName;
	ClientSeatData.m_oBody.m_strDeptCode = l_oSyncObj.m_oBody.m_strDeptCode;
	ClientSeatData.m_oBody.m_strDeptName = l_oSyncObj.m_oBody.m_strDeptName;
	if (l_oSyncObj.m_oBody.m_strSyncType == "1"&& l_oSyncObj.m_oBody.m_strSeatNo != ClientSeatData.m_oBody.m_strSeatNo) 
	{
		ClientSeatData.m_oBody.m_strAlarmCount = "0";
		ClientSeatData.m_oBody.m_strSeatType = "3";
	}
	l_strSeatInfo = ClientSeatData.ToString(m_pJsonFac->CreateJson());
	if (!l_oSyncObj.m_oBody.m_strSeatNo.empty())
	{
		ICC_LOG_INFO(m_pLog, "new seatinfo Sync: %s-%s", l_oSyncObj.m_oBody.m_strSeatNo.c_str(), l_strSeatInfo.c_str());
		m_pRedisClient->HSet(SEAT_DATA_INFO, l_oSyncObj.m_oBody.m_strSeatNo, l_strSeatInfo);
		// 发送同步消息消息
		SendSeatDataInfoSync(ClientSeatData);
		return;
	}
}

void CBusinessImpl::SendSeatDataInfoSync(PROTOCOL::CSeatDataInfo SeatData)
{
	SeatData.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	SeatData.m_oHeader.m_strCmd = "seat_data_info_sync";
	SeatData.m_oHeader.m_strRequest = "topic_seat_monitor_data_sync";
	SeatData.m_oHeader.m_strRequestType = MQTYPE_TOPIC;
	SeatData.m_oHeader.m_strSystemID = SYSTEMID;
	SeatData.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	SeatData.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	std::string strDeptInfo;
	if (!m_pRedisClient->HGet(DEPT_CODE_INFO_KEY, SeatData.m_oBody.m_strDeptCode, strDeptInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Hget Dept Info Failed!!!");
	}
	PROTOCOL::CDeptInfo l_DeptInfo;
	if (!l_DeptInfo.Parse(strDeptInfo, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "Parse Dept Info Failed!!!");
	}
	else {
		SeatData.m_oBody.m_strOrgIndentifier = l_DeptInfo.m_strPucOrgIdentifier;
	}	
	std::string l_strMessage = SeatData.ToClient(m_pJsonFac->CreateJson());

	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_INFO(m_pLog, "send SendSeatDataInfoSync [%s]", l_strMessage.c_str());

}

void CBusinessImpl::OnNotifiCtiConnectStateSync(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strMessage = p_pNotify->GetMessages();
	ICC_LOG_INFO(m_pLog, "Receive OnNotifiCtiConnectStateSync l_strMessage: %s", l_strMessage.c_str());
	// 初始化 话机状态
	if (!InitClientDevicesStateInfo()) 
	{
		ICC_LOG_ERROR(m_pLog, " init InitClientDevicesStateInfo failed !");
	}
}

void CBusinessImpl::OnNotifiSeatLoginStateSync(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strMessage = p_pNotify->GetMessages();
	ICC_LOG_INFO(m_pLog, "Receive OnNotifiSeatLoginStateSync Sync: %s", l_strMessage.c_str());
	PROTOCOL::CSetInOutInfo l_oSetInOutInfo;
	if (!l_oSetInOutInfo.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Monitor Error ParseString Error:[%s]", l_strMessage.c_str());
		return;
	}
	// ICC_LOG_DEBUG(m_pLog, "receice SetInOutInfoRequest [%s]", l_strMessage.c_str());
	PROTOCOL::CSeatDataInfo SeatData;
	
	std::string strClientInfo;
	m_pRedisClient->HGet(SEAT_DATA_INFO, l_oSetInOutInfo.m_oBody.m_strClient, strClientInfo);
	ICC_LOG_DEBUG(m_pLog, "receice strClientInfo [%s]", strClientInfo.c_str());
	SeatData.ParseString(strClientInfo, m_pJsonFac->CreateJson());
	SeatData.m_oBody.m_strLoginMode = l_oSetInOutInfo.m_oBody.m_strLogMode;
	SeatData.m_oBody.m_strReadyState = l_oSetInOutInfo.m_oBody.m_strReadyState;
	SeatData.m_oBody.m_strAcd = l_oSetInOutInfo.m_oBody.m_strACD;
	if (SeatData.m_oBody.m_strLoginMode == "logout") 
	{
		SeatData.m_oBody.m_strReadyState = "busy";
	}
	std::string l_strSeatInfo = SeatData.ToString(m_pJsonFac->CreateJson());
	// ICC_LOG_INFO(m_pLog, "send OnNotifiSeatLoginStateSync Sync: %s", l_strSeatInfo.c_str());
	if (!l_oSetInOutInfo.m_oBody.m_strClient.empty()) 
	{
		m_pRedisClient->HSet(SEAT_DATA_INFO, l_oSetInOutInfo.m_oBody.m_strClient, l_strSeatInfo);
		// 发送同步消息消息
		SendSeatDataInfoSync(SeatData);
	}
	if (m_strCheckDeviceState == "1")
	{
		ICC_LOG_INFO(m_pLog, " m_strCheckDeviceState start: %s - %s ", m_strCheckDeviceState.c_str(), m_strMinFreeCount.c_str());
		CheckDeviceState(SeatData.m_oBody.m_strDeptCode);
	}

}

void CBusinessImpl::CheckDeviceState(std::string& strSeatDeptCode)
{
	std::map<std::string, std::string> l_mapClientRegisterInfo;
	m_pRedisClient->HGetAll(SEAT_DATA_INFO, l_mapClientRegisterInfo);
	int l_Count = 0;
	PROTOCOL::CSeatDataInfo ClientSeatData;
	std::string l_strSeatInfo;
	for (auto it = l_mapClientRegisterInfo.begin(); it != l_mapClientRegisterInfo.end(); it++) 
	{
		if (ClientSeatData.ParseString(it->second, m_pJsonFac->CreateJson())) {
			if (ClientSeatData.m_oBody.m_strDeptCode == strSeatDeptCode && ClientSeatData.m_oBody.m_strReadyState == "idle") {
				ICC_LOG_INFO(m_pLog, "  seat idle count [%s-%s]", strSeatDeptCode.c_str(), ClientSeatData.m_oBody.m_strSeatNo.c_str());
				l_Count++;
			}
		}
	}
	ICC_LOG_INFO(m_pLog, " current seat count [%s-%d]", strSeatDeptCode.c_str(), l_Count);
	if (l_Count < std::atoi(m_strMinFreeCount.c_str()))
	{
		//将消息推送至消息中心
		std::string l_strConfigMsg;
		std::string l_strSeatConfig;
		GetSeatParamsFromConfig(SEATIDLECOUNT, l_strConfigMsg);
		if (!l_strConfigMsg.empty()) {
			l_strConfigMsg = m_pString->ReplaceFirst(l_strConfigMsg, "$", std::to_string(l_Count));
			l_strConfigMsg = m_pString->ReplaceFirst(l_strConfigMsg, "$", m_strMinFreeCount);
			ICC_LOG_INFO(m_pLog, " send seat content [%s]", l_strConfigMsg.c_str());
			m_pMsgCenter->SendMsgToCenter(l_strConfigMsg, "monitorMessage", SEATIDLECOUNT, strSeatDeptCode, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);
		}
	}
}

void CBusinessImpl::GetSeatParamsFromConfig(const std::string strParamsKey, std::string& strParamsValue)
{
	std::string strParamInfo;
	if (!m_pRedisClient->HGet(PARAMINFO, strParamsKey, strParamInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Hget ParamInfo Failed!!!");
		return;
	}
	PROTOCOL::CParamInfo l_ParamInfo;
	if (!l_ParamInfo.Parse(strParamInfo, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "Json Parse ParamInfo Failed!!!");
		return;
	}
	strParamsValue = l_ParamInfo.m_strValue;
}

void CBusinessImpl::OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Server nacos params Sync: %s", l_strRequestMsg.c_str());

	PROTOCOL::CSyncNacosParams syn;
	if (!syn.ParseString(l_strRequestMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "l_strRequestMsg parse json failed");
		return;
	}

	SetNacosParams(syn.m_oBody.m_strNacosServerIp, syn.m_oBody.m_strNacosServerPort, syn.m_oBody.m_strNacosNamespace, syn.m_oBody.m_strNacosGroupName);
}

void CBusinessImpl::SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	boost::lock_guard<boost::mutex> lock(m_mutexNacosParams);
	m_strNacosServerIp = strNacosIp;
	m_strNacosServerPort = strNacosPort;
	m_strNacosServerNamespace = strNameSpace;
	m_strNacosServerGroupName = strGroupName;
}

void CBusinessImpl::OnNotifiSeatDeviceStateSync(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strMessage = p_pNotify->GetMessages();
	ICC_LOG_INFO(m_pLog, "Receive OnNotifiSeatDeviceStateSync Sync: %s", l_strMessage.c_str());
	PROTOCOL::CDeviceStateEvent l_oSetInOutInfo;
	if (!l_oSetInOutInfo.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Monitor Error ParseString Error:[%s]", l_strMessage.c_str());
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "receice SetInOutInfoRequest [%s]", l_strMessage.c_str());
	PROTOCOL::CSeatDataInfo SeatData;
	if (l_oSetInOutInfo.m_oBody.m_strDevice.empty())
	{
		ICC_LOG_ERROR(m_pLog, "receice m_strDevice is empty [%s]", l_strMessage.c_str());
		return;
	}
	std::string strClientInfo;
	std::string l_strCallInfo;
	m_pRedisClient->HGet(SEAT_DATA_INFO, l_oSetInOutInfo.m_oBody.m_strDevice, strClientInfo);
	SeatData.ParseString(strClientInfo, m_pJsonFac->CreateJson());
	if (SeatData.m_oBody.m_strDeviceState != l_oSetInOutInfo.m_oBody.m_strState) {
		// 如果状态是挂断状态， 查看该 话务是否还存在，存在则需要取 Redis的状态
		if (l_oSetInOutInfo.m_oBody.m_strState == CTI_HANGUPSTATE)
		{
			PROTOCOL::CCallInfo l_oCallInfo;
			m_pRedisClient->HGet(CALL_INFO, l_oSetInOutInfo.m_oBody.m_strCallRefId, l_strCallInfo);
			if (!l_strCallInfo.empty() && l_oCallInfo.ParseString(l_strCallInfo, m_pJsonFac->CreateJson())) {
				l_oSetInOutInfo.m_oBody.m_strState = l_oCallInfo.m_strState;
			} 
		}
		SeatData.m_oBody.m_strDeviceState = l_oSetInOutInfo.m_oBody.m_strState;
		SeatData.m_oBody.m_strAcd = l_oSetInOutInfo.m_oBody.m_strACD;
		SeatData.m_oBody.m_strDeviceType = l_oSetInOutInfo.m_oBody.m_strDeviceType;
		SeatData.m_oBody.m_strCallDirection = l_oSetInOutInfo.m_oBody.m_strCallDirection;
		SeatData.m_oBody.m_strCallrefId = l_oSetInOutInfo.m_oBody.m_strCallRefId;
		SeatData.m_oBody.m_strCallerId = l_oSetInOutInfo.m_oBody.m_strCallerId;
		SeatData.m_oBody.m_strCalledId = l_oSetInOutInfo.m_oBody.m_strCalledId;
		SeatData.m_oBody.m_strOriginalCallId = l_oSetInOutInfo.m_oBody.m_strOriginalCalledId;
		SeatData.m_oBody.m_strTime = l_oSetInOutInfo.m_oBody.m_strTime;
		SeatData.m_oBody.m_strBeginTalkTime = l_oSetInOutInfo.m_oBody.m_strBeginTalkTime;
		// 话机变为空闲的时刻需要清理之前的数据
		if (l_oSetInOutInfo.m_oBody.m_strState == CTI_FREESTATE)
		{
			SeatData.m_oBody.m_strAcd = "";
			SeatData.m_oBody.m_strCallDirection = "";
			SeatData.m_oBody.m_strCallrefId = "";
			SeatData.m_oBody.m_strCallerId = "";
			SeatData.m_oBody.m_strCalledId = "";
			SeatData.m_oBody.m_strOriginalCallId = "";
			SeatData.m_oBody.m_strBeginTalkTime = "";
		}
	}
	else {
		ICC_LOG_INFO(m_pLog, "Device State Same [%s]", SeatData.m_oBody.m_strDeviceState.c_str());
		return;
	}
	std::string l_strSeatInfo = SeatData.ToString(m_pJsonFac->CreateJson());
	ICC_LOG_INFO(m_pLog, "send OnNotifiSeatDeviceStateSync Sync: %s", l_strSeatInfo.c_str());
	m_pRedisClient->HSet(SEAT_DATA_INFO, l_oSetInOutInfo.m_oBody.m_strDevice, l_strSeatInfo);
	// 发送同步消息消息
	SendSeatDataInfoSync(SeatData);
}

void CBusinessImpl::BuildSeatDataFromClientRegister(PROTOCOL::CClientRegisterSync l_oSyncObj, ICC::PROTOCOL::CSeatDataInfo& SeatData)
{
	SeatData.m_oBody.m_strSeatNo = l_oSyncObj.m_oBody.m_strClientID;
	SeatData.m_oBody.m_strClientName = l_oSyncObj.m_oBody.m_strClientName;
	SeatData.m_oBody.m_strClientIp = l_oSyncObj.m_oBody.m_strClientIP;
	SeatData.m_oBody.m_strStaffCode = l_oSyncObj.m_oBody.m_strStaffCode;
	if ((l_oSyncObj.m_oBody.m_strClientStatus == "0") && (!SeatData.m_oBody.m_strDeptCode.empty()) && (SeatData.m_oBody.m_strDeptCode != l_oSyncObj.m_oBody.m_strDeptCode)) 
	{
		ICC_LOG_INFO(m_pLog, "logout dept code no Sync: %s-%s", SeatData.m_oBody.m_strDeptCode.c_str(), l_oSyncObj.m_oBody.m_strDeptCode.c_str());
	}
	else 
	{
		SeatData.m_oBody.m_strDeptCode = l_oSyncObj.m_oBody.m_strDeptCode;
	}
	SeatData.m_oBody.m_strDeptName = l_oSyncObj.m_oBody.m_strDeptName;
	SeatData.m_oBody.m_strStaffName = l_oSyncObj.m_oBody.m_strStaffName;
	// 0 表示下线 1 表示上线 
	if (l_oSyncObj.m_oBody.m_strClientStatus == "0") 
	{
		SeatData.m_oBody.m_strStaffName = "";
		SeatData.m_oBody.m_strStaffCount = "0";
		SeatData.m_oBody.m_strStaffCode = "";
	}
	/*
	if (l_oSyncObj.m_oBody.m_strClientStatus == "0") {
		if (SeatData.m_oBody.m_strSeatType == "0") {
			SeatData.m_oBody.m_strLoginMode = "logout";
		}
		SeatData.m_oBody.m_strStaffName = "";
	}
	else if (l_oSyncObj.m_oBody.m_strClientStatus == "1") {
		SeatData.m_oBody.m_strLoginMode = "login";		
	}
	*/
}

void CBusinessImpl::BuildAlarmCountByConfig()
{
	std::vector<ICC::PROTOCOL::CSeatDataInfo> tmp_vecSeatAlarmCount;
	if (!_QueryTodayAlarmCount(tmp_vecSeatAlarmCount))
	{
		ICC_LOG_ERROR(m_pLog, "_QueryAll Seat TodayAlarmCount failed!!!");
		return ;
	}


	{
		std::vector<ICC::PROTOCOL::CStaffDataInfo::CStaffInfo> tmp_vecStaffCount;
		if (_QueryStaffTodayAlarmCount(tmp_vecStaffCount))
		{
			ICC_LOG_ERROR(m_pLog, "_Query Staff Today AlarmCount failed!!!");
		}

		for (size_t i = 0; i < tmp_vecStaffCount.size(); ++i)
		{
			{
				PROTOCOL::CStaffDataInfo l_StaffDataInfo;
				l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCode = tmp_vecStaffCount[i].m_strStaffCode;
				l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffName = tmp_vecStaffCount[i].m_strStaffName;
				l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCount = "0";
				if (!tmp_vecStaffCount[i].m_strStaffCount.empty())
				{
					l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCount = tmp_vecStaffCount[i].m_strStaffCount;
				}

				std::string l_strStaffInfo = l_StaffDataInfo.ToString(m_pJsonFac->CreateJson());
				ICC_LOG_INFO(m_pLog, "SetStaffAlarmCountByStaff Sync: %s-%s", l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCode.c_str(), l_strStaffInfo.c_str());
				if (!l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCode.empty())
				{
					m_pRedisClient->HSet(STAFF_DATA_INFO, l_StaffDataInfo.m_oBody.m_StaffInfo.m_strStaffCode, l_strStaffInfo);
				}
			}
		}
	}

	int iCount = tmp_vecSeatAlarmCount.size();
	ICC_LOG_INFO(m_pLog, "sync Seat TodayAlarmCount !!!");
	for (int i = 0; i < iCount; ++i)
	{
		if (m_pRedisClient->HExists(SEAT_DATA_INFO, tmp_vecSeatAlarmCount[i].m_oBody.m_strSeatNo)) 
		{
			SetSeatAlarmCountBySeatNo(tmp_vecSeatAlarmCount[i].m_oBody.m_strSeatNo, tmp_vecSeatAlarmCount[i].m_oBody.m_strAlarmCount);
		}
	}
}

bool CBusinessImpl::_QueryTodayAlarmCount(std::vector<ICC::PROTOCOL::CSeatDataInfo>& tmp_vecSeatAlarmCount)
{
	DataBase::SQLRequest l_SqlRequest;

	l_SqlRequest.sql_id = "statistics_icc_t_jjdb_count_groupby_seatno";

	std::string p_strEndTime = m_pDateTime->CurrentDateTimeStr();
	std::string p_startTime = GetYestertodayTime();
	std::string p_strTomorrow = m_pDateTime->GetAfterTime(p_startTime);
	l_SqlRequest.param["groupby"] = "jjxwh";
	l_SqlRequest.param["begin_time"] = p_startTime;
	l_SqlRequest.param["end_time"] = p_strTomorrow;

	DataBase::IResultSetPtr l_pResult = m_pIDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "select_icc_t_jjdb_count_groupby_seatno sql: %s", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "_QueryTodayAlarmCount exec sql error: %s", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	

	unsigned int tmp_uiCount = 0;
	while (l_pResult->Next())
	{
		PROTOCOL::CSeatDataInfo data;
		data.m_oBody.m_strSeatNo = l_pResult->GetValue("receipt_seatno");
		data.m_oBody.m_strAlarmCount = l_pResult->GetValue("num");
		ICC_LOG_INFO(m_pLog, "_QueryTodayAlarmCount sql : %s-%s", data.m_oBody.m_strSeatNo.c_str(), data.m_oBody.m_strAlarmCount.c_str());
		tmp_vecSeatAlarmCount.push_back(data);
	}
	return true;
}

bool CBusinessImpl::_QueryStaffTodayAlarmCount(std::vector<ICC::PROTOCOL::CStaffDataInfo::CStaffInfo>& tmp_vecStaffCount)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "statistics_icc_t_jjdb_count_groupby_staff";
	std::string p_strEndTime = m_pDateTime->CurrentDateTimeStr();
	std::string p_startTime = GetYestertodayTime();
	std::string p_strTomorrow = m_pDateTime->GetAfterTime(p_startTime);

	l_SqlRequest.param["begin_time"] = p_startTime;
	l_SqlRequest.param["end_time"] = p_strTomorrow;

	DataBase::IResultSetPtr l_pResult = m_pIDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql: %s", l_pResult->GetSQL().c_str());

	ICC_LOG_DEBUG(m_pLog, "statistics_icc_t_jjdb_count_groupby_staff sql: %s", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "_QueryTodayAlarmCount exec sql error: %s", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	while (l_pResult->Next())
	{
		PROTOCOL::CStaffDataInfo::CStaffInfo data;
		data.m_strStaffCode = l_pResult->GetValue("staff_code");
		data.m_strStaffName = l_pResult->GetValue("staff_name");
		data.m_strStaffCount = l_pResult->GetValue("num");

		if (m_strCodeMode == "1")
		{
			std::string strStaffInfo;
			if (!m_pRedisClient->HGet("StaffIdInfo", data.m_strStaffCode, strStaffInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "Hget StaffIdInfo failed,staff_id_no:[%s]", data.m_strStaffCode.c_str());
		
			}
			Data::CStaffInfo l_oStaffInfo;
			if (!l_oStaffInfo.Parse(strStaffInfo, m_pJsonFac->CreateJson()))
			{
				ICC_LOG_DEBUG(m_pLog, "parse staff info failed!!!");
			}

			if (!l_oStaffInfo.m_strCode.empty())
			{
				data.m_strStaffCode = l_oStaffInfo.m_strCode;
			}

		}

		tmp_vecStaffCount.push_back(data);
	}

	return true;
}


std::string CBusinessImpl::GetYestertodayTime()
{
	std::string p_strEndTime = m_pDateTime->CurrentDateTimeStr();
	std::string yestertodaytime = m_pDateTime->GetFrontTime(p_strEndTime);
	std::string temptime = yestertodaytime.substr(0, 11);
	std::string strStartTime = temptime + "16:00:00";
	DateTime::CDateTime startTime = m_pDateTime->FromString(strStartTime);
	DateTime::CDateTime yestertodaydate = m_pDateTime->FromString(yestertodaytime);
	if (yestertodaydate > startTime) {
		temptime = p_strEndTime.substr(0, 11);
		strStartTime = temptime + "16:00:00";
	}
	return strStartTime;
}

void CBusinessImpl::OnTimer(std::string uMsgId)
{
	if (uMsgId == SEAT_SYNC_MSGID) 
	{
		_LoadSeatDataToTimer();
	}
	else if (uMsgId == TOMORROW_MSGID) 
	{
		_LoadSeatDataToTimer();
		m_pTimer.RemoveTimer(TOMORROW_MSGID);
	}

	else if (uMsgId == INIT_SEAT_DEVICE_STATE) 
	{
		if (InitClientDevicesStateInfo()) 
		{
			m_pTimer.RemoveTimer(INIT_SEAT_DEVICE_STATE);
		}
	} else if (uMsgId == COUNT_ALARM_CONFIG) 
	{
		BuildAlarmCountByConfig();
	}
	else 
	{

	}
}
