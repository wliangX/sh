#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmsAlarm
		{
		public:
			std::string ToRequest(JsonParser::IJsonPtr p_pJson)
			{
				if (NULL == p_pJson)
				{
					return "";
				}
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/smd_id", m_oBody.m_strSmsId);
				p_pJson->SetNodeValue("/body/caller_no", m_oBody.m_strCallerNo);
				p_pJson->SetNodeValue("/body/caller_name", m_oBody.m_strCallerName);
				p_pJson->SetNodeValue("/body/receiver", m_oBody.m_strReceiver);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);
				p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				p_pJson->SetNodeValue("/body/content", m_oBody.m_strContent);
				p_pJson->SetNodeValue("/body/longitude", m_oBody.m_strLongitude);
				p_pJson->SetNodeValue("/body/latitude", m_oBody.m_strLatitude);
				p_pJson->SetNodeValue("/body/address", m_oBody.m_strAddress);
				return p_pJson->ToString();
			}
		public:
			class CBody
			{
			public:
				std::string m_strSmsId;
				std::string m_strCallerNo;
				std::string m_strCallerName;
				std::string m_strReceiver;
				std::string m_strTime;
				std::string m_strType;
				std::string m_strContent;
				std::string m_strLongitude;
				std::string m_strLatitude;
				std::string m_strAddress;
			};
					
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
