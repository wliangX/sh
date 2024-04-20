#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CVcsQueryCallInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_strIsRecursive = p_pJson->GetNodeValue("/body/is_recursive", "");
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");

				m_oBody.m_strCallNumber = p_pJson->GetNodeValue("/body/call_number", "");
				m_oBody.m_strCallType = p_pJson->GetNodeValue("/body/call_type", "");
				m_oBody.m_strAlarmId = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strAccpectState = p_pJson->GetNodeValue("/body/accept_state", "");

				m_oBody.m_strIsNeedPaging = p_pJson->GetNodeValue("/body/is_need_paging", "");
				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				m_oBody.m_strTalkingTimeMin = p_pJson->GetNodeValue("/body/talking_time_len_min", "");
				m_oBody.m_strTalkingTimeMax = p_pJson->GetNodeValue("/body/talking_time_len_max", "");
				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strDeptCode;	
				std::string m_strIsRecursive;
				std::string m_strBeginTime;
				std::string m_strEndTime;

				std::string m_strCallNumber;
				std::string m_strCallType;
				std::string m_strAlarmId;
				std::string m_strAccpectState;
				
				std::string m_strIsNeedPaging;
				std::string m_strPageSize;
				std::string m_strPageIndex;
				std::string m_strTalkingTimeMin;
				std::string m_strTalkingTimeMax;
			};

			CBody m_oBody;
		};
	}
}
