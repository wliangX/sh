#include "Boost.h"
#include "ManagerKeyDept.h"

CManagerKeyDept::CManagerKeyDept(IResourceManagerPtr p_pResourceManager)
{
	m_pResourceManager = p_pResourceManager;
	m_pLockFac = ICCGetILockFactory();
	m_pLock = m_pLockFac->CreateLock(Lock::TypeRecursiveMutex);
}

IResourceManagerPtr CManagerKeyDept::GetResourceManager()
{
	return m_pResourceManager;
}

void CManagerKeyDept::AddKeyDept(std::string p_strPhone, CKeyDept& p_tKeyDept)
{
	Lock::AutoLock l_lock(m_pLock);
	if (p_strPhone.empty())
	{
		return;
	}
	std::map<std::string, CKeyDept>::iterator iter = m_mapKeyDept.find(p_strPhone);
	if (iter == m_mapKeyDept.end())
	{
		m_mapKeyDept[p_strPhone] = p_tKeyDept;
	}
}

bool CManagerKeyDept::DeleteKeyDept(std::string p_strPhone)
{
	Lock::AutoLock l_lock(m_pLock);
	bool l_bIsDelete = false;
	if (p_strPhone.empty())
	{
		return l_bIsDelete;
	}
	std::map<std::string, CKeyDept>::iterator iter = m_mapKeyDept.find(p_strPhone);
	if (iter != m_mapKeyDept.end())
	{
		m_mapKeyDept.erase(iter);
		l_bIsDelete = true;
	}
	return l_bIsDelete;
}

KeyDeptMAP CManagerKeyDept::GetAllKeyDept()
{
	Lock::AutoLock l_lock(m_pLock);
	return m_mapKeyDept;
}

bool CManagerKeyDept::GetKeyDeptByPhone(std::string p_strPhone, CKeyDept &p_tKeyDept)
{
	Lock::AutoLock l_lock(m_pLock);
	if (!p_strPhone.empty())
	{
		std::map<std::string, CKeyDept>::iterator iter = m_mapKeyDept.find(p_strPhone);
		if (iter != m_mapKeyDept.end())
		{
			p_tKeyDept.strID = iter->second.strID;
			p_tKeyDept.strType = iter->second.strType;
			p_tKeyDept.strName = iter->second.strName;
			p_tKeyDept.strPhone = iter->second.strPhone;
			p_tKeyDept.strAddress = iter->second.strAddress;
			p_tKeyDept.strLongitude = iter->second.strLongitude;
			p_tKeyDept.strLatitude = iter->second.strLatitude;
			return true;
		}
	}
	return false;
}

std::string CManagerKeyDept::GetIDByPhone(std::string p_strPhone)
{
	Lock::AutoLock l_lock(m_pLock);
	if (!p_strPhone.empty())
	{
		std::map<std::string, CKeyDept>::iterator iter = m_mapKeyDept.find(p_strPhone);
		if (iter != m_mapKeyDept.end())
		{
			return iter->second.strID;
		}
	}
	return "";
}

bool CManagerKeyDept::ExitInCache(std::string p_strPhone)
{
	Lock::AutoLock l_lock(m_pLock);
	if (!p_strPhone.empty())
	{
		std::map<std::string, CKeyDept>::iterator iter = m_mapKeyDept.find(p_strPhone);
		if (iter != m_mapKeyDept.end())
		{
			return true;
		}
	}
	return false;
}

bool ICC::CKeyDept::Parse(std::string p_strPhoneMark, JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson->LoadJson(p_strPhoneMark))
	{
		return false;
	}
	strID = p_pJson->GetNodeValue("/id", "");
	strType = p_pJson->GetNodeValue("/type", "");
	strName = p_pJson->GetNodeValue("/name", "");
	strPhone = p_pJson->GetNodeValue("/phone", "");
	strAddress = p_pJson->GetNodeValue("/addr", "");
	strLongitude = p_pJson->GetNodeValue("/longitude", "");
	strLatitude = p_pJson->GetNodeValue("/latitude", "");
	return true;
}


std::string ICC::CKeyDept::ToJson(JsonParser::IJsonPtr p_pJson) const
{
	if (nullptr == p_pJson)
	{
		return "";
	}

	p_pJson->SetNodeValue("/id", strID);
	p_pJson->SetNodeValue("/type", strType);
	p_pJson->SetNodeValue("/name", strName);
	p_pJson->SetNodeValue("/phone", strPhone);
	p_pJson->SetNodeValue("/addr", strAddress);
	p_pJson->SetNodeValue("/longitude", strLongitude);
	p_pJson->SetNodeValue("/latitude", strLatitude);
	return p_pJson->ToString();
}
