#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAllRedListRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strPhoneNumber = p_pJson->GetNodeValue("/body/callNo", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				return p_pJson->ToString();
			}


		public:			
			class CBody
			{
			public:
				std::string	m_strPageSize;					//暂无使用
				std::string	m_strPageIndex;					//暂无使用
				std::string	m_strPhoneNumber;				//需要查询的号码					
			};

			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}
