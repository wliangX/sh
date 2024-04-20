#pragma once
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetAlarmSyntheticalRequest :
			public IRequest,public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strMergeID = p_pJson->GetNodeValue("/body/merge_id", "");
				m_oBody.m_strTitle = p_pJson->GetNodeValue("/body/title", "");
				m_oBody.m_strContent = p_pJson->GetNodeValue("/body/content", "");
				m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");
				m_oBody.m_strActualOccurTime = p_pJson->GetNodeValue("/body/actual_occur_time", "");
				m_oBody.m_strAddr = p_pJson->GetNodeValue("/body/addr", "");
				m_oBody.m_strLongitude = p_pJson->GetNodeValue("/body/longitude", "");
				m_oBody.m_strLatitude = p_pJson->GetNodeValue("/body/latitude", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strLevel = p_pJson->GetNodeValue("/body/level", "");
				m_oBody.m_strSourceType = p_pJson->GetNodeValue("/body/source_type", "");
				m_oBody.m_strSourceID = p_pJson->GetNodeValue("/body/source_id", "");
				m_oBody.m_strHandleType = p_pJson->GetNodeValue("/body/handle_type", "");
				m_oBody.m_strFirstType = p_pJson->GetNodeValue("/body/first_type", "");
				m_oBody.m_strSecondType = p_pJson->GetNodeValue("/body/second_type", "");
				m_oBody.m_strThirdType = p_pJson->GetNodeValue("/body/third_type", "");
				m_oBody.m_strFourthType = p_pJson->GetNodeValue("/body/fourth_type", "");
				m_oBody.m_strVehicleNo = p_pJson->GetNodeValue("/body/vehicle_no", "");
				m_oBody.m_strVehicleType = p_pJson->GetNodeValue("/body/vehicle_type", "");
				m_oBody.m_strSymbolCode = p_pJson->GetNodeValue("/body/symbol_code", "");
				m_oBody.m_strSymbolAddr = p_pJson->GetNodeValue("/body/symbol_addr", "");
				m_oBody.m_strFireBuildingType = p_pJson->GetNodeValue("/body/fire_building_type", "");

				m_oBody.m_strIsInvolveForeign = p_pJson->GetNodeValue("/body/is_involve_foreign", "");
				m_oBody.m_strIsInvolvePolice = p_pJson->GetNodeValue("/body/is_involve_police", "");
				m_oBody.m_strIsInvolveGun = p_pJson->GetNodeValue("/body/is_involve_gun", "");
				m_oBody.m_strIsInvolveKnife = p_pJson->GetNodeValue("/body/is_involve_knife", "");
				m_oBody.m_strIsInvolveTerror = p_pJson->GetNodeValue("/body/is_involve_terror", "");
				m_oBody.m_strIsInvolvePornography = p_pJson->GetNodeValue("/body/is_involve_pornography", "");
				m_oBody.m_strIsInvolveGamble = p_pJson->GetNodeValue("/body/is_involve_gamble", "");
				m_oBody.m_strIsInvolvePoison = p_pJson->GetNodeValue("/body/is_involve_poison", "");
				m_oBody.m_strIsInvolveExt1 = p_pJson->GetNodeValue("/body/is_involve_ext1", "");
				m_oBody.m_strIsInvolveExt2 = p_pJson->GetNodeValue("/body/is_involve_ext2", "");
				m_oBody.m_strIsInvolveExt3 = p_pJson->GetNodeValue("/body/is_involve_ext3", "");

				m_oBody.m_strCalledNoType = p_pJson->GetNodeValue("/body/called_no_type", "");
				m_oBody.m_strActualCalledNoType = p_pJson->GetNodeValue("/body/actual_called_no_type", "");

				m_oBody.m_strCallerNo = p_pJson->GetNodeValue("/body/caller_no", "");
				m_oBody.m_strCallerName = p_pJson->GetNodeValue("/body/caller_name", "");
				m_oBody.m_strCallerAddr = p_pJson->GetNodeValue("/body/caller_addr", "");
				m_oBody.m_strCallerID = p_pJson->GetNodeValue("/body/caller_id", "");
				m_oBody.m_strCallerIDType = p_pJson->GetNodeValue("/body/caller_id_type", "");
				m_oBody.m_strCallerGender = p_pJson->GetNodeValue("/body/caller_gender", "");
				m_oBody.m_strCallerAge = p_pJson->GetNodeValue("/body/caller_age", "");
				m_oBody.m_strCallerBirthday = p_pJson->GetNodeValue("/body/caller_birthday", "");

				m_oBody.m_strContactNo = p_pJson->GetNodeValue("/body/contact_no", "");
				m_oBody.m_strContactName = p_pJson->GetNodeValue("/body/contact_name", "");
				m_oBody.m_strContactAddr = p_pJson->GetNodeValue("/body/contact_addr", "");
				m_oBody.m_strContactID = p_pJson->GetNodeValue("/body/contact_id", "");
				m_oBody.m_strContactIDType = p_pJson->GetNodeValue("/body/contact_id_type", "");
				m_oBody.m_strContactGender = p_pJson->GetNodeValue("/body/contact_gender", "");
				m_oBody.m_strContactAge = p_pJson->GetNodeValue("/body/contact_age", "");
				m_oBody.m_strContactBirthday = p_pJson->GetNodeValue("/body/contact_birthday", "");

				m_oBody.m_strAdminDeptDistrictCode = p_pJson->GetNodeValue("/body/admin_dept_district_code", "");
				m_oBody.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/admin_dept_code", "");
				m_oBody.m_strAdminDeptName = p_pJson->GetNodeValue("/body/admin_dept_name", "");

				m_oBody.m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue("/body/receipt_dept_district_code", "");
				m_oBody.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/receipt_dept_code", "");
				m_oBody.m_strReceiptDeptName = p_pJson->GetNodeValue("/body/receipt_dept_name", "");
				m_oBody.m_strLeaderCode = p_pJson->GetNodeValue("/body/leader_code", "");
				m_oBody.m_strLeaderName = p_pJson->GetNodeValue("/body/leader_name", "");
				m_oBody.m_strReceiptCode = p_pJson->GetNodeValue("/body/receipt_code", "");
				m_oBody.m_strReceiptName = p_pJson->GetNodeValue("/body/receipt_name", "");

				m_oBody.m_strCreateUser = p_pJson->GetNodeValue("/body/create_user", "");
				m_oBody.m_strCreateTime = p_pJson->GetNodeValue("/body/create_time", "");
				m_oBody.m_strUpdateUser = p_pJson->GetNodeValue("/body/update_user", "");
				m_oBody.m_strUpdateTime = p_pJson->GetNodeValue("/body/update_time", "");
				
				// 
				m_oBody.m_strCount = p_pJson->GetNodeValue("/body/process_list/count", "");
				int l_iCount = std::stoi(m_oBody.m_strCount);
				for (int i = 0; i<l_iCount; i++)
				{
					std::string l_strPrefixPath("/body/process_list/data/" + std::to_string(i) + "/");
					CData l_oData;
					l_oData.m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "id", "");
					l_oData.m_strAlarmID = p_pJson->GetNodeValue(l_strPrefixPath + "alarm_id", "");
					l_oData.m_strState = p_pJson->GetNodeValue(l_strPrefixPath + "state", "");
					l_oData.m_strTimeEdit = p_pJson->GetNodeValue(l_strPrefixPath + "time_edit", "");
					l_oData.m_strTimeSubmit = p_pJson->GetNodeValue(l_strPrefixPath + "time_submit", "");
					l_oData.m_strTimeArrived = p_pJson->GetNodeValue(l_strPrefixPath + "time_arrived", "");
					l_oData.m_strTimeSigned = p_pJson->GetNodeValue(l_strPrefixPath + "time_signed", "");
					l_oData.m_strTimeFeedBack = p_pJson->GetNodeValue(l_strPrefixPath + "time_feedback", "");
					l_oData.m_strIsNeedFeedback = p_pJson->GetNodeValue(l_strPrefixPath + "is_need_feedback", "");
					l_oData.m_strDispatchDeptDistrictCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_district_code", "");
					l_oData.m_strDispatchDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_code", "");
					l_oData.m_strDispatchDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_name", "");
					l_oData.m_strDispatchCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_code", "");
					l_oData.m_strDispatchName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_name", "");
					l_oData.m_strDispatchLeaderCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_leader_code", "");
					l_oData.m_strDispatchLeaderName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_leader_name", "");
					l_oData.m_strDispatchSuggestion = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_suggestion", "");
					l_oData.m_strDispatchLeaderInstruction = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_leader_instruction", "");
					l_oData.m_strProcessDeptDistrictCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_district_code", "");
					l_oData.m_strProcessDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_code", "");
					l_oData.m_strProcessDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_name", "");
					l_oData.m_strProcessCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_code", "");
					l_oData.m_strProcessName = p_pJson->GetNodeValue(l_strPrefixPath + "process_name", "");
					l_oData.m_strProcessLeaderCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_leader_code", "");
					l_oData.m_strProcessLeaderName = p_pJson->GetNodeValue(l_strPrefixPath + "process_leader_name", "");
					l_oData.m_strProcessFeedback = p_pJson->GetNodeValue(l_strPrefixPath + "process_feedback", "");
					l_oData.m_strProcessLeaderInstruction = p_pJson->GetNodeValue(l_strPrefixPath + "process_leader_instruction", "");
					m_oBody.m_vecData.push_back(l_oData);
				}
				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				std::string l_strPrefixPath("/body/");

				p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_strID);
				p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", m_oBody.m_strMergeID);
				p_pJson->SetNodeValue(l_strPrefixPath + "title", m_oBody.m_strTitle);
				p_pJson->SetNodeValue(l_strPrefixPath + "content", m_oBody.m_strContent);
				p_pJson->SetNodeValue(l_strPrefixPath + "time", m_oBody.m_strTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_time", m_oBody.m_strActualOccurTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "addr", m_oBody.m_strAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "longitude", m_oBody.m_strLongitude);
				p_pJson->SetNodeValue(l_strPrefixPath + "latitude", m_oBody.m_strLatitude);
				p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.m_strState);
				p_pJson->SetNodeValue(l_strPrefixPath + "level", m_oBody.m_strLevel);
				p_pJson->SetNodeValue(l_strPrefixPath + "source_type", m_oBody.m_strSourceType);
				p_pJson->SetNodeValue(l_strPrefixPath + "source_id", m_oBody.m_strSourceID);
				p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", m_oBody.m_strHandleType);
				p_pJson->SetNodeValue(l_strPrefixPath + "first_type", m_oBody.m_strFirstType);
				p_pJson->SetNodeValue(l_strPrefixPath + "second_type", m_oBody.m_strSecondType);
				p_pJson->SetNodeValue(l_strPrefixPath + "third_type", m_oBody.m_strThirdType);
				p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", m_oBody.m_strFourthType);
				p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", m_oBody.m_strVehicleNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", m_oBody.m_strVehicleType);
				p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", m_oBody.m_strSymbolCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "symbol_addr", m_oBody.m_strSymbolAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_type", m_oBody.m_strFireBuildingType);

				p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_foreign", m_oBody.m_strIsInvolveForeign);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_police", m_oBody.m_strIsInvolvePolice);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_gun", m_oBody.m_strIsInvolveGun);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_knife", m_oBody.m_strIsInvolveKnife);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_terror", m_oBody.m_strIsInvolveTerror);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_pornography", m_oBody.m_strIsInvolvePornography);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_gamble", m_oBody.m_strIsInvolveGamble);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_poison", m_oBody.m_strIsInvolvePoison);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext1", m_oBody.m_strIsInvolveExt1);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext2", m_oBody.m_strIsInvolveExt2);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext3", m_oBody.m_strIsInvolveExt3);

				p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", m_oBody.m_strCalledNoType);
				p_pJson->SetNodeValue(l_strPrefixPath + "actual_called_no_type", m_oBody.m_strActualCalledNoType);

				p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", m_oBody.m_strCallerNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", m_oBody.m_strCallerName);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", m_oBody.m_strCallerAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", m_oBody.m_strCallerID);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", m_oBody.m_strCallerIDType);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", m_oBody.m_strCallerGender);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_age", m_oBody.m_strCallerAge);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_birthday", m_oBody.m_strCallerBirthday);

				p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", m_oBody.m_strContactNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_name", m_oBody.m_strContactName);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_addr", m_oBody.m_strContactAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_id", m_oBody.m_strContactID);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_id_type", m_oBody.m_strContactIDType);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_gender", m_oBody.m_strContactGender);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_age", m_oBody.m_strContactAge);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_birthday", m_oBody.m_strContactBirthday);

				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_district_code", m_oBody.m_strAdminDeptDistrictCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", m_oBody.m_strAdminDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", m_oBody.m_strAdminDeptName);

				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", m_oBody.m_strReceiptDeptDistrictCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", m_oBody.m_strReceiptDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", m_oBody.m_strReceiptDeptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "leader_code", m_oBody.m_strLeaderCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "leader_name", m_oBody.m_strLeaderName);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", m_oBody.m_strReceiptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", m_oBody.m_strReceiptName);

				p_pJson->SetNodeValue(l_strPrefixPath + "create_user", m_oBody.m_strCreateUser);
				p_pJson->SetNodeValue(l_strPrefixPath + "create_time", m_oBody.m_strCreateTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_user", m_oBody.m_strUpdateUser);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_time", m_oBody.m_strUpdateTime);
				
				// 
				p_pJson->SetNodeValue("/body/process_list/count", m_oBody.m_strCount);
				unsigned int l_uiIndex = 0;
				for (CData data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/process_list/data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "id", data.m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", data.m_strAlarmID);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", data.m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_edit", data.m_strTimeEdit);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_submit", data.m_strTimeSubmit);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_arrived", data.m_strTimeArrived);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_signed", data.m_strTimeSigned);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_feedback", data.m_strTimeFeedBack);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_need_feedback", data.m_strIsNeedFeedback);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_district_code", data.m_strDispatchDeptDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_code", data.m_strDispatchDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_name", data.m_strDispatchDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", data.m_strDispatchCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", data.m_strDispatchName);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_leader_code", data.m_strDispatchLeaderCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_leader_name", data.m_strDispatchLeaderName);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", data.m_strDispatchSuggestion);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_leader_instruction", data.m_strDispatchLeaderInstruction);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_district_code", data.m_strProcessDeptDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_code", data.m_strProcessDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_name", data.m_strProcessDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_code", data.m_strProcessCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_name", data.m_strProcessName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_leader_code", data.m_strProcessLeaderCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_leader_name", data.m_strProcessLeaderName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_feedback", data.m_strProcessFeedback);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_leader_instruction", data.m_strProcessLeaderInstruction);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}
		public:
			class CData
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
			
			class CBody
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

				std::string m_strIsInvolveForeign;			//�Ƿ�����
				std::string m_strIsInvolvePolice;			//�Ƿ��澯
				std::string m_strIsInvolveGun;				//�Ƿ���ǹ
				std::string m_strIsInvolveKnife;			//�Ƿ��浶
				std::string m_strIsInvolveTerror;			//�Ƿ����
				std::string m_strIsInvolvePornography;		//�Ƿ����
				std::string m_strIsInvolveGamble;			//�Ƿ����
				std::string m_strIsInvolvePoison;			//�Ƿ��涾
				std::string m_strIsInvolveExt1;				//��չ�ֶ�1
				std::string m_strIsInvolveExt2;				//��չ�ֶ�2
				std::string m_strIsInvolveExt3;				//��չ�ֶ�3

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

				std::string m_strCreateUser;				//������
				std::string m_strCreateTime;				//����ʱ��
				std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
				std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ
				
				std::string m_strCount;
				std::vector<CData> m_vecData;
			};
			CHeader m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
