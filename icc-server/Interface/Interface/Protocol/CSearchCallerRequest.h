#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchCallerRequest :
			public IRequest
		{
		public:

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/id", m_oBody.m_str_id);
				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_str_alarm_id);
				p_pJson->SetNodeValue("/body/process_id", m_oBody.m_str_process_id);
				p_pJson->SetNodeValue("/body/feedback_id", m_oBody.m_str_feedback_id);
				p_pJson->SetNodeValue("/body/centre_process_dept_code", m_oBody.m_str_centre_process_dept_code);
				p_pJson->SetNodeValue("/body/centre_alarm_dept_code", m_oBody.m_str_centre_alarm_dept_code);
				p_pJson->SetNodeValue("/body/centre_process_id", m_oBody.m_str_centre_process_id);
				p_pJson->SetNodeValue("/body/centre_feedback_id", m_oBody.m_str_centre_feedback_id);
				p_pJson->SetNodeValue("/body/district", m_oBody.m_str_district);
				p_pJson->SetNodeValue("/body/district_name", m_oBody.m_str_district_name);
				p_pJson->SetNodeValue("/body/name", m_oBody.m_str_name);
				p_pJson->SetNodeValue("/body/sex", m_oBody.m_str_sex);
				p_pJson->SetNodeValue("/body/identification_type", m_oBody.m_str_identification_type);
				p_pJson->SetNodeValue("/body/identification_id", m_oBody.m_str_identification_id);
				p_pJson->SetNodeValue("/body/identity", m_oBody.m_str_identity);
				p_pJson->SetNodeValue("/body/is_focus_pesron", m_oBody.m_str_is_focus_pesron);
				p_pJson->SetNodeValue("/body/focus_pesron_info", m_oBody.m_str_focus_pesron_info);
				p_pJson->SetNodeValue("/body/registered_address_division", m_oBody.m_str_registered_address_division);
				p_pJson->SetNodeValue("/body/registered_address", m_oBody.m_str_registered_address);
				p_pJson->SetNodeValue("/body/current_address_division", m_oBody.m_str_current_address_division);
				p_pJson->SetNodeValue("/body/current_address", m_oBody.m_str_current_address);
				p_pJson->SetNodeValue("/body/work_unit", m_oBody.m_str_work_unit);
				p_pJson->SetNodeValue("/body/occupation", m_oBody.m_str_occupation);
				p_pJson->SetNodeValue("/body/contact_number", m_oBody.m_str_contact_number);
				p_pJson->SetNodeValue("/body/other_certificates", m_oBody.m_str_other_certificates);
				p_pJson->SetNodeValue("/body/lost_item_information", m_oBody.m_str_lost_item_information);
				p_pJson->SetNodeValue("/body/create_time", m_oBody.m_str_create_time);
				p_pJson->SetNodeValue("/body/update_time", m_oBody.m_str_update_time);
				p_pJson->SetNodeValue("/body/date_of_birth", m_oBody.m_str_date_of_birth);
				p_pJson->SetNodeValue("/body/digital_signature", m_oBody.m_str_digital_signature);
				p_pJson->SetNodeValue("/body/is_delete", m_oBody.m_str_is_delete);
				p_pJson->SetNodeValue("/body/createTeminal", m_oBody.m_str_createTeminal);
				p_pJson->SetNodeValue("/body/updateTeminal", m_oBody.m_str_updateTeminal);
				p_pJson->SetNodeValue("/body/nationality", m_oBody.m_str_nationality);
				p_pJson->SetNodeValue("/body/nation", m_oBody.m_str_nation);
				p_pJson->SetNodeValue("/body/educationLevel", m_oBody.m_str_educationLevel);
				p_pJson->SetNodeValue("/body/marriageStatus", m_oBody.m_str_marriageStatus);
				p_pJson->SetNodeValue("/body/politicalOutlook", m_oBody.m_str_politicalOutlook);
				p_pJson->SetNodeValue("/body/is_NPC_deputy", m_oBody.m_str_is_NPC_deputy);
				p_pJson->SetNodeValue("/body/is_national_staff", m_oBody.m_str_is_national_staff);
				p_pJson->SetNodeValue("/body/createUserId", m_oBody.m_str_createUserId);
				p_pJson->SetNodeValue("/body/createUserName", m_oBody.m_str_createUserName);
				p_pJson->SetNodeValue("/body/createUserOrgCode", m_oBody.m_str_createUserOrgCode);
				p_pJson->SetNodeValue("/body/createUserOrgName", m_oBody.m_str_createUserOrgName);
				p_pJson->SetNodeValue("/body/updateUserId", m_oBody.m_str_updateUserId);
				p_pJson->SetNodeValue("/body/updateUserName", m_oBody.m_str_updateUserName);
				p_pJson->SetNodeValue("/body/updateUserOrgCode", m_oBody.m_str_updateUserOrgCode);
				p_pJson->SetNodeValue("/body/updateUserOrgName", m_oBody.m_str_updateUserOrgName);

				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strVcsSyncFlag = p_pJson->GetNodeValue("/body/vcs_sync_flag", "");
				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_strOperateType = p_pJson->GetNodeValue("/body/operate_type", "");
				m_oBody.m_str_id = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_str_alarm_id = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_str_process_id = p_pJson->GetNodeValue("/body/process_id", "");
				m_oBody.m_str_feedback_id = p_pJson->GetNodeValue("/body/feedback_id", "");
				m_oBody.m_str_centre_process_dept_code = p_pJson->GetNodeValue("/body/centre_process_dept_code", "");
				m_oBody.m_str_centre_alarm_dept_code = p_pJson->GetNodeValue("/body/centre_alarm_dept_code", "");
				m_oBody.m_str_centre_process_id = p_pJson->GetNodeValue("/body/centre_process_id", "");
				m_oBody.m_str_centre_feedback_id = p_pJson->GetNodeValue("/body/centre_feedback_id", "");
				m_oBody.m_str_district = p_pJson->GetNodeValue("/body/district", "");
				m_oBody.m_str_district_name = p_pJson->GetNodeValue("/body/district_name", "");
				m_oBody.m_str_name = p_pJson->GetNodeValue("/body/name", "");
				m_oBody.m_str_sex = p_pJson->GetNodeValue("/body/sex", "");
				m_oBody.m_str_identification_type = p_pJson->GetNodeValue("/body/identification_type", "");
				m_oBody.m_str_identification_id = p_pJson->GetNodeValue("/body/identification_id", "");
				m_oBody.m_str_identity = p_pJson->GetNodeValue("/body/identity", "");
				m_oBody.m_str_is_focus_pesron = p_pJson->GetNodeValue("/body/is_focus_pesron", "");
				m_oBody.m_str_focus_pesron_info = p_pJson->GetNodeValue("/body/focus_pesron_info", "");
				m_oBody.m_str_registered_address_division = p_pJson->GetNodeValue("/body/registered_address_division", "");
				m_oBody.m_str_registered_address = p_pJson->GetNodeValue("/body/registered_address", "");
				m_oBody.m_str_current_address_division = p_pJson->GetNodeValue("/body/current_address_division", "");
				m_oBody.m_str_current_address = p_pJson->GetNodeValue("/body/current_address", "");
				m_oBody.m_str_work_unit = p_pJson->GetNodeValue("/body/work_unit", "");
				m_oBody.m_str_occupation = p_pJson->GetNodeValue("/body/occupation", "");
				m_oBody.m_str_contact_number = p_pJson->GetNodeValue("/body/contact_number", "");
				m_oBody.m_str_other_certificates = p_pJson->GetNodeValue("/body/other_certificates", "");
				m_oBody.m_str_lost_item_information = p_pJson->GetNodeValue("/body/lost_item_information", "");
				m_oBody.m_str_create_time = p_pJson->GetNodeValue("/body/create_time", "");
				m_oBody.m_str_update_time = p_pJson->GetNodeValue("/body/update_time", "");
				m_oBody.m_str_date_of_birth = p_pJson->GetNodeValue("/body/date_of_birth", "");
				m_oBody.m_str_digital_signature = p_pJson->GetNodeValue("/body/digital_signature", "");
				m_oBody.m_str_is_delete = p_pJson->GetNodeValue("/body/is_delete", "");
				m_oBody.m_str_createTeminal = p_pJson->GetNodeValue("/body/createTeminal", "");
				m_oBody.m_str_updateTeminal = p_pJson->GetNodeValue("/body/updateTeminal", "");
				m_oBody.m_str_nationality = p_pJson->GetNodeValue("/body/nationality", "");
				m_oBody.m_str_nation = p_pJson->GetNodeValue("/body/nation", "");
				m_oBody.m_str_educationLevel = p_pJson->GetNodeValue("/body/educationLevel", "");
				m_oBody.m_str_marriageStatus = p_pJson->GetNodeValue("/body/marriageStatus", "");
				m_oBody.m_str_politicalOutlook = p_pJson->GetNodeValue("/body/politicalOutlook", "");
				m_oBody.m_str_is_NPC_deputy = p_pJson->GetNodeValue("/body/is_NPC_deputy", "");
				m_oBody.m_str_is_national_staff = p_pJson->GetNodeValue("/body/is_national_staff", "");
				m_oBody.m_str_createUserId = p_pJson->GetNodeValue("/body/createUserId", "");
				m_oBody.m_str_createUserName = p_pJson->GetNodeValue("/body/createUserName", "");
				m_oBody.m_str_createUserOrgCode = p_pJson->GetNodeValue("/body/createUserOrgCode", "");
				m_oBody.m_str_createUserOrgName = p_pJson->GetNodeValue("/body/createUserOrgName", "");
				m_oBody.m_str_updateUserId = p_pJson->GetNodeValue("/body/updateUserId", "");
				m_oBody.m_str_updateUserName = p_pJson->GetNodeValue("/body/updateUserName", "");
				m_oBody.m_str_updateUserOrgCode = p_pJson->GetNodeValue("/body/updateUserOrgCode", "");
				m_oBody.m_str_updateUserOrgName = p_pJson->GetNodeValue("/body/updateUserOrgName", "");

				return true;
			}

		public:

			class CBody
			{
			public:
				std::string m_strVcsSyncFlag;
				std::string	m_strPageSize;					//ÿҳ����������Ϊ�գ�
				std::string	m_strPageIndex;					//ҳ�룬1��ʾ��һҳ������Ϊ�գ�
				std::string	m_strBeginTime;					//��ѯ��ʼʱ�䣨����Ϊ�գ�
				std::string	m_strEndTime;					//��ѯ����ʱ�䣨����Ϊ�գ�
				std::string	m_strOperateType;				//��ѯ����ʱ�䣨����Ϊ�գ�  1 Ϊ�������� 2�Ǹ�������
				std::string m_str_id;    // ���鵱���˵����
				std::string m_str_alarm_id;    // �Ӿ������
				std::string m_str_process_id;    // �ɾ������
				std::string m_str_feedback_id;    // ���������
				std::string m_str_centre_process_dept_code;    // �������鵱���˵�����
				std::string m_str_centre_alarm_dept_code;    // �����Ӿ�������
				std::string m_str_centre_process_id;    // �����ɾ�������
				std::string m_str_centre_feedback_id;    // ��������������
				std::string m_str_district;    // ������������
				std::string m_str_district_name;    // ������������
				std::string m_str_name;    // ���鵱��������
				std::string m_str_sex;    // �Ա����
				std::string m_str_identification_type;    // ֤������
				std::string m_str_identification_id;    // ֤������
				std::string m_str_identity;    // ���鵱�������
				std::string m_str_is_focus_pesron;    // �Ƿ��ص���Ա
				std::string m_str_focus_pesron_info;    // �ص���Ա����
				std::string m_str_registered_address_division;    // ������������
				std::string m_str_registered_address;    // ������ַ
				std::string m_str_current_address_division;    // ��ס��������
				std::string m_str_current_address;    // ��ס��ַ
				std::string m_str_work_unit;    // ������λ
				std::string m_str_occupation;    // ְҵ
				std::string m_str_contact_number;    // ��ϵ�绰
				std::string m_str_other_certificates;    // ����֤��
				std::string m_str_lost_item_information;    // ��ʧ��Ʒ��Ϣ
				std::string m_str_create_time;    // ���ʱ��
				std::string m_str_update_time;    // ����ʱ��
				std::string m_str_date_of_birth;    // ��������
				std::string m_str_digital_signature;    // ����ǩ��
				std::string m_str_is_delete;    // ɾ����ʶ
				std::string m_str_createTeminal;    // �����ն˱�ʶ
				std::string m_str_updateTeminal;    // �����ն˱�ʶ
				std::string m_str_nationality;    // ����
				std::string m_str_nation;    // ����
				std::string m_str_educationLevel;    // �Ļ��̶�
				std::string m_str_marriageStatus;    // ����״��
				std::string m_str_politicalOutlook;    // ������ò
				std::string m_str_is_NPC_deputy;    // �Ƿ��˴����
				std::string m_str_is_national_staff;    // ���ҹ�����Ա
				std::string m_str_createUserId;    // �Ǽ���֤��
				std::string m_str_createUserName;    // �Ǽ�������
				std::string m_str_createUserOrgCode;    // �Ǽǵ�λ
				std::string m_str_createUserOrgName;    // �Ǽǵ�λ����
				std::string m_str_updateUserId;    // �޸���֤��
				std::string m_str_updateUserName;    // �޸�������
				std::string m_str_updateUserOrgCode;    // �޸ĵ�λ
				std::string m_str_updateUserOrgName;    // �޸ĵ�λ����
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};

		class CSetCallerRespond :
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
				//  p_pJson->SetNodeValue("/body/body/result", m_oBody.m_strResult);
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				//std::string m_strResult;
			};
			CBody m_oBody;
		};
	}
}