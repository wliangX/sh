#include "Boost.h"
#include "RequestMemory.h"


CRequestMemory::CRequestMemory()
{
	m_pNotifiRequest = nullptr;
	m_strCaseId = "";
	m_strMsgId = "";
	m_strRelateCallRefId = "";
	m_strReceiptCode = "";
	m_strReceiptName = "";
	m_lRequestId = 0;
}
CRequestMemory::~CRequestMemory()
{
	//
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
boost::shared_ptr<CRequestMemoryManager> CRequestMemoryManager::m_pInstance = nullptr;
CRequestMemoryManager::CRequestMemoryManager()
{
	//
}
CRequestMemoryManager::~CRequestMemoryManager()
{
	//
}

boost::shared_ptr<CRequestMemoryManager> CRequestMemoryManager::Instance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = boost::make_shared<CRequestMemoryManager>();
	}

	return m_pInstance;
}
void CRequestMemoryManager::ExitInstance()
{
	//
}

void CRequestMemoryManager::AddRequestMemory(long p_lRequestId, boost::shared_ptr<CRequestMemory> p_pRequest)
{
	SAFE_LOCK(m_RequestListMutex);
	m_mapRequestList[p_lRequestId] = p_pRequest;
}
boost::shared_ptr<CRequestMemory> CRequestMemoryManager::GetRequestMemory(long p_lRequestId)
{
	boost::shared_ptr<CRequestMemory> l_pRequest = nullptr;

	SAFE_LOCK(m_RequestListMutex);
	auto it = m_mapRequestList.find(p_lRequestId);
	if ( it != m_mapRequestList.end())
	{
		if (it->second)
		{
			l_pRequest = it->second;
		}
	}

	return l_pRequest;
}
ObserverPattern::INotificationPtr CRequestMemoryManager::GetNotifiRequest(long p_lRequestId)
{
	ObserverPattern::INotificationPtr l_pNotificationPtr = nullptr;

	SAFE_LOCK(m_RequestListMutex);
	auto it = m_mapRequestList.find(p_lRequestId);
	if (it != m_mapRequestList.end())
	{
		if (it->second)
		{
			l_pNotificationPtr = it->second->GetNotifiRequest();
		}
	}

	return l_pNotificationPtr;
}
void CRequestMemoryManager::DeleteRequestMemory(long p_lRequestId)
{
	SAFE_LOCK(m_RequestListMutex);
	auto it = m_mapRequestList.find(p_lRequestId);
	if (it != m_mapRequestList.end())
	{
		m_mapRequestList.erase(it);
	}
}
void CRequestMemoryManager::ClearRequestList()
{
	SAFE_LOCK(m_RequestListMutex);
	m_mapRequestList.clear();
}

