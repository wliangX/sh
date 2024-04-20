#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetConferencePartyRespond :
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
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				p_pJson->SetNodeValue("/body/conference_id", m_oBody.m_strConferenceId);
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/compere", m_oBody.m_strCompere);
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strIndex = std::to_string(i);
					p_pJson->SetNodeValue("/body/party/" + l_strIndex + "/target", m_oBody.m_vecData[i].m_strTarget);
					p_pJson->SetNodeValue("/body/party/" + l_strIndex + "/target_device_type", m_oBody.m_vecData[i].m_strTargetDeviceType);
					p_pJson->SetNodeValue("/body/party/" + l_strIndex + "/state", m_oBody.m_vecData[i].m_strState);
					p_pJson->SetNodeValue("/body/party/" + l_strIndex + "/join_time", m_oBody.m_vecData[i].m_strAddTimer);

					if (m_oBody.m_vecData[i].m_strFinallyAdd.empty())
					{
						m_oBody.m_vecData[i].m_strFinallyAdd = "0";
					}
					p_pJson->SetNodeValue("/body/party/" + l_strIndex + "/finally_add", m_oBody.m_vecData[i].m_strFinallyAdd);

					if (m_oBody.m_vecData[i].m_strDisEnable.empty())
					{
						m_oBody.m_vecData[i].m_strDisEnable = "0";
					}
					p_pJson->SetNodeValue("/body/party/" + l_strIndex + "/disenable", m_oBody.m_vecData[i].m_strDisEnable);
				}
				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCompere;
				std::string m_strCount;
				std::string m_strConferenceId;
				std::string m_strResult;
				class CData
				{
				public:
					std::string m_strTarget;
					std::string m_strTargetDeviceType;
					std::string m_strState;
					std::string m_strAddTimer;  //添加进会议的时间

					std::string m_strDisEnable;
					std::string m_strFinallyAdd;
				};
				std::vector<CData> m_vecData;

			};
			CBody m_oBody;
		};

		////////////////////////////////////////
		class CQueryConferenceByCallidRespond :
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
				p_pJson->SetNodeValue("/body/conference_id", m_oBody.m_strConferenceId);
				p_pJson->SetNodeValue("/body/in_meeting", m_oBody.m_strIsMeeting);
				p_pJson->SetNodeValue("/body/is_bargein", m_oBody.m_strIsBargein);
				p_pJson->SetNodeValue("/body/compere", m_oBody.m_strCompere);

				if (m_oBody.m_strIsBargein == "1")
				{
					p_pJson->SetNodeValue("/body/bargein_sponsor", m_oBody.m_strIsBargein);
					p_pJson->SetNodeValue("/body/bargein_target", m_oBody.m_strBargeinTarget);
				}
				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strConferenceId;
				std::string m_strIsMeeting;
				std::string m_strIsBargein;
				std::string m_strBargeinSponSor;
				std::string m_strBargeinTarget;
				std::string m_strCompere;
			};
			CBody m_oBody;
		};

	}
}
