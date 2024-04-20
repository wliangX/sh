#include "Boost.h"
#include "BlackListManager.h"
#include "SysConfig.h"


CBlackListManager::CBlackListManager(void)
{
	//
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
	ClearBlackList();

	if (m_pInstance)
	{
		m_pInstance.reset();
	}
}

void CBlackListManager::OnInit(IResourceManagerPtr p_pResourceManager)
{
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_pResourceManager)->GetLogger(MODULE_NAME);
}
void CBlackListManager::OnStart()
{
	//
}
void CBlackListManager::OnStop()
{
	ClearBlackList();
}
//////////////////////////////////////////////////////////////////////////
bool CBlackListManager::FindBlackNumber(const std::string& p_strLimitNum)
{
	bool l_bFind = false;
	std::string l_strLimitNum = CSysConfig::Instance()->GetCheckNum(p_strLimitNum);

	ICC_LOG_DEBUG(m_pLog, "inlLimitNum:[%s],outLimitNum:[%s].size:[%d]",p_strLimitNum.c_str(), l_strLimitNum.c_str(), m_blackList.size());

	SAFE_LOCK(m_blackListMutex);
	for (auto l_blackObj : m_blackList)
	{
		std::string l_strTempNum = l_blackObj;
		if (l_strTempNum.compare(l_strLimitNum) == 0)
		{
			//	È«Æ¥Åä
			l_bFind = true;

			break;
		}
		else
		{
			//×ÖÍ·Æ¥Åä
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

void CBlackListManager::AddBlackList(const std::string& p_strLimitNum)
{
	std::string l_strLimitNum = CSysConfig::Instance()->GetCheckNum(p_strLimitNum);

	if (!FindBlackNumber(l_strLimitNum))
	{
		SAFE_LOCK(m_blackListMutex);
		m_blackList.push_back(l_strLimitNum);

		ICC_LOG_DEBUG(m_pLog, "Add Black List, LimitNum [%s]",
			l_strLimitNum.c_str());
	}
}

void CBlackListManager::DeleteBlackList(const std::string& p_strLimitNum)
{
	std::string l_strLimitNum = CSysConfig::Instance()->GetCheckNum(p_strLimitNum);

	SAFE_LOCK(m_blackListMutex);
	for (auto l_blackObj : m_blackList)
	{
		if (l_blackObj.compare(l_strLimitNum) == 0)
		{
			m_blackList.remove(l_blackObj);

			ICC_LOG_DEBUG(m_pLog, "Delete Black List, LimitNum [%s]",
				l_strLimitNum.c_str());

			break;
		}
	}
}

void CBlackListManager::ClearBlackList()
{
	SAFE_LOCK(m_blackListMutex);
	m_blackList.clear();

	ICC_LOG_DEBUG(m_pLog, "Clear Black List");
}
