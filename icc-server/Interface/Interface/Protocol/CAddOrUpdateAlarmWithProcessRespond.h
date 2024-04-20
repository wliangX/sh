#pragma once
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAddOrUpdateAlarmWithProcessRespond :
			public IRequest,public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				//m_oBody.m_strResult = p_pJson->GetNodeValue("/body/result", "");
				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				//p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				return p_pJson->ToString();
			}
			std::string ToString4AddAlarm(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/id", m_oBody.m_strID);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);
				p_pJson->SetNodeValue("/body/bj_time", m_oBody.m_strBJTime);

				if (m_oBody.m_strNoCreate.empty())
				{
					m_oBody.m_strNoCreate = "0";
				}
				p_pJson->SetNodeValue("/body/no_create", m_oBody.m_strNoCreate);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				//std::string m_strResult;
				std::string m_strID;
				std::string m_strTime;
				std::string m_strBJTime;
				std::string m_strNoCreate;
			};
			CBody m_oBody;
		};
	}
}
