#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetContactInfoRespond :
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
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				unsigned int l_uiIndex = 0;
				for (CInfo data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "guid", data.m_strGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "group_guid", data.m_strGrpGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "type", data.m_strType);
					p_pJson->SetNodeValue(l_strPrefixPath + "name", data.m_strName);
					p_pJson->SetNodeValue(l_strPrefixPath + "phone", data.m_strPhone);
					p_pJson->SetNodeValue(l_strPrefixPath + "sort", data.m_strSort);
					p_pJson->SetNodeValue(l_strPrefixPath + "remark", data.m_strRemark);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CInfo
			{
			public:
				std::string m_strGuid;			//guid
				std::string m_strGrpGuid;		//������guid
				std::string m_strType;			//1��UserCode����ϯ�ţ�2����¼�û��ľ�Ա���
				std::string m_strName;			//����
				std::string m_strPhone;			//��ϵ�绰
				std::string m_strSort;			//���
				std::string m_strRemark;		//��ע
			};
			class CBody
			{
			public:
				std::string m_strResult;
				std::string m_strCount;
				std::vector<CInfo> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}