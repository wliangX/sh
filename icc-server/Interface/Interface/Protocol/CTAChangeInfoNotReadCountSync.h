#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAChangeInfoNotReadCountSync
		{
		public:
			std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				p_pJson->SetNodeValue("/body/third_alarm_guid", m_oBody.m_strThirdAlarmGuid);
				return p_pJson->ToString();
			}
		public:
			class CBody
			{
			public:
				std::string m_strCount;				//结果总数量
				std::string m_strThirdAlarmGuid;	//报警主键
			};
					
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
