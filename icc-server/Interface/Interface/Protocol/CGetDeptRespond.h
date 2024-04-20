#pragma once
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CBaseDataEntities.h>

namespace ICC
{
	namespace PROTOCOL
	{		
		class CGetDeptRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
			//	p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
				p_pJson->SetNodeValue("/body/page_index", m_oBody.m_strPageIndex);

				unsigned int l_uiIndex = 0;
				for (CDept l_oDeptData : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex)+"/");
					p_pJson->SetNodeValue(l_strPrefixPath + "guid", l_oDeptData.m_strGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "parent_guid", l_oDeptData.m_strParentGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "code", l_oDeptData.m_strCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "district_code", l_oDeptData.m_strDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "type", l_oDeptData.m_strType);
					p_pJson->SetNodeValue(l_strPrefixPath + "name", l_oDeptData.m_strName);
					p_pJson->SetNodeValue(l_strPrefixPath + "phone", l_oDeptData.m_strPhone);
					p_pJson->SetNodeValue(l_strPrefixPath + "shortcut", l_oDeptData.m_strShortcut);
					//p_pJson->SetNodeValue(l_strPrefixPath + "remark", l_oDeptData.m_strRemark);
					p_pJson->SetNodeValue(l_strPrefixPath + "sort", l_oDeptData.m_strSort);
					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CDept
			{
			public:
				std::string m_strGuid;			//单位guid
				std::string m_strParentGuid;	//上级单位guid
				std::string m_strCode;			//单位编码
				std::string m_strDistrictCode;	//单位行政区划编码
				std::string m_strType;			//单位类型
				std::string m_strName;			//单位名称
				std::string m_strPhone;			//单位座机
				std::string m_strShortcut;		//单位快捷查询缩写
				//std::string m_strRemark;		//
				std::string m_strSort;			//单位序号，一般从1开始排序	
			};
			class CBody
			{
			public:
			//	std::string m_strResult;
				std::string m_strCount;
				std::string m_strAllCount;
				std::string m_strPageIndex;
				std::vector<CDept> m_vecData;
			};

			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}