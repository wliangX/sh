#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CCallStateEvent :
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

				p_pJson->SetNodeValue("/body/agent", m_oBody.m_strAgent);
				p_pJson->SetNodeValue("/body/acd", m_oBody.m_strACD);
				p_pJson->SetNodeValue("/body/original_acd", m_oBody.m_strOriginalACD);
				p_pJson->SetNodeValue("/body/acd_dept", m_oBody.m_strACDDept);
				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallRefId);
				p_pJson->SetNodeValue("/body/csta_callref_id", m_oBody.m_strCSTACallRefId);
				p_pJson->SetNodeValue("/body/caller_id", m_oBody.m_strCallerId);
				p_pJson->SetNodeValue("/body/called_id", m_oBody.m_strCalledId);
				p_pJson->SetNodeValue("/body/state", m_oBody.m_strState);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);
				p_pJson->SetNodeValue("/body/original_acd_dept", m_oBody.m_strOriginalACDDept);

				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strAgent = p_pJson->GetNodeValue("/body/agent", "");
				m_oBody.m_strACD = p_pJson->GetNodeValue("/body/acd", "");
				m_oBody.m_strOriginalACD = p_pJson->GetNodeValue("/body/original_acd", "");
				m_oBody.m_strACDDept = p_pJson->GetNodeValue("/body/acd_dept", "");
				m_oBody.m_strCallRefId = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strCSTACallRefId = p_pJson->GetNodeValue("/body/csta_callref_id", "");
				m_oBody.m_strCallerId = p_pJson->GetNodeValue("/body/caller_id", "");
				m_oBody.m_strCalledId = p_pJson->GetNodeValue("/body/called_id", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");

				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strAgent;
				std::string m_strACD;
				std::string m_strOriginalACD;
				std::string m_strACDDept;
				std::string m_strCallRefId;
				std::string m_strCSTACallRefId;	//	交换机产生的话务 ID
				std::string m_strCallerId;
				std::string m_strCalledId;
				std::string m_strState;
				std::string m_strTime;
				std::string m_strOriginalACDDept;
			};
			CBody m_oBody;
		};
	}
}
