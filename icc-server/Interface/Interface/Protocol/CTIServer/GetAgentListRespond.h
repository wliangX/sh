#pragma once 
#include <vector>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAgentListRespond :
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
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/agent", m_oBody.m_vecData[i].m_strAgent);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/acd", m_oBody.m_vecData[i].m_strACD);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/login_mode", m_oBody.m_vecData[i].m_strLoginMode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/ready_state", m_oBody.m_vecData[i].m_strReadyState);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/time", m_oBody.m_vecData[i].m_strTime);
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
					std::string m_strAgent;
					std::string m_strACD;
					std::string m_strLoginMode;
					std::string m_strReadyState;
					std::string m_strTime;
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};

		class CGetFreeAgentRespond :
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
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/agent", m_oBody.m_vecData[i].m_strAgent);
					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/acd", m_oBody.m_vecData[i].m_strACD);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/login_mode", m_oBody.m_vecData[i].m_strLoginMode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/ready_state", m_oBody.m_vecData[i].m_strReadyState);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/dept_code", m_oBody.m_vecData[i].m_strDeptCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/dept_name", m_oBody.m_vecData[i].m_strDeptName);
					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/time", m_oBody.m_vecData[i].m_strTime);
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
					std::string m_strAgent;
					std::string m_strACD;
					std::string m_strLoginMode;
					std::string m_strReadyState;
					
					std::string m_strDeptCode;
					std::string m_strDeptName;
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
