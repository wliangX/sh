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
				std::string m_strCallrefID;			//�����
				std::string m_strACD;				//ACD �����ƣ�110��119��120��122��
				std::string m_strCallerID;			//����
				std::string m_strCalledID;			//����
				std::string m_strCallDirection;		//���з���
				std::string m_strTalkTime;			//ͨ��ʱ�䣺YYYY - mm - dd HH24 : MM : SS
				std::string m_strHangupTime;		//�һ�ʱ�䣺YYYY - mm - dd HH24 : MM : SS
				std::string m_strHangupType;		//�һ�����(TimeOut, Caller, Called)					
			}; 
			class CSUBCALLEVENT
			{
			public:
				std::string m_strEventCallState;	//����״̬
				std::string m_strStateTime;			//ʱ��
				std::string m_strSponsor;			//����
				std::string m_strReceiver;			//���շ�			
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
