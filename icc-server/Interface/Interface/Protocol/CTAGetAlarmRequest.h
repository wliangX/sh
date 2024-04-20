#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAGetAlarmRequest :
			public IReceive
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
				m_oBody.m_strCurrDeptCode = p_pJson->GetNodeValue("/body/curr_dept_code", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_strKeyword = p_pJson->GetNodeValue("/body/keyword", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strPageSize;			//每页数量（不能为空）
				std::string m_strPageIndex;			//页码，1表示第一页（不能为空）
				std::string m_strBeginTime;			//开始时间
				std::string m_strEndTime;			//结束时间
				std::string m_strCurrDeptCode;		//部门编码（查询结果需要包含下级部门）
				std::string m_strState;				//空值取全部
				std::string m_strKeyword;			//关键字搜索
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};		
	}
}
