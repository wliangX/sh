#pragma once 
#include <vector>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CServerStatus : public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				return true;
			}
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strIndex = std::to_string(i);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/client_id", m_oBody.m_vecData[i].m_strClientID);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/client_type", m_oBody.m_vecData[i].m_strClientType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/client_status", m_oBody.m_vecData[i].m_strClientStatus);
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
					std::string m_strClientType;
					std::string m_strClientStatus;
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
