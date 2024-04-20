#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchCallWaitingRequest :
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
				m_oBody.m_strCallerId = p_pJson->GetNodeValue("/body/caller_id", "");
				m_oBody.m_strCalledId = p_pJson->GetNodeValue("/body/called_id", "");
				m_oBody.m_strHangupType = p_pJson->GetNodeValue("/body/hangup_type", "");
				m_oBody.m_strWaitlen = p_pJson->GetNodeValue("/body/wait_len", "");

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
				std::string m_strCallerId;					//主叫号
				std::string m_strCalledId;					// 被叫号
				std::string m_strHangupType;				// 挂断类型
				std::string m_strWaitlen;					// 排队时长

			};


			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}