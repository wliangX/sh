#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CGetPhoneInfoRespond :
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
					std::string l_strValueNum = std::to_string(i);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/phone", m_oBody.m_vecData.at(i).m_strPhone);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/name", m_oBody.m_vecData.at(i).m_strName);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/address", m_oBody.m_vecData.at(i).m_strAddress);
				}

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCount;
				std::string m_strAllCount;
				std::string m_strPageIndex;
				class CData
				{
				public:
					std::string m_strPhone;
					std::string m_strName;
					std::string m_strAddress;
					std::string m_DelFlag;

				};
				std::vector<CData> m_vecData;
			};
			CBody m_oBody;
		};
	}
}
