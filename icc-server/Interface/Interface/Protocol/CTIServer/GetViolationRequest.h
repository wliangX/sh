#pragma once
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CGetViolationRequest:
			public IRequest
		{

		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				
				m_oBody.m_strStartTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_strViolationType = p_pJson->GetNodeValue("/body/violation_type", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strPageSize;
				std::string m_strPageIndex;
				
				std::string m_strStartTime;
				std::string m_strEndTime;
				std::string m_strViolationType;
				std::string m_strDeptCode;
			};
			CBody m_oBody;
		};
	}
}
