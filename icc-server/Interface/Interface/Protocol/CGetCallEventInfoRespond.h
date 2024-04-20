#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetCallEventInfoRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallrefID);
				p_pJson->SetNodeValue("/body/acd", m_oBody.m_strACD);
				p_pJson->SetNodeValue("/body/caller_id", m_oBody.m_strCallerID);
				p_pJson->SetNodeValue("/body/called_id", m_oBody.m_strCalledID);
				p_pJson->SetNodeValue("/body/call_direction", m_oBody.m_strCallDirection);
				p_pJson->SetNodeValue("/body/talk_time", m_oBody.m_strTalkTime);
				p_pJson->SetNodeValue("/body/hangup_time", m_oBody.m_strHangupTime);
				p_pJson->SetNodeValue("/body/hangup_type", m_oBody.m_strHangupType);

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCallrefID;			//话务号
				std::string m_strACD;				//ACD 组名称（110、119、120、122）
				std::string m_strCallerID;			//主叫
				std::string m_strCalledID;			//被叫
				std::string m_strCallDirection;		//呼叫方向
				std::string m_strTalkTime;			//通话时间：YYYY - mm - dd HH24 : MM : SS
				std::string m_strHangupTime;		//挂机时间：YYYY - mm - dd HH24 : MM : SS
				std::string m_strHangupType;		//挂机类型(TimeOut, Caller, Called)					
			};
			CBody m_oBody;
		};
	}
}
