#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CCallEventQueryRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCallref_Id = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strTalk_Time = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_strHangup_Time = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_strCall_Direction = p_pJson->GetNodeValue("/body/call_direction", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strCaller_Id = p_pJson->GetNodeValue("/body/caller_id", "");
				m_oBody.m_strCalled_Id = p_pJson->GetNodeValue("/body/called_id", "");
				m_oBody.m_strLatestMsg_Num = p_pJson->GetNodeValue("/body/latestmsg_num", "");
				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strCallref_Id;
				std::string m_strTalk_Time;
				std::string m_strHangup_Time;
				std::string m_strCall_Direction;
				std::string m_strState;
				std::string m_strCaller_Id;
				std::string m_strCalled_Id;
				std::string m_strLatestMsg_Num;
			};
			CBody m_oBody;
		};
	}
}
