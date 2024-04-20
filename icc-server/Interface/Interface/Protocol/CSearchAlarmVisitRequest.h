#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

//回访单查询类型
enum ESearchType
{
	BY_DEPTCODE = 1,
	BY_ALARMID,
	BY_ID
};

namespace ICC
{
    namespace PROTOCOL
    {
		class CSearchAlarmVisitRequest :
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
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");

				if (!m_oBody.m_strDeptCode.empty())
				{
					m_oBody.m_eSearchType = ESearchType::BY_DEPTCODE;
					m_oBody.m_strRespCMD = "search_alarm_visit_respond";
				}
				else if (!m_oBody.m_strAlarmID.empty())
				{
					m_oBody.m_eSearchType = ESearchType::BY_ALARMID;
					m_oBody.m_strRespCMD = "get_alarm_visit_by_alarmid_respond";
				}
				else
				{
					m_oBody.m_eSearchType = ESearchType::BY_ID;
					m_oBody.m_strRespCMD = "get_alarm_visit_by_id_respond";
				}
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
				std::string m_strAlarmID;			//警情ID
				std::string m_strID;				//回访单ID
				ESearchType m_eSearchType;		//查询类型
				std::string m_strRespCMD;
            };
            CHeaderEx m_oHeader;
            CBody m_oBody;
        };
    }
}
