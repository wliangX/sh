#pragma once

namespace ICC
{
	enum eSyncType
	{
		ICC_ADD_PHONEREMARM = 1,
		ICC_MODIFY_PHONEREMARM,
		ICC_DELETE_PHONEREMARM
	};

	class CPhoneMark
	{
	public:
		bool Parse(std::string p_strPhoneMark, JsonParser::IJsonPtr p_pJson);
		std::string ToJson(JsonParser::IJsonPtr p_pJson) const;
	public:
		std::string strGuid;
		std::string strPhone;		
		std::string strType;
		std::string strStaffCode;
		std::string strRemark;
	};

	typedef  std::map<std::string, CPhoneMark> PhoneMarkMAP;
	typedef  std::map<std::string, CPhoneMark>::iterator PhoneMarkMapIter;

	class CManagerPhoneMark
	{
	public:
		CManagerPhoneMark(IResourceManagerPtr p_pResourceManager);

	public:
		virtual void AddPhoneMark(std::string p_strGuid, CPhoneMark p_tPhoneMark);
		virtual bool DeletePhoneMark(std::string p_strGuid);
		virtual PhoneMarkMAP GetAllPhoneMark();
		virtual bool GetPhoneMarkByGuid(std::string p_strGuid, CPhoneMark &p_tPhoneMark);
		virtual bool ExitInCache(std::string p_strGuid);

	public:
		virtual IResourceManagerPtr GetResourceManager();

	private:
		IResourceManagerPtr m_pResourceManager;

	private:
		Lock::ILockFactoryPtr	m_pLockFac;
		Lock::ILockPtr		    m_pLock;
		PhoneMarkMAP			m_mapPhoneMark;
	};

	typedef boost::shared_ptr<CManagerPhoneMark> ManagerPhoneMarkPtr;

}