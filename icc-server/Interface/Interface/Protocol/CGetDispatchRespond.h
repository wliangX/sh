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
				std::string m_strGuid;			//�ɵ��ɵ�λguid
				std::string m_strParentGuid;	//�ϼ��ɵ��ɵ�λguid
				std::string m_strCode;			//ԭ��λ����
				std::string m_strDistrictCode;	//ԭ��λ������������
				std::string m_strType;			//ԭ��λ����
				std::string m_strName;			//ԭ��λ����
				std::string m_strPhone;			//ԭ��λ����
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