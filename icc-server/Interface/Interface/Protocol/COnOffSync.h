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
				std::string m_strClientId;		        //用户ID
				std::string m_strClientName;		    //用户名
				std::string m_strOnTime;			    //上线时间
				std::string m_strOffTime;               //下线时间
				std::string m_strDuration;               //在线时长
			};
			CBody m_oBody;
		};
	}
}
