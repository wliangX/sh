#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebGetAlarmDetailRespond :
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
				
				unsigned int l_uiAlarmIndex = 0;
				unsigned int l_uiProcessIndex = 0;
				unsigned int l_uiRemarkIndex = 0;
				unsigned int l_uiFeedbackIndex = 0;
				unsigned int l_uiAlarmLogIndex = 0;
				for (auto alarm : m_oBody.m_vecAlarm)
				{
					std::string l_strAlarmPrefixPath("/body/alarm/" + std::to_string(l_uiAlarmIndex) + "/");

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "id", alarm.m_strID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "merge_id", alarm.m_strMergeID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "title", alarm.m_strTitle);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "content", alarm.m_strContent);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "time", alarm.m_strTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "actual_occur_time", alarm.m_strActualOccurTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "addr", alarm.m_strAddr);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "longitude", alarm.m_strLongitude);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "latitude", alarm.m_strLatitude);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "state", alarm.m_strState);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "level", alarm.m_strLevel);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "source_type", alarm.m_strSourceType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "source_id", alarm.m_strSourceID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "handle_type", alarm.m_strHandleType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "first_type", alarm.m_strFirstType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "second_type", alarm.m_strSecondType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "third_type", alarm.m_strThirdType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "fourth_type", alarm.m_strFourthType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "vehicle_no", alarm.m_strVehicleNo);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "vehicle_type", alarm.m_strVehicleType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "symbol_code", alarm.m_strSymbolCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "symbol_addr", alarm.m_strSymbolAddr);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "fire_building_type", alarm.m_strFireBuildingType);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "event_type", alarm.m_strEventType);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "called_no_type", alarm.m_strCalledNoType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "actual_called_no_type", alarm.m_strActualCalledNoType);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_no", alarm.m_strCallerNo);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_name", alarm.m_strCallerName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_addr", alarm.m_strCallerAddr);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_id", alarm.m_strCallerID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_id_type", alarm.m_strCallerIDType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_gender", alarm.m_strCallerGender);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_age", alarm.m_strCallerAge);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_birthday", alarm.m_strCallerBirthday);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_no", alarm.m_strContactNo);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_name", alarm.m_strContactName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_addr", alarm.m_strContactAddr);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_id", alarm.m_strContactID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_id_type", alarm.m_strContactIDType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_gender", alarm.m_strContactGender);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_age", alarm.m_strContactAge);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_birthday", alarm.m_strContactBirthday);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "admin_dept_district_code", alarm.m_strAdminDeptDistrictCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "admin_dept_code", alarm.m_strAdminDeptCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "admin_dept_name", alarm.m_strAdminDeptName);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_district_code", alarm.m_strReceiptDeptDistrictCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_code", alarm.m_strReceiptDeptCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_name", alarm.m_strReceiptDeptName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "leader_code", alarm.m_strLeaderCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "leader_name", alarm.m_strLeaderName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_code", alarm.m_strReceiptCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_name", alarm.m_strReceiptName);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "dispatch_suggestion", alarm.m_strDispatchSuggestion);

					// ��l_uiAlarmIndex+1���Ӿ�����Ӧ�Ĵ������б�
					for (auto process : m_oBody.m_vecAlarmProcess.at(l_uiAlarmIndex))
					{
						std::string l_strPrefixPathProcess("/body/alarm_process/" + std::to_string(l_uiProcessIndex) + "/");
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "id", process.m_strID);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "alarm_id", process.m_strAlarmID);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "state", process.m_strState);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_edit", process.m_strTimeEdit);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_submit", process.m_strTimeSubmit);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_arrived", process.m_strTimeArrived);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_signed", process.m_strTimeSigned);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_feedback", process.m_strTimeFeedBack);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "is_need_feedback", process.m_strIsNeedFeedback);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_district_code", process.m_strDispatchDeptDistrictCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_code", process.m_strDispatchDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_name", process.m_strDispatchDeptName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_code", process.m_strDispatchCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_name", process.m_strDispatchName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_leader_code", process.m_strDispatchLeaderCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_leader_name", process.m_strDispatchLeaderName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_suggestion", process.m_strDispatchSuggestion);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_leader_instruction", process.m_strDispatchLeaderInstruction);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_district_code", process.m_strProcessDeptDistrictCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_code", process.m_strProcessDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_name", process.m_strProcessDeptName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_code", process.m_strProcessCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_name", process.m_strProcessName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_leader_code", process.m_strProcessLeaderCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_leader_name", process.m_strProcessLeaderName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_feedback", process.m_strProcessFeedback);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_leader_instruction", process.m_strProcessLeaderInstruction);
						l_uiProcessIndex++;
					}
					
					l_uiAlarmIndex++;
				}
				for (auto alarmLog : m_oBody.m_vecAlarmLog)
				{
					std::string l_strAlarmPrefixPath("/body/alarm_log/" + std::to_string(l_uiAlarmLogIndex) + "/");

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "alarm_id", alarmLog.m_strAlarmID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "code", alarmLog.m_strCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "content", alarmLog.m_strContent);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "resource_type", alarmLog.m_strResourceType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "resource_id", alarmLog.m_strResourceId);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "create_user", alarmLog.m_strCreateUser);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "create_time", alarmLog.m_strCreateTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "update_user", alarmLog.m_strUpdateUser);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "update_time", alarmLog.m_strUpdateTime);

					l_uiAlarmLogIndex++;
				}

				return p_pJson->ToString();
			}

		public:
			class CAlarm
			{
			public:
				std::string	m_strID;						//����id
				std::string	m_strMergeID;					//����ϲ�id
				std::string m_strTitle;						//�������
				std::string m_strContent;					//��������
				std::string m_strTime;						//����id����ʱ��			
				std::string m_strActualOccurTime;			//����idʵ�ʷ���ʱ��
				std::string m_strAddr;						//����id��ϸ������ַ
				std::string m_strLongitude;					//����id����
				std::string m_strLatitude;					//����idά��
				std::string m_strState;						//����id״̬
				std::string m_strLevel;						//����id����
				std::string m_strSourceType;				//����id������Դ����
				std::string m_strSourceID;					//����id������Դid
				std::string m_strHandleType;				//����id��������
				std::string m_strFirstType;					//����idһ������
				std::string m_strSecondType;				//����id��������
				std::string m_strThirdType;					//����id��������
				std::string m_strFourthType;				//����id�ļ�����
				std::string m_strVehicleNo;					//����id��ͨ���ͱ������ƺ�
				std::string m_strVehicleType;				//����id��ͨ���ͱ���������
				std::string m_strSymbolCode;				//����id������ַ������ʾ����
				std::string m_strSymbolAddr;				//����id������ַ������ʾ���ַ
				std::string m_strFireBuildingType;			//����id������ȼ�ս�������

				std::string m_strEventType;					//�¼����ͣ����Ÿ���

				std::string m_strCalledNoType;				//����id���������ֵ�����
				std::string m_strActualCalledNoType;		//����idʵ�ʱ��������ֵ�����

				std::string m_strCallerNo;					//����id�����˺���
				std::string m_strCallerName;				//����id����������
				std::string m_strCallerAddr;				//����id�����˵�ַ
				std::string m_strCallerID;					//����id���������֤
				std::string m_strCallerIDType;				//����id���������֤����
				std::string m_strCallerGender;				//����id�������Ա�
				std::string m_strCallerAge;					//����id����������
				std::string m_strCallerBirthday;			//����id�����˳���������

				std::string m_strContactNo;					//����id��ϵ�˺���
				std::string m_strContactName;				//����id��ϵ������
				std::string m_strContactAddr;				//����id��ϵ�˵�ַ
				std::string m_strContactID;					//����id��ϵ�����֤
				std::string m_strContactIDType;				//����id��ϵ�����֤����
				std::string m_strContactGender;				//����id��ϵ���Ա�
				std::string m_strContactAge;				//����id��ϵ������
				std::string m_strContactBirthday;			//����id��ϵ�˳���������

				std::string m_strAdminDeptDistrictCode;		//����id��Ͻ��λ��������
				std::string m_strAdminDeptCode;				//����id��Ͻ��λ����
				std::string m_strAdminDeptName;				//����id��Ͻ��λ����

				std::string m_strReceiptDeptDistrictCode;	//����id�Ӿ���λ��������
				std::string m_strReceiptDeptCode;			//����id�Ӿ���λ����
				std::string m_strReceiptDeptName;			//����id�Ӿ���λ����
				std::string m_strLeaderCode;				//����idֵ���쵼����
				std::string m_strLeaderName;				//����idֵ���쵼����
				std::string m_strReceiptCode;				//����id�Ӿ��˾���
				std::string m_strReceiptName;				//����id�Ӿ�������

				std::string m_strDispatchSuggestion;		//�������		
			};
			class CAlarmProcess
			{
			public:
				std::string m_strID;						//����ID
				std::string m_strAlarmID;					//����ID
				std::string m_strState;						//������״̬
				std::string m_strTimeEdit;					//�ɾ�����дʱ��
				std::string m_strTimeSubmit;				//�ɾ����ύʱ��
				std::string m_strTimeArrived;				//�ɾ���ϵͳ����ʱ��
				std::string m_strTimeSigned;				//������λǩ��ʱ��
				std::string m_strTimeFeedBack;				//������λ����ʱ��
				std::string m_strIsNeedFeedback;			//�Ƿ���Ҫ����
				std::string m_strDispatchDeptDistrictCode;	//�ɾ���λ��������
				std::string m_strDispatchDeptCode;			//�ɾ���λ����
				std::string m_strDispatchDeptName;			//�ɾ���λ����
				std::string m_strDispatchCode;				//�ɾ��˾���
				std::string m_strDispatchName;				//�ɾ�������
				std::string m_strDispatchLeaderCode;		//�ɾ���λֵ���쵼����
				std::string m_strDispatchLeaderName;		//�ɾ���λֵ���쵼����
				std::string m_strDispatchSuggestion;		//�ɾ���λ�ɾ����
				std::string m_strDispatchLeaderInstruction;	//�ɾ���λ�쵼ָʾ
				std::string m_strProcessDeptDistrictCode;	//������λ��������
				std::string m_strProcessDeptCode;			//������λ����
				std::string m_strProcessDeptName;			//������λ����
				std::string m_strProcessCode;				//�����˾���
				std::string m_strProcessName;				//����������
				std::string m_strProcessLeaderCode;			//������λֵ���쵼����
				std::string m_strProcessLeaderName;			//������λֵ���쵼����
				std::string m_strProcessFeedback;			//��������
				std::string m_strProcessLeaderInstruction;	//������λ�쵼ָʾ		
			};	
			class CAlarmLog
			{
			public:
				std::string m_strAlarmID;					//����ID
				std::string m_strCode;						//��ˮ����
				std::string m_strContent;					//������ˮ�����б�
				std::string m_strResourceType;				//1.�Ӿ�2.����3.����4.��ע5.IP�绰¼��6.pdt¼��
				std::string m_strResourceId;				//������ˮ��Դid
				std::string m_strCreateUser;				//������
				std::string m_strCreateTime;				//����ʱ��
				std::string m_strUpdateUser;			    //������
				std::string m_strUpdateTime;	            //����ʱ��
			};
			class CBody
			{
			public:
				std::string m_strResult;	//0-�ɹ���1-ʧ��
				std::vector<CAlarm> m_vecAlarm;
				std::vector<std::vector<CAlarmProcess>> m_vecAlarmProcess;
				std::vector<CAlarmLog> m_vecAlarmLog;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}