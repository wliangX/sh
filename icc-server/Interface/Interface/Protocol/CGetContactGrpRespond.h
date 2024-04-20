#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetContactGrpRespond :
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
				for (CGroup data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "guid", data.m_strGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "title", data.m_strTitle);
					p_pJson->SetNodeValue(l_strPrefixPath + "user_code", data.m_strUserCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "type", data.m_strType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "sort", data.m_strSort);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CGroup
			{
			public:
				std::string m_strGuid;			//��guid
				std::string m_strTitle;			//����
				std::string m_strUserCode;		//��ϯ��ž�Ա����
				std::string m_strType;			//1��UserCode����ϯ�ţ�2����¼�û��ľ�Ա���
				std::string m_strSort;			//��Ա��ţ�һ���1��ʼ����
			};
			class CBody
			{
			public:
				std::string m_strResult;
				std::string m_strCount;
				std::vector<CGroup> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}