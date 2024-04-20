#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CCallOverEvent :
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
				p_pJson->SetNodeValue("/body/caller_id", m_oBody.m_strCallerId);
				p_pJson->SetNodeValue("/body/called_id", m_oBody.m_strCalledId);
				p_pJson->SetNodeValue("/body/call_direction", m_oBody.m_strCallDirection);
				p_pJson->SetNodeValue("/body/dial_time", m_oBody.m_strDialTime);
				p_pJson->SetNodeValue("/body/incoming_time", m_oBody.m_strIncomingTime);
				p_pJson->SetNodeValue("/body/assign_time", m_oBody.m_strAssignTime);
				p_pJson->SetNodeValue("/body/ring_time", m_oBody.m_strRingTime);
				p_pJson->SetNodeValue("/body/ringback_time", m_oBody.m_strRingBackTime);
				p_pJson->SetNodeValue("/body/release_time", m_oBody.m_strReleaseTime);
				p_pJson->SetNodeValue("/body/talk_time", m_oBody.m_strTalkTime);
				p_pJson->SetNodeValue("/body/hangup_time", m_oBody.m_strHangupTime);
				p_pJson->SetNodeValue("/body/hangup_type", m_oBody.m_strHangupType);
				p_pJson->SetNodeValue("/body/switch_type", m_oBody.m_strSwitchType);
				p_pJson->SetNodeValue("/body/acd_dep_ex", m_oBody.m_strACDDeptEx);
				p_pJson->SetNodeValue("/body/release_type", m_oBody.m_strReleaseType);
				if (!m_oBody.m_strRecordFilePath.empty())
				{
					p_pJson->SetNodeValue("/body/record_file", m_oBody.m_strRecordFilePath);
				}
				
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
				m_oBody.m_strCallerId = p_pJson->GetNodeValue("/body/caller_id", "");
				m_oBody.m_strCalledId = p_pJson->GetNodeValue("/body/called_id", "");
				m_oBody.m_strCallDirection = p_pJson->GetNodeValue("/body/call_direction", "");
				m_oBody.m_strDialTime = p_pJson->GetNodeValue("/body/dial_time", "");
				m_oBody.m_strIncomingTime = p_pJson->GetNodeValue("/body/incoming_time", "");
				m_oBody.m_strAssignTime = p_pJson->GetNodeValue("/body/assign_time", "");
				m_oBody.m_strRingTime = p_pJson->GetNodeValue("/body/ring_time", "");
				m_oBody.m_strRingBackTime = p_pJson->GetNodeValue("/body/ringback_time", "");
				m_oBody.m_strReleaseTime = p_pJson->GetNodeValue("/body/release_time", "");
				m_oBody.m_strTalkTime = p_pJson->GetNodeValue("/body/talk_time", "");
				m_oBody.m_strHangupTime = p_pJson->GetNodeValue("/body/hangup_time", "");
				m_oBody.m_strHangupType = p_pJson->GetNodeValue("/body/hangup_type", "");
				m_oBody.m_strSwitchType = p_pJson->GetNodeValue("/body/switch_type", "");

				m_oBody.m_strRecordFilePath = p_pJson->GetNodeValue("/body/record_file", "");
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
				std::string m_strCallerId;
				std::string m_strCalledId;
				std::string m_strCallDirection;
				std::string m_strDialTime;
				std::string m_strIncomingTime;
				std::string m_strAssignTime;
				std::string m_strRingTime;
				std::string m_strRingBackTime;
				std::string m_strReleaseTime;
				std::string m_strTalkTime;
				std::string m_strHangupTime;
				std::string m_strHangupType;
				std::string m_strReleaseType;
				std::string m_strSwitchType;
				std::string m_strACDDeptEx;

				std::string m_strRecordFilePath;
			};
			CBody m_oBody;
		};
	}
}
