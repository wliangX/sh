#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"


namespace ICC
{
	namespace PROTOCOL
	{
		class CMagorAlarmConfigDeleteRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_data.m_guid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_data.m_type_name = p_pJson->GetNodeValue("/body/type_name", "");
				m_oBody.m_data.m_type_value = p_pJson->GetNodeValue("/body/type_value", "");
				m_oBody.m_data.m_remark = p_pJson->GetNodeValue("/body/remark", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				CMajorAlarmConfigData m_data;
			};
			CBody m_oBody;
		};
	}
}

