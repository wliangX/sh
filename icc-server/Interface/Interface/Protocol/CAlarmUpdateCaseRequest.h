#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/AlarmStructDefine.h>
#include <Protocol/CAlarmSync.h>
namespace ICC
{
    namespace PROTOCOL
    {
        class CAlarmUpdateCaseRequest :
			public IRequest, public IRespond
        {
        public:
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_oAlarm.m_strID = p_pJson->GetNodeValue("/body/JJDBH", "");
				m_oBody.m_oAlarm.m_strReceiptCode = p_pJson->GetNodeValue("/body/JJYBH", "");  //�Ӿ�Ա���
				m_oBody.m_oAlarm.m_strTime = p_pJson->GetNodeValue("/body/BJSJ", ""); //����ʱ��
				//-------------------------------------------
				m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue("/body/XZQHDM", "");//����������������
				m_oBody.m_oAlarm.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/JJDWDM", ""); //�Ӿ���λ��������
				m_oBody.m_oAlarm.m_strMergeID = p_pJson->GetNodeValue("/body/GLZJJDBH", "");  //�������Ӿ������
				m_oBody.m_oAlarm.m_strCalledNoType = p_pJson->GetNodeValue("/body/JJLX", ""); //�Ӿ����ͣ�110��122��119�ԽӾ��������Ӿ��ȣ�
				m_oBody.m_oAlarm.m_strSourceType = p_pJson->GetNodeValue("/body/BJFS", ""); //������ʽ���绰���������ű��������籨���ȣ�
				m_oBody.m_oAlarm.m_strHandleType = p_pJson->GetNodeValue("/body/LHLX", "");  //��������
				//p_pJson->GetNodeValue("/body/JJLYH", ""); //�Ӿ�¼���ţ��Ӿ�ʱ��¼��ϵͳ�Զ��������Ӵ���ϵͳ�Զ�������
				m_oBody.m_oAlarm.m_strReceiptName = p_pJson->GetNodeValue("/body/JJYXM", "");  //�Ӿ�Ա����
				//l_oAlarm. p_pJson->GetNodeValue("/body/JJSJ", "");  //�Ӿ�ʱ��
				//p_pJson->GetNodeValue("/body/WCSJ", ""); //�Ӿ����ʱ��
				m_oBody.m_oAlarm.m_strCallerNo = p_pJson->GetNodeValue("/body/BJDH", "");  //�����绰
				//p_pJson->GetNodeValue("/body/BJDHYHXM", "");					//�����绰�û�����
				m_oBody.m_oAlarm.m_strCallerID = p_pJson->GetNodeValue("/body/YHSFZ", "");					//�û����֤���
				m_oBody.m_oAlarm.m_strContactAddr = p_pJson->GetNodeValue("/body/BJDHYHDZ", "");					//�����绰�û���ַ
				m_oBody.m_oAlarm.m_strContactName = p_pJson->GetNodeValue("/body/BJRXM", "");  //����������
				m_oBody.m_oAlarm.m_strContactGender = p_pJson->GetNodeValue("/body/BJRXBDM", "");  //�������Ա����
				m_oBody.m_oAlarm.m_strContactNo = p_pJson->GetNodeValue("/body/LXDH", "");   //��ϵ�绰
				m_oBody.m_oAlarm.m_strAddr = p_pJson->GetNodeValue("/body/JQDZ", "");  //�����ַ
				m_oBody.m_oAlarm.m_strContent = p_pJson->GetNodeValue("/body/BJNR", "");    //��������
				m_oBody.m_oAlarm.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/GXDWDM", "");  //��Ͻ��λ����
				m_oBody.m_oAlarm.m_strFirstType = p_pJson->GetNodeValue("/body/JQLBDM", ""); //����������
				m_oBody.m_oAlarm.m_strSecondType = p_pJson->GetNodeValue("/body/JQLXDM", "");  //�������ʹ���
				m_oBody.m_oAlarm.m_strThirdType = p_pJson->GetNodeValue("/body/JQXLDM", ""); //����ϸ�����
				//l_oAlarm. = p_pJson->GetNodeValue("/body/TZDBH", "");  //��������
				//l_oAlarm. = p_pJson->GetNodeValue("/body/YWWXWZ", ""); //����Σ������
				//p_pJson->GetNodeValue("/body/YWBZXL", "");  //���ޱ�ը/й©
				//p_pJson->GetNodeValue("/body/BKRYQKSM", ""); //������Ա���˵��
				//p_pJson->GetNodeValue("/body/SSRYQKSM", "");  //������Ա���˵��
				//p_pJson->GetNodeValue("/body/SWRYQKSM", "");  //������Ա���˵��
				//p_pJson->GetNodeValue("/body/SFSWYBJ", "");  //�Ƿ������ﱨ��
				m_oBody.m_oAlarm.m_strLongitude = p_pJson->GetNodeValue("/body/BJRXZB", ""); //�����˶�λX����
				m_oBody.m_oAlarm.m_strLatitude = p_pJson->GetNodeValue("/body/BJRYZB", "");  //�����˶�λY����
				//p_pJson->GetNodeValue("/body/FXDWJD", ""); //����λ���ֹ���ͼʰȡ��X����
				//p_pJson->GetNodeValue("/body/FXDWWD", "");  //����λ���ֹ���ͼʰȡ��Y����
				//p_pJson->GetNodeValue("/body/BCJJNR", "");  //����Ӿ�����
				m_oBody.m_oAlarm.m_strLevel = p_pJson->GetNodeValue("/body/JQDJDM", ""); //����ȼ����루�ļ����飩
				m_oBody.m_oAlarm.m_strState = p_pJson->GetNodeValue("/body/JQCLZTDM", ""); //���鴦��״̬����
				//p_pJson->GetNodeValue("/body/YJJYDJDM", ""); //Ӧ����Ԯ�ȼ����루�ļ���--
				//p_pJson->GetNodeValue("/body/SJCHPZLDM", ""); //�漰�������������--
				//p_pJson->GetNodeValue("/body/SJCPH", ""); //�漰�����ƺ�
				//p_pJson->GetNodeValue("/body/SFSWHCL", "");  //�Ƿ���Σ������
				//p_pJson->GetNodeValue("/body/RKSJ", "");  //���ʱ��
				m_oBody.m_oAlarm.m_strUpdateTime = p_pJson->GetNodeValue("/body/GXSJ", "");  //����ʱ��
				m_oBody.m_oAlarm.m_strTitle = p_pJson->GetNodeValue("/body/JQGJZ", ""); //����ؼ���
				//p_pJson->GetNodeValue("/body/BM", "");//�Ƿ���
				//p_pJson->GetNodeValue("/body/JJYID", "");  //�Ӿ�Ա�û�id
				//p_pJson->GetNodeValue("/body/ZHTBBTH", ""); //���ͬ���汾��
				//p_pJson->GetNodeValue("/body/SFMN", ""); //�Ƿ�ģ���¼�
				m_oBody.m_oAlarm.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/GXDWID", ""); //��Ͻ��λid
				m_oBody.m_oAlarm.m_strAdminDeptName = p_pJson->GetNodeValue("/body/GXDWMC", "");  //��Ͻ��λ����
				//???????p_pJson->GetNodeValue("/body/JQZLDM", "");  //�����������
				//p_pJson->GetNodeValue("/body/DQYWZT", "");  //��ǰҵ��״̬
				//p_pJson->GetNodeValue("/body/GXDWIDENTIFIER", "");  //��Ͻ��λ:��֯�ڲ����
				//p_pJson->GetNodeValue("/body/ISDELETE", "");
				//-------------------------------------------
				int l_iCount = p_pJson->GetCount("/body/RELATED_PERSONS");
				for (int i = 0; i < l_iCount; i++)
				{
					PROTOCOL::CAlarmRelatedPersonsInfo tmpPersonsInfo;
					std::string strPre("/body/RELATED_PERSONS/" + std::to_string(i) + "/");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_guid] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_guid, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_alarm_id] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_alarm_id, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_first_type] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_first_type, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_second_type] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_second_type, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_name] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_name, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_name_verify_status] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_name_verify_status, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_person_id] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_person_id, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_person_id_verify_status] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_person_id_verify_status, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_cpf] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_cpf, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_cpf_verify_status] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_cpf_verify_status, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_age] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_age, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_age_verify_status] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_age_verify_status, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_sex] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_sex, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_sex_verify_status] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_sex_verify_status, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_birthday] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_birthday, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_birthday_verify_status] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_birthday_verify_status, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_mother_name] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_mother_name, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_mother_name_verify_status] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_mother_name_verify_status, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_remark] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_remark, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_create_time] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_create_time, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_create_user] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_create_user, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_update_time] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_update_time, "");
					tmpPersonsInfo.m_mapInfo[PROTOCOL::Persons_update_user] = p_pJson->GetNodeValue(strPre + PROTOCOL::Persons_update_user, "");

					m_oBody.m_alarmRelatedPersonsData.push_back(tmpPersonsInfo);
				}

				l_iCount = 0;
				l_iCount = p_pJson->GetCount("/body/RELATED_CARS");
				for (int i = 0; i < l_iCount; i++)
				{
					PROTOCOL::CAlarmRelatedCarsInfo tmpCarsInfo;
					std::string strPre("/body/RELATED_CARS/" + std::to_string(i) + "/");

					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_guid] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_guid, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_alarm_id] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_alarm_id, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_car_type] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_car_type, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_car_id] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_car_id, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_car_id_verify_status] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_car_id_verify_status, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_cpf] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_cpf, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_cpf_verify_status] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_cpf_verify_status, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_car_number] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_car_number, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_car_number_verify_status] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_car_number_verify_status, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_frame_number] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_frame_number, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_frame_number_verify_status] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_frame_number_verify_status, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_manufacture_date] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_manufacture_date, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_manufacture_date_verify_status] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_manufacture_date_verify_status, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_brand] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_brand, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_brand_verify_status] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_brand_verify_status, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_car_mode] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_car_mode, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_car_mode_verify_status] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_car_mode_verify_status, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_color] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_color, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_color_verify_status] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_color_verify_status, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_remark] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_remark, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_create_time] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_create_time, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_create_user] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_create_user, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_update_time] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_update_time, "");
					tmpCarsInfo.m_mapInfo[PROTOCOL::Cars_update_user] = p_pJson->GetNodeValue(strPre + PROTOCOL::Cars_update_user, "");

					m_oBody.m_alarmRelatedCarsData.push_back(tmpCarsInfo);

				}

				l_iCount = 0;
				l_iCount = p_pJson->GetCount("/body/RELATED_MATTERS");
				for (int i = 0; i < l_iCount; i++)
				{
					PROTOCOL::CAlarmRelatedMattersInfo tmpMattersInfo;
					std::string strPre("/body/RELATED_MATTERS/" + std::to_string(i) + "/");

					tmpMattersInfo.m_mapInfo[PROTOCOL::Matters_guid] = p_pJson->GetNodeValue(strPre + PROTOCOL::Matters_guid, "");
					tmpMattersInfo.m_mapInfo[PROTOCOL::Matters_alarm_id] = p_pJson->GetNodeValue(strPre + PROTOCOL::Matters_alarm_id, "");
					tmpMattersInfo.m_mapInfo[PROTOCOL::Matters_matter_type] = p_pJson->GetNodeValue(strPre + PROTOCOL::Matters_matter_type, "");
					tmpMattersInfo.m_mapInfo[PROTOCOL::Matters_name] = p_pJson->GetNodeValue(strPre + PROTOCOL::Matters_name, "");
					tmpMattersInfo.m_mapInfo[PROTOCOL::Matters_description] = p_pJson->GetNodeValue(strPre + PROTOCOL::Matters_description, "");
					tmpMattersInfo.m_mapInfo[PROTOCOL::Matters_remark] = p_pJson->GetNodeValue(strPre + PROTOCOL::Matters_remark, "");
					tmpMattersInfo.m_mapInfo[PROTOCOL::Matters_create_time] = p_pJson->GetNodeValue(strPre + PROTOCOL::Matters_create_time, "");
					tmpMattersInfo.m_mapInfo[PROTOCOL::Matters_create_user] = p_pJson->GetNodeValue(strPre + PROTOCOL::Matters_create_user, "");
					tmpMattersInfo.m_mapInfo[PROTOCOL::Matters_update_time] = p_pJson->GetNodeValue(strPre + PROTOCOL::Matters_update_time, "");
					tmpMattersInfo.m_mapInfo[PROTOCOL::Matters_update_user] = p_pJson->GetNodeValue(strPre + PROTOCOL::Matters_update_user, "");

					m_oBody.m_alarmRelatedMattersData.push_back(tmpMattersInfo);
				}
                return true;
            }

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				return p_pJson->ToString();
			}

        public:

            class CBody
            {
            public:
				PROTOCOL::CAlarmSync::CBody m_oAlarm;
				std::vector<CAlarmRelatedPersonsInfo> m_alarmRelatedPersonsData;
                std::vector<CAlarmRelatedCarsInfo> m_alarmRelatedCarsData;
                std::vector<CAlarmRelatedMattersInfo> m_alarmRelatedMattersData;
            };
            CHeaderEx m_oHeader;
            CBody	m_oBody;
        };

    }
}

