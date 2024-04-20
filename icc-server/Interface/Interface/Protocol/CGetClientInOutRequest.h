#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetInOutInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				m_oBody.m_strClientId = p_pJson->GetNodeValue("/body/client_id", "");
				m_oBody.m_strClientName = p_pJson->GetNodeValue("/body/client_name", "");
				m_oBody.m_strStartTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strPageSize;
				std::string m_strPageIndex;
				std::string m_strClientId;
				std::string m_strClientName;
				std::string m_strStartTime;
				std::string m_strEndTime;
			};
			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
