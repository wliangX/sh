#include "Boost.h"

#include "BlackListManager.h"
#include "HytProtocol.h"

#define SLEEP_TIME	50  //单位：毫秒

CLimitNum::CLimitNum()
{
	m_strLimitType = "0";
	m_strLimitNum = "";		// 受限号码
	m_strLimitMin = "";		// 受限时长
	m_strLimitReason = "";	// 受限原因
	m_strBeginTime = "";	// 开始时间
	m_strEndTime = "";		// 结束时间
	m_strStaffCode = "";	// 警员编号
	m_strStaffName = "";	// 警员名称
}

CLimitNum::~CLimitNum()
{
	//
}

//////////////////////////////////////////////////////////////////////////
CBlackListManager::CBlackListManager(void)
{
	m_bThreadAlive = false;
	m_pCheckTimeoutThread = nullptr;

	m_SwitchClientPtr = nullptr;
	m_LogPtr = nullptr;
	m_DateTimePtr = nullptr;
}


CBlackListManager::~CBlackListManager(void)
{
	StopThread();
}

boost::shared_ptr<CBlackListManager> CBlackListManager::m_pInstance = nullptr;
boost::shared_ptr<CBlackListManager> CBlackListManager::Instance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = boost::make_shared<CBlackListManager>();
	}

	return m_pInstance;
}
void CBlackListManager::ExitInstance()
{
	StopThread();
}

//////////////////////////////////////////////////////////////////////////

void CBlackListManager::InitBlackList(std::map<std::string, boost::shared_ptr<CLimitNum>> p_mapBlackList)
{
	SAFE_LOCK(m_blackListMutex);
	m_mapBlackList = p_mapBlackList;
}
void CBlackListManager::WriteBlackListToSwitch()
{
	SAFE_LOCK(m_blackListMutex);
	for (auto l_blackObj : m_mapBlackList)
	{
		std::string l_strType = l_blackObj.second->GetLimitType();
		std::string l_strNum = l_blackObj.second->GetLimitNum();

		this->SetBlackListToSwitch(l_strType, l_strNum);
	}
}

void CBlackListManager::SetBlackListToSwitch(const std::string& p_strLimitType, const std::string& p_strLimitNum)
{
	if (m_SwitchClientPtr)
	{
		std::string l_strTempNum = (p_strLimitType.compare("1") == 0 ? p_strLimitNum + "*" : p_strLimitNum);
		m_SwitchClientPtr->SetBlackList(l_strTempNum);

		ICC_LOG_DEBUG(m_LogPtr, "Add BlackList To Switch, LimitNum: %s", l_strTempNum.c_str());
	}
}
void CBlackListManager::DeleteBlackListFromSwitch(const std::string& p_strLimitType, const std::string& p_strLimitNum)
{
	if (m_SwitchClientPtr)
	{
		std::string l_strTempNum = (p_strLimitType.compare("1") == 0 ? p_strLimitNum + "*" : p_strLimitNum);
		m_SwitchClientPtr->DeleteBlackList(l_strTempNum);

		ICC_LOG_DEBUG(m_LogPtr, "Delete BlackList From Switch, LimitNum: %s", l_strTempNum.c_str());
	}
}
void CBlackListManager::DeleteAllBlackListFromSwitch()
{
	if (m_SwitchClientPtr)
	{
		ICC_LOG_INFO(m_LogPtr, "Delete All BlackList From Switch");

		m_SwitchClientPtr->DeleteAllBlackList();
	}
}

bool CBlackListManager::FindBlackNumber(const std::string& p_strLimitNum)
{
	bool l_bFind = false;

	SAFE_LOCK(m_blackListMutex);
	auto it = m_mapBlackList.find(p_strLimitNum);
	if (it != m_mapBlackList.end())
	{
		l_bFind = true;
	}

	return l_bFind;
}

void CBlackListManager::AddBlackList(const std::string& p_strLimitNum, boost::shared_ptr<CLimitNum> p_pLimitNumObj)
{
	SAFE_LOCK(m_blackListMutex);
	m_mapBlackList[p_strLimitNum] = p_pLimitNumObj;

	ICC_LOG_INFO(m_LogPtr, "Add BlackList, LimitNum: %s, BeginTime: %s, EndTime: %s",
		p_strLimitNum.c_str(), 
		p_pLimitNumObj->GetBeginTime().c_str(), 
		p_pLimitNumObj->GetEndTime().c_str());
}

void CBlackListManager::DeleteBlackList(const std::string& p_strLimitNum)
{
	SAFE_LOCK(m_blackListMutex);
	auto it = m_mapBlackList.find(p_strLimitNum);
	if (it != m_mapBlackList.end())
	{
		m_mapBlackList.erase(it);
	}
}

void CBlackListManager::ClearBlackList()
{
	SAFE_LOCK(m_blackListMutex);
	m_mapBlackList.clear();
}

bool CBlackListManager::GetLimitTime(const std::string& p_strDictionary, std::string& p_strBeginTime, std::string& p_strEndTime)
{
	DateTime::CDateTime l_oCurrentTime = m_DateTimePtr->CurrentDateTime();
	if (!p_strBeginTime.empty())
	{
		l_oCurrentTime = m_DateTimePtr->FromString(p_strBeginTime);
	}
	else
	{
		p_strBeginTime = m_DateTimePtr->ToString(l_oCurrentTime);
	}

	if (p_strDictionary.compare(Dic_OneMinute) == 0)
	{
		// 1 分钟
		p_strEndTime = m_DateTimePtr->ToString(m_DateTimePtr->AddMinutes(l_oCurrentTime, Time_One));
	}
	else if (p_strDictionary.compare(Dic_FiveMinutes) == 0)
	{
		// 5 分钟
		p_strEndTime = m_DateTimePtr->ToString(m_DateTimePtr->AddMinutes(l_oCurrentTime, Time_Five));
	}
	else if (p_strDictionary.compare(Dic_ThrityMinutes) == 0)
	{
		// 30 分钟
		p_strEndTime = m_DateTimePtr->ToString(m_DateTimePtr->AddMinutes(l_oCurrentTime, Time_Thrity));
	}
	else if (p_strDictionary.compare(Dic_OneHour) == 0)
	{
		// 1 小时
		p_strEndTime = m_DateTimePtr->ToString(m_DateTimePtr->AddHours(l_oCurrentTime, Time_One));
	}
	else if (p_strDictionary.compare(Dic_TwoHours) == 0)
	{
		//	2 小时
		p_strEndTime = m_DateTimePtr->ToString(m_DateTimePtr->AddHours(l_oCurrentTime, Time_Two));
	} 
	else if (p_strDictionary.compare(Dic_SixHours) == 0)
	{
		//	6 小时
		p_strEndTime = m_DateTimePtr->ToString(m_DateTimePtr->AddHours(l_oCurrentTime, Time_Six));
	}
	else if (p_strDictionary.compare(Dic_TwelveHours) == 0)
	{
		//	12 小时
		p_strEndTime = m_DateTimePtr->ToString(m_DateTimePtr->AddHours(l_oCurrentTime, Time_Twelve));
	}
	else if (p_strDictionary.compare(Dic_OneDay) == 0)
	{
		//	1 天
		p_strEndTime = m_DateTimePtr->ToString(m_DateTimePtr->AddDays(l_oCurrentTime, Time_One));
	}
	else if (p_strDictionary.compare(Dic_SevenDays) == 0)
	{
		//	12 天
		p_strEndTime = m_DateTimePtr->ToString(m_DateTimePtr->AddDays(l_oCurrentTime, Time_Seven));
	}
	else if (p_strDictionary.compare(Dic_FifteenDays) == 0)
	{
		//	15 天
		p_strEndTime = m_DateTimePtr->ToString(m_DateTimePtr->AddDays(l_oCurrentTime, Time_Fifteen));
	}
	else if (p_strDictionary.compare(Dic_OneMonth) == 0)
	{
		//	1 个月
		p_strEndTime = m_DateTimePtr->ToString(m_DateTimePtr->AddMonths(l_oCurrentTime, Time_One));
	}
	else if (p_strDictionary.compare(Dic_SixMonthes) == 0)
	{
		//	6 个月
		p_strEndTime = m_DateTimePtr->ToString(m_DateTimePtr->AddMonths(l_oCurrentTime, Time_Six));
	}
	else if (p_strDictionary.compare(Dic_OneYear) == 0)
	{
		// 1 年
		p_strEndTime = m_DateTimePtr->ToString(m_DateTimePtr->AddYears(l_oCurrentTime, Time_One));
	}
	else
	{
		p_strEndTime = m_DateTimePtr->ToString(m_DateTimePtr->AddHours(l_oCurrentTime, Time_Two));
	}

	return true;
}
void CBlackListManager::GetAllBlackList(PROTOCOL::CGetAllBlackListRespond& p_oOutRespond, const std::string& p_strInLimitNum,
	int p_nInPageSize, int p_nInPageIndex)
{
	std::vector<std::pair<std::string, boost::shared_ptr<CLimitNum>>> l_vecBlackList;

	{
		SAFE_LOCK(m_blackListMutex);

		if (p_strInLimitNum.empty())
		{
			l_vecBlackList.assign(m_mapBlackList.begin(), m_mapBlackList.end());
		} 
		else
		{
			for (auto l_blackObj : m_mapBlackList)
			{
				std::string l_strTempNum = l_blackObj.second->GetLimitNum();
				if (l_strTempNum.find(p_strInLimitNum) != std::string::npos)
				{
					l_vecBlackList.push_back(l_blackObj);
				}
			}
		}
	}

	int l_nBeginIndex = (p_nInPageIndex - 1) * p_nInPageSize;
	int l_nEndIndex = p_nInPageIndex * p_nInPageSize;

	if (l_nBeginIndex > l_vecBlackList.size())
	{
		p_oOutRespond.m_oHeader.m_strResult = "1";
		p_oOutRespond.m_oHeader.m_strMsg = "Out of range";
		p_oOutRespond.m_oBody.m_strCount = "0";
	}
	else
	{
		for (auto it = l_vecBlackList.begin() + l_nBeginIndex;
			it < l_vecBlackList.begin() + l_nEndIndex && it != l_vecBlackList.end(); it++)
		{
			PROTOCOL::CGetAllBlackListRespond::CBody::CData l_CData;
			l_CData.m_strLimitType = it->second->GetLimitType();
			l_CData.m_strLimitNum = it->second->GetLimitNum();
			l_CData.m_strLimitMin = it->second->GetLimitMin();
			l_CData.m_strLimitReason = it->second->GetLimitReason();
			l_CData.m_strStaffCode = it->second->GetStaffCode();
			l_CData.m_strStaffName = it->second->GetStaffName();
			l_CData.m_strBeginTime = it->second->GetBeginTime();
			l_CData.m_strEndTime = it->second->GetEndTime();
			l_CData.m_strApprover = it->second->GetApprover();
			l_CData.m_strApprovedInfo = it->second->GetApprovedInfo();
			l_CData.m_strReceiptDeptName = it->second->GetReceiptDeptName();
			l_CData.m_strReceiptDeptDistrictCode = it->second->GetReceiptDeptDistrictCode();

			p_oOutRespond.m_oBody.m_vecData.push_back(l_CData);
		}
	}

	p_oOutRespond.m_oBody.m_strAllCount = std::to_string(l_vecBlackList.size());
	p_oOutRespond.m_oBody.m_strCount = std::to_string(p_oOutRespond.m_oBody.m_vecData.size());
}

bool CBlackListManager::BlackListIsTimeout(std::string& p_strLimitType, std::string& p_strLimitNum)
{
	bool l_bTimeOut = false;

	DateTime::CDateTime l_CurrentTime = m_DateTimePtr->CurrentDateTime();

	SAFE_LOCK(m_blackListMutex);
	auto it = m_mapBlackList.begin();
	while (it != m_mapBlackList.end())
	{
		if (it->second)
		{
			DateTime::CDateTime l_EndTime = m_DateTimePtr->FromString(it->second->GetEndTime());
			if (l_CurrentTime > l_EndTime)
			{
				l_bTimeOut = true;

				p_strLimitType = it->second->GetLimitType();
				p_strLimitNum = it->first;
				m_mapBlackList.erase(it);

				break;
			}
		}

		++it;
	}

	return l_bTimeOut;
}

//////////////////////////////////////////////////////////////////////////
void CBlackListManager::ProcessBlackListTimeout(const std::string& p_strLimitType, const std::string& p_strLimitNum)
{
	this->DeleteBlackListFromSwitch(p_strLimitType, p_strLimitNum);

	CHytProtocol::Instance()->Sync_DeleteBlackList(p_strLimitType, p_strLimitNum);

	ICC_LOG_INFO(m_LogPtr, "BlackListTimeout, Remove LimitNum: %s. ",
		p_strLimitNum.c_str());
}
void CBlackListManager::DoCheckBlackList()
{
	while (CBlackListManager::Instance()->m_bThreadAlive)
	{
		std::string l_strLimitType = "";
		std::string l_strLimitNum = "";

		if (CBlackListManager::Instance()->BlackListIsTimeout(l_strLimitType, l_strLimitNum))
		{
			CBlackListManager::Instance()->ProcessBlackListTimeout(l_strLimitType, l_strLimitNum);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
		}
	}
}

void CBlackListManager::StartThread()
{
	if (!m_bThreadAlive)
	{
		m_bThreadAlive = true;

		m_pCheckTimeoutThread = boost::make_shared<boost::thread>(boost::bind(&CBlackListManager::DoCheckBlackList, this));
	}
}
void CBlackListManager::StopThread()
{
	if (m_bThreadAlive)
	{
		m_bThreadAlive = false;

		if (m_pCheckTimeoutThread)
		{
			m_pCheckTimeoutThread->join();
		}
	}
	
}

