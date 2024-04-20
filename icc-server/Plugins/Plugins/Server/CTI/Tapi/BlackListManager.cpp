#include "Boost.h"

#include "SysConfig.h"
#include "BlackListManager.h"
#include "HytProtocol.h"

#define SLEEP_TIME	50  //单位：毫秒

CLimitNum::CLimitNum()
{
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
	m_pLog = nullptr;
	m_pDateTime = nullptr;
}


CBlackListManager::~CBlackListManager(void)
{
	//
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
	//
}

//////////////////////////////////////////////////////////////////////////

void CBlackListManager::InitBlackList(std::map<std::string, boost::shared_ptr<CLimitNum>> p_mapBlackList)
{
	std::lock_guard<std::mutex> guard(m_blackListMutex);
	m_mapBlackList = p_mapBlackList;
}

bool CBlackListManager::FindBlackNumber(const std::string& p_strLimitNum)
{
	bool l_bFind = false;
	std::string l_strLimitNum = CSysConfig::Instance()->GetCheckNum(p_strLimitNum);

	std::lock_guard<std::mutex> guard(m_blackListMutex);
	for each (auto l_pTempObj in m_mapBlackList)
	{
		std::string l_strTempNum = l_pTempObj.first;
		if (l_strTempNum.compare(l_strLimitNum) == 0)
		{
			//	全匹配
			l_bFind = true;

			break;
		}
		else
		{
			//	字头匹配
			if (l_strTempNum.find("*") != std::string::npos)
			{
				std::string l_strSubNum = l_strTempNum.substr(0, l_strTempNum.length() - 1);
				if (p_strLimitNum.find(l_strSubNum) == 0)
				{
					l_bFind = true;

					break;
				}
			}
		}
	}

	return l_bFind;
}

void CBlackListManager::AddBlackList(const std::string& p_strLimitNum, boost::shared_ptr<CLimitNum> p_pLimitNumObj)
{
	std::string l_strLimitNum = CSysConfig::Instance()->GetCheckNum(p_strLimitNum);

	std::lock_guard<std::mutex> guard(m_blackListMutex);
	m_mapBlackList[l_strLimitNum] = p_pLimitNumObj;

	ICC_LOG_DEBUG(m_pLog, "Add BlackList, LimitNum: %s, BeginTime: %s, EndTime: %s",
		l_strLimitNum.c_str(), 
		p_pLimitNumObj->GetBeginTime().c_str(), 
		p_pLimitNumObj->GetEndTime().c_str());
}

void CBlackListManager::DeleteBlackList(const std::string& p_strLimitNum)
{
	std::string l_strLimitNum = CSysConfig::Instance()->GetCheckNum(p_strLimitNum);

	std::lock_guard<std::mutex> guard(m_blackListMutex);
	auto it = m_mapBlackList.find(l_strLimitNum);
	if (it != m_mapBlackList.end())
	{
		m_mapBlackList.erase(it);

		ICC_LOG_DEBUG(m_pLog, "Delete BlackList, LimitNum: %s",
			l_strLimitNum.c_str());
	}
}

void CBlackListManager::ClearBlackList()
{
	std::lock_guard<std::mutex> guard(m_blackListMutex);
	m_mapBlackList.clear();

	ICC_LOG_DEBUG(m_pLog, "Clear BlackList");
}

bool CBlackListManager::GetLimitTime(const std::string& p_strDictionary, std::string& p_strBeginTime, std::string& p_strEndTime)
{
	DateTime::CDateTime l_oCurrentTime = m_pDateTime->CurrentDateTime();
	if (!p_strBeginTime.empty())
	{
		l_oCurrentTime = m_pDateTime->FromString(p_strBeginTime);
	}
	else
	{
		p_strBeginTime = m_pDateTime->ToString(l_oCurrentTime);
	}

	if (p_strDictionary.compare(Dic_OneMinute) == 0)
	{
		// 1 分钟
		p_strEndTime = m_pDateTime->ToString(m_pDateTime->AddMinutes(l_oCurrentTime, Time_One));
	}
	else if (p_strDictionary.compare(Dic_FiveMinutes) == 0)
	{
		// 5 分钟
		p_strEndTime = m_pDateTime->ToString(m_pDateTime->AddMinutes(l_oCurrentTime, Time_Five));
	}
	else if (p_strDictionary.compare(Dic_ThrityMinutes) == 0)
	{
		// 30 分钟
		p_strEndTime = m_pDateTime->ToString(m_pDateTime->AddMinutes(l_oCurrentTime, Time_Thrity));
	}
	else if (p_strDictionary.compare(Dic_OneHour) == 0)
	{
		// 1 小时
		p_strEndTime = m_pDateTime->ToString(m_pDateTime->AddHours(l_oCurrentTime, Time_One));
	}
	else if (p_strDictionary.compare(Dic_TwoHours) == 0)
	{
		//	2 小时
		p_strEndTime = m_pDateTime->ToString(m_pDateTime->AddHours(l_oCurrentTime, Time_Two));
	} 
	else if (p_strDictionary.compare(Dic_SixHours) == 0)
	{
		//	6 小时
		p_strEndTime = m_pDateTime->ToString(m_pDateTime->AddHours(l_oCurrentTime, Time_Six));
	}
	else if (p_strDictionary.compare(Dic_TwelveHours) == 0)
	{
		//	12 小时
		p_strEndTime = m_pDateTime->ToString(m_pDateTime->AddHours(l_oCurrentTime, Time_Twelve));
	}
	else if (p_strDictionary.compare(Dic_OneDay) == 0)
	{
		//	1 天
		p_strEndTime = m_pDateTime->ToString(m_pDateTime->AddDays(l_oCurrentTime, Time_One));
	}
	else if (p_strDictionary.compare(Dic_SevenDays) == 0)
	{
		//	12 天
		p_strEndTime = m_pDateTime->ToString(m_pDateTime->AddDays(l_oCurrentTime, Time_Seven));
	}
	else if (p_strDictionary.compare(Dic_FifteenDays) == 0)
	{
		//	15 天
		p_strEndTime = m_pDateTime->ToString(m_pDateTime->AddDays(l_oCurrentTime, Time_Fifteen));
	}
	else if (p_strDictionary.compare(Dic_OneMonth) == 0)
	{
		//	1 个月
		p_strEndTime = m_pDateTime->ToString(m_pDateTime->AddMonths(l_oCurrentTime, Time_One));
	}
	else if (p_strDictionary.compare(Dic_SixMonthes) == 0)
	{
		//	6 个月
		p_strEndTime = m_pDateTime->ToString(m_pDateTime->AddMonths(l_oCurrentTime, Time_Six));
	}
	else if (p_strDictionary.compare(Dic_OneYear) == 0)
	{
		// 1 年
		p_strEndTime = m_pDateTime->ToString(m_pDateTime->AddYears(l_oCurrentTime, Time_One));
	}
	else
	{
		p_strEndTime = m_pDateTime->ToString(m_pDateTime->AddHours(l_oCurrentTime, Time_Two));
	}

	return true;
}
/*
void CBlackListManager::GetAllBlackList(PROTOCOL::CGetAllBlackListRespond& p_oRespond)
{
	std::lock_guard<std::mutex> guard(m_blackListMutex);
	p_oRespond.m_oBody.m_strCount = std::to_string(m_mapBlackList.size());

	for each (auto l_pBlackListObj in m_mapBlackList)
	{
		PROTOCOL::CGetAllBlackListRespond::CBody::CData l_CData;
		l_CData.m_strLimitNum = l_pBlackListObj.second->GetLimitNum();
		l_CData.m_strLimitMin = l_pBlackListObj.second->GetLimitMin();
		l_CData.m_strLimitReason = l_pBlackListObj.second->GetLimitReason();
		l_CData.m_strStaffCode = l_pBlackListObj.second->GetStaffCode();
		l_CData.m_strStaffName = l_pBlackListObj.second->GetStaffName();
		l_CData.m_strBeginTime = l_pBlackListObj.second->GetBeginTime();
		l_CData.m_strEndTime = l_pBlackListObj.second->GetEndTime();

		p_oRespond.m_oBody.m_vecData.push_back(l_CData);
	}
}*/
void CBlackListManager::GetAllBlackList(PROTOCOL::CGetAllBlackListRespond& p_oOutRespond, const std::string& p_strInLimitNum,
	int p_nInPageSize, int p_nInPageIndex)
{
	std::vector<std::pair<std::string, boost::shared_ptr<CLimitNum>>> l_vecBlackList;

	{
		std::lock_guard<std::mutex> guard(m_blackListMutex);

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

	if (l_nBeginIndex >= l_vecBlackList.size())
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
			l_CData.m_strLimitNum = it->second->GetLimitNum();
			l_CData.m_strLimitMin = it->second->GetLimitMin();
			l_CData.m_strLimitReason = it->second->GetLimitReason();
			l_CData.m_strStaffCode = it->second->GetStaffCode();
			l_CData.m_strStaffName = it->second->GetStaffName();
			l_CData.m_strBeginTime = it->second->GetBeginTime();
			l_CData.m_strEndTime = it->second->GetEndTime();

			p_oOutRespond.m_oBody.m_vecData.push_back(l_CData);
		}
	}

	p_oOutRespond.m_oBody.m_strAllCount = std::to_string(l_vecBlackList.size());
	p_oOutRespond.m_oBody.m_strCount = std::to_string(p_oOutRespond.m_oBody.m_vecData.size());
}

//////////////////////////////////////////////////////////////////////////
bool CBlackListManager::BlackListIsTimeout(std::string& p_strLimitNum)
{
	bool l_bTimeOut = false;
	DateTime::CDateTime l_CurrentTime = m_pDateTime->CurrentDateTime();

	std::lock_guard<std::mutex> guard(m_blackListMutex);
	for each (auto l_pBlackListObj in m_mapBlackList)
	{
		DateTime::CDateTime l_EndTime = m_pDateTime->FromString(l_pBlackListObj.second->GetEndTime());
		if (l_CurrentTime > l_EndTime)
		{
			l_bTimeOut = true;
			p_strLimitNum = l_pBlackListObj.second->GetLimitNum();

			/*p_strLimitNum = l_pBlackListObj.first;
			m_mapBlackList.erase(p_strLimitNum);*/

			break;
		}
	}

	return l_bTimeOut;
}
void CBlackListManager::ProcessBlackListTimeout(const std::string& p_strLimitNum)
{
	this->DeleteBlackList(p_strLimitNum);
	CHytProtocol::Instance()->Sync_DeleteBlackList(p_strLimitNum);

	ICC_LOG_DEBUG(m_pLog, "BlackListTimeout, Remove LimitNum: %s. ",
		p_strLimitNum.c_str());
}