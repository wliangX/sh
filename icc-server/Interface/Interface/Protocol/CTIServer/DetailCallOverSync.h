#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CDetailCallOverSync :
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
				
				p_pJson->SetNodeValue("/body/switch_type", m_oBody.m_strSwitchType);
				p_pJson->SetNodeValue("/body/device", m_oBody.m_strDeviceNum);
				p_pJson->SetNodeValue("/body/device_type", m_oBody.m_strDeviceType);
				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallId);
				p_pJson->SetNodeValue("/body/relation_callrefid", m_oBody.m_strRelationCallId);
				
				p_pJson->SetNodeValue("/body/caller_id", m_oBody.m_strCaller);
				p_pJson->SetNodeValue("/body/caller_type", m_oBody.m_strCallerType);
				p_pJson->SetNodeValue("/body/called_id", m_oBody.m_strCalled);
				p_pJson->SetNodeValue("/body/called_type", m_oBody.m_strCalledType);
				
				p_pJson->SetNodeValue("/body/detail_type", m_oBody.m_strDetailType);
				p_pJson->SetNodeValue("/body/role", m_oBody.m_strRole);
				
				p_pJson->SetNodeValue("/body/ring_time", m_oBody.m_strRingTime);
				p_pJson->SetNodeValue("/body/talk_time", m_oBody.m_strTalkTime);
				p_pJson->SetNodeValue("/body/join_time", m_oBody.m_strJoinTime);
				p_pJson->SetNodeValue("/body/hangup_time", m_oBody.m_strHangupTime);
				
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strSwitchType = p_pJson->GetNodeValue("/body/switch_type", "");
				m_oBody.m_strDeviceNum = p_pJson->GetNodeValue("/body/device", "");
				m_oBody.m_strDeviceType = p_pJson->GetNodeValue("/body/device_type", "");
				m_oBody.m_strCallId = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strRelationCallId = p_pJson->GetNodeValue("/body/relation_callrefid", "");
				
				m_oBody.m_strCaller = p_pJson->GetNodeValue("/body/caller_id", "");
				m_oBody.m_strCallerType = p_pJson->GetNodeValue("/body/caller_type", "");
				m_oBody.m_strCalled = p_pJson->GetNodeValue("/body/called_id", "");
				m_oBody.m_strCalledType = p_pJson->GetNodeValue("/body/called_type", "");
				
				m_oBody.m_strDetailType = p_pJson->GetNodeValue("/body/detail_type", "");
				m_oBody.m_strRole = p_pJson->GetNodeValue("/body/role", "");
				
				m_oBody.m_strRingTime = p_pJson->GetNodeValue("/body/ring_time", "");
				m_oBody.m_strTalkTime = p_pJson->GetNodeValue("/body/talk_time", "");
				m_oBody.m_strJoinTime = p_pJson->GetNodeValue("/body/join_time", "");
				m_oBody.m_strHangupTime = p_pJson->GetNodeValue("/body/hangup_time", "");
				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strSwitchType;		//交换机类型
				std::string m_strDeviceNum;			//设备号
				std::string m_strDeviceType;		//设备类型
				std::string m_strCallId;			//话务Id
				std::string m_strRelationCallId;	//源呼叫ID 关联话务ID
				std::string m_strCaller;			//主叫
				std::string m_strCallerType;			//主叫类型
				std::string m_strCalled;			//被叫
				std::string m_strCalledType;		//被叫类型
				
				std::string m_strDetailType;		//子话务类型,
				std::string m_strRole;				//角色 1主叫；2被叫；3会议主持人；4会议成员；5转出方；6转入方；7强插；8代答
					
				std::string m_strRingTime;			//振铃时间
				std::string m_strTalkTime;			//通话时间
				std::string m_strJoinTime;			//加入时间
				std::string m_strHangupTime;		//挂机时间
			};
			CBody m_oBody;
		};
	}
}
