#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class COnOffSync :
			public ISync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/client_id", m_oBody.m_strClientId);
				p_pJson->SetNodeValue("/body/client_name", m_oBody.m_strClientName);
				p_pJson->SetNodeValue("/body/on_time", m_oBody.m_strOnTime);
				p_pJson->SetNodeValue("/body/off_time", m_oBody.m_strOffTime);
				p_pJson->SetNodeValue("/body/duration", m_oBody.m_strDuration);
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strClientId;		        //�û�ID
				std::string m_strClientName;		    //�û���
				std::string m_strOnTime;			    //����ʱ��
				std::string m_strOffTime;               //����ʱ��
				std::string m_strDuration;               //����ʱ��
			};
			CBody m_oBody;
		};
	}
}
