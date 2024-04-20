#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		//���ݴ澯��
		class OpenTemporaryAlarmRequst :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_alarmId = p_pJson->GetNodeValue("/body/alarm_id", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_alarmId;
			};
			CBody m_oBody;
		};

		class OpenTemporaryAlarmRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
			};
			CBody m_oBody;
		};

		//�ر��ݴ澯��
		class CloseTemporaryAlarmRequst :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_alarmId = p_pJson->GetNodeValue("/body/caseid", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_alarmId;
			};
			CBody m_oBody;
		};
	}
}

