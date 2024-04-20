#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetPhoneMarkRespond :
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

				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
				p_pJson->SetNodeValue("/body/page_index", m_oBody.m_strPageIndex);
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strIndex = std::to_string(i);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/guid", m_oBody.m_vecData[i].m_strGuid);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/phone", m_oBody.m_vecData[i].m_strPhone);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/type", m_oBody.m_vecData[i].m_strType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/staff_code", m_oBody.m_vecData[i].m_strStaffCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/remark", m_oBody.m_vecData[i].m_strRemark);

					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/times", m_oBody.m_vecData[i].m_strTimes);
				}
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				class CData
				{
				public:
					std::string m_strGuid;
					std::string m_strPhone;
					std::string m_strType;
					std::string m_strStaffCode;
					std::string m_strRemark;

					std::string m_strTimes;
				};

				std::string m_strCount;
				std::string m_strAllCount;
				std::string m_strPageIndex;

				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
