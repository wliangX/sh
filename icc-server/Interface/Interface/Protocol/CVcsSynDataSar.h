#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CVcsAlarmSynDataSar :public IRequest, IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strCode = p_pJson->GetNodeValue("/code", "");
				m_strMessage = p_pJson->GetNodeValue("/message", "");

				int l_iCount = p_pJson->GetCount("/data");
				for (int i = 0; i < l_iCount; i++)
				{
					std::string p_strPrefix("/data/" + std::to_string(i));
					CData l_oData;
					l_oData.m_str_id = p_pJson->GetNodeValue(p_strPrefix + "/id", "");
					l_oData.m_str_alarm_id = p_pJson->GetNodeValue(p_strPrefix + "/alarm_id", "");
					l_oData.m_str_process_id = p_pJson->GetNodeValue(p_strPrefix + "/process_id", "");
					l_oData.m_str_feedback_id = p_pJson->GetNodeValue(p_strPrefix + "/feedback_id", "");
					l_oData.m_str_centre_process_dept_code = p_pJson->GetNodeValue(p_strPrefix + "/centre_process_dept_code", "");
					l_oData.m_str_centre_alarm_dept_code = p_pJson->GetNodeValue(p_strPrefix + "/centre_alarm_dept_code", "");
					l_oData.m_str_centre_process_id = p_pJson->GetNodeValue(p_strPrefix + "/centre_process_id", "");
					l_oData.m_str_centre_feedback_id = p_pJson->GetNodeValue(p_strPrefix + "/centre_feedback_id", "");
					l_oData.m_str_district = p_pJson->GetNodeValue(p_strPrefix + "/district", "");
					l_oData.m_str_district_name = p_pJson->GetNodeValue(p_strPrefix + "/district_name", "");
					l_oData.m_str_name = p_pJson->GetNodeValue(p_strPrefix + "/name", "");
					l_oData.m_str_sex = p_pJson->GetNodeValue(p_strPrefix + "/sex", "");
					l_oData.m_str_identification_type = p_pJson->GetNodeValue(p_strPrefix + "/identification_type", "");
					l_oData.m_str_identification_id = p_pJson->GetNodeValue(p_strPrefix + "/identification_id", "");
					l_oData.m_str_identity = p_pJson->GetNodeValue(p_strPrefix + "/identity", "");
					l_oData.m_str_is_focus_pesron = p_pJson->GetNodeValue(p_strPrefix + "/is_focus_pesron", "");
					l_oData.m_str_focus_pesron_info = p_pJson->GetNodeValue(p_strPrefix + "/focus_pesron_info", "");
					l_oData.m_str_registered_address_division = p_pJson->GetNodeValue(p_strPrefix + "/registered_address_division", "");
					l_oData.m_str_registered_address = p_pJson->GetNodeValue(p_strPrefix + "/registered_address", "");
					l_oData.m_str_current_address_division = p_pJson->GetNodeValue(p_strPrefix + "/current_address_division", "");
					l_oData.m_str_current_address = p_pJson->GetNodeValue(p_strPrefix + "/current_address", "");
					l_oData.m_str_work_unit = p_pJson->GetNodeValue(p_strPrefix + "/work_unit", "");
					l_oData.m_str_occupation = p_pJson->GetNodeValue(p_strPrefix + "/occupation", "");
					l_oData.m_str_contact_number = p_pJson->GetNodeValue(p_strPrefix + "/contact_number", "");
					l_oData.m_str_other_certificates = p_pJson->GetNodeValue(p_strPrefix + "/other_certificates", "");
					l_oData.m_str_lost_item_information = p_pJson->GetNodeValue(p_strPrefix + "/lost_item_information", "");
					l_oData.m_str_create_time = p_pJson->GetNodeValue(p_strPrefix + "/create_time", "");
					l_oData.m_str_update_time = p_pJson->GetNodeValue(p_strPrefix + "/update_time", "");
					l_oData.m_str_date_of_birth = p_pJson->GetNodeValue(p_strPrefix + "/date_of_birth", "");
					l_oData.m_str_digital_signature = p_pJson->GetNodeValue(p_strPrefix + "/digital_signature", "");
					l_oData.m_str_is_delete = p_pJson->GetNodeValue(p_strPrefix + "/is_delete", "");
					l_oData.m_str_createTeminal = p_pJson->GetNodeValue(p_strPrefix + "/createTeminal", "");
					l_oData.m_str_updateTeminal = p_pJson->GetNodeValue(p_strPrefix + "/updateTeminal", "");
					l_oData.m_str_nationality = p_pJson->GetNodeValue(p_strPrefix + "/nationality", "");
					l_oData.m_str_nation = p_pJson->GetNodeValue(p_strPrefix + "/nation", "");
					l_oData.m_str_educationLevel = p_pJson->GetNodeValue(p_strPrefix + "/educationLevel", "");
					l_oData.m_str_marriageStatus = p_pJson->GetNodeValue(p_strPrefix + "/marriageStatus", "");
					l_oData.m_str_politicalOutlook = p_pJson->GetNodeValue(p_strPrefix + "/politicalOutlook", "");
					l_oData.m_str_is_NPC_deputy = p_pJson->GetNodeValue(p_strPrefix + "/is_NPC_deputy", "");
					l_oData.m_str_is_national_staff = p_pJson->GetNodeValue(p_strPrefix + "/is_national_staff", "");
					l_oData.m_str_createUserId = p_pJson->GetNodeValue(p_strPrefix + "/createUserId", "");
					l_oData.m_str_createUserName = p_pJson->GetNodeValue(p_strPrefix + "/createUserName", "");
					l_oData.m_str_createUserOrgCode = p_pJson->GetNodeValue(p_strPrefix + "/createUserOrgCode", "");
					l_oData.m_str_createUserOrgName = p_pJson->GetNodeValue(p_strPrefix + "/createUserOrgName", "");
					l_oData.m_str_updateUserId = p_pJson->GetNodeValue(p_strPrefix + "/updateUserId", "");
					l_oData.m_str_updateUserName = p_pJson->GetNodeValue(p_strPrefix + "/updateUserName", "");
					l_oData.m_str_updateUserOrgCode = p_pJson->GetNodeValue(p_strPrefix + "/updateUserOrgCode", "");
					l_oData.m_str_updateUserOrgName = p_pJson->GetNodeValue(p_strPrefix + "/updateUserOrgName", "");
					m_vecData.push_back(l_oData);
				}
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/code", m_strCode);
				// p_pJson->SetNodeValue("/body/message", m_strMessage);
				p_pJson->SetNodeValue("/body/all_count", m_strAllCount);
				p_pJson->SetNodeValue("/body/page_index", m_strPageIndex);
				p_pJson->SetNodeValue("/body/time", m_strEndTime); 
				p_pJson->SetNodeValue("/body/count", m_strCount);
				for (size_t i = 0; i < m_vecData.size(); ++i)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/id", m_vecData.at(i).m_str_id);
					p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_id", m_vecData.at(i).m_str_alarm_id);
					p_pJson->SetNodeValue(l_strPrefixPath + "/process_id", m_vecData.at(i).m_str_process_id);
					p_pJson->SetNodeValue(l_strPrefixPath + "/feedback_id", m_vecData.at(i).m_str_feedback_id);
					p_pJson->SetNodeValue(l_strPrefixPath + "/centre_process_dept_code", m_vecData.at(i).m_str_centre_process_dept_code);
					p_pJson->SetNodeValue(l_strPrefixPath + "/centre_alarm_dept_code", m_vecData.at(i).m_str_centre_alarm_dept_code);
					p_pJson->SetNodeValue(l_strPrefixPath + "/centre_process_id", m_vecData.at(i).m_str_centre_process_id);
					p_pJson->SetNodeValue(l_strPrefixPath + "/centre_feedback_id", m_vecData.at(i).m_str_centre_feedback_id);
					p_pJson->SetNodeValue(l_strPrefixPath + "/district", m_vecData.at(i).m_str_district);
					p_pJson->SetNodeValue(l_strPrefixPath + "/district_name", m_vecData.at(i).m_str_district_name);
					p_pJson->SetNodeValue(l_strPrefixPath + "/name", m_vecData.at(i).m_str_name);
					p_pJson->SetNodeValue(l_strPrefixPath + "/sex", m_vecData.at(i).m_str_sex);
					p_pJson->SetNodeValue(l_strPrefixPath + "/identification_type", m_vecData.at(i).m_str_identification_type);
					p_pJson->SetNodeValue(l_strPrefixPath + "/identification_id", m_vecData.at(i).m_str_identification_id);
					p_pJson->SetNodeValue(l_strPrefixPath + "/identity", m_vecData.at(i).m_str_identity);
					p_pJson->SetNodeValue(l_strPrefixPath + "/is_focus_pesron", m_vecData.at(i).m_str_is_focus_pesron);
					p_pJson->SetNodeValue(l_strPrefixPath + "/focus_pesron_info", m_vecData.at(i).m_str_focus_pesron_info);
					p_pJson->SetNodeValue(l_strPrefixPath + "/registered_address_division", m_vecData.at(i).m_str_registered_address_division);
					p_pJson->SetNodeValue(l_strPrefixPath + "/registered_address", m_vecData.at(i).m_str_registered_address);
					p_pJson->SetNodeValue(l_strPrefixPath + "/current_address_division", m_vecData.at(i).m_str_current_address_division);
					p_pJson->SetNodeValue(l_strPrefixPath + "/current_address", m_vecData.at(i).m_str_current_address);
					p_pJson->SetNodeValue(l_strPrefixPath + "/work_unit", m_vecData.at(i).m_str_work_unit);
					p_pJson->SetNodeValue(l_strPrefixPath + "/occupation", m_vecData.at(i).m_str_occupation);
					p_pJson->SetNodeValue(l_strPrefixPath + "/contact_number", m_vecData.at(i).m_str_contact_number);
					p_pJson->SetNodeValue(l_strPrefixPath + "/other_certificates", m_vecData.at(i).m_str_other_certificates);
					p_pJson->SetNodeValue(l_strPrefixPath + "/lost_item_information", m_vecData.at(i).m_str_lost_item_information);
					p_pJson->SetNodeValue(l_strPrefixPath + "/create_time", m_vecData.at(i).m_str_create_time);
					p_pJson->SetNodeValue(l_strPrefixPath + "/update_time", m_vecData.at(i).m_str_update_time);
					p_pJson->SetNodeValue(l_strPrefixPath + "/date_of_birth", m_vecData.at(i).m_str_date_of_birth);
					p_pJson->SetNodeValue(l_strPrefixPath + "/digital_signature", m_vecData.at(i).m_str_digital_signature);
					p_pJson->SetNodeValue(l_strPrefixPath + "/is_delete", m_vecData.at(i).m_str_is_delete);
					p_pJson->SetNodeValue(l_strPrefixPath + "/createTeminal", m_vecData.at(i).m_str_createTeminal);
					p_pJson->SetNodeValue(l_strPrefixPath + "/updateTeminal", m_vecData.at(i).m_str_updateTeminal);
					p_pJson->SetNodeValue(l_strPrefixPath + "/nationality", m_vecData.at(i).m_str_nationality);
					p_pJson->SetNodeValue(l_strPrefixPath + "/nation", m_vecData.at(i).m_str_nation);
					p_pJson->SetNodeValue(l_strPrefixPath + "/educationLevel", m_vecData.at(i).m_str_educationLevel);
					p_pJson->SetNodeValue(l_strPrefixPath + "/marriageStatus", m_vecData.at(i).m_str_marriageStatus);
					p_pJson->SetNodeValue(l_strPrefixPath + "/politicalOutlook", m_vecData.at(i).m_str_politicalOutlook);
					p_pJson->SetNodeValue(l_strPrefixPath + "/is_NPC_deputy", m_vecData.at(i).m_str_is_NPC_deputy);
					p_pJson->SetNodeValue(l_strPrefixPath + "/is_national_staff", m_vecData.at(i).m_str_is_national_staff);
					p_pJson->SetNodeValue(l_strPrefixPath + "/createUserId", m_vecData.at(i).m_str_createUserId);
					p_pJson->SetNodeValue(l_strPrefixPath + "/createUserName", m_vecData.at(i).m_str_createUserName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/createUserOrgCode", m_vecData.at(i).m_str_createUserOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/createUserOrgName", m_vecData.at(i).m_str_createUserOrgName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/updateUserId", m_vecData.at(i).m_str_updateUserId);
					p_pJson->SetNodeValue(l_strPrefixPath + "/updateUserName", m_vecData.at(i).m_str_updateUserName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/updateUserOrgCode", m_vecData.at(i).m_str_updateUserOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/updateUserOrgName", m_vecData.at(i).m_str_updateUserOrgName);
				}
				return p_pJson->ToString();
			}
		public:
			class CData
			{
			public:
				std::string m_str_id;    // 警情当事人单编号
				std::string m_str_alarm_id;    // 接警单编号
				std::string m_str_process_id;    // 派警单编号
				std::string m_str_feedback_id;    // 反馈单编号
				std::string m_str_centre_process_dept_code;    // 部级警情当事人单编码
				std::string m_str_centre_alarm_dept_code;    // 部级接警单编码
				std::string m_str_centre_process_id;    // 部级派警单编码
				std::string m_str_centre_feedback_id;    // 部级反馈单编码
				std::string m_str_district;    // 行政区划代码
				std::string m_str_district_name;    // 行政区划名称
				std::string m_str_name;    // 警情当事人姓名
				std::string m_str_sex;    // 性别代码
				std::string m_str_identification_type;    // 证件代码
				std::string m_str_identification_id;    // 证件号码
				std::string m_str_identity;    // 警情当事人身份
				std::string m_str_is_focus_pesron;    // 是否重点人员
				std::string m_str_focus_pesron_info;    // 重点人员属性
				std::string m_str_registered_address_division;    // 户籍行政区划
				std::string m_str_registered_address;    // 户籍详址
				std::string m_str_current_address_division;    // 现住行政区划
				std::string m_str_current_address;    // 现住详址
				std::string m_str_work_unit;    // 工作单位
				std::string m_str_occupation;    // 职业
				std::string m_str_contact_number;    // 联系电话
				std::string m_str_other_certificates;    // 其他证件
				std::string m_str_lost_item_information;    // 损失物品信息
				std::string m_str_create_time;    // 入库时间
				std::string m_str_update_time;    // 更新时间
				std::string m_str_date_of_birth;    // 出生日期
				std::string m_str_digital_signature;    // 数字签名
				std::string m_str_is_delete;    // 删除标识
				std::string m_str_createTeminal;    // 创建终端标识
				std::string m_str_updateTeminal;    // 更新终端标识
				std::string m_str_nationality;    // 国籍
				std::string m_str_nation;    // 民族
				std::string m_str_educationLevel;    // 文化程度
				std::string m_str_marriageStatus;    // 婚姻状况
				std::string m_str_politicalOutlook;    // 政治面貌
				std::string m_str_is_NPC_deputy;    // 是否人大代表
				std::string m_str_is_national_staff;    // 国家工作人员
				std::string m_str_createUserId;    // 登记人证号
				std::string m_str_createUserName;    // 登记人姓名
				std::string m_str_createUserOrgCode;    // 登记单位
				std::string m_str_createUserOrgName;    // 登记单位名称
				std::string m_str_updateUserId;    // 修改人证号
				std::string m_str_updateUserName;    // 修改人姓名
				std::string m_str_updateUserOrgCode;    // 修改单位
				std::string m_str_updateUserOrgName;    // 修改单位名称
			};
			CHeaderEx m_oHeader;
			std::string m_strEndTime;
			std::string m_strCode;
			std::string m_strMessage;
			std::string m_strAllCount;
			std::string m_strPageIndex;
			std::string m_strCount;
			std::vector<CData> m_vecData;		
		};
	}
}
