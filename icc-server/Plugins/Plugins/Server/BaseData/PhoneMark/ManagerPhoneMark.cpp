#include "Boost.h"
#include "ManagerPhoneMark.h"

CManagerPhoneMark::CManagerPhoneMark(IResourceManagerPtr p_pResourceManager)
{
	m_pResourceManager = p_pResourceManager;
	m_pLockFac = ICCGetILockFactory();
	m_pLock = m_pLockFac->CreateLock(Lock::TypeRecursiveMutex);
}

IResourceManagerPtr CManagerPhoneMark::GetResourceManager()
{
	return m_pResourceManager;
}

void CManagerPhoneMark::AddPhoneMark(std::string p_strGuid, CPhoneMark p_tPhoneMark)
{
	Lock::AutoLock l_lock(m_pLock);
	if (p_strGuid.empty())
	{
		return;
	}
	std::map<std::string, CPhoneMark>::iterator iter = m_mapPhoneMark.find(p_strGuid);
	if (iter == m_mapPhoneMark.end())
	{
		m_mapPhoneMark[p_strGuid] = p_tPhoneMark;
	}
}

bool CManagerPhoneMark::DeletePhoneMark(std::string p_strGuid)
{
	Lock::AutoLock l_lock(m_pLock);
	bool l_bIsDelete = false;
	if (p_strGuid.empty())
	{
		return l_bIsDelete;
	}
	std::map<std::string, CPhoneMark>::iterator iter = m_mapPhoneMark.find(p_strGuid);
	if (iter != m_mapPhoneMark.end())
	{
		m_mapPhoneMark.erase(iter);
		l_bIsDelete = true;
	}
	return l_bIsDelete;
}

PhoneMarkMAP CManagerPhoneMark::GetAllPhoneMark()
{
	Lock::AutoLock l_lock(m_pLock);
	return m_mapPhoneMark;
}

bool CManagerPhoneMark::GetPhoneMarkByGuid(std::string p_strGuid, CPhoneMark &p_tPhoneMark)
{
	Lock::AutoLock l_lock(m_pLock);
	if (!p_strGuid.empty())
	{
		std::map<std::string, CPhoneMark>::iterator iter = m_mapPhoneMark.find(p_strGuid);
		if (iter != m_mapPhoneMark.end())
		{
			p_tPhoneMark.strGuid = iter->second.strGuid;
			p_tPhoneMark.strPhone = iter->second.strPhone;
			p_tPhoneMark.strType = iter->second.strType;
			p_tPhoneMark.strStaffCode = iter->second.strStaffCode;
			p_tPhoneMark.strRemark = iter->second.strRemark;
			return true;
		}
	}
	return false;
}

bool CManagerPhoneMark::ExitInCache(std::string p_strGuid)
{
	Lock::AutoLock l_lock(m_pLock);
	if (!p_strGuid.empty())
	{
		std::map<std::string, CPhoneMark>::iterator iter = m_mapPhoneMark.find(p_strGuid);
		if (iter != m_mapPhoneMark.end())
		{
			return true;
		}
	}
	return false;
}

bool ICC::CPhoneMark::Parse(std::string p_strPhoneMark, JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strPhoneMark))
	{
		return false;
	}

	strGuid = p_pJson->GetNodeValue("/guid", "");
	strPhone = p_pJson->GetNodeValue("/phone", "");
	strType = p_pJson->GetNodeValue("/type", "");
	strStaffCode = p_pJson->GetNodeValue("/staff_code", "");
	strRemark = p_pJson->GetNodeValue("/remark", "");
	return true;
}

std::string ICC::CPhoneMark::ToJson(JsonParser::IJsonPtr p_pJson) const
{
	if (nullptr == p_pJson)
	{
		return "";
	}

	p_pJson->SetNodeValue("/guid", strGuid);
	p_pJson->SetNodeValue("/phone",strPhone);
	p_pJson->SetNodeValue("/type", strType);
	p_pJson->SetNodeValue("/staff_code", strStaffCode);
	p_pJson->SetNodeValue("/remark", strRemark );

	return p_pJson->ToString();
}
