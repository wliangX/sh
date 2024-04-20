#pragma once 

namespace ICC
{
	namespace PROTOCOL
	{
		struct SmpDataDict
		{
			std::string m_strGuid;
			std::string m_strDictKey;
			std::string m_strParentDictKey;
			std::string m_strValue;
			std::string m_strLanguage;
			std::string m_strSystemCode;
			std::string m_strEnableFlag;
			std::string m_strVersion;
			std::string m_strCreateUser;
			std::string m_strUpdateUser;
			std::string m_strCreateTime;
			std::string m_strUpdateTime;
			std::string m_strDictCode;
			std::string m_strLevel;
			std::string m_strSort;
			std::string m_strDictType;
			std::string m_strDescribe;
			std::string m_strSynVersion;

			//协议新增
			std::string m_strDictValueJson;
			std::string m_strUnionKey;
			std::string m_strParentUnionKey;
		};

		struct SmpDataFunc
		{
			std::string m_strGuid;
			std::string m_strFunctionCode;
			std::string m_strSystemCode;
			std::string m_strFunctionParentGuid;
			std::string m_strFunctionName;
			std::string m_strFunctionMenuType;
			std::string m_strFunctionSourceUri;
			std::string m_strFunctionSourceUriMethod;
			std::string m_strIcon;
			std::string m_strRemark;
			std::string m_strSort;
			std::string m_strStatus;
			std::string m_strEnableFlag;
			std::string m_strVersion;
			std::string m_strCreateUser;
			std::string m_strUpdateUser;
			std::string m_strCreateTime;
			std::string m_strUpdateTime;
			std::string m_strSynVersion;
		};

		struct SmpDataOrg
		{
			std::string m_strGuid;
			std::string m_strParentGuid;
			std::string m_strOrgGovCode;
			std::string m_strPucOrgIdentifier;
			std::string m_strOrgFullName;
			std::string m_strOrgName;
			std::string m_strOrgShortName;
			std::string m_strOrgSimpleName;
			std::string m_strOrgType;
			std::string m_strOrgBusinessType;
			std::string m_strOrgRegionType;
			std::string m_strLongitude;
			std::string m_strLatitude;
			std::string m_strOrgContact;
			std::string m_strOrgContactNo;
			std::string m_strOrgFax;
			std::string m_strOrgSegmentContent;
			std::string m_strRemark;
			std::string m_strIcon;
			std::string m_strSort;
			std::string m_strPucId;
			std::string m_strPudSystemId;
			std::string m_strEnableFlag;
			std::string m_strVersion;
			std::string m_strCreateUser;
			std::string m_strUpdateUser;
			std::string m_strCreateTime;
			std::string m_strUpdateTime;
			std::string m_strSynVersion;
			std::string m_strDistrictCode;
		};

		struct SmpDataRole
		{
			std::string m_strGuid;
			std::string m_strBelongOrgGuid;
			std::string m_strRoleName;
			std::string m_strRoleCode;
			std::string m_strRemark;
			std::string m_strEnableFlag;
			std::string m_strVersion;
			std::string m_strCreateUser;
			std::string m_strUpdateUser;
			std::string m_strCreateTime;
			std::string m_strUpdateTime;
			std::string m_strSynVersion;
		};

		struct SmpDataStaff
		{
			std::string m_strGuid;
			std::string m_strStaffCode;
			std::string m_strStaffIdNo;
			std::string m_strStaffName;
			std::string m_strBelongOrgGuid;
			std::string m_strPucOrgIdentifier;
			std::string m_strStaffSex;
			std::string m_strStaffType;
			std::string m_strStaffStatus;
			std::string m_strStaffCategory;
			std::string m_strTelephone;
			std::string m_strStaffMobile;
			std::string m_strStaffAddress;
			std::string m_strRemark;
			std::string m_strIsLeader;
			std::string m_strStaffPosition;
			std::string m_strSystemNo;
			std::string m_strSort;
			std::string m_strPucId;
			std::string m_strPucSystemId;
			std::string m_strEnableFlag;
			std::string m_strVersion;
			std::string m_strCreateUser;
			std::string m_strUpdateUser;
			std::string m_strCreateTime;
			std::string m_strUpdateTime;
			std::string m_strSynVersion;
			std::string m_strbusinessOrgGuid;
		};

		struct SmpDataUser
		{
			std::string m_strGuid;
			std::string m_strStaffGuid;
			std::string m_strBelongOrgGuid;
			std::string m_strUserName;
			std::string m_strUserPassword;
			std::string m_strUserType;
			std::string m_strUserIcon;
			std::string m_strRemark;
			std::string m_strLoginTime;
			std::string m_strOnlineStatus;
			std::string m_strPasswordErrorNum;
			std::string m_strLockStatus;
			std::string m_strLockTime;
			std::string m_strEnableFlag;
			std::string m_strVersion;
			std::string m_strCreateUser;
			std::string m_strUpdateUser;
			std::string m_strCreateTime;
			std::string m_strUpdateTime;
			std::string m_strSynVersion;
		};
	}
}
