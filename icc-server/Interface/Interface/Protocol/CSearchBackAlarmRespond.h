#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchBackAlarmRespond :
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

				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);				

				unsigned int l_uiIndex = 0;
				for (CData data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", data.m_strAlarmId);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_id", data.m_strProcessId);
					p_pJson->SetNodeValue(l_strPrefixPath + "back_user_code", data.m_strBackUserCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "back_user_name", data.m_strBackUserName);
					p_pJson->SetNodeValue(l_strPrefixPath + "back_time", data.m_strBackTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "back_dept_code", data.m_strBackDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "back_dept_name", data.m_strBackDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "back_reason", data.m_strBackReason);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				std::string	m_strAlarmId;			
				std::string m_strProcessId;				
				std::string	m_strBackUserCode;	
				std::string m_strBackUserName;
				std::string m_strBackTime;
				std::string m_strBackDeptCode;
				std::string m_strBackDeptName;
				std::string m_strBackReason;				
			};
			class CBody
			{
			public:
				std::string m_strCount;						
				std::string m_strAllCount;				
				std::vector<CData> m_vecData;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}