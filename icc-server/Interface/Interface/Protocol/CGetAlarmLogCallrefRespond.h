#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAlarmLogCallrefRespond :
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
				p_pJson->SetNodeValue("/body/data/callref_id", m_oBody.m_oCallEvent.m_strCallrefID);
				p_pJson->SetNodeValue("/body/data/acd", m_oBody.m_oCallEvent.m_strACD);
				p_pJson->SetNodeValue("/body/data/caller_id", m_oBody.m_oCallEvent.m_strCallerID);
				p_pJson->SetNodeValue("/body/data/called_id", m_oBody.m_oCallEvent.m_strCalledID);
				p_pJson->SetNodeValue("/body/data/call_direction", m_oBody.m_oCallEvent.m_strCallDirection);
				p_pJson->SetNodeValue("/body/data/talk_time", m_oBody.m_oCallEvent.m_strTalkTime);
				p_pJson->SetNodeValue("/body/data/hangup_time", m_oBody.m_oCallEvent.m_strHangupTime);
				p_pJson->SetNodeValue("/body/data/hangup_type", m_oBody.m_oCallEvent.m_strHangupType);

				unsigned int l_uiIndex = 0;
				for (auto subcallevent : m_oBody.m_vecSubCallEvent)
				{
					std::string l_strPrefixPath("/body/data/sub_call_callevent_data/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "eventcall_state", subcallevent.m_strEventCallState);
					p_pJson->SetNodeValue(l_strPrefixPath + "state_time", subcallevent.m_strStateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "sponsor", subcallevent.m_strSponsor);
					p_pJson->SetNodeValue(l_strPrefixPath + "receiver", subcallevent.m_strReceiver);
					l_uiIndex++;
				}

				return p_pJson->ToString();
			}


		public:
			class CCALLEVENT
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
			class CSUBCALLEVENT
			{
			public:
				std::string m_strEventCallState;	//话务状态
				std::string m_strStateTime;			//时间
				std::string m_strSponsor;			//发起方
				std::string m_strReceiver;			//接收方			
			};
			class CBody
			{
			public:
				CCALLEVENT m_oCallEvent;
				std::vector<CSUBCALLEVENT> m_vecSubCallEvent;
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}
