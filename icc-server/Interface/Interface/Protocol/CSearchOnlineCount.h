#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchOnlineCount : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_oData.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_oData.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_oData.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_oData.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");

				return true;
			}

		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/total_online_count", m_oBody.m_strSum);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strIndex = std::to_string(i);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/user_code", m_oBody.m_vecData[i].m_strUserCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/onoff_count", m_oBody.m_vecData[i].m_strOnlineSum);
				}
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strSum;
				std::string m_strCount;
				class CData
				{
				public:
					std::string m_strUserCode;
					std::string m_strOnlineSum;
					std::string m_strBeginTime;
					std::string m_strEndTime;
					std::string m_strPageSize;
					std::string m_strPageIndex;
				};
				std::vector<CData>m_vecData;
				CData m_oData;
			};
			CBody m_oBody;
			
		};
	}
}
