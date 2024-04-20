#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchStatisticInfoRequest :
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
				m_oBody.m_strClientID = p_pJson->GetNodeValue("/body/client_id", "");
				m_oBody.m_strClientName = p_pJson->GetNodeValue("/body/client_name", "");
				m_oBody.m_strCurDeptCode = p_pJson->GetNodeValue("/body/curr_dept_code", "");

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
				std::string	m_strClientID;	//坐席号
				std::string m_strClientName;//接警员
				std::string m_strCurDeptCode;//当前用户所在单位编码(只可查询该部门及其下级部门的信息)
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}