/*
敏感警情同步消息
*/
#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CKeyWordAlarm.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CKeyWordAlarmQryRequest :
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

				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");

				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				return true;
			}


		public:
			class CBody
			{
			public:
				std::string m_strPageSize;  //每页数量
				std::string  m_strPageIndex;  //第几页
				std::string m_strDeptCode;  //部门编码

				std::string m_strBeginTime;
				std::string m_strEndTime;
			};

			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
