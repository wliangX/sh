/*
敏感警情同步消息
*/
#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CKeyWordAlarm.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CKeyWordAlarmQryRespond :
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
				std::string l_strPrefixPath("/body/");
				
				p_pJson->SetNodeValue(l_strPrefixPath + "all_count", m_oBody.m_strAllCount);
				p_pJson->SetNodeValue(l_strPrefixPath + "count", m_oBody.m_strCount);
				
				unsigned int tmp_uiSize =  m_oBody.m_vecKeyWordAlarm.size();
				
				l_strPrefixPath += "data/";
				
				for (unsigned int i =0; i < tmp_uiSize; ++i)
				{
					std::string l_strIndex = std::to_string(i);

				    p_pJson->SetNodeValue(l_strPrefixPath +l_strIndex+ "/guid", m_oBody.m_vecKeyWordAlarm[i].m_strGuid);

					p_pJson->SetNodeValue(l_strPrefixPath +l_strIndex+ "/alarm_id", m_oBody.m_vecKeyWordAlarm[i].m_strAlarmID);
				
					p_pJson->SetNodeValue(l_strPrefixPath + l_strIndex+"/key_word", m_oBody.m_vecKeyWordAlarm[i].m_strKeyWord);
				
					p_pJson->SetNodeValue(l_strPrefixPath + l_strIndex+"/key_content", m_oBody.m_vecKeyWordAlarm[i].m_strKeyContent);
				
					p_pJson->SetNodeValue(l_strPrefixPath +l_strIndex + "/alarm_content", m_oBody.m_vecKeyWordAlarm[i].m_strAlarmContent);
				
				}

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
			    std::string m_strAllCount;
				std::string m_strCount;
				std::vector<CKeyWordAlarm> m_vecKeyWordAlarm;
			};

			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
