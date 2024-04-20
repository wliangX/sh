#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetOnOffInfo : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_oData.m_strClientName = p_pJson->GetNodeValue("/body/client_name", "");
				m_oBody.m_oData.m_strOnTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_oBody.m_oData.m_strOffTime = p_pJson->GetNodeValue("/body/end_time", "");
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
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				if (m_oBody.m_vecData.size() == 0)
				{
					p_pJson->SetNodeValue("/body/count", "0");
					p_pJson->SetNodeValue("/body/data", "");
				}
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strIndex = std::to_string(i);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/client_id", m_oBody.m_vecData[i].m_strClientID);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/client_name", m_oBody.m_vecData[i].m_strClientName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/on_time", m_oBody.m_vecData[i].m_strOnTime);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/off_time", m_oBody.m_vecData[i].m_strOffTime);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/duration", m_oBody.m_vecData[i].m_strDuration);
				}
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strCount;
				class CData
				{
				public:
					std::string m_strClientID;
					std::string m_strClientName;
					std::string m_strOnTime;
					std::string m_strOffTime;
					std::string m_strDuration;
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
