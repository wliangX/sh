#pragma once 
#include <vector>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetCallListRespond :
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
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strIndex = std::to_string(i);

					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/agent", m_oBody.m_vecData[i].m_strAgentId);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/acd", m_oBody.m_vecData[i].m_strACDGrp);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/callref_id", m_oBody.m_vecData[i].m_strCallRefId);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/caller_id", m_oBody.m_vecData[i].m_strCallerId);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/called_id", m_oBody.m_vecData[i].m_strCalledId);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/state", m_oBody.m_vecData[i].m_strState);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/time", m_oBody.m_vecData[i].m_strStateTime);
				}
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCount;

				class CData
				{
				public:
					std::string m_strAgentId;
					std::string m_strACDGrp;
					std::string m_strCallRefId;
					std::string m_strCallerId;
					std::string m_strCalledId;
					std::string m_strState;
					std::string m_strStateTime;
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
