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
		class CKeyWordAlarmSync :
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
				p_pJson->SetNodeValue(l_strPrefixPath + "guid", m_oBody.m_oKeyWordAlarm.m_strGuid);

				if (!m_oBody.m_oKeyWordAlarm.m_strAlarmID.empty())
				{
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", m_oBody.m_oKeyWordAlarm.m_strAlarmID);
				}
			
				if (!m_oBody.m_oKeyWordAlarm.m_strKeyWord.empty())
				{
					p_pJson->SetNodeValue(l_strPrefixPath + "key_word", m_oBody.m_oKeyWordAlarm.m_strKeyWord);
				}

				if (!m_oBody.m_oKeyWordAlarm.m_strKeyContent.empty())
				{
					p_pJson->SetNodeValue(l_strPrefixPath + "key_content", m_oBody.m_oKeyWordAlarm.m_strKeyContent);
				}

				if (!m_oBody.m_oKeyWordAlarm.m_strAlarmContent.empty())
				{
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_content", m_oBody.m_oKeyWordAlarm.m_strAlarmContent);
				}

				if (!m_oBody.m_oKeyWordAlarm.m_strDeptCode.empty())
				{
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_code", m_oBody.m_oKeyWordAlarm.m_strDeptCode);
				}

				p_pJson->SetNodeValue(l_strPrefixPath + "sync_type", m_oBody.m_strSyncType);
				
				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				CKeyWordAlarm m_oKeyWordAlarm;
				std::string  m_strSyncType;
			};

			CHeader m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
