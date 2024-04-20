#pragma once

namespace ICC
{
	enum eSyncType
	{
		ICC_ADD_KEYDEPT = 1,
		ICC_MODIFY_KDYDEPT,
		ICC_DELETE_KEYDEPT
	};

	typedef struct CKeyDept
	{
	public:
		bool Parse(std::string p_strPhoneMark, JsonParser::IJsonPtr p_pJson);
		std::string ToJson(JsonParser::IJsonPtr p_pJson) const;
	public:
		std::string strID;
		std::string strType;		
		std::string strName;		
		std::string strPhone; 
		std::string strAddress;
		std::string strLongitude;
		std::string strLatitude;
	}CKeyDept;

	typedef  std::map<std::string, CKeyDept> KeyDeptMAP;
	typedef  std::map<std::string, CKeyDept>::iterator KeyDeptMapIter;

	class CManagerKeyDept
	{
	public:
		CManagerKeyDept(IResourceManagerPtr p_pResourceManager);

	public:
		virtual void AddKeyDept(std::string p_strPhone, CKeyDept& p_tKeyDept);
		virtual bool DeleteKeyDept(std::string p_strPhone);
		virtual KeyDeptMAP GetAllKeyDept();
		virtual bool GetKeyDeptByPhone(std::string p_strPhone, CKeyDept &p_tKeyDept);
		std::string GetIDByPhone(std::string p_strPhone);
		virtual bool ExitInCache(std::string p_strPhone);

	public:
		virtual IResourceManagerPtr GetResourceManager();

	private:
		IResourceManagerPtr m_pResourceManager;

	private:
		Lock::ILockFactoryPtr	m_pLockFac;
		Lock::ILockPtr		    m_pLock;
		KeyDeptMAP			m_mapKeyDept;
	};

	typedef boost::shared_ptr<CManagerKeyDept> ManagerKeyDeptPtr;

}