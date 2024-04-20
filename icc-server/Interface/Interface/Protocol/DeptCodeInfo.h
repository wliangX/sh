#pragma once
#include <Json/IJsonFactory.h>

#include <Data/IBaseData.h>


#define DEPT_INFO_KEY "DeptInfoKey"
#define DEPT_CODE_GUID_MAP "DeptCodeGuidMap"
#define DEPTLEVEL "DeptLevel:"

namespace ICC
{
	namespace PROTOCOL
	{
		class CDeptInfo
		{
		public:
			std::string ToJson(JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/guid", m_strGuid);
				p_pJson->SetNodeValue("/parent_guid", m_strParentGuid);
				p_pJson->SetNodeValue("/code", m_strCode);
				p_pJson->SetNodeValue("/district_code", m_strDistrictCode);
				p_pJson->SetNodeValue("/type", m_strType);
				p_pJson->SetNodeValue("/name", m_strName);
				p_pJson->SetNodeValue("/phone", m_strPhone);
				p_pJson->SetNodeValue("/shortcut", m_strShortcut);
				p_pJson->SetNodeValue("/sort", m_strSort);
				p_pJson->SetNodeValue("/org_indentifier", m_strPucOrgIdentifier);
				p_pJson->SetNodeValue("/path", m_strCodeLevel);
				p_pJson->SetNodeValue("/depth", m_strDepth);
				return p_pJson->ToString();
			}

			bool Parse(std::string l_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(l_strJson))
				{
					return false;
				}
				m_strGuid = p_pJson->GetNodeValue("/guid", "");
				m_strParentGuid = p_pJson->GetNodeValue("/parent_guid", "");
				m_strCode = p_pJson->GetNodeValue("/code", "");
				m_strDistrictCode = p_pJson->GetNodeValue("/district_code", "");
				m_strType = p_pJson->GetNodeValue("/type", "");
				m_strName = p_pJson->GetNodeValue("/name", "");
				m_strPhone = p_pJson->GetNodeValue("/phone", "");
				m_strShortcut = p_pJson->GetNodeValue("/shortcut", "");
				m_strSort = p_pJson->GetNodeValue("/sort", "");
				m_strPucOrgIdentifier = p_pJson->GetNodeValue("/org_indentifier", "");
				m_strCodeLevel = p_pJson->GetNodeValue("/path", "");
				m_strDepth = p_pJson->GetNodeValue("/depth", "");
				return true;
			}

		public:
			std::string m_strGuid;			//单位guid
			std::string m_strParentGuid;	//上级单位guid
			std::string m_strCode;			//单位编码
			std::string m_strDistrictCode;	//单位行政区划编码
			std::string m_strType;			//单位类型
			std::string m_strName;			//单位名称
			std::string m_strPhone;			//单位座机
			std::string m_strShortcut;		//单位快捷查询缩写			
			std::string m_strSort;			//单位序号，一般从1开始排序		
			std::string m_strPucOrgIdentifier;
			std::string m_strCodeLevel;
			std::string m_strDepth;
		};
	}

};