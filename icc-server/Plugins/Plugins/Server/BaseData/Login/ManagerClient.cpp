#include "Boost.h"
#include "ManagerClient.h"

//************************************
// Method:    CManagerClient
// FullName:  ICC::Monitor::CManagerClient::CManagerClient
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: IResourceManagerPtr p_pResourceManager
//************************************
CManagerClient::CManagerClient(IResourceManagerPtr p_pResourceManager)
{
	m_pResourceManager = p_pResourceManager;
	m_pLockFac = ICCGetILockFactory();
	m_pLock = m_pLockFac->CreateLock(Lock::TypeRecursiveMutex);
}

//************************************
// Method:    GetResourceManager
// FullName:  ICC::Monitor::CManagerClient::GetResourceManager
// Access:    virtual public 
// Returns:   ICC::IResourceManagerPtr
// Qualifier:
//************************************
IResourceManagerPtr CManagerClient::GetResourceManager()
{
	return m_pResourceManager;
}

//************************************
// Method:    AddClient
// FullName:  ICC::Monitor::CManagerClient::AddClient
// Access:    virtual public 
// Returns:   void
// Qualifier:
// Parameter: std::string p_strClientID
// Parameter: ClientInfo & p_ClientInfo
//************************************
void CManagerClient::AddClient(std::string p_strClientID, ClientInfo &p_ClientInfo)
{
	Lock::AutoLock l_lock(m_pLock);
	if (p_strClientID.empty())
	{
		return;
	}
	std::map<std::string, ClientInfo>::iterator iter = m_mapClientInfo.find(p_strClientID);
	if (iter == m_mapClientInfo.end())
	{
		m_mapClientInfo[p_strClientID] = p_ClientInfo;
	}
}

//************************************
// Method:    DeleteClient
// FullName:  ICC::Monitor::CManagerClient::DeleteClient
// Access:    virtual public 
// Returns:   bool
// Qualifier:
// Parameter: std::string p_strClientID
//************************************
bool CManagerClient::DeleteClient(std::string p_strClientID)
{
	Lock::AutoLock l_lock(m_pLock);
	bool l_bIsDelete = false;
	if (p_strClientID.empty())
	{
		return l_bIsDelete;
	}
	std::map<std::string, ClientInfo>::iterator iter = m_mapClientInfo.find(p_strClientID);
	if (iter != m_mapClientInfo.end())
	{
		m_mapClientInfo.erase(iter);
		l_bIsDelete = true;
	}
	return l_bIsDelete;
}

//************************************
// Method:    GetAllClientInfo
// FullName:  ICC::Monitor::CManagerClient::GetAllClientInfo
// Access:    virtual public 
// Returns:   ClientInfoMAP &
// Qualifier:
//************************************
ClientInfoMAP CManagerClient::GetAllClientInfo()
{
	Lock::AutoLock l_lock(m_pLock);
	return m_mapClientInfo;
}

//************************************
// Method:    IsConnected
// FullName:  ICC::Monitor::CManagerClient::IsConnected
// Access:    virtual public 
// Returns:   bool
// Qualifier:
// Parameter: std::string p_strClientID
//************************************
bool CManagerClient::IsConnected(std::string p_strClientID)
{
	Lock::AutoLock l_lock(m_pLock);
	if (!p_strClientID.empty())
	{
		std::map<std::string, ClientInfo>::iterator iter = m_mapClientInfo.find(p_strClientID);
		if (iter != m_mapClientInfo.end())
		{
			return true;
		}
	}
	return false;
}

//************************************
// Method:    GetClientInfoByClientID
// FullName:  ICC::Monitor::CManagerClient::GetClientInfoByClientID
// Access:    virtual public 
// Returns:   bool
// Qualifier:
// Parameter: std::string p_strClientID
// Parameter: ClientInfo & p_ClientInfo
//************************************
bool CManagerClient::GetClientInfoByClientID(std::string p_strClientID, ClientInfo &p_ClientInfo)
{
	Lock::AutoLock l_lock(m_pLock);
	if (!p_strClientID.empty())
	{
		std::map<std::string, ClientInfo>::iterator iter = m_mapClientInfo.find(p_strClientID);
		if (iter != m_mapClientInfo.end())
		{
			p_ClientInfo.strClientType = iter->second.strClientType;
			p_ClientInfo.strClientID = iter->second.strClientID;
			p_ClientInfo.strClientName = iter->second.strClientName;
			return true;
		}
	}
	return false;
}

//************************************
// Method:    GetClientInfoByUserName
// FullName:  ICC::Monitor::CManagerClient::GetClientInfoByUserName
// Access:    virtual public 
// Returns:   bool
// Qualifier:
// Parameter: std::string p_strUserName
// Parameter: ClientInfo & p_ClientInfo
//************************************
bool CManagerClient::GetClientInfoByUserName(std::string p_strUserName, ClientInfo &p_ClientInfo)
{
	Lock::AutoLock l_lock(m_pLock);
	bool bFind = false;
	if (p_strUserName.empty())
	{
		return bFind;
	}
	std::map<std::string, ClientInfo>::iterator iter;
	for (iter = m_mapClientInfo.begin(); iter != m_mapClientInfo.end(); iter++)
	{
		ClientInfo pClientInfo = iter->second;
		if (pClientInfo.strClientName.compare(p_strUserName) == 0)
		{
			p_ClientInfo.strClientType = iter->second.strClientType;
			p_ClientInfo.strClientID = iter->second.strClientID;
			p_ClientInfo.strClientName = iter->second.strClientName;
			bFind = true;
		}
	}
	return bFind;
}

std::string CManagerClient::GetUserNameByClientID(std::string p_strClientID)
{
	Lock::AutoLock l_lock(m_pLock);
	std::string l_strUserName;
	if (!p_strClientID.empty())
	{
		std::map<std::string, ClientInfo>::iterator iter = m_mapClientInfo.find(p_strClientID);
		if (iter != m_mapClientInfo.end())
		{
			l_strUserName = iter->second.strClientName;
			return l_strUserName;
		}
	}
	return l_strUserName;
}

