#include "Boost.h"
#include "MQClient.h"

//************************************
// Method:    CManagerMQClient
// FullName:  ICC::Monitor::CManagerMQClient::CManagerMQClient
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: IResourceManagerPtr p_pResourceManager
//************************************
CManagerMQClient::CManagerMQClient(IResourceManagerPtr p_pResourceManager)
{
	m_pResourceManager = p_pResourceManager;
	m_pLockFac = ICCGetILockFactory();
	m_pLock = m_pLockFac->CreateLock(Lock::TypeRecursiveMutex);
}

//************************************
// Method:    GetResourceManager
// FullName:  ICC::Monitor::CManagerMQClient::GetResourceManager
// Access:    virtual public 
// Returns:   ICC::IResourceManagerPtr
// Qualifier:
//************************************
IResourceManagerPtr CManagerMQClient::GetResourceManager()
{
	return m_pResourceManager;
}

//************************************
// Method:    AddMQConnect
// FullName:  ICC::Monitor::CManagerMQClient::AddMQConnect
// Access:    virtual public 
// Returns:   void
// Qualifier:
// Parameter: std::string p_strConnectionID
// Parameter: MQClientInfo & p_MQClientInfo
//************************************
void CManagerMQClient::AddMQConnect(std::string p_strConnectionID, MQClientInfo &p_MQClientInfo)
{
	Lock::AutoLock lock(m_pLock);
	if (p_strConnectionID.empty())
	{
		return;
	}
	m_mapMQList[p_strConnectionID] = p_MQClientInfo;
}

//************************************
// Method:    DeleteMQConnect
// FullName:  ICC::Monitor::CManagerMQClient::DeleteMQConnect
// Access:    virtual public 
// Returns:   void
// Qualifier:
// Parameter: std::string p_strConnectionID
//************************************
void CManagerMQClient::DeleteMQConnect(std::string p_strConnectionID)
{
	Lock::AutoLock lock(m_pLock);
	if (p_strConnectionID.empty())
	{
		return;
	}
	std::map<std::string, MQClientInfo>::iterator iter = m_mapMQList.find(p_strConnectionID);
	if (iter != m_mapMQList.end())
	{
		m_mapMQList.erase(iter);
	}
}

//************************************
// Method:    GetMQConnectInfoByConnectionID
// FullName:  ICC::Monitor::CManagerMQClient::GetMQConnectInfoByConnectionID
// Access:    virtual public 
// Returns:   bool
// Qualifier:
// Parameter: std::string p_strConnectionID
// Parameter: MQClientInfo & p_MQClientInfo
//************************************
bool CManagerMQClient::GetMQConnectInfoByConnectionID(std::string p_strConnectionID, MQClientInfo &p_MQClientInfo)
{
	Lock::AutoLock lock(m_pLock);
	if (!p_strConnectionID.empty())
	{
		std::map<std::string, MQClientInfo>::iterator iter = m_mapMQList.find(p_strConnectionID);
		if (iter != m_mapMQList.end())
		{
			p_MQClientInfo.strClientID = iter->second.strClientID;
			p_MQClientInfo.strClientIP = iter->second.strClientIP;
			p_MQClientInfo.strConnectionID = iter->second.strConnectionID;
			return true;
		}
	}
	return false;
}

//************************************
// Method:    GetMQConnectInfoByClientID
// FullName:  ICC::Monitor::CManagerMQClient::GetMQConnectInfoByClientID
// Access:    virtual public 
// Returns:   bool
// Qualifier:
// Parameter: std::string p_strClientID
// Parameter: MQClientInfo & p_MQClientInfo
//************************************
bool CManagerMQClient::GetMQConnectInfoByClientID(std::string p_strClientID, MQClientInfo &p_MQClientInfo)
{
	Lock::AutoLock lock(m_pLock);
	if (!p_strClientID.empty())
	{
		std::map<std::string, MQClientInfo>::iterator iter = m_mapMQList.begin();
		for (; iter != m_mapMQList.end(); iter++)
		{
			if (iter->second.strClientID.compare(p_strClientID) == 0)
			{
				p_MQClientInfo.strClientID = iter->second.strClientID;
				p_MQClientInfo.strClientIP = iter->second.strClientIP;
				p_MQClientInfo.strConnectionID = iter->second.strConnectionID;
				return true;
			}
		}
	}
	return false;
}

//************************************
// Method:    GetMQConnectInfoByClientIP
// FullName:  ICC::Monitor::CManagerMQClient::GetMQConnectInfoByClientIP
// Access:    virtual public 
// Returns:   bool
// Qualifier:
// Parameter: std::string p_strClientIP
// Parameter: MQClientInfo & p_MQClientInfo
//************************************
bool CManagerMQClient::GetMQConnectInfoByClientIP(std::string p_strClientIP, MQClientInfo &p_MQClientInfo)
{
	Lock::AutoLock lock(m_pLock);
	if (!p_strClientIP.empty())
	{
		std::map<std::string, MQClientInfo>::iterator iter = m_mapMQList.begin();
		for (; iter != m_mapMQList.end(); iter++)
		{
			if (iter->second.strClientIP.compare(p_strClientIP) == 0)
			{
				p_MQClientInfo.strClientID = iter->second.strClientID;
				p_MQClientInfo.strClientIP = iter->second.strClientIP;
				p_MQClientInfo.strConnectionID = iter->second.strConnectionID;
				return true;
			}
		}
	}
	return false;
}

//************************************
// Method:    IsConnect
// FullName:  ICC::Monitor::CManagerMQClient::IsConnect
// Access:    virtual public 
// Returns:   bool
// Qualifier:
// Parameter: std::string p_strClientID
//************************************
bool CManagerMQClient::IsConnect(std::string p_strClientID)
{
	Lock::AutoLock lock(m_pLock);
	if (!p_strClientID.empty())
	{
		std::map<std::string, MQClientInfo>::iterator iter;
		for (iter = m_mapMQList.begin(); iter != m_mapMQList.end(); iter++)
		{
			if (iter->second.strClientID.compare(p_strClientID) == 0)
			{
				return true;
			}
		}
	}
	return false;
}

MQConnectMap CManagerMQClient::GetConnectList()
{
	return m_mapMQList;
}
