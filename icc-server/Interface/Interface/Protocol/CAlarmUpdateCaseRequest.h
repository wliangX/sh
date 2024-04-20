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
				m_oBody.m_oAlarm.m_strReceiptCode = p_pJson->GetNodeValue("/body/JJYBH", "");  //接警员编号
				m_oBody.m_oAlarm.m_strTime = p_pJson->GetNodeValue("/body/BJSJ", ""); //报警时间
				//-------------------------------------------
				m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue("/body/XZQHDM", "");//案发行政区划代码
				m_oBody.m_oAlarm.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/JJDWDM", ""); //接警单位机构代码
				m_oBody.m_oAlarm.m_strMergeID = p_pJson->GetNodeValue("/body/GLZJJDBH", "");  //关联主接警单编号
				m_oBody.m_oAlarm.m_strCalledNoType = p_pJson->GetNodeValue("/body/JJLX", ""); //接警类型（110、122、119自接警和其他接警等）
				m_oBody.m_oAlarm.m_strSourceType = p_pJson->GetNodeValue("/body/BJFS", ""); //报警方式（电话报警、短信报警、网络报警等）
				m_oBody.m_oAlarm.m_strHandleType = p_pJson->GetNodeValue("/body/LHLX", "");  //来话类型
				//p_pJson->GetNodeValue("/body/JJLYH", ""); //接警录音号（接警时由录音系统自动产生，接处警系统自动关联）
				m_oBody.m_oAlarm.m_strReceiptName = p_pJson->GetNodeValue("/body/JJYXM", "");  //接警员姓名
				//l_oAlarm. p_pJson->GetNodeValue("/body/JJSJ", "");  //接警时间
				//p_pJson->GetNodeValue("/body/WCSJ", ""); //接警完成时间
				m_oBody.m_oAlarm.m_strCallerNo = p_pJson->GetNodeValue("/body/BJDH", "");  //报警电话
				//p_pJson->GetNodeValue("/body/BJDHYHXM", "");					//报警电话用户姓名
				m_oBody.m_oAlarm.m_strCallerID = p_pJson->GetNodeValue("/body/YHSFZ", "");					//用户身份证编号
				m_oBody.m_oAlarm.m_strContactAddr = p_pJson->GetNodeValue("/body/BJDHYHDZ", "");					//报警电话用户地址
				m_oBody.m_oAlarm.m_strContactName = p_pJson->GetNodeValue("/body/BJRXM", "");  //报警人姓名
				m_oBody.m_oAlarm.m_strContactGender = p_pJson->GetNodeValue("/body/BJRXBDM", "");  //报警人性别代码
				m_oBody.m_oAlarm.m_strContactNo = p_pJson->GetNodeValue("/body/LXDH", "");   //联系电话
				m_oBody.m_oAlarm.m_strAddr = p_pJson->GetNodeValue("/body/JQDZ", "");  //警情地址
				m_oBody.m_oAlarm.m_strContent = p_pJson->GetNodeValue("/body/BJNR", "");    //报警内容
				m_oBody.m_oAlarm.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/GXDWDM", "");  //管辖单位代码
				m_oBody.m_oAlarm.m_strFirstType = p_pJson->GetNodeValue("/body/JQLBDM", ""); //警情类别代码
				m_oBody.m_oAlarm.m_strSecondType = p_pJson->GetNodeValue("/body/JQLXDM", "");  //警情类型代码
				m_oBody.m_oAlarm.m_strThirdType = p_pJson->GetNodeValue("/body/JQXLDM", ""); //警情细类代码
				//l_oAlarm. = p_pJson->GetNodeValue("/body/TZDBH", "");  //特征点编号
				//l_oAlarm. = p_pJson->GetNodeValue("/body/YWWXWZ", ""); //有无危险物质
				//p_pJson->GetNodeValue("/body/YWBZXL", "");  //有无爆炸/泄漏
				//p_pJson->GetNodeValue("/body/BKRYQKSM", ""); //被困人员情况说明
				//p_pJson->GetNodeValue("/body/SSRYQKSM", "");  //受伤人员情况说明
				//p_pJson->GetNodeValue("/body/SWRYQKSM", "");  //死亡人员情况说明
				//p_pJson->GetNodeValue("/body/SFSWYBJ", "");  //是否是外语报警
				m_oBody.m_oAlarm.m_strLongitude = p_pJson->GetNodeValue("/body/BJRXZB", ""); //报警人定位X坐标
				m_oBody.m_oAlarm.m_strLatitude = p_pJson->GetNodeValue("/body/BJRYZB", "");  //报警人定位Y坐标
				//p_pJson->GetNodeValue("/body/FXDWJD", ""); //反向定位（手工地图拾取）X坐标
				//p_pJson->GetNodeValue("/body/FXDWWD", "");  //反向定位（手工地图拾取）Y坐标
				//p_pJson->GetNodeValue("/body/BCJJNR", "");  //补充接警内容
				m_oBody.m_oAlarm.m_strLevel = p_pJson->GetNodeValue("/body/JQDJDM", ""); //警情等级代码（四级警情）
				m_oBody.m_oAlarm.m_strState = p_pJson->GetNodeValue("/body/JQCLZTDM", ""); //警情处理状态代码
				//p_pJson->GetNodeValue("/body/YJJYDJDM", ""); //应急救援等级代码（四级）--
				//p_pJson->GetNodeValue("/body/SJCHPZLDM", ""); //涉及车号牌种类代码--
				//p_pJson->GetNodeValue("/body/SJCPH", ""); //涉及车号牌号
				//p_pJson->GetNodeValue("/body/SFSWHCL", "");  //是否是危化车辆
				//p_pJson->GetNodeValue("/body/RKSJ", "");  //入库时间
				m_oBody.m_oAlarm.m_strUpdateTime = p_pJson->GetNodeValue("/body/GXSJ", "");  //更新时间
				m_oBody.m_oAlarm.m_strTitle = p_pJson->GetNodeValue("/body/JQGJZ", ""); //警情关键词
				//p_pJson->GetNodeValue("/body/BM", "");//是否保密
				//p_pJson->GetNodeValue("/body/JJYID", "");  //接警员用户id
				//p_pJson->GetNodeValue("/body/ZHTBBTH", ""); //最后同步版本号
				//p_pJson->GetNodeValue("/body/SFMN", ""); //是否模拟事件
				m_oBody.m_oAlarm.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/GXDWID", ""); //管辖单位id
				m_oBody.m_oAlarm.m_strAdminDeptName = p_pJson->GetNodeValue("/body/GXDWMC", "");  //管辖单位名称
				//???????p_pJson->GetNodeValue("/body/JQZLDM", "");  //警情子类代码
				//p_pJson->GetNodeValue("/body/DQYWZT", "");  //当前业务状态
				//p_pJson->GetNodeValue("/body/GXDWIDENTIFIER", "");  //管辖单位:组织内部编号
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

