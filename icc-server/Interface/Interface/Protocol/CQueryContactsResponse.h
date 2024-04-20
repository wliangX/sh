#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CQueryContactsResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/count", std::to_string(m_oBody.m_vecData.size()));
				for (unsigned int i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "guid", m_oBody.m_vecData[i].m_guid);
					p_pJson->SetNodeValue(l_strPrefixPath + "staff_code", m_oBody.m_vecData[i].m_staff_code);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_number", m_oBody.m_vecData[i].m_contact_number);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_name", m_oBody.m_vecData[i].m_contact_name);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_dept", m_oBody.m_vecData[i].m_contact_dept);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_number2", m_oBody.m_vecData[i].m_contact_number2);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_number3", m_oBody.m_vecData[i].m_contact_number3);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_number4", m_oBody.m_vecData[i].m_contact_number4);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_guid", m_oBody.m_vecData[i].m_contact_guid);
				}

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;

			class CData
			{
			public:
				std::string m_guid;
				std::string m_staff_code;
				std::string m_contact_number;
				std::string m_contact_name;
				std::string m_contact_dept;
				std::string m_contact_number2;
				std::string m_contact_number3;
				std::string m_contact_number4;
				std::string m_contact_guid;
			};

			class CBody
			{
			public:
				std::vector<CData> m_vecData;
			};
			CBody m_oBody;
		};
	}
}
