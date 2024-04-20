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
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strValueNum = std::to_string(i);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/callref_id", m_oBody.m_vecData.at(i).m_strCallref_Id);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/acd", m_oBody.m_vecData.at(i).m_strAcd);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/caller_id", m_oBody.m_vecData.at(i).m_strCaller_Id);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/called_id", m_oBody.m_vecData.at(i).m_strCalled_Id);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/incoming_time", m_oBody.m_vecData.at(i).m_strIncoming_Time);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/waiting_time", m_oBody.m_vecData.at(i).m_strWaiting_Time);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/assign_time", m_oBody.m_vecData.at(i).m_strAssign_Time);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/release_time", m_oBody.m_vecData.at(i).m_strRelease_Time);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/ring_time", m_oBody.m_vecData.at(i).m_strRing_Time);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/ringback_time", m_oBody.m_vecData.at(i).m_strRingback_Time);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/dial_time", m_oBody.m_vecData.at(i).m_strDial_Time);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/talk_time", m_oBody.m_vecData.at(i).m_strTalk_Time);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/hangup_time", m_oBody.m_vecData.at(i).m_strHangup_Time);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/call_direction", m_oBody.m_vecData.at(i).m_strCall_Direction);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/hangup_type", m_oBody.m_vecData.at(i).m_strHangup_Type);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/hangup_device", m_oBody.m_vecData.at(i).m_strHangup_Device);
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
					std::string m_strCallref_Id;
					std::string m_strAcd;
					std::string m_strCaller_Id;
					std::string m_strCalled_Id;
					std::string m_strIncoming_Time;
					std::string m_strWaiting_Time;
					std::string m_strAssign_Time;
					std::string m_strRelease_Time;
					std::string m_strRing_Time;
					std::string m_strRingback_Time;
					std::string m_strDial_Time;
					std::string m_strTalk_Time;
					std::string m_strHangup_Time;
					std::string m_strCall_Direction;
					std::string m_strHangup_Type;
					std::string m_strHangup_Device;
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
