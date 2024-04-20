#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchCallWaitingRespond :
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

					p_pJson->SetNodeValue(l_strPrefixPath + "callref_id", data.m_strCallRefId);
					p_pJson->SetNodeValue(l_strPrefixPath + "acd", data.m_strAcd);
					p_pJson->SetNodeValue(l_strPrefixPath + "original_acd", data.m_strOriginalAcd);
					p_pJson->SetNodeValue(l_strPrefixPath + "acd_dept", data.m_strAcdDept);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_number", data.m_strCallerNumber);					
					p_pJson->SetNodeValue(l_strPrefixPath + "wait_start_time", data.m_strWaitStartTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "wait_time_len", data.m_strWaitLen);
					p_pJson->SetNodeValue(l_strPrefixPath + "hangup_type", data.m_strHangupType);					

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				std::string	m_strCallRefId;			
				std::string m_strAcd;
				std::string m_strOriginalAcd;
				std::string m_strAcdDept;
				std::string	m_strCallerNumber;				
				std::string m_strWaitStartTime;
				std::string m_strWaitLen;
				std::string m_strHangupType;				
			};
			class CBody
			{
			public:
				std::string m_strCount;						
				std::string m_strAllCount;				
				std::vector<CData> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}