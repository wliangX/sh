#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CCallEventQueryRespond :
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
					std::string l_strValueNum = std::to_string(i);

					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/callref_id", m_oBody.m_vecData.at(i).m_strCallrefId);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/acd", m_oBody.m_vecData.at(i).m_strAcd);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/caller_id", m_oBody.m_vecData.at(i).m_strCallerId);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/called_id", m_oBody.m_vecData.at(i).m_strCalledId);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/incoming_time", m_oBody.m_vecData.at(i).m_strIncomingTime);
					/*p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/waiting_time", m_oBody.m_vecData.at(i).m_strWaitingTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/assign_time", m_oBody.m_vecData.at(i).m_strAssignTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/release_time", m_oBody.m_vecData.at(i).m_strReleaseTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/ring_time", m_oBody.m_vecData.at(i).m_strRingTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/ringback_time", m_oBody.m_vecData.at(i).m_strRingbackTime);*/
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/dial_time", m_oBody.m_vecData.at(i).m_strDialTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/talk_time", m_oBody.m_vecData.at(i).m_strTalkTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/hangup_time", m_oBody.m_vecData.at(i).m_strHangupTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/call_direction", m_oBody.m_vecData.at(i).m_strCallDirection);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/hangup_type", m_oBody.m_vecData.at(i).m_strHangupType);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/is_callback", m_oBody.m_vecData.at(i).m_strIsCallBack);
				}

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCount = "0";

				class CData
				{
				public:
					std::string m_strCallrefId;
					std::string m_strAcd;
					std::string m_strCallerId;
					std::string m_strCalledId;
					std::string m_strIncomingTime;
					std::string m_strWaitingTime;
					std::string m_strAssignTime;
					std::string m_strReleaseTime;
					std::string m_strRingTime;
					std::string m_strRingbackTime;
					std::string m_strDialTime;
					std::string m_strTalkTime;
					std::string m_strHangupTime;
					std::string m_strCallDirection;
					std::string m_strHangupType;
					std::string m_strIsCallBack;
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
