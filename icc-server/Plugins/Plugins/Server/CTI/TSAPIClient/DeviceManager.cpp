#include "Boost.h"

#include "DeviceManager.h"
#include "SysConfig.h"
#include "TaskManager.h"
#include <Protocol/CSeatInfo.h>

#define SLEEP_TIMESPAN				50			//	单位：毫秒
#define MONITOR_TIMESPAN			6			//	重新监视设备时间间隔，单位：秒
#define MAX_MONITOR_COUNT			10			//	最大监视次数

//////////////////////////////////////////////////////////////////////
int CDeviceManager::m_nMonitorCount = 0;
boost::shared_ptr<CDeviceManager> CDeviceManager::m_pInstance = nullptr;
boost::shared_ptr<CDeviceManager> CDeviceManager::Instance()
{
	if(m_pInstance == nullptr)
		m_pInstance = boost::make_shared<CDeviceManager>();

	return m_pInstance;
}
void CDeviceManager::ExitInstance()
{
	DeleteAllDevice();

	if (m_pInstance)
	{
		m_pInstance.reset();
	}
}

CDeviceManager::CDeviceManager()
{
	m_pLog = nullptr;
	m_pDateTimePtr = nullptr;
	m_pMonitorDeviceThread = nullptr;
	m_pQueryDeviceInfoThread = nullptr;

	m_bDeviceIsLoad = false;
	m_bMonitorThreadAlive = false;
	m_bQueryDeviceInfoThreadAlive = false;

	m_oLastHeartBeatTime = 0;
	m_oLastMonitorTime = 0;

	m_bSwitchConnectFlag = false;
}
CDeviceManager::~CDeviceManager()
{
	//
}

void CDeviceManager::OnInit(IResourceManagerPtr p_pResourceManager)
{
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_pResourceManager)->GetLogger(MODULE_NAME);
	m_pDateTimePtr = ICCGetResourceEx(DateTime::IDateTimeFactory, ICCIDateTimeFactoryResourceName, p_pResourceManager)->CreateDateTime();


	m_pRedisClient = ICCGetResourceEx(Redis::IRedisClientFactory, ICCIRedisClientFactoryResourceName, p_pResourceManager)->CreateRedisClient();
	m_pJsonFty = ICCGetResourceEx(JsonParser::IJsonFactory, ICCIJsonFactoryResourceName, p_pResourceManager);
	//装载所有设备
	LoadAllDevice();
}
void CDeviceManager::OnStart()
{
	m_bMonitorThreadAlive = false;
	m_bQueryDeviceInfoThreadAlive = false;

	m_oLastHeartBeatTime = m_pDateTimePtr->CurrentDateTime();
	m_oLastMonitorTime = m_pDateTimePtr->CurrentDateTime();
}

void CDeviceManager::OnStop()
{
	ResetMonitorAllDevice();
	
	if (m_bMonitorThreadAlive)
	{
		m_bMonitorThreadAlive = false;
		if (m_pMonitorDeviceThread)
		{
			m_pMonitorDeviceThread->join();
		}
	}

	if (m_bQueryDeviceInfoThreadAlive)
	{
		m_bQueryDeviceInfoThreadAlive = false;
		if (m_pQueryDeviceInfoThread)
		{
			m_pQueryDeviceInfoThread->join();
		}
	}
}

void CDeviceManager::SetSwitchConnect(bool p_bConnectFlag)
{
	m_bSwitchConnectFlag = p_bConnectFlag;
	if (!p_bConnectFlag)
	{
		ResetMonitorAllDevice();
	}
	else
	{
		if (!m_bMonitorThreadAlive)
		{
			//启动定时监听设备线程
			m_nMonitorCount = 0;
			m_bMonitorThreadAlive = true;
			m_oLastMonitorTime = m_pDateTimePtr->CurrentDateTime();
			m_pMonitorDeviceThread = boost::make_shared<boost::thread>(boost::bind(&CDeviceManager::DoMonitorDevice, this));
		}

		if (!m_bQueryDeviceInfoThreadAlive)
		{
			//启动定时查询设备信息线程
			m_bQueryDeviceInfoThreadAlive = true;
			m_oLastHeartBeatTime = m_pDateTimePtr->CurrentDateTime();
			m_pQueryDeviceInfoThread = boost::make_shared<boost::thread>(boost::bind(&CDeviceManager::DoQueryDeviceInfo, this));
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CDeviceManager::ResetMonitorDevice(const std::string& p_strDeviceNum)
{
	if (!m_bMonitorThreadAlive)
	{
		//重新启动定时监听设备线程
		m_nMonitorCount = 0;
		m_bMonitorThreadAlive = true;
		m_oLastMonitorTime = m_pDateTimePtr->CurrentDateTime();
		m_pMonitorDeviceThread = boost::make_shared<boost::thread>(boost::bind(&CDeviceManager::DoMonitorDevice, this));
	}

	{
		SAFE_LOCK(m_deviceQueueMutex);
		for (auto l_deviceObj : m_deviceQueue)
		{
			boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
			if (l_pDevice && l_pDevice->GetDeviceNum().compare(p_strDeviceNum) == 0)
			{
				l_pDevice->SetIsMonistor(false);

				break;
			}
		}
	}
	
}

void CDeviceManager::ResetMonitorTime()
{
	m_oLastMonitorTime = m_pDateTimePtr->CurrentDateTime();
}
bool CDeviceManager::MonitorIsTimeout()
{
	int l_nTimeSpan = CSysConfig::Instance()->GetMonitorTimeSpan();
	DateTime::CDateTime l_oCurrentTime = m_pDateTimePtr->CurrentDateTime();
	DateTime::CDateTime l_oEndTime = m_pDateTimePtr->AddSeconds(m_oLastMonitorTime, l_nTimeSpan/*MONITOR_TIMESPAN*/);

	if (l_oCurrentTime > l_oEndTime)
	{
		return true;
	}

	return false;
}

bool CDeviceManager::MonitorAllDevice()
{
	bool l_bIsMonitorAllDevice = true;

	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
		if (l_pDevice && !l_pDevice->GetIsMonistor())
		{
			l_bIsMonitorAllDevice = false;
			std::string l_strDeviceNum = l_pDevice->GetDeviceNum();

			IMonitorDeviceNotifPtr l_pRequestNotif = boost::make_shared<CMonitorDeviceNotif>();
			l_pRequestNotif->SetDeviceNum(l_strDeviceNum);

			long l_lTaskId = DEFAULT_TASKID;
			if (l_pDevice->GetDeviceType() == CTC_TYPE_ACDGROUP)
			{
				l_lTaskId = CTaskManager::Instance()->AddCmdTask(Task_MonitorCallVirDevice, l_pRequestNotif);
			}
			else
			{
				l_lTaskId = CTaskManager::Instance()->AddCmdTask(Task_MonitorDevice, l_pRequestNotif);
			}
			ICC_LOG_DEBUG(m_pLog, "AddCmdTask MonitorDevice, TaskId: [%u] Device: [%s]",
				l_lTaskId, l_strDeviceNum.c_str());
		}
	}

	return l_bIsMonitorAllDevice;
}
void CDeviceManager::ResetMonitorAllDevice()
{
	m_nMonitorCount = 0;

	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
		if (l_pDevice)
		{
			l_pDevice->SetIsMonistor(false);
		}
	}

	ICC_LOG_DEBUG(m_pLog, "Reset Monitor All Devices !!!");
}

void CDeviceManager::DoMonitorDevice()
{
	ICC_LOG_DEBUG(m_pLog, "=============== MonistorDevice Thread Start!! ===============");

	bool l_bExitMonitor = false;
	int l_nMaxCount = CSysConfig::Instance()->GetMaxMonitorCount();

	while (CDeviceManager::Instance()->m_bMonitorThreadAlive)
	{
		if (!CDeviceManager::Instance()->m_bSwitchConnectFlag)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN));
			continue;
		}

		if (m_nMonitorCount == 0 || CDeviceManager::Instance()->MonitorIsTimeout())
		{
			if (!CDeviceManager::Instance()->MonitorAllDevice())
			{
				m_nMonitorCount++;
			}
			else
			{
				l_bExitMonitor = true;
				//m_nMonitorCount += MAX_MONITOR_COUNT;
			}

			CDeviceManager::Instance()->ResetMonitorTime();
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN));
			//boost::this_thread::sleep_for(boost::chrono::milliseconds(SLEEP_TIMESPAN));
		}

		if (l_bExitMonitor || m_nMonitorCount > l_nMaxCount/*m_nMonitorCount > MAX_MONITOR_COUNT*/)
		{
			CDeviceManager::Instance()->m_bMonitorThreadAlive = false;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "------------------ MonistorDevice Thread Exit!! ------------------");
}

//////////////////////////////////////////////////////////////////////////

void CDeviceManager::ResetHeartBeatTime()
{
	m_oLastHeartBeatTime = m_pDateTimePtr->CurrentDateTime();
}
bool CDeviceManager::HeartBeatIsTimeout()
{
	int l_nHeartBeatTimeout = CSysConfig::Instance()->GetHeartbeatTime();
	DateTime::CDateTime l_oCurrentTime = m_pDateTimePtr->CurrentDateTime();
	DateTime::CDateTime l_oEndTime = m_pDateTimePtr->AddSeconds(m_oLastHeartBeatTime, l_nHeartBeatTimeout);

	if (l_oCurrentTime > l_oEndTime)
	{
		return true;
	}

	return false;
}
void CDeviceManager::QueryDeviceInfo()
{
	SAFE_LOCK(m_deviceQueueMutex);

	ICC_LOG_DEBUG(m_pLog, "QueryDeviceInfo enter, device size: [%u] ", m_deviceQueue.size());

	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
		if (l_pDevice)
		{
			std::string l_strDeviceNum = l_pDevice->GetDeviceNum();
			IQueryDeviceInfoNotifPtr l_pRequestNotif = boost::make_shared<CQueryDeviceInfoNotif>();
			l_pRequestNotif->SetDeviceNum(l_strDeviceNum);

			long l_lTaskId = CTaskManager::Instance()->AddCmdTask(Task_QueryDeviceInfo, l_pRequestNotif);
			ICC_LOG_DEBUG(m_pLog, "AddCmdTask QueryDeviceInfo, TaskId: [%u] DeviceNum: [%s]",
				l_lTaskId, l_strDeviceNum.c_str());

			break;
		}
	}
}


void CDeviceManager::DoQueryDeviceInfo()
{
	ICC_LOG_DEBUG(m_pLog, "=============== QueryDeviceInfo Thread Start!! ===============");

	while (CDeviceManager::Instance()->m_bQueryDeviceInfoThreadAlive)
	{
		if (CDeviceManager::Instance()->m_bSwitchConnectFlag)
		{
			if (CDeviceManager::Instance()->HeartBeatIsTimeout())
			{
				CDeviceManager::Instance()->QueryDeviceInfo();
				CDeviceManager::Instance()->ResetHeartBeatTime();
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN));
				//boost::this_thread::sleep_for(boost::chrono::milliseconds(SLEEP_TIMESPAN));
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN));
		}
	}

	ICC_LOG_DEBUG(m_pLog, "------------------ QueryDeviceInfo Thread Exit!! ------------------");
}
//////////////////////////////////////////////////////////////////////////

void CDeviceManager::DeleteAllDevice()
{
	SAFE_LOCK(m_deviceQueueMutex);
	m_deviceQueue.clear();

	m_bDeviceIsLoad = false;
}

void CDeviceManager::LoadACDDevice()
{
	std::list<CACDGroup> l_ACDGroupList = CSysConfig::Instance()->m_ACDGroupList;

	for (auto l_acdGrpObj : l_ACDGroupList)
	{
		std::string l_strACDNum = l_acdGrpObj.m_strACDNum;
		std::string l_strACDAlias = l_acdGrpObj.m_strACDAlias;

		if (!l_strACDNum.empty())
		{
			boost::shared_ptr<CDevice> pDevice = boost::make_shared<CDevice>(l_strACDNum, CTC_TYPE_ACDGROUP, "", "");
			m_deviceQueue.push_back(pDevice);
			ICC_LOG_DEBUG(m_pLog, "LoadACDDevice ACDNum: [%s]", l_strACDNum.c_str());

			if (!l_strACDAlias.empty() && l_strACDNum.compare(l_strACDAlias) != 0)
			{
				boost::shared_ptr<CDevice> pAliasDevice = boost::make_shared<CDevice>(l_strACDAlias, CTC_TYPE_ACDGROUP, "", "");
				m_deviceQueue.push_back(pAliasDevice);
				ICC_LOG_DEBUG(m_pLog, "LoadACDDevice ACDAliasNum: [%s]", l_strACDAlias.c_str());
			}
		}
	}
}


void CDeviceManager::LoadPhoneDevice()
{
	m_pRedisClient->Del("AgentPhoneState");
	std::list<CDevicePhone>	l_phoneList = CSysConfig::Instance()->m_DevicePhoneList;
	for (auto l_phoneObj : l_phoneList)
	{
		std::string l_strDeviceNum = l_phoneObj.m_strDeviceNum;
		std::string l_strDevicetype = l_phoneObj.m_strDeviceType;
		std::string l_strAgent = l_phoneObj.m_strAgent;
		std::string l_strPsw = l_phoneObj.m_strPsw;

		int l_iType = CTC_TYPE_UNKNOW;
		if (l_strDevicetype.compare(DEVICE_TYPE_DIGITAL) == 0)
			l_iType = CTC_TYPE_DIGITAL;
		else if (l_strDevicetype.compare(DEVICE_TYPE_IPPHONE) == 0)
			l_iType = CTC_TYPE_IPPHONE;
		else if (l_strDevicetype.compare(DEVICE_TYPE_ANALOG) == 0)
			l_iType = CTC_TYPE_ANALOG;
		else if (l_strDevicetype.compare(DEVICE_TYPE_VIRDN) == 0)
			l_iType = CTC_TYPE_VIRDN;
		else
			l_iType = CTC_TYPE_UNKNOW;

		boost::shared_ptr<CDevice> l_pDevice = boost::make_shared<CDevice>(l_strDeviceNum, l_iType, l_strAgent, l_strPsw);
		l_pDevice->SetIsAgent(true);


		CAgentPhoneState l_CAgentPhoneState;
		l_CAgentPhoneState.m_strPhoneNum = l_strDeviceNum;

		std::string l_strSeatInfo;
		if (m_pRedisClient->HGet("SeatInfo", l_strDeviceNum, l_strSeatInfo))
		{
			PROTOCOL::CSeatInfo cSeatInfo;
			cSeatInfo.Parse(l_strSeatInfo, m_pJsonFty->CreateJson());
			if (!cSeatInfo.m_oBody.m_strDeptCode.empty() && cSeatInfo.m_oBody.m_strDeptCode.size()>= 6)
			{
				// 资阳现场部门编码前6位是行政编码，默认取编码前6位。
				l_CAgentPhoneState.m_strZZJGDM = cSeatInfo.m_oBody.m_strDeptCode;
				l_CAgentPhoneState.m_strDistrictCode = cSeatInfo.m_oBody.m_strDeptCode.substr(0, 6);
				l_CAgentPhoneState.m_strSID = l_CAgentPhoneState.m_strDistrictCode + l_strDeviceNum;
			}
			l_CAgentPhoneState.m_strBMMC = cSeatInfo.m_oBody.m_strDeptName;
			l_CAgentPhoneState.m_strBMDM = cSeatInfo.m_oBody.m_strDeptCode;
		}
		std::string l_strVal = l_CAgentPhoneState.ToString(m_pJsonFty->CreateJson());
		if (!m_pRedisClient->HSet("AgentPhoneState", l_strDeviceNum, l_strVal))
		{

		}
		m_deviceQueue.push_back(l_pDevice);
		ICC_LOG_DEBUG(m_pLog, "LoadPhoneDevice DeviceNum: [%s], DeviceType: [%s]", 
			l_strDeviceNum.c_str(), l_strDevicetype.c_str());
	}
}


void CDeviceManager::LoadVirDn()
{
	std::list<CDeviceVir>	l_virList = CSysConfig::Instance()->m_DeviceVirList;
	for (auto l_deviceObj : l_virList)
	{
		std::string l_strDeviceNum = l_deviceObj.m_strDeviceNum;
		std::string l_strDevicetype = l_deviceObj.m_strDeviceType;

		int l_iType = CTC_TYPE_UNKNOW;
		if (l_strDevicetype.compare(DEVICE_TYPE_DIGITAL) == 0)
			l_iType = CTC_TYPE_DIGITAL;
		else if (l_strDevicetype.compare(DEVICE_TYPE_IPPHONE) == 0)
			l_iType = CTC_TYPE_IPPHONE;
		else if (l_strDevicetype.compare(DEVICE_TYPE_ANALOG) == 0)
			l_iType = CTC_TYPE_ANALOG;
		else if (l_strDevicetype.compare(DEVICE_TYPE_VIRDN) == 0)
			l_iType = CTC_TYPE_VIRDN;
		else
			l_iType = CTC_TYPE_UNKNOW;

		boost::shared_ptr<CDevice> pDevice = boost::make_shared<CDevice>(l_strDeviceNum, l_iType, "", "");
		m_deviceQueue.push_back(pDevice);

		ICC_LOG_DEBUG(m_pLog, "LoadPhoneDevice DeviceNum: [%s], DeviceType: [%s]",
			l_strDeviceNum.c_str(), l_strDevicetype.c_str());
	}
}
void CDeviceManager::LoadAllDevice()
{
	if(!m_bDeviceIsLoad)
	{
		m_bDeviceIsLoad = true;

		//装载虚拟号码
		LoadVirDn();

		//装载ACD
		LoadACDDevice();
		//装载Phone
		LoadPhoneDevice();		
	}
}

bool CDeviceManager::DeviceIsFree(const std::string& p_strDeviceNum)
{
	bool l_bRet = false;

	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> pDevice = l_deviceObj;
		if (pDevice && pDevice->GetDeviceNum().compare(p_strDeviceNum) == 0)
		{
			if (pDevice->GetDeviceState().compare(CallStateString[STATE_FREE]) == 0)
			{
				l_bRet = true;

				break;
			}
		}
	}

	return l_bRet;
}
std::string CDeviceManager::GetDeviceType(const std::string& p_strDeviceNum)
{
	std::string l_strDeviceType = DEVICE_TYPE_UNKNOW;

	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
		if (l_pDevice)
		{
			if (l_pDevice->GetDeviceNum().compare(p_strDeviceNum) == 0)
			{
				l_strDeviceType = l_pDevice->GetDeviceTypeString();

				break;
			}
		}
	}

	return l_strDeviceType;
}
boost::shared_ptr<CDevice> CDeviceManager::FindDeviceByDn(const std::string& p_strDeviceNum)
{	
	boost::shared_ptr<CDevice> l_pDevice = nullptr;

	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		if (l_deviceObj && l_deviceObj->GetDeviceNum().compare(p_strDeviceNum) == 0)
		{
			l_pDevice = l_deviceObj;

			break;
		}
	}
	
	return l_pDevice;
}

boost::shared_ptr<CDevice> CDeviceManager::FindDeviceByMonitorId(long p_lMonitorId)
{
	boost::shared_ptr<CDevice> l_pDevice = nullptr;

	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		if (l_deviceObj && l_deviceObj->GetMonitorId() == p_lMonitorId)
		{
			l_pDevice = l_deviceObj;

			break;
		}
	}

	return l_pDevice;
}

boost::shared_ptr<CDevice> CDeviceManager::FindACDDevice(const std::string& p_strDevice)
{
	boost::shared_ptr<CDevice> l_pDevice = nullptr;

	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		if (l_deviceObj && l_deviceObj->GetDeviceNum().compare(p_strDevice) == 0)
		{
			l_pDevice = l_deviceObj;

			break;
		}
	}

	return l_pDevice;
}

IGetDeviceListResultNotifPtr CDeviceManager::GetDeviceList(long p_lRequestId)
{
	IGetDeviceListResultNotifPtr l_pResultNotif = boost::make_shared<CGetDeviceListResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);

		SAFE_LOCK(m_deviceQueueMutex);
		for (auto l_pDevice : m_deviceQueue)
		{
			if (l_pDevice && (l_pDevice->GetDeviceType() == CTC_TYPE_IPPHONE || l_pDevice->GetDeviceType() == CTC_TYPE_DIGITAL || l_pDevice->GetDeviceType() == CTC_TYPE_ANALOG))
			{
				boost::shared_ptr<CDeviceStateNotif> l_pDeviceNotif = boost::make_shared<CDeviceStateNotif>();
				if (l_pDeviceNotif)
				{
					std::string l_strDeviceState = CallStateString[STATE_FREE];
					std::string l_strStateTime = m_pDateTimePtr->CurrentDateTimeStr();

					long l_lCSTACallRefId = DEFAULT_CALLREFID;
					l_pDevice->GetCurrentLogicalCall(l_lCSTACallRefId, l_strDeviceState, l_strStateTime);
					l_pDeviceNotif->SetCSTACallRefId(l_lCSTACallRefId);
					l_pDeviceNotif->SetDeviceNum(l_pDevice->GetDeviceNum());
					l_pDeviceNotif->SetDeviceType(l_pDevice->GetDeviceTypeString());
					l_pDeviceNotif->SetDeviceState(l_strDeviceState);
					l_pDeviceNotif->SetStateTime(l_strStateTime);

					l_pResultNotif->m_DeviceList.push_back(l_pDeviceNotif);
				}
			}
		}
	}

	return l_pResultNotif;
}

IDeviceStateNotifPtr CDeviceManager::GetDeviceState(long p_lCSTACallRefId, const std::string& p_strDeviceNum)
{
	IDeviceStateNotifPtr l_pDeviceNotif = nullptr;

	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_pDevice : m_deviceQueue)
	{
		if (l_pDevice && p_strDeviceNum.compare(l_pDevice->GetDeviceNum()) == 0 && l_pDevice->GetDeviceType() != CTC_TYPE_ACDGROUP)
		{
			l_pDeviceNotif = boost::make_shared<CDeviceStateNotif>();
			if (l_pDeviceNotif)
			{
				l_pDeviceNotif->SetDeviceNum(p_strDeviceNum);
				l_pDeviceNotif->SetDeviceType(l_pDevice->GetDeviceTypeString());
				l_pDeviceNotif->SetCSTACallRefId(p_lCSTACallRefId);
				l_pDeviceNotif->SetDeviceState(l_pDevice->GetLogicalCallState(p_lCSTACallRefId));
				l_pDeviceNotif->SetStateTime(l_pDevice->GetLogicalCallStateTime(p_lCSTACallRefId));
			}
			else
			{
				ICC_LOG_FATAL(m_pLog, "Create IDeviceStateNotif Object Failed !!!");
			}

			break;
		}
	}

	return l_pDeviceNotif;
}
/*
void CDeviceManager::PostDeviceState(long p_lCSTACallRefId, const std::string& p_strCTICallRefId, const std::string& p_strDeviceNum)
{
	std::string l_strCTICallRefId = "";
	std::string l_strCallerId = "";
	std::string l_strCalledParty = "";
	std::string l_strOriginalCallerId = "";
	std::string l_strOriginalCalledId = "";
	std::string l_strCallDirection = "";
	CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, l_strCTICallRefId, l_strCallerId, l_strCalledParty, l_strOriginalCallerId, l_strOriginalCalledId, l_strCallDirection);

	m_deviceQueueMutex.lock();
	auto it = m_deviceQueue.begin();
	while (it != m_deviceQueue.end())
	{
		boost::shared_ptr<CDevice> l_pDevice = *it;
		if (l_pDevice)
		{
			if (p_strDeviceNum.compare(l_pDevice->GetDeviceNum()) == 0 && l_pDevice->GetDeviceType() != CTC_TYPE_ACDGROUP)
			{
				IDeviceStateNotifPtr l_pDeviceNotif = boost::make_shared<CDeviceStateNotif>();
				if (l_pDeviceNotif)
				{
					l_pDeviceNotif->SetCallerId(l_strCallerId);
					l_pDeviceNotif->SetCalledId(l_strCalledParty);
					l_pDeviceNotif->SetOriginalCallerId(l_strOriginalCallerId);
					l_pDeviceNotif->SetOriginalCalledId(l_strOriginalCalledId);
					l_pDeviceNotif->SetCallDirection(l_strCallDirection);
					l_pDeviceNotif->SetDeviceNum(p_strDeviceNum);
					l_pDeviceNotif->SetDeviceType(l_pDevice->GetDeviceTypeString());
					l_pDeviceNotif->SetCSTACallRefId(p_lCSTACallRefId);
					l_pDeviceNotif->SetCTICallRefId(p_strCTICallRefId);
					l_pDeviceNotif->SetDeviceState(l_pDevice->GetLogicalCallState(p_lCSTACallRefId));
					l_pDeviceNotif->SetStateTime(l_pDevice->GetLogicalCallStateTime(p_lCSTACallRefId));

					ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
					if (l_pTask)
					{
						long l_lTaskId = l_pTask->GetTaskId();
						l_pTask->SetTaskName(Task_DeviceStateEvent);
						l_pTask->SetSwitchNotif(l_pDeviceNotif);

						CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
						ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask , TaskId: [%u]", l_lTaskId);
					}
					else
					{
						ICC_LOG_FATAL(m_pLog, "Create DeviceStateEvent Task Object Failed !!!");
					}
				}
				else
				{
					ICC_LOG_FATAL(m_pLog, "Create IDeviceStateNotif Object Failed !!!");
				}

				break;
			}
		}//	end l_pDevice

		++it;
	}	// end while
	m_deviceQueueMutex.unlock();
}*/

int CDeviceManager::GetDeviceCallCount(const std::string& strDeviceNum)
{
	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
		if (l_pDevice)
		{
			if (strDeviceNum.compare(l_pDevice->GetDeviceNum()) == 0)
			{
				int iCallount =  l_pDevice->GetLogicalCallCount();		
				ICC_LOG_DEBUG(m_pLog, "get device call count, device:%s, callcount: %d", strDeviceNum.c_str(), iCallount);
				return iCallount;
			}
		}
	}
	return 0;
}

void CDeviceManager::AddLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum, const std::string& p_strDeviceState)
{
	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
		if (l_pDevice)
		{
			if (p_strDeviceNum.compare(l_pDevice->GetDeviceNum()) == 0)
			{
				l_pDevice->AddLogicalCall(p_lCSTACallRefId, p_strDeviceState, m_pDateTimePtr->CurrentDateTimeStr());

				ICC_LOG_DEBUG(m_pLog, "Add Logical Call State, CSTACallRefId: [%u], LogicalDevice: [%s], DeviceState: [%s]",
					p_lCSTACallRefId, p_strDeviceNum.c_str(), p_strDeviceState.c_str());

				break;
			}
		}
	}
}
int CDeviceManager::UpdateLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum, const std::string& p_strDeviceState)
{
	int nRes = -1;
	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
		if (l_pDevice)
		{
			nRes = 0;
			if (p_strDeviceNum.compare(l_pDevice->GetDeviceNum()) == 0)
			{
				std::string l_lastState = l_pDevice->GetDeviceState();
				if (l_lastState.compare(p_strDeviceState) == 0)
				{
					nRes = 1;
				}
				else
				{
					nRes = 2;
				}
				l_pDevice->SetDeviceState(p_lCSTACallRefId, p_strDeviceState, m_pDateTimePtr->CurrentDateTimeStr());

				ICC_LOG_DEBUG(m_pLog, "Update Logical Call, CSTACallRefId: [%u], LogicalDevice: [%s], DeviceState: [%s],lastState:[%s]",
					p_lCSTACallRefId, p_strDeviceNum.c_str(), p_strDeviceState.c_str(), l_lastState.c_str());

				break;
			}
		}
	}
	return nRes;
}
void CDeviceManager::DeleteAllLogicalCallState(long p_lCSTACallRefId)
{
	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
		if (l_pDevice)
		{
			l_pDevice->DeleteLogicalCall(p_lCSTACallRefId);
		}
	}

	ICC_LOG_DEBUG(m_pLog, "Delete All Logical Call State, CSTACallRefId: [%u]",
		p_lCSTACallRefId);
}
void CDeviceManager::DeleteLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum)
{
	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
		if (l_pDevice)
		{
			if (p_strDeviceNum.compare(l_pDevice->GetDeviceNum()) == 0)
			{
				l_pDevice->DeleteLogicalCall(p_lCSTACallRefId);

				ICC_LOG_DEBUG(m_pLog, "Delete Logical Call State, CSTACallRefId: [%u], LogicalDevice: [%s]",
					p_lCSTACallRefId, p_strDeviceNum.c_str());

				break;
			}
		}
	}
}
std::string CDeviceManager::GetLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum)
{
	std::string l_strDeviceState = "";

	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
		if (l_pDevice)
		{
			if (p_strDeviceNum.compare(l_pDevice->GetDeviceNum()) == 0)
			{
				l_strDeviceState = l_pDevice->GetLogicalCallState(p_lCSTACallRefId);

				break;
			}
		}
	}

	return l_strDeviceState;
}
bool CDeviceManager::FindLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum)
{
	bool l_bFind = false;

	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
		if (l_pDevice)
		{
			if (p_strDeviceNum.compare(l_pDevice->GetDeviceNum()) == 0)
			{
				l_bFind = l_pDevice->FindLogicalCall(p_lCSTACallRefId);

				break;
			}
		}
	}

	return l_bFind;
}
bool CDeviceManager::FindLogicalCall(long p_lCSTACallRefId)
{
	bool l_bFind = false;

	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
		if (l_pDevice)
		{
			if (l_pDevice->FindLogicalCall(p_lCSTACallRefId))
			{
				l_bFind = true;

				break;
			}
		}
	}

	return l_bFind;
}

std::string CDeviceManager::GetDeviceState(const std::string& p_strDeviceNum)
{
	std::string l_strDeviceState = "";
	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
		if (l_pDevice)
		{
			if (l_pDevice->GetDeviceNum().compare(p_strDeviceNum) == 0)
			{
				l_strDeviceState = l_pDevice->GetDeviceState();

				break;
			}
		}
	}

	return l_strDeviceState;
}

std::string CDeviceManager::GetDeptCodeByDeviceNum(const std::string& p_strDeviceNum)
{
	std::string l_strSeatInfo;
	std::string l_strDeptCode = "";
	if (m_pRedisClient->HGet("SeatInfo", p_strDeviceNum, l_strSeatInfo))
	{
		PROTOCOL::CSeatInfo cSeatInfo;
		if (cSeatInfo.Parse(l_strSeatInfo, m_pJsonFty->CreateJson()))
		{
			l_strDeptCode = cSeatInfo.m_oBody.m_strDeptCode;
		}
	}
	return l_strDeptCode;
}

void CDeviceManager::SetCTITestData()
{
	SAFE_LOCK(m_deviceQueueMutex);
	for (auto l_deviceObj : m_deviceQueue)
	{
		boost::shared_ptr<CDevice> l_pDevice = l_deviceObj;
		if (l_pDevice)
		{
			std::string l_strDeviceNum = l_pDevice->GetDeviceNum();
			long l_lMonitorId = atoi(l_strDeviceNum.c_str());
			l_pDevice->SetMonitorId(l_lMonitorId);
			l_pDevice->SetIsMonistor(true);
			ICC_LOG_DEBUG(m_pLog, "SetCTITestData MonitorDeviceConf,Set Device:%s MonitorId: %d", l_strDeviceNum.c_str(), l_lMonitorId);
		}
	}

	
}
