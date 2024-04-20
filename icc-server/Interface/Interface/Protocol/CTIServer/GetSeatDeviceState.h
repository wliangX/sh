#pragma once 
#include <vector>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetSeatDeviceListRespond :
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
					/*p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/agent", m_oBody.m_vecData[i].m_strAgent);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/acd", m_oBody.m_vecData[i].m_strACD);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/login_mode", m_oBody.m_vecData[i].m_strLoginMode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/ready_state", m_oBody.m_vecData[i].m_strReadyState);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/time", m_oBody.m_vecData[i].m_strTime);*/
				}
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				int l_nCount = p_pJson->GetCount("/body/data");
				m_oBody.m_strCount = std::to_string(l_nCount);

				CBody::CData l_oData;
				for (int i = 0; i < l_nCount; ++i)
				{
					std::string l_strIndex = std::to_string(i);
					l_oData.m_strDevice = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/device", "");
					l_oData.m_strDeviceType = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/device_type", "");
					l_oData.m_strState = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/state", "");
					l_oData.m_strLoginMode = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/login_mode", "");
					l_oData.m_strReadyState = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/ready_state", "");
					l_oData.m_strDeptCode = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/dept_code", "");
					l_oData.m_strAcd = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/acd", "");
					l_oData.m_strCallDirection = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/call_direction", "");
					l_oData.m_strCallrefId = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/callref_id", "");
					l_oData.m_strCallerId = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/caller_id", "");
					l_oData.m_strCalledId = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/called_id", "");
					l_oData.m_strOriginalCallId = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/original_called_id", "");
					l_oData.m_strTime = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/time", "");
					l_oData.m_strBeginTalkTime = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/begin_talk_time", "");
					m_oBody.m_vecData.push_back(l_oData);
				}

				return true;
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
					std::string m_strDevice;
					std::string m_strDeviceType;
					std::string m_strLoginMode;
					std::string m_strReadyState;
					std::string m_strDeptCode;
					std::string m_strState;
					std::string m_strAcd;
					std::string m_strBeginTalkTime;
					std::string m_strCallDirection;
					std::string m_strCalledId;
					std::string m_strCallerId;
					std::string m_strCallrefId;
					std::string m_strOriginalCallId;
					std::string m_strTime;
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};

		class CGetSeatAgentRespond :
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
					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/agent", m_oBody.m_vecData[i].m_strAgent);
					////p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/acd", m_oBody.m_vecData[i].m_strACD);
					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/login_mode", m_oBody.m_vecData[i].m_strLoginMode);
					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/ready_state", m_oBody.m_vecData[i].m_strReadyState);
					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/dept_code", m_oBody.m_vecData[i].m_strDeptCode);
					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/dept_name", m_oBody.m_vecData[i].m_strDeptName);
					////p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/time", m_oBody.m_vecData[i].m_strTime);
				}
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				int l_nCount = p_pJson->GetCount("/body/data");
				m_oBody.m_strCount = std::to_string(l_nCount);

				CBody::CData l_oData;
				for (int i = 0; i < l_nCount; ++i)
				{
					std::string l_strIndex = std::to_string(i);
					l_oData.m_strAgent = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/agent", "");
					l_oData.m_strLoginMode = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/login_mode", "");
					l_oData.m_strReadyState = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/ready_state", "");
					m_oBody.m_vecData.push_back(l_oData);
				}

				return true;
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
					std::string m_strLoginMode;
					std::string m_strReadyState;
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
