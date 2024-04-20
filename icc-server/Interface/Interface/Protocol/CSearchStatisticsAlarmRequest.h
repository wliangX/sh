#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchStatisticsAlarmRequest :
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
				m_oBody.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");		
				m_oBody.m_strStaffDeptCode = p_pJson->GetNodeValue("/body/staff_dept_code", "");
				m_oBody.m_strFirstType = p_pJson->GetNodeValue("/body/first_type", "");

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
				std::string m_strAdminDeptCode;             //辖区部门编码
				std::string m_strStaffDeptCode;             //当前查询人的部门编码，在辖区部门编码为空时使用
				std::string m_strFirstType;
			};


			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}