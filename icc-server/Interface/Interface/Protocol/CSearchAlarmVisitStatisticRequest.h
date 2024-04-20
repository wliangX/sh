#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
    namespace PROTOCOL
    {
		class CSearchAlarmVisitStatisticRequest :
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
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");

                return true;
            }

        public:
            class CBody
            {
            public:
				std::string m_strPageSize;			//每页数量
				std::string m_strPageIndex;			//页码，1表示第一页
				std::string m_strBeginTime;			//开始时间
				std::string m_strEndTime;			//结束时间
				std::string m_strDeptCode;			//警情归属单位编码
            };
            CHeaderEx m_oHeader;
            CBody m_oBody;
        };
    }
}
