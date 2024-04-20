#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CDetailCallQueryRespond :
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

				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strValueNum = std::to_string(i);

					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/dept_code", m_oBody.m_vecData.at(i).m_strDeptCode);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/dept_path", m_oBody.m_vecData.at(i).m_strDeptPath);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/dept_name", m_oBody.m_vecData.at(i).m_strDeptName);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/dept_identify", m_oBody.m_vecData.at(i).m_strDeptIdentify);
					
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/switch_type", m_oBody.m_vecData.at(i).m_strSwitchType);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/device", m_oBody.m_vecData.at(i).m_strDeviceNum);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/device_type", m_oBody.m_vecData.at(i).m_strDeviceType);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/callref_id", m_oBody.m_vecData.at(i).m_strCallId);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/relation_callrefid", m_oBody.m_vecData.at(i).m_strRelationCallId);
					
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/caller_id", m_oBody.m_vecData.at(i).m_strCaller);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/caller_type", m_oBody.m_vecData.at(i).m_strCallerType);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/called_id", m_oBody.m_vecData.at(i).m_strCalled);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/called_type", m_oBody.m_vecData.at(i).m_strCalledType);
					
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/role", m_oBody.m_vecData.at(i).m_strRole);
					
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/setup_time", m_oBody.m_vecData.at(i).m_strSetupTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/ring_time", m_oBody.m_vecData.at(i).m_strRingTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/talk_time", m_oBody.m_vecData.at(i).m_strTalkTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/in_meeting_time", m_oBody.m_vecData.at(i).m_strJoinTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/hangup_time", m_oBody.m_vecData.at(i).m_strHangupTime);
					
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/ring_len", m_oBody.m_vecData.at(i).m_strRingTimeLen);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/talk_len", m_oBody.m_vecData.at(i).m_strTalkTimeLen);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/meeting_len", m_oBody.m_vecData.at(i).m_strMeetingTimeLen);
					
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/record_id", m_oBody.m_vecData.at(i).m_strRecordId);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/create_time", m_oBody.m_vecData.at(i).m_strCreateTime);
					
				}

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCount = "0";
				class CData
				{
				public:
					std::string m_strSwitchType;		//交换机类型
					std::string m_strDeviceNum;			//设备号
					std::string m_strDeviceType;		//设备类型
					
					std::string m_strDeptCode;			//所属部门代码
					std::string m_strDeptPath;			//所属部门路径
					std::string m_strDeptName;			//部门名称
					std::string m_strDeptIdentify;		//部门短码
	
					std::string m_strCallId;			//话务Id
					std::string m_strRelationCallId;	//源呼叫ID 关联话务ID
					std::string m_strCaller;			//主叫
					std::string m_strCallerType;			//主叫类型
					std::string m_strCalled;			//被叫
					std::string m_strCalledType;		//被叫类型
					
					//std::string m_strDetailType;		//子话务类型,
					std::string m_strRole;				//角色 1主叫；2被叫；3会议主持人；4会议成员；5转出方；6转入方；7强插；8代答
					
					std::string m_strSetupTime;
					std::string m_strRingTime;			//振铃时间
					std::string m_strTalkTime;			//通话时间
					std::string m_strJoinTime;			//加入时间
					std::string m_strHangupTime;		//挂机时间
					
					std::string m_strRingTimeLen;			//振铃时长
					std::string m_strTalkTimeLen;			//通话时长
					std::string m_strMeetingTimeLen;		//会议时长

					std::string m_strRecordId;
					
					std::string m_strCreateTime;
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
