#pragma once

#include <Json/IJson.h>

namespace ICC
{
	namespace PROTOCOL
	{
		/************************************************************************/
/*                     alarm related persons                            */
/************************************************************************/
		const std::string strTmpPre = "/";

		const std::string Persons_guid = "guid";
		const std::string Persons_alarm_id = "alarm_id";
		const std::string Persons_first_type = "first_type";
		const std::string Persons_second_type = "second_type";
		const std::string Persons_name = "name";
		const std::string Persons_name_verify_status = "name_verify_status";
		const std::string Persons_person_id = "person_id";
		const std::string Persons_person_id_verify_status = "person_id_verify_status";
		const std::string Persons_cpf = "cpf";
		const std::string Persons_cpf_verify_status = "cpf_verify_status";
		const std::string Persons_age = "age";
		const std::string Persons_age_verify_status = "age_verify_status";
		const std::string Persons_sex = "sex";
		const std::string Persons_sex_verify_status = "sex_verify_status";
		const std::string Persons_birthday = "birthday";
		const std::string Persons_birthday_verify_status = "birthday_verify_status";
		const std::string Persons_mother_name = "mother_name";
		const std::string Persons_mother_name_verify_status = "mother_name_verify_status";
		const std::string Persons_remark = "remark";
		const std::string Persons_create_time = "create_time";
		const std::string Persons_create_user = "create_user";
		const std::string Persons_update_time = "update_time";
		const std::string Persons_update_user = "update_user";

		class CAlarmRelatedPersonsInfo
		{
		public:
			void ParseString(const std::string& strPre, JsonParser::IJsonPtr p_pJson)
			{
				m_mapInfo[Persons_guid] = p_pJson->GetNodeValue(strPre + Persons_guid, "");
				m_mapInfo[Persons_alarm_id] = p_pJson->GetNodeValue(strPre + Persons_alarm_id, "");
				m_mapInfo[Persons_first_type] = p_pJson->GetNodeValue(strPre + Persons_first_type, "");
				m_mapInfo[Persons_second_type] = p_pJson->GetNodeValue(strPre + Persons_second_type, "");
				m_mapInfo[Persons_name] = p_pJson->GetNodeValue(strPre + Persons_name, "");
				m_mapInfo[Persons_name_verify_status] = p_pJson->GetNodeValue(strPre + Persons_name_verify_status, "");
				m_mapInfo[Persons_person_id] = p_pJson->GetNodeValue(strPre + Persons_person_id, "");
				m_mapInfo[Persons_person_id_verify_status] = p_pJson->GetNodeValue(strPre + Persons_person_id_verify_status, "");
				m_mapInfo[Persons_cpf] = p_pJson->GetNodeValue(strPre + Persons_cpf, "");
				m_mapInfo[Persons_cpf_verify_status] = p_pJson->GetNodeValue(strPre + Persons_cpf_verify_status, "");
				m_mapInfo[Persons_age] = p_pJson->GetNodeValue(strPre + Persons_age, "");
				m_mapInfo[Persons_age_verify_status] = p_pJson->GetNodeValue(strPre + Persons_age_verify_status, "");
				m_mapInfo[Persons_sex] = p_pJson->GetNodeValue(strPre + Persons_sex, "");
				m_mapInfo[Persons_sex_verify_status] = p_pJson->GetNodeValue(strPre + Persons_sex_verify_status, "");
				m_mapInfo[Persons_birthday] = p_pJson->GetNodeValue(strPre + Persons_birthday, "");
				m_mapInfo[Persons_birthday_verify_status] = p_pJson->GetNodeValue(strPre + Persons_birthday_verify_status, "");
				m_mapInfo[Persons_mother_name] = p_pJson->GetNodeValue(strPre + Persons_mother_name, "");
				m_mapInfo[Persons_mother_name_verify_status] = p_pJson->GetNodeValue(strPre + Persons_mother_name_verify_status, "");
				m_mapInfo[Persons_remark] = p_pJson->GetNodeValue(strPre + Persons_remark, "");
				m_mapInfo[Persons_create_time] = p_pJson->GetNodeValue(strPre + Persons_create_time, "");
				m_mapInfo[Persons_create_user] = p_pJson->GetNodeValue(strPre + Persons_create_user, "");
				m_mapInfo[Persons_update_time] = p_pJson->GetNodeValue(strPre + Persons_update_time, "");
				m_mapInfo[Persons_update_user] = p_pJson->GetNodeValue(strPre + Persons_update_user, "");

				/*m_str_guid = p_pJson->GetNodeValue(strPre + Persons_guid, "");
				m_str_alarm_id = p_pJson->GetNodeValue(strPre + Persons_alarm_id, "");
				m_str_first_type = p_pJson->GetNodeValue(strPre + Persons_first_type, "");
				m_str_second_type = p_pJson->GetNodeValue(strPre + Persons_second_type, "");
				m_str_name = p_pJson->GetNodeValue(strPre + Persons_name, "");
				m_str_name_verify_status = p_pJson->GetNodeValue(strPre + Persons_name_verify_status, "");
				m_str_person_id = p_pJson->GetNodeValue(strPre + Persons_person_id, "");
				m_str_person_id_verify_status = p_pJson->GetNodeValue(strPre + Persons_person_id_verify_status, "");
				m_str_cpf = p_pJson->GetNodeValue(strPre + Persons_cpf, "");
				m_str_cpf_verify_status = p_pJson->GetNodeValue(strPre + Persons_cpf_verify_status, "");
				m_str_age = p_pJson->GetNodeValue(strPre + Persons_age, "");
				m_str_age_verify_status = p_pJson->GetNodeValue(strPre + Persons_age_verify_status, "");
				m_str_sex = p_pJson->GetNodeValue(strPre + Persons_sex, "");
				m_str_sex_verify_status = p_pJson->GetNodeValue(strPre + Persons_sex_verify_status, "");
				m_str_birthday = p_pJson->GetNodeValue(strPre + Persons_birthday, "");
				m_str_birthday_verify_status = p_pJson->GetNodeValue(strPre + Persons_birthday_verify_status, "");
				m_str_mother_name = p_pJson->GetNodeValue(strPre + Persons_mother_name, "");
				m_str_mother_name_verify_status = p_pJson->GetNodeValue(strPre + Persons_mother_name_verify_status, "");
				m_str_remark = p_pJson->GetNodeValue(strPre + Persons_remark, "");
				m_str_create_time = p_pJson->GetNodeValue(strPre + Persons_create_time, "");
				m_str_create_user = p_pJson->GetNodeValue(strPre + Persons_create_user, "");
				m_str_update_time = p_pJson->GetNodeValue(strPre + Persons_update_time, "");
				m_str_update_user = p_pJson->GetNodeValue(strPre + Persons_update_user, "");*/
			}

			void ToString(const std::string& strPre, JsonParser::IJsonPtr p_pJson)
			{
				std::map<std::string, std::string>::const_iterator itr_const;
				for (itr_const = m_mapInfo.begin(); itr_const != m_mapInfo.end(); ++itr_const)
				{
					p_pJson->SetNodeValue(strPre + itr_const->first, itr_const->second);
				}

				/*p_pJson->SetNodeValue(strPre + Persons_guid, m_mapInfo[Persons_guid]);
				p_pJson->SetNodeValue(strPre + Persons_alarm_id, m_mapInfo[Persons_alarm_id]);
				p_pJson->SetNodeValue(strPre + Persons_first_type, m_mapInfo[Persons_first_type]);
				p_pJson->SetNodeValue(strPre + Persons_second_type, m_mapInfo[Persons_second_type]);
				p_pJson->SetNodeValue(strPre + Persons_name, m_mapInfo[Persons_name]);
				p_pJson->SetNodeValue(strPre + Persons_name_verify_status, m_mapInfo[Persons_name_verify_status]);
				p_pJson->SetNodeValue(strPre + Persons_person_id, m_mapInfo[Persons_person_id]);
				p_pJson->SetNodeValue(strPre + Persons_person_id_verify_status, m_mapInfo[Persons_person_id_verify_status]);
				p_pJson->SetNodeValue(strPre + Persons_cpf, m_mapInfo[Persons_cpf]);
				p_pJson->SetNodeValue(strPre + Persons_cpf_verify_status, m_mapInfo[Persons_cpf_verify_status]);
				p_pJson->SetNodeValue(strPre + Persons_age, m_mapInfo[Persons_age]);
				p_pJson->SetNodeValue(strPre + Persons_age_verify_status, m_mapInfo[Persons_age_verify_status]);
				p_pJson->SetNodeValue(strPre + Persons_sex, m_mapInfo[Persons_sex]);
				p_pJson->SetNodeValue(strPre + Persons_sex_verify_status, m_mapInfo[Persons_sex_verify_status]);
				p_pJson->SetNodeValue(strPre + Persons_birthday, m_mapInfo[Persons_birthday]);
				p_pJson->SetNodeValue(strPre + Persons_birthday_verify_status, m_mapInfo[Persons_birthday_verify_status]);
				p_pJson->SetNodeValue(strPre + Persons_mother_name, m_mapInfo[Persons_mother_name]);
				p_pJson->SetNodeValue(strPre + Persons_mother_name_verify_status, m_mapInfo[Persons_mother_name_verify_status]);
				p_pJson->SetNodeValue(strPre + Persons_remark, m_mapInfo[Persons_remark]);
				p_pJson->SetNodeValue(strPre + Persons_create_time, m_mapInfo[Persons_create_time]);
				p_pJson->SetNodeValue(strPre + Persons_create_user, m_mapInfo[Persons_create_user]);
				p_pJson->SetNodeValue(strPre + Persons_update_time, m_mapInfo[Persons_update_time]);
				p_pJson->SetNodeValue(strPre + Persons_update_user, m_mapInfo[Persons_update_user]);*/

				/*p_pJson->SetNodeValue(strPre + Persons_guid, m_str_guid);
				p_pJson->SetNodeValue(strPre + Persons_alarm_id, m_str_alarm_id);
				p_pJson->SetNodeValue(strPre + Persons_first_type, m_str_first_type);
				p_pJson->SetNodeValue(strPre + Persons_second_type, m_str_second_type);
				p_pJson->SetNodeValue(strPre + Persons_name, m_str_name);
				p_pJson->SetNodeValue(strPre + Persons_name_verify_status, m_str_name_verify_status);
				p_pJson->SetNodeValue(strPre + Persons_person_id, m_str_person_id);
				p_pJson->SetNodeValue(strPre + Persons_person_id_verify_status, m_str_person_id_verify_status);
				p_pJson->SetNodeValue(strPre + Persons_cpf, m_str_cpf);
				p_pJson->SetNodeValue(strPre + Persons_cpf_verify_status, m_str_cpf_verify_status);
				p_pJson->SetNodeValue(strPre + Persons_age, m_str_age);
				p_pJson->SetNodeValue(strPre + Persons_age_verify_status, m_str_age_verify_status);
				p_pJson->SetNodeValue(strPre + Persons_sex, m_str_sex);
				p_pJson->SetNodeValue(strPre + Persons_sex_verify_status, m_str_sex_verify_status);
				p_pJson->SetNodeValue(strPre + Persons_birthday, m_str_birthday);
				p_pJson->SetNodeValue(strPre + Persons_birthday_verify_status, m_str_birthday_verify_status);
				p_pJson->SetNodeValue(strPre + Persons_mother_name, m_str_mother_name);
				p_pJson->SetNodeValue(strPre + Persons_mother_name_verify_status, m_str_mother_name_verify_status);
				p_pJson->SetNodeValue(strPre + Persons_remark, m_str_remark);
				p_pJson->SetNodeValue(strPre + Persons_create_time, m_str_create_time);
				p_pJson->SetNodeValue(strPre + Persons_create_user, m_str_create_user);
				p_pJson->SetNodeValue(strPre + Persons_update_time, m_str_update_time);
				p_pJson->SetNodeValue(strPre + Persons_update_user, m_str_update_user);*/
			}

		public:
			std::map<std::string, std::string> m_mapInfo;

			std::string m_str_guid;
			std::string m_str_alarm_id;				    //����ID
			std::string m_str_first_type;			    //�永��Ա��Ŀ��֤�ˡ��ܺ��ˡ������˵�
			std::string m_str_second_type;			    //δ���ˡ����ˡ����ˡ�������
			std::string m_str_name;			            //�永��Ա����
			std::string m_str_name_verify_status;	    //������֤״̬
			std::string m_str_person_id;		        //��Ա��ţ����֤����
			std::string m_str_person_id_verify_status;	//��Ա�����֤״̬0:δ��֤��1������֤
			std::string m_str_cpf;			            //��˰�˺���
			std::string m_str_cpf_verify_status;		//��˰�˺�����֤״̬
			std::string m_str_age;				        //����
			std::string m_str_age_verify_status;		//������֤״̬
			std::string m_str_sex;				        //�Ա�
			std::string m_str_sex_verify_status;		//�Ա���֤״̬
			std::string m_str_birthday;                 //����
			std::string m_str_birthday_verify_status;   //������֤״̬
			std::string m_str_mother_name;              //ĸ������
			std::string m_str_mother_name_verify_status;//ĸ��������֤״̬
			std::string m_str_remark;                   //��ע
			std::string m_str_create_time;              //����ʱ��
			std::string m_str_create_user;              //������
			std::string m_str_update_time;              //����ʱ��
			std::string m_str_update_user;              //������


		};

		/************************************************************************/
		/*                            alarm related cars                        */
		/************************************************************************/

		const std::string Cars_guid = "guid";
		const std::string Cars_alarm_id = "alarm_id";
		const std::string Cars_car_type = "car_type";
		const std::string Cars_car_id = "car_id";
		const std::string Cars_car_id_verify_status = "car_id_verify_status";
		const std::string Cars_cpf = "cpf";
		const std::string Cars_cpf_verify_status = "cpf_verify_status";
		const std::string Cars_car_number = "car_number";
		const std::string Cars_car_number_verify_status = "car_number_verify_status";
		const std::string Cars_frame_number = "frame_number";
		const std::string Cars_frame_number_verify_status = "frame_number_verify_status";
		const std::string Cars_manufacture_date = "manufacture_date";
		const std::string Cars_manufacture_date_verify_status = "manufacture_date_verify_status";
		const std::string Cars_brand = "brand";
		const std::string Cars_brand_verify_status = "brand_verify_status";
		const std::string Cars_car_mode = "car_mode";
		const std::string Cars_car_mode_verify_status = "car_mode_verify_status";
		const std::string Cars_color = "color";
		const std::string Cars_color_verify_status = "color_verify_status";
		const std::string Cars_remark = "remark";
		const std::string Cars_create_time = "create_time";
		const std::string Cars_create_user = "create_user";
		const std::string Cars_update_time = "update_time";
		const std::string Cars_update_user = "update_user";

		class CAlarmRelatedCarsInfo
		{
		public:
			void ParseString(const std::string& strPre, JsonParser::IJsonPtr p_pJson)
			{
				m_mapInfo[Cars_guid] = p_pJson->GetNodeValue(strPre + Cars_guid, "");				
				m_mapInfo[Cars_alarm_id] = p_pJson->GetNodeValue(strPre + Cars_alarm_id, "");
				m_mapInfo[Cars_car_type] = p_pJson->GetNodeValue(strPre + Cars_car_type, "");
				m_mapInfo[Cars_car_id] = p_pJson->GetNodeValue(strPre + Cars_car_id, "");
				m_mapInfo[Cars_car_id_verify_status] = p_pJson->GetNodeValue(strPre + Cars_car_id_verify_status, "");
				m_mapInfo[Cars_cpf] = p_pJson->GetNodeValue(strPre + Cars_cpf, "");
				m_mapInfo[Cars_cpf_verify_status] = p_pJson->GetNodeValue(strPre + Cars_cpf_verify_status, "");
				m_mapInfo[Cars_car_number] = p_pJson->GetNodeValue(strPre + Cars_car_number, "");
				m_mapInfo[Cars_car_number_verify_status] = p_pJson->GetNodeValue(strPre + Cars_car_number_verify_status, "");
				m_mapInfo[Cars_frame_number] = p_pJson->GetNodeValue(strPre + Cars_frame_number, "");
				m_mapInfo[Cars_frame_number_verify_status] = p_pJson->GetNodeValue(strPre + Cars_frame_number_verify_status, "");
				m_mapInfo[Cars_manufacture_date] = p_pJson->GetNodeValue(strPre + Cars_manufacture_date, "");
				m_mapInfo[Cars_manufacture_date_verify_status] = p_pJson->GetNodeValue(strPre + Cars_manufacture_date_verify_status, "");
				m_mapInfo[Cars_brand] = p_pJson->GetNodeValue(strPre + Cars_brand, "");
				m_mapInfo[Cars_brand_verify_status] = p_pJson->GetNodeValue(strPre + Cars_brand_verify_status, "");
				m_mapInfo[Cars_car_mode] = p_pJson->GetNodeValue(strPre + Cars_car_mode, "");
				m_mapInfo[Cars_car_mode_verify_status] = p_pJson->GetNodeValue(strPre + Cars_car_mode_verify_status, "");
				m_mapInfo[Cars_color] = p_pJson->GetNodeValue(strPre + Cars_color, "");
				m_mapInfo[Cars_color_verify_status] = p_pJson->GetNodeValue(strPre + Cars_color_verify_status, "");
				m_mapInfo[Cars_remark] = p_pJson->GetNodeValue(strPre + Cars_remark, "");
				m_mapInfo[Cars_create_time] = p_pJson->GetNodeValue(strPre + Cars_create_time, "");
				m_mapInfo[Cars_create_user] = p_pJson->GetNodeValue(strPre + Cars_create_user, "");
				m_mapInfo[Cars_update_time] = p_pJson->GetNodeValue(strPre + Cars_update_time, "");
				m_mapInfo[Cars_update_user] = p_pJson->GetNodeValue(strPre + Cars_update_user, "");

				/*m_str_guid = p_pJson->GetNodeValue(strPre + Cars_guid, "");
				m_str_alarm_id = p_pJson->GetNodeValue(strPre + Cars_alarm_id, "");
				m_str_car_type = p_pJson->GetNodeValue(strPre + Cars_car_type, "");
				m_str_car_id = p_pJson->GetNodeValue(strPre + Cars_car_id, "");
				m_str_car_id_verify_status = p_pJson->GetNodeValue(strPre + Cars_car_id_verify_status, "");
				m_str_cpf = p_pJson->GetNodeValue(strPre + Cars_cpf, "");
				m_str_cpf_verify_status = p_pJson->GetNodeValue(strPre + Cars_cpf_verify_status, "");
				m_str_car_number = p_pJson->GetNodeValue(strPre + Cars_car_number, "");
				m_str_car_number_verify_status = p_pJson->GetNodeValue(strPre + Cars_car_number_verify_status, "");
				m_str_frame_number = p_pJson->GetNodeValue(strPre + Cars_frame_number, "");
				m_str_frame_number_verify_status = p_pJson->GetNodeValue(strPre + Cars_frame_number_verify_status, "");
				m_str_manufacture_date = p_pJson->GetNodeValue(strPre + Cars_manufacture_date, "");
				m_str_manufacture_date_verify_status = p_pJson->GetNodeValue(strPre + Cars_manufacture_date_verify_status, "");
				m_str_brand = p_pJson->GetNodeValue(strPre + Cars_brand, "");
				m_str_brand_verify_status = p_pJson->GetNodeValue(strPre + Cars_brand_verify_status, "");
				m_str_car_mode = p_pJson->GetNodeValue(strPre + Cars_car_mode, "");
				m_str_car_mode_verify_status = p_pJson->GetNodeValue(strPre + Cars_car_mode_verify_status, "");
				m_str_color = p_pJson->GetNodeValue(strPre + Cars_color, "");
				m_str_color_verify_status = p_pJson->GetNodeValue(strPre + Cars_color_verify_status, "");
				m_str_remark = p_pJson->GetNodeValue(strPre + Cars_remark, "");
				m_str_create_time = p_pJson->GetNodeValue(strPre + Cars_create_time, "");
				m_str_create_user = p_pJson->GetNodeValue(strPre + Cars_create_user, "");
				m_str_update_time = p_pJson->GetNodeValue(strPre + Cars_update_time, "");
				m_str_update_user = p_pJson->GetNodeValue(strPre + Cars_update_user, "");*/
			}

			void ToString(const std::string& strPre, JsonParser::IJsonPtr p_pJson)
			{
				std::map<std::string, std::string>::const_iterator itr_const;
				for (itr_const = m_mapInfo.begin(); itr_const != m_mapInfo.end(); ++itr_const)
				{
					p_pJson->SetNodeValue(strPre + itr_const->first, itr_const->second);
				}

				/*p_pJson->SetNodeValue(strPre + Cars_guid, m_mapInfo[Cars_guid]);
				p_pJson->SetNodeValue(strPre + Cars_alarm_id, m_mapInfo[Cars_alarm_id]);
				p_pJson->SetNodeValue(strPre + Cars_car_type, m_mapInfo[Cars_car_type]);
				p_pJson->SetNodeValue(strPre + Cars_car_id, m_mapInfo[Cars_car_id]);
				p_pJson->SetNodeValue(strPre + Cars_car_id_verify_status, m_mapInfo[Cars_car_id_verify_status]);
				p_pJson->SetNodeValue(strPre + Cars_cpf, m_mapInfo[Cars_cpf]);
				p_pJson->SetNodeValue(strPre + Cars_cpf_verify_status, m_mapInfo[Cars_cpf_verify_status]);
				p_pJson->SetNodeValue(strPre + Cars_car_number, m_mapInfo[Cars_car_number]);
				p_pJson->SetNodeValue(strPre + Cars_car_number_verify_status, m_mapInfo[Cars_car_number_verify_status]);
				p_pJson->SetNodeValue(strPre + Cars_frame_number, m_mapInfo[Cars_frame_number]);
				p_pJson->SetNodeValue(strPre + Cars_frame_number_verify_status, m_mapInfo[Cars_frame_number_verify_status]);
				p_pJson->SetNodeValue(strPre + Cars_manufacture_date, m_mapInfo[Cars_manufacture_date]);
				p_pJson->SetNodeValue(strPre + Cars_manufacture_date_verify_status, m_mapInfo[Cars_manufacture_date_verify_status]);
				p_pJson->SetNodeValue(strPre + Cars_brand, m_mapInfo[Cars_brand]);
				p_pJson->SetNodeValue(strPre + Cars_brand_verify_status, m_mapInfo[Cars_brand_verify_status]);
				p_pJson->SetNodeValue(strPre + Cars_car_mode, m_mapInfo[Cars_car_mode]);
				p_pJson->SetNodeValue(strPre + Cars_car_mode_verify_status, m_mapInfo[Cars_car_mode_verify_status]);
				p_pJson->SetNodeValue(strPre + Cars_color, m_mapInfo[Cars_color]);
				p_pJson->SetNodeValue(strPre + Cars_color_verify_status, m_mapInfo[Cars_color_verify_status]);
				p_pJson->SetNodeValue(strPre + Cars_remark, m_mapInfo[Cars_remark]);
				p_pJson->SetNodeValue(strPre + Cars_create_time, m_mapInfo[Cars_create_time]);
				p_pJson->SetNodeValue(strPre + Cars_create_user, m_mapInfo[Cars_create_user]);
				p_pJson->SetNodeValue(strPre + Cars_update_time, m_mapInfo[Cars_update_time]);
				p_pJson->SetNodeValue(strPre + Cars_update_user, m_mapInfo[Cars_update_user]);*/

				/*p_pJson->SetNodeValue(strPre + Cars_guid, m_str_guid);
				p_pJson->SetNodeValue(strPre + Cars_alarm_id, m_str_alarm_id);
				p_pJson->SetNodeValue(strPre + Cars_car_type, m_str_car_type);
				p_pJson->SetNodeValue(strPre + Cars_car_id, m_str_car_id);
				p_pJson->SetNodeValue(strPre + Cars_car_id_verify_status, m_str_car_id_verify_status);
				p_pJson->SetNodeValue(strPre + Cars_cpf, m_str_cpf);
				p_pJson->SetNodeValue(strPre + Cars_cpf_verify_status, m_str_cpf_verify_status);
				p_pJson->SetNodeValue(strPre + Cars_car_number, m_str_car_number);
				p_pJson->SetNodeValue(strPre + Cars_car_number_verify_status, m_str_car_number_verify_status);
				p_pJson->SetNodeValue(strPre + Cars_frame_number, m_str_frame_number);
				p_pJson->SetNodeValue(strPre + Cars_frame_number_verify_status, m_str_frame_number_verify_status);
				p_pJson->SetNodeValue(strPre + Cars_manufacture_date, m_str_manufacture_date);
				p_pJson->SetNodeValue(strPre + Cars_manufacture_date_verify_status, m_str_manufacture_date_verify_status);
				p_pJson->SetNodeValue(strPre + Cars_brand, m_str_brand);
				p_pJson->SetNodeValue(strPre + Cars_brand_verify_status, m_str_brand_verify_status);
				p_pJson->SetNodeValue(strPre + Cars_car_mode, m_str_car_mode);
				p_pJson->SetNodeValue(strPre + Cars_car_mode_verify_status, m_str_car_mode_verify_status);
				p_pJson->SetNodeValue(strPre + Cars_color, m_str_color);
				p_pJson->SetNodeValue(strPre + Cars_color_verify_status, m_str_color_verify_status);
				p_pJson->SetNodeValue(strPre + Cars_remark, m_str_remark);
				p_pJson->SetNodeValue(strPre + Cars_create_time, m_str_create_time);
				p_pJson->SetNodeValue(strPre + Cars_create_user, m_str_create_user);
				p_pJson->SetNodeValue(strPre + Cars_update_time, m_str_update_time);
				p_pJson->SetNodeValue(strPre + Cars_update_user, m_str_update_user);*/
			}

		public:
			std::map<std::string, std::string> m_mapInfo;

			std::string m_str_guid;
			std::string m_str_alarm_id;				          //����ID
			std::string m_str_car_type;                       //��ͨ�����������������޷����೵����
			std::string m_str_car_id;                         //�����ǼǺ���
			std::string m_str_car_id_verify_status;           //�����ǼǺ�����֤״̬
			std::string m_str_cpf;                            //����˰��
			std::string m_str_cpf_verify_status;              //����˰����֤״̬
			std::string m_str_car_number;                     //���ƺ�
			std::string m_str_car_number_verify_status;       //���ƺ���֤״̬
			std::string m_str_frame_number;                   //���ܺ�
			std::string m_str_frame_number_verify_status;     //���ܺ���֤״̬
			std::string m_str_manufacture_date;               //��������
			std::string m_str_manufacture_date_verify_status; //����������֤״̬
			std::string m_str_brand;                          //Ʒ��
			std::string m_str_brand_verify_status;            //Ʒ����֤״̬
			std::string m_str_car_mode;                       //�ͺ�
			std::string m_str_car_mode_verify_status;         //�ͺ���֤״̬
			std::string m_str_color;                          //��ɫ
			std::string m_str_color_verify_status;            //��ɫ��֤״̬
			std::string m_str_remark;                         //��ע
			std::string m_str_create_time;                    //����ʱ��
			std::string m_str_create_user;                    //������
			std::string m_str_update_time;                    //����ʱ��
			std::string m_str_update_user;	                  //������
		};


		/************************************************************************/
		/*                            alarm related matters                     */
		/************************************************************************/

		const std::string Matters_guid = "guid";
		const std::string Matters_alarm_id = "alarm_id";
		const std::string Matters_matter_type = "matter_type";
		const std::string Matters_name = "name";
		const std::string Matters_description = "description";
		const std::string Matters_remark = "remark";
		const std::string Matters_create_time = "create_time";
		const std::string Matters_create_user = "create_user";
		const std::string Matters_update_time = "update_time";
		const std::string Matters_update_user = "update_user";

		class CAlarmRelatedMattersInfo
		{
		public:
			void ParseString(const std::string& strPre, JsonParser::IJsonPtr p_pJson)
			{
				m_mapInfo[Matters_guid] = p_pJson->GetNodeValue(strPre + Matters_guid, "");
				m_mapInfo[Matters_alarm_id] = p_pJson->GetNodeValue(strPre + Matters_alarm_id, "");
				m_mapInfo[Matters_matter_type] = p_pJson->GetNodeValue(strPre + Matters_matter_type, "");
				m_mapInfo[Matters_name] = p_pJson->GetNodeValue(strPre + Matters_name, "");
				m_mapInfo[Matters_description] = p_pJson->GetNodeValue(strPre + Matters_description, "");
				m_mapInfo[Matters_remark] = p_pJson->GetNodeValue(strPre + Matters_remark, "");
				m_mapInfo[Matters_create_time] = p_pJson->GetNodeValue(strPre + Matters_create_time, "");
				m_mapInfo[Matters_create_user] = p_pJson->GetNodeValue(strPre + Matters_create_user, "");
				m_mapInfo[Matters_update_time] = p_pJson->GetNodeValue(strPre + Matters_update_time, "");
				m_mapInfo[Matters_update_user] = p_pJson->GetNodeValue(strPre + Matters_update_user, "");

				/*m_str_guid = p_pJson->GetNodeValue(strPre + Matters_guid, "");
				m_str_alarm_id = p_pJson->GetNodeValue(strPre + Matters_alarm_id, "");
				m_str_matter_type = p_pJson->GetNodeValue(strPre + Matters_matter_type, "");
				m_str_name = p_pJson->GetNodeValue(strPre + Matters_name, "");
				m_str_description = p_pJson->GetNodeValue(strPre + Matters_description, "");
				m_str_remark = p_pJson->GetNodeValue(strPre + Matters_remark, "");
				m_str_create_time = p_pJson->GetNodeValue(strPre + Matters_create_time, "");
				m_str_create_user = p_pJson->GetNodeValue(strPre + Matters_create_user, "");
				m_str_update_time = p_pJson->GetNodeValue(strPre + Matters_update_time, "");
				m_str_update_user = p_pJson->GetNodeValue(strPre + Matters_update_user, "");*/
			}

			void ToString(const std::string& strPre, JsonParser::IJsonPtr p_pJson)
			{
				std::map<std::string, std::string>::const_iterator itr_const;
				for (itr_const = m_mapInfo.begin(); itr_const != m_mapInfo.end(); ++itr_const)
				{
					p_pJson->SetNodeValue(strPre + itr_const->first, itr_const->second);
				}

				/*p_pJson->SetNodeValue(strPre + Matters_guid, m_mapInfo[Matters_guid]);
				p_pJson->SetNodeValue(strPre + Matters_alarm_id, m_mapInfo[Matters_alarm_id]);
				p_pJson->SetNodeValue(strPre + Matters_matter_type, m_mapInfo[Matters_matter_type]);
				p_pJson->SetNodeValue(strPre + Matters_name, m_mapInfo[Matters_name]);
				p_pJson->SetNodeValue(strPre + Matters_description, m_mapInfo[Matters_description]);
				p_pJson->SetNodeValue(strPre + Matters_remark, m_mapInfo[Matters_remark]);
				p_pJson->SetNodeValue(strPre + Matters_create_time, m_mapInfo[Matters_create_time]);
				p_pJson->SetNodeValue(strPre + Matters_create_user, m_mapInfo[Matters_create_user]);
				p_pJson->SetNodeValue(strPre + Matters_update_time, m_mapInfo[Matters_update_time]);
				p_pJson->SetNodeValue(strPre + Matters_update_user, m_mapInfo[Matters_update_user]);*/

				/*p_pJson->SetNodeValue(strPre + Matters_guid, m_str_guid);
				p_pJson->SetNodeValue(strPre + Matters_alarm_id, m_str_alarm_id);
				p_pJson->SetNodeValue(strPre + Matters_matter_type, m_str_matter_type);
				p_pJson->SetNodeValue(strPre + Matters_name, m_str_name);
				p_pJson->SetNodeValue(strPre + Matters_description, m_str_description);
				p_pJson->SetNodeValue(strPre + Matters_remark, m_str_remark);
				p_pJson->SetNodeValue(strPre + Matters_create_time, m_str_create_time);
				p_pJson->SetNodeValue(strPre + Matters_create_user, m_str_create_user);
				p_pJson->SetNodeValue(strPre + Matters_update_time, m_str_update_time);
				p_pJson->SetNodeValue(strPre + Matters_update_user, m_str_update_user);*/
			}

		public:
			std::map<std::string, std::string> m_mapInfo;
			std::string m_str_guid;
			std::string m_str_alarm_id;				          //����ID
			std::string m_str_matter_type;				      //����ID
			std::string m_str_name;				              //����ID
			std::string m_str_description;				      //����ID
			std::string m_str_remark;                         //��ע
			std::string m_str_create_time;                    //����ʱ��
			std::string m_str_create_user;                    //������
			std::string m_str_update_time;                    //����ʱ��
			std::string m_str_update_user;	                  //������
		};
	}
}

