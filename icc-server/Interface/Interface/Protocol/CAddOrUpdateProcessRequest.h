#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAddOrUpdateProcessRequest :
			public IRequest,public IRespond
		{
		public:

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strMsgSource = p_pJson->GetNodeValue("/body/msg_source", "");

				int l_iCount = p_pJson->GetCount("/body/data");
				for (int i = 0; i<l_iCount; i++)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
					CProcessData l_oData;
					l_oData.m_bIsNewProcess = false;
					l_oData.ParseString(l_strPrefixPath, p_pJson);
					/*
					l_oData.m_strMsgSource = p_pJson->GetNodeValue(l_strPrefixPath + "msg_source", "");
					l_oData.m_strDispatchDeptDistrictCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_district_code", "");
					l_oData.m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "id", "");
					l_oData.m_strAlarmID = p_pJson->GetNodeValue(l_strPrefixPath + "alarm_id", "");
					l_oData.m_strDispatchDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_code", "");
					l_oData.m_strDispatchCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_code", "");
					l_oData.m_strDispatchName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_name", "");
					l_oData.m_strRecordID = p_pJson->GetNodeValue(l_strPrefixPath + "record_id", "");
					l_oData.m_strDispatchSuggestion = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_suggestion", "");
					l_oData.m_strProcessDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_code", "");
					l_oData.m_strTimeSubmit = p_pJson->GetNodeValue(l_strPrefixPath + "time_submit", "");
					l_oData.m_strTimeArrived = p_pJson->GetNodeValue(l_strPrefixPath + "time_arrived", "");
					l_oData.m_strTimeSigned = p_pJson->GetNodeValue(l_strPrefixPath + "time_signed", "");
					l_oData.m_strProcessName = p_pJson->GetNodeValue(l_strPrefixPath + "process_name", "");
					l_oData.m_strProcessCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_code", "");
					l_oData.m_strDispatchPersonnel = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_personnel", "");
					l_oData.m_strDispatchVehicles = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_vehicles", "");
					l_oData.m_strDispatchBoats = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_boats", "");
					l_oData.m_strState = p_pJson->GetNodeValue(l_strPrefixPath + "state", "");
					l_oData.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "create_time", "");
					l_oData.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "update_time", "");
					l_oData.m_strDispatchDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_name", "");
					l_oData.m_strDispatchDeptOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_org_code", "");
					l_oData.m_strProcessDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_name", "");
					l_oData.m_strProcessDeptOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_org_code", "");
					l_oData.m_strProcessObjectType = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_type", "");
					l_oData.m_strProcessObjectName = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_name", "");
					l_oData.m_strProcessObjectCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_code", "");
					l_oData.m_strBusinessStatus = p_pJson->GetNodeValue(l_strPrefixPath + "business_status", "");
					l_oData.m_strSeatCode = p_pJson->GetNodeValue(l_strPrefixPath + "seat_code", "");
					l_oData.m_strCancelTime = p_pJson->GetNodeValue(l_strPrefixPath + "cancel_time", "");
					l_oData.m_strCancelReason = p_pJson->GetNodeValue(l_strPrefixPath + "cancel_reason", "");
					l_oData.m_strIsAutoAssignJob = p_pJson->GetNodeValue(l_strPrefixPath + "is_auto_assign_job", "");
					l_oData.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "create_user", "");
					l_oData.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "update_user", "");
					l_oData.m_strOvertimeState = p_pJson->GetNodeValue(l_strPrefixPath + "overtime_state", "");

					l_oData.m_strUpdateType = p_pJson->GetNodeValue(l_strPrefixPath + "update_type", "");

					l_oData.m_strProcessObjectID = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_id", "");
					l_oData.m_strTransfDeptOrjCode = p_pJson->GetNodeValue(l_strPrefixPath + "transfers_dept_org_code", "");
					l_oData.m_strIsOver = p_pJson->GetNodeValue(l_strPrefixPath + "is_over", "");
					l_oData.m_strOverRemark = p_pJson->GetNodeValue(l_strPrefixPath + "over_remark", "");
					l_oData.m_strParentID = p_pJson->GetNodeValue(l_strPrefixPath + "parent_id", "");
					l_oData.m_strGZLDM = p_pJson->GetNodeValue(l_strPrefixPath + "flow_code", "");
					*/
					m_oBody.m_vecData.push_back(l_oData);
				}
				l_iCount = p_pJson->GetCount("/body/linked_data");
				for (int i = 0; i < l_iCount; i++)
				{
					std::string l_strPrefixPath("/body/linked_data/" + std::to_string(i) + "/");
					CLinkedData l_oLinked;
					l_oLinked.m_bIsNewLinked = false;
					l_oLinked.ParseString(l_strPrefixPath, p_pJson);
					m_oBody.m_vecLinked.push_back(l_oLinked);
				}
				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/msg_source", m_oBody.m_strMsgSource);
				unsigned int l_uiIndex = 0;
				for (CProcessData data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					
					p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", data.m_strMsgSource);
					if (!data.m_strUpdateType.empty())
					{
						p_pJson->SetNodeValue(l_strPrefixPath + "update_type", data.m_strUpdateType);
					}

					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_district_code", data.m_strDispatchDeptDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "id", data.m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", data.m_strAlarmID);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_code", data.m_strDispatchDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", data.m_strDispatchCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", data.m_strDispatchName);
					p_pJson->SetNodeValue(l_strPrefixPath + "record_id", data.m_strRecordID);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", data.m_strDispatchSuggestion);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_code", data.m_strProcessDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_submit", data.m_strTimeSubmit);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_arrived", data.m_strTimeArrived);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_signed", data.m_strTimeSigned);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_name", data.m_strProcessName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_code", data.m_strProcessCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_personnel", data.m_strDispatchPersonnel);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_vehicles", data.m_strDispatchVehicles);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_boats", data.m_strDispatchBoats);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", data.m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_time", data.m_strCreateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_time", data.m_strUpdateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_name", data.m_strDispatchDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_org_code", data.m_strDispatchDeptOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_name", data.m_strProcessDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_org_code", data.m_strProcessDeptOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_type", data.m_strProcessObjectType);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_name", data.m_strProcessObjectName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_code", data.m_strProcessObjectCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "business_status", data.m_strBusinessStatus);
					p_pJson->SetNodeValue(l_strPrefixPath + "seat_code", data.m_strSeatCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "cancel_time", data.m_strCancelTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "cancel_reason", data.m_strCancelReason);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_auto_assign_job", data.m_strIsAutoAssignJob);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_user", data.m_strCreateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_user", data.m_strUpdateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "overtime_state", data.m_strOvertimeState);

					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_short_name", data.m_strProcessDeptShortName);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_short_name", data.m_strDispatchDeptShortName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_id", data.m_strProcessObjectID);
					p_pJson->SetNodeValue(l_strPrefixPath + "transfers_dept_org_code", data.m_strTransfDeptOrjCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_over", data.m_strIsOver);
					p_pJson->SetNodeValue(l_strPrefixPath + "over_remark", data.m_strOverRemark);
					p_pJson->SetNodeValue(l_strPrefixPath + "parent_id", data.m_strParentID);
					p_pJson->SetNodeValue(l_strPrefixPath + "flow_code", data.m_strGZLDM);

					p_pJson->SetNodeValue(l_strPrefixPath + "createTeminal", data.m_strCreateTeminal);
					p_pJson->SetNodeValue(l_strPrefixPath + "updateTeminal", data.m_strUpdateTeminal);

					p_pJson->SetNodeValue(l_strPrefixPath + "received_time", data.m_strReceivedTime);
					
					++l_uiIndex;
				}

				l_uiIndex = 0;
				for (CLinkedData data : m_oBody.m_vecLinked)
				{
					std::string l_strPrefixPath("/body/linked_data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", data.m_strMsgSource);
					if (!data.m_strUpdateType.empty())
					{
						p_pJson->SetNodeValue(l_strPrefixPath + "update_type", data.m_strUpdateType);
					}
					p_pJson->SetNodeValue(l_strPrefixPath + "id", data.m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", data.m_strAlarmID);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", data.m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_code", data.m_strLinkedOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_name", data.m_strLinkedOrgName);
					p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_type", data.m_strLinkedOrgType);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", data.m_strDispatchCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", data.m_strDispatchName);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_user", data.m_strCreateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_time", data.m_strCreateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_user", data.m_strUpdateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_time", data.m_strUpdateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "result", data.m_strResult);
					++l_uiIndex;
				}
				return p_pJson->ToString();
			}
		public:
			class CProcessData
			{
			public:
				CProcessData()
				{
					m_bIsNewProcess = false;
				}

			public:
				bool ParseString(const std::string& p_strPrefix, JsonParser::IJsonPtr& p_pJson)
				{
					m_strMsgSource = p_pJson->GetNodeValue(p_strPrefix + "/msg_source", "");

					m_strMsgSource = p_pJson->GetNodeValue(p_strPrefix + "msg_source", "");

					m_strDispatchDeptDistrictCode = p_pJson->GetNodeValue(p_strPrefix + "dispatch_dept_district_code", "");
					m_strID = p_pJson->GetNodeValue(p_strPrefix + "id", "");
					m_strAlarmID = p_pJson->GetNodeValue(p_strPrefix + "alarm_id", "");
					m_strDispatchDeptCode = p_pJson->GetNodeValue(p_strPrefix + "dispatch_dept_code", "");
					m_strDispatchCode = p_pJson->GetNodeValue(p_strPrefix + "dispatch_code", "");
					m_strDispatchName = p_pJson->GetNodeValue(p_strPrefix + "dispatch_name", "");
					m_strRecordID = p_pJson->GetNodeValue(p_strPrefix + "record_id", "");
					m_strDispatchSuggestion = p_pJson->GetNodeValue(p_strPrefix + "dispatch_suggestion", "");
					m_strProcessDeptCode = p_pJson->GetNodeValue(p_strPrefix + "process_dept_code", "");
					m_strTimeSubmit = p_pJson->GetNodeValue(p_strPrefix + "time_submit", "");
					m_strTimeArrived = p_pJson->GetNodeValue(p_strPrefix + "time_arrived", "");
					m_strTimeSigned = p_pJson->GetNodeValue(p_strPrefix + "time_signed", "");
					m_strProcessName = p_pJson->GetNodeValue(p_strPrefix + "process_name", "");
					m_strProcessCode = p_pJson->GetNodeValue(p_strPrefix + "process_code", "");
					m_strDispatchPersonnel = p_pJson->GetNodeValue(p_strPrefix + "dispatch_personnel", "");
					m_strDispatchVehicles = p_pJson->GetNodeValue(p_strPrefix + "dispatch_vehicles", "");
					m_strDispatchBoats = p_pJson->GetNodeValue(p_strPrefix + "dispatch_boats", "");
					m_strState = p_pJson->GetNodeValue(p_strPrefix + "state", "");
					m_strCreateTime = p_pJson->GetNodeValue(p_strPrefix + "create_time", "");
					m_strUpdateTime = p_pJson->GetNodeValue(p_strPrefix + "update_time", "");
					m_strDispatchDeptName = p_pJson->GetNodeValue(p_strPrefix + "dispatch_dept_name", "");
					m_strDispatchDeptOrgCode = p_pJson->GetNodeValue(p_strPrefix + "dispatch_dept_org_code", "");
					m_strProcessDeptName = p_pJson->GetNodeValue(p_strPrefix + "process_dept_name", "");
					m_strProcessDeptOrgCode = p_pJson->GetNodeValue(p_strPrefix + "process_dept_org_code", "");
					m_strProcessObjectType = p_pJson->GetNodeValue(p_strPrefix + "process_object_type", "");
					m_strProcessObjectName = p_pJson->GetNodeValue(p_strPrefix + "process_object_name", "");
					m_strProcessObjectCode = p_pJson->GetNodeValue(p_strPrefix + "process_object_code", "");
					m_strBusinessStatus = p_pJson->GetNodeValue(p_strPrefix + "business_status", "");
					m_strSeatCode = p_pJson->GetNodeValue(p_strPrefix + "seat_code", "");
					m_strCancelTime = p_pJson->GetNodeValue(p_strPrefix + "cancel_time", "");
					m_strCancelReason = p_pJson->GetNodeValue(p_strPrefix + "cancel_reason", "");
					m_strIsAutoAssignJob = p_pJson->GetNodeValue(p_strPrefix + "is_auto_assign_job", "");
					m_strCreateUser = p_pJson->GetNodeValue(p_strPrefix + "create_user", "");
					m_strUpdateUser = p_pJson->GetNodeValue(p_strPrefix + "update_user", "");
					m_strOvertimeState = p_pJson->GetNodeValue(p_strPrefix + "overtime_state", "");

					m_strUpdateType = p_pJson->GetNodeValue(p_strPrefix + "update_type", "");

					m_strProcessObjectID = p_pJson->GetNodeValue(p_strPrefix + "process_object_id", "");
					m_strTransfDeptOrjCode = p_pJson->GetNodeValue(p_strPrefix + "transfers_dept_org_code", "");
					m_strIsOver = p_pJson->GetNodeValue(p_strPrefix + "is_over", "");
					m_strOverRemark = p_pJson->GetNodeValue(p_strPrefix + "over_remark", "");
					m_strParentID = p_pJson->GetNodeValue(p_strPrefix + "parent_id", "");
					m_strGZLDM = p_pJson->GetNodeValue(p_strPrefix + "flow_code", "");

					m_strCentreProcessDeptCode = p_pJson->GetNodeValue(p_strPrefix + "centre_process_dept_code", "");
					m_strCentreAlarmDeptCode = p_pJson->GetNodeValue(p_strPrefix + "centre_alarm_dept_code", "");
					m_strDispatchDeptDistrictName = p_pJson->GetNodeValue(p_strPrefix + "dispatch_dept_district_name", "");
					m_strLinkedDispatchCode = p_pJson->GetNodeValue(p_strPrefix + "linked_dispatch_code", "");
					m_strOverTime = p_pJson->GetNodeValue(p_strPrefix + "over_time", "");
					m_strFinishedTimeoutState = p_pJson->GetNodeValue(p_strPrefix + "finished_timeout_state", "");
					m_strPoliceType = p_pJson->GetNodeValue(p_strPrefix + "police_type", "");
					m_strProcessDeptShortName = p_pJson->GetNodeValue(p_strPrefix + "process_dept_short_name", "");
					m_strDispatchDeptShortName = p_pJson->GetNodeValue(p_strPrefix + "dispatch_dept_short_name", "");
					m_strCreateTeminal = p_pJson->GetNodeValue(p_strPrefix + "createTeminal", "");
					m_strUpdateTeminal = p_pJson->GetNodeValue(p_strPrefix + "updateTeminal", "");
					m_strReceivedTime  = p_pJson->GetNodeValue(p_strPrefix + "received_time", "");

					return true;
				};
			public:
				std::string m_strMsgSource;
				std::string m_strUpdateType;					//
				std::string m_strDelFlag;

				std::string m_strProcessFeedback;   //tzx Ϊ�˱���ͨ���ӵģ�����Ҫȥ��

				bool m_bIsNewProcess;							//�Ƿ��������Ĵ�������true�����ǣ�false�����
				std::string m_strDispatchDeptDistrictCode;		//	ͨ�����Ӧ�Ӿ�����������һ�¡�
				std::string m_strID;							//	�ɾ�����ţ���������Ϊ������������+ϵͳ�ɾ�����ţ����ⲻͬ��������ͬ����ţ�Ψһ�ţ�����������
				std::string m_strAlarmID;						//	����������Ӿ������еĽӾ�������ֶΡ�
				std::string m_strDispatchDeptCode;				//	�´��ɾ�ָ��ĵ�λ��������
				std::string m_strDispatchCode;					//	�ɾ�Ա��ţ�������ʶ�ɾ�Ա��ݡ�
				std::string m_strDispatchName;					//	�ɾ�Ա����
				std::string m_strRecordID;						//	�ɾ�¼���ţ��ɾ�ʱ��¼��ϵͳ�Զ��������Ӵ���ϵͳ�Զ�������
				std::string m_strDispatchSuggestion;			//	�ɾ��������ϸ��������
				std::string m_strProcessDeptCode;				//	������λ��������
				std::string m_strTimeSubmit;					//	�ɾ�Ա�´��ɾ�����ʱ��
				std::string m_strTimeArrived;					//	������λ��Ӧ��ϵͳ�Զ����յ���ʱ��
				std::string m_strTimeSigned;					//	�ɵ�ǩ��ʱ��
				std::string m_strProcessName;					//	ǩ�վ�����Ա����
				std::string m_strProcessCode;					//	ǩ�վ�����Ա֤�����루���֤
				std::string m_strDispatchPersonnel;				//	��¼������Ա��������ϸ��Ϣ
				std::string m_strDispatchVehicles;				//	��¼������������ϸ��Ϣ
				std::string m_strDispatchBoats;					//	��¼������ͧ����ϸ��Ϣ
				std::string m_strState;							//	��¼��������Ŀǰ��ҵ��״̬
				std::string m_strCreateTime;					//	��¼������ʱ��
				std::string m_strUpdateTime;					//	��¼�����������ʱ��
				std::string m_strDispatchDeptName;				//	�ɾ���λ����
				std::string m_strDispatchDeptOrgCode;			//	�ɾ���λ����
				std::string m_strProcessDeptName;				//	������λ����
				std::string m_strProcessDeptOrgCode;			//	������λ����
				std::string m_strProcessObjectType;				//	�����������ͣ���λ����Ա��������
				std::string m_strProcessObjectName;				//	������������
				std::string m_strProcessObjectCode;				//	����������루��λ�ǵ�λ���룬��Ա�Ǿ��ţ������ǳ������룩���ǵ�λʱ�����������λ���������ǲ����ظ�
				std::string m_strBusinessStatus;				//	vcs�ڲ�ҵ��״̬
				std::string m_strSeatCode;						//	�����˾���(��ϯ���)
				std::string m_strCancelTime;					//	�˵�����ʱ��
				std::string m_strCancelReason;					//	ȡ��ԭ��
				std::string m_strIsAutoAssignJob;				//	�Ƿ��Զ��������񴴽�
				std::string m_strCreateUser;					//	������
				std::string m_strUpdateUser;					//	�޸���,ȡ���һ���޸�ֵ�޸�ʱ��,ȡ���һ���޸�ֵ
				std::string m_strOvertimeState;					//

				std::string m_strTransfDeptOrjCode;				//	�ƽ�������λ����
				std::string m_strIsOver;						//	�Ƿ�᰸
				std::string m_strOverRemark;					//	�᰸˵��
				std::string m_strParentID;						//	�ϼ��ɾ������
				std::string m_strGZLDM;							//	����������

				std::string m_strProcessObjectID;

				std::string m_strCentreProcessDeptCode;				//	�����ɾ�������
				std::string m_strCentreAlarmDeptCode;				//	�����Ӿ�������
				std::string m_strDispatchDeptDistrictName;			//	������������
				std::string m_strLinkedDispatchCode;				//	���������λ����
				std::string m_strOverTime;							//	�᰸ʱ��
				std::string m_strFinishedTimeoutState;				//	��ϳ�ʱ״̬
				std::string m_strPoliceType;						//	��Ա���ͱ���
				std::string m_strProcessDeptShortName;				//	������λ�����Ӧ���
				std::string m_strDispatchDeptShortName;				//	�ɾ���λ���
				std::string m_strCreateTeminal;					//�����ն˱�ʶ
				std::string m_strUpdateTeminal;					//�����ն˱�ʶ
				std::string m_strReceivedTime;					//�Ӿ�ʱ��
			};

		class CLinkedData
			{
			public:
				CLinkedData()
				{
					m_bIsNewLinked = false;
				}
			public:
				bool ParseString(const std::string& l_strPrefixPath, JsonParser::IJsonPtr& p_pJson)
				{
					m_strMsgSource = p_pJson->GetNodeValue(l_strPrefixPath + "/msg_source", "");
					m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "id", "");
					m_strAlarmID = p_pJson->GetNodeValue(l_strPrefixPath + "alarm_id", "");
					m_strState = p_pJson->GetNodeValue(l_strPrefixPath + "state", "");
					m_strLinkedOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "linked_org_code", "");
					m_strLinkedOrgName = p_pJson->GetNodeValue(l_strPrefixPath + "linked_org_name", "");
					m_strLinkedOrgType = p_pJson->GetNodeValue(l_strPrefixPath + "linked_org_type", "");
					m_strDispatchCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_code", "");
					m_strDispatchName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_name", "");
					m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "create_user", "");
					m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "create_time", "");
					m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "update_user", "");
					m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "update_time", "");
					m_strResult = p_pJson->GetNodeValue(l_strPrefixPath + "result", "");
					return true;
				};
			public:
				std::string m_strMsgSource;
				std::string m_strUpdateType;					//
				std::string m_strDelFlag;

			public:
				bool m_bIsNewLinked;					//�Ƿ��������Ĵ�������true�����ǣ�false�����

				std::string m_strID;							//	�ɾ�����ţ���������Ϊ������������+ϵͳ�ɾ�����ţ����ⲻͬ��������ͬ����ţ�Ψһ�ţ�����������
				std::string m_strAlarmID;						//	����������Ӿ������еĽӾ�������ֶΡ�
				std::string m_strState;							//	����״̬
				std::string m_strLinkedOrgCode;					//	������λ����
				std::string m_strLinkedOrgName;					//	������λ����
				std::string m_strLinkedOrgType;					//	������λ����
				std::string m_strDispatchCode;					//  ������ɾ�Ա���
				std::string m_strDispatchName;					//	������ɾ�Ա���				
				std::string m_strCreateUser;					//	������
				std::string m_strUpdateUser;					//	�޸���,ȡ���һ���޸�ֵ�޸�ʱ��,ȡ���һ���޸�ֵ
				std::string m_strCreateTime;					//	������ʱ��
				std::string m_strUpdateTime;					//	�����������ʱ��
				std::string m_strResult;						//	���ý������

			};
			class CBody
			{
			public:
				std::string m_strMsgSource;                 //��Ϣ��Դ����Ҫ��Ҫ����VCS���ģ���д������ˮ��
				std::vector<CProcessData> m_vecData;
				std::vector<CLinkedData> m_vecLinked;
			};
			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
