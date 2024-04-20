#pragma once
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CGetViolationTimeOutRequest:
			public IRequest
		{

		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strViolationType = p_pJson->GetNodeValue("/body/violation_type", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strViolationType;
			};
			CBody m_oBody;
		};
		
		class CGetViolationTimeOutRespond :public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/alarm_timeout", m_oBody.m_strAlarmTimeout);
				p_pJson->SetNodeValue("/body/callring_timeout", m_oBody.m_strCallringTimeout);
				p_pJson->SetNodeValue("/body/calltalk_timeout", m_oBody.m_strCalltalkTimeout);
				p_pJson->SetNodeValue("/body/busystate_timeout", m_oBody.m_strBusyStateTimeout);
				//p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strAlarmTimeout;
				std::string m_strCallringTimeout;
				std::string m_strCalltalkTimeout;
				std::string m_strBusyStateTimeout;
			};
			CBody m_oBody;
		};
	}
}
