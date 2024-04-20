#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CMajorAlarmConfigSync : public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				//p_pJson->SetNodeValue("/header/cmd",m_oHeader.m_strCmd);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_data.m_guid);
				p_pJson->SetNodeValue("/body/type_name", m_oBody.m_data.m_type_name);
				p_pJson->SetNodeValue("/body/type_value", m_oBody.m_data.m_type_value);
				p_pJson->SetNodeValue("/body/remark", m_oBody.m_data.m_remark);
				p_pJson->SetNodeValue("/body/update_type", m_oBody.m_update_type);

				return p_pJson->ToString();
			}


		public:
			CHeader m_oHeader;			

			class CBody
			{
			public:
				CMajorAlarmConfigData m_data;
				std::string m_update_type;
			};
			CBody m_oBody;
		};
	}
}
