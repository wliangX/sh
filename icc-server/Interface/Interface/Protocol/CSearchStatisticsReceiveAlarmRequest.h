#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchStatisticsReceiveAlarmRequest :
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
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");				
				m_oBody.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");				;
				m_oBody.m_strStaffDeptCode = p_pJson->GetNodeValue("/body/staff_dept_code", "");
				return true;
			}

		public:

			class CBody
			{
			public:
				std::string	m_strPageSize;					//每页数量（不能为空）
				std::string	m_strPageIndex;					//页码，1表示第一页（不能为空）
				std::string	m_strBeginTime;					//查询开始时间（不能为空）
				std::string	m_strEndTime;					//查询结束时间（不能为空）				
				std::string	m_strReceiptDeptCode;			//警情接警单位编码（递归查询）
				std::string m_strStaffDeptCode;             //接警人部门编码，在接警单位为空时使用，递归查询
			};


			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}