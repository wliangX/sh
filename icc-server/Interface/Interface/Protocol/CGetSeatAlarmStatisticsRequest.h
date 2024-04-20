/*
警情统计请求消息
*/
#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>


namespace ICC
{
	namespace PROTOCOL
	{
		class CGetSeatAlarmStatisticsRequest :
			public IRequest
		{
		public:
			
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
                m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				return true;
			}


		public:
			class CBody
			{
			public:
				std::string m_strBeginTime;  //开始时间
				std::string  m_strEndTime;  //结束时间，允许没有
			};

			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
