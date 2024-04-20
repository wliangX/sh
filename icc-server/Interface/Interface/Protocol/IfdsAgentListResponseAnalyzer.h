#ifndef __IfdsAgentListResponseAnalyzer_H__
#define __IfdsAgentListResponseAnalyzer_H__

#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>


namespace ICC
{
	namespace PROTOCOL
	{
		class CIfdsAgentListReponseAnalyzer : public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCount = p_pJson->GetNodeValue("/body/count", "");
				int iSeatCount = p_pJson->GetCount("/body/data");
				for (size_t index = 0; index < iSeatCount; index++)
				{
					CSeatInfo info;
					info.m_strAgentNumber = p_pJson->GetNodeValue("/body/data/" + std::to_string(index) + "/agent", "");
					info.m_strAcdName = p_pJson->GetNodeValue("/body/data/" + std::to_string(index) + "/acd", "");
					info.m_strLoginMode = p_pJson->GetNodeValue("/body/data/" + std::to_string(index) + "/login_mode", "");
					info.m_strReadyStatus = p_pJson->GetNodeValue("/body/data/" + std::to_string(index) + "/ready_state", "");
					info.m_strTime = p_pJson->GetNodeValue("/body/data/" + std::to_string(index) + "/time", "");
					m_oBody.m_vecSeats.push_back(info);
				}
				return true;
			}

			class CSeatInfo
			{
			public:
				CSeatInfo(){}
				CSeatInfo(const CSeatInfo& other)
				{
					_Assign(other);
				}
				CSeatInfo& operator=(const CSeatInfo& other)
				{
					if (&other != this)
					{
						_Assign(other);
					}
					return *this;
				}
				~CSeatInfo(){}


			public:
				std::string m_strAgentNumber;
				std::string m_strAcdName;
				std::string m_strLoginMode;
				std::string m_strReadyStatus;
				std::string m_strTime;

			private:
				void _Assign(const CSeatInfo& other)
				{
					m_strAgentNumber = other.m_strAgentNumber;
					m_strAcdName = other.m_strAcdName;
					m_strLoginMode = other.m_strLoginMode;
					m_strReadyStatus = other.m_strReadyStatus;
					m_strTime = other.m_strTime;
				}
			};

			class CBody
			{
			public:
				std::string           m_strCount;
				std::vector<CSeatInfo> m_vecSeats;
			};

		public:
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
	
}

#endif