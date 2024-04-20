#pragma once 
#include <vector>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetDeviceListRespond :
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

					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/device", m_oBody.m_vecData[i].m_strDevice);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/device_type", m_oBody.m_vecData[i].m_strDeviceType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/state", m_oBody.m_vecData[i].m_strState);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/time", m_oBody.m_vecData[i].m_strTime);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/begin_talk_time", m_oBody.m_vecData[i].m_strBeginTalkTime);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/call_direction", m_oBody.m_vecData[i].m_strCallDirection);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/callref_id", m_oBody.m_vecData[i].m_strCallRefId);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/caller_id", m_oBody.m_vecData[i].m_strCallerId);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/called_id", m_oBody.m_vecData[i].m_strCalledId);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/login_mode", m_oBody.m_vecData[i].m_strLoginMode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/ready_state", m_oBody.m_vecData[i].m_strReadyState);

					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/dept_code", m_oBody.m_vecData[i].m_strDeptCode);

					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/acd", m_oBody.m_vecData[i].m_strAcdGrp);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/original_called_id", m_oBody.m_vecData[i].m_strOriginalCalled);

					if (!m_oBody.m_vecData[i].m_strConferenceId.empty())
					{
						p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/conference_id", m_oBody.m_vecData[i].m_strConferenceId);
						p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/conference_compere", m_oBody.m_vecData[i].m_strConferenceCompere);
						if (!m_oBody.m_vecData[i].m_strConfBargeinSponsor.empty())
						{
							p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/conference_bargeinsponsor", m_oBody.m_vecData[i].m_strConfBargeinSponsor);
						}
						
					}

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
					std::string m_strDevice;
					std::string m_strDeviceType;
					std::string m_strState;
					std::string m_strTime;
					std::string m_strBeginTalkTime;
					std::string m_strCallDirection;
					std::string m_strCallRefId;
					std::string m_strCallerId;
					std::string m_strCalledId;
					std::string	m_strLoginMode;		//	login/logout ״̬
					std::string	m_strReadyState;	//	busy/idle ״̬

					std::string m_strDeptCode;
					std::string m_strAcdGrp;
					std::string m_strOriginalCalled;

					std::string m_strConferenceId;
					std::string m_strConferenceCompere;
					std::string m_strConfBargeinSponsor;

				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
