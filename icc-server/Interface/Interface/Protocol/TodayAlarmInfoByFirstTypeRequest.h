#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTodayAlarmInfoByFirstTypeRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");			
				m_oBody.m_strFirstType = p_pJson->GetNodeValue("/body/first_type", "");
				m_oBody.m_strIsRecursive = p_pJson->GetNodeValue("/body/is_recursive", "");
				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");

				return true;
			}

		public:

			class CBody
			{
			public:				
				std::string	m_strBeginTime;					//查询开始时间（不能为空）
				std::string	m_strEndTime;					//查询结束时间（不能为空）
				std::string	m_strDeptCode;				    //查询部门编码		
				std::string m_strFirstType;                //查询一级类型，警情类别代码
				std::string m_strIsRecursive;        		//0：不包含下级部门；1：包含下级部门
				std::string m_strPageSize;
				std::string m_strPageIndex;
			};

			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}