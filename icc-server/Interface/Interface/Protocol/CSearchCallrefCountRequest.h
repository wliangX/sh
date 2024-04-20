#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchCallrefCountRequest :
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
				std::string m_strPageSize;
				std::string m_strPageIndex;
				std::string	m_strBeginTime;	//查询开始时间，不能为空
				std::string	m_strEndTime;	//查询开始时间，不能为空
				std::string	m_strDeptCode;	//部门编码，只查询用户部门及下属部门的话务
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}