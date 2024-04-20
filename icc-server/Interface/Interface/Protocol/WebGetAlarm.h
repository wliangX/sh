#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebGetAlarm :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/total_count", m_oBody.m_strTotalCount);
				p_pJson->SetNodeValue("/body/state_id", m_oBody.m_strStateID);

				unsigned int l_uiAlarmIndex = 0;
				unsigned int l_uiProcessIndex = 0;
				unsigned int l_uiRemarkIndex = 0;
				unsigned int l_uiFeedbackIndex = 0;
				for (auto alarm : m_oBody.m_vecAlarm)
				{
					std::string l_strAlarmPrefixPath("/body/alarm/" + std::to_string(l_uiAlarmIndex) + "/");

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "id", alarm.m_strID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "content", alarm.m_strContent);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "time", alarm.m_strTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "addr", alarm.m_strAddr);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "state", alarm.m_strState);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "level", alarm.m_strLevel);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "first_type", alarm.m_strFirstType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "second_type", alarm.m_strSecondType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "third_type", alarm.m_strThirdType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "fourth_type", alarm.m_strFourthType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "called_no_type", alarm.m_strCalledNoType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_no", alarm.m_strContactNo);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_name", alarm.m_strContactName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_name", alarm.m_strReceiptName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "source_type", alarm.m_strSourceType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "source_id", alarm.m_strSourceId);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_code", alarm.m_strReceiptDeptCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_name", alarm.m_strReceiptDeptName);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "process_id", alarm.m_alarmProcess.m_strID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "process_state", alarm.m_alarmProcess.m_strState);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "time_signed", alarm.m_alarmProcess.m_strTimeSigned);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "time_feedback", alarm.m_alarmProcess.m_strTimeFeedBack);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "process_dept_code", alarm.m_alarmProcess.m_strProcessDeptCode);

					l_uiAlarmIndex++;
				}

				return p_pJson->ToString();
			}
			public:
			class CAlarmProcess
			{
			public:
				std::string m_strID;						//处警ID
				std::string m_strAlarmID;					//警情ID
				std::string m_strState;						//处警单状态
				std::string m_strTimeSigned;				//处警单位签收时间
				std::string m_strTimeFeedBack;				//处警单位反馈时间
				std::string m_strProcessDeptCode;			//处警单位代码	
			};
			class CAlarm
			{
			public:
				std::string	m_strID;						//警情id
				std::string m_strContent;					//警情内容
				std::string m_strTime;						//警情id报警时间			
				std::string m_strAddr;						//警情id详细发生地址
				std::string m_strState;						//警情id状态
				std::string m_strLevel;						//警情级别
				std::string m_strSourceType;				//警情报警来源类型
				std::string m_strSourceId;					//警情报警来源ID
				std::string m_strReceiptDeptCode;			//警情接警单位编码
				std::string m_strReceiptDeptName;			//警情接警单位名称
				std::string m_strFirstType;					//警情id一级类型
				std::string m_strSecondType;				//警情id二级类型
				std::string m_strThirdType;					//警情id三级类型
				std::string m_strFourthType;				//警情id四级类型
				std::string m_strCalledNoType;				//警情id报警号码字典类型
				std::string m_strContactNo;					//警情id联系人号码
				std::string m_strContactName;				//警情id联系人姓名
				std::string m_strReceiptName;				//警情id接警人姓名	
				CAlarmProcess m_alarmProcess;				//处警信息
			};
			class CBody
			{
			public:
				std::string m_strResult;
				std::string m_strTotalCount;						// 记录总数
				std::string m_strStateID;							// 状态类型
				std::vector<CAlarm> m_vecAlarm;						// 警情信息
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};

	}
}