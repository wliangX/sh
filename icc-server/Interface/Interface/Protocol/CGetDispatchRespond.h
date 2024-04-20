#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetDispatchRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				unsigned int l_uiIndex = 0;
				for (CDispatch data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "guid", data.m_strGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "parent_guid", data.m_strParentGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "code", data.m_strCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "district_code", data.m_strDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "type", data.m_strType);
					p_pJson->SetNodeValue(l_strPrefixPath + "name", data.m_strName);
					p_pJson->SetNodeValue(l_strPrefixPath + "phone", data.m_strPhone);
					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CDispatch
			{
			public:
				std::string m_strGuid;			//可调派单位guid
				std::string m_strParentGuid;	//上级可调派单位guid
				std::string m_strCode;			//原单位编码
				std::string m_strDistrictCode;	//原单位行政区划编码
				std::string m_strType;			//原单位类型
				std::string m_strName;			//原单位名称
				std::string m_strPhone;			//原单位座机
			};
			class CBody
			{
			public:
				std::string m_strCount;
				std::vector<CDispatch> m_vecData;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}