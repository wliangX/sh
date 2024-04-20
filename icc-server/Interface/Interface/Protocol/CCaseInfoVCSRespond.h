#pragma once
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CCaseInfoVCSRespond :
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

                unsigned int l_uiIndex = 0;
                for (CAlarmInfo data : m_oBody.m_vecData)
                {
                    std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					/*p_pJson->SetNodeValue(l_strPrefixPath + "id", data.m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", data.m_strMerge_ID);
                    p_pJson->SetNodeValue(l_strPrefixPath + "callerno", data.m_strCallerNo);
                    p_pJson->SetNodeValue(l_strPrefixPath + "calledno", data.m_strCalledNo);
                    p_pJson->SetNodeValue(l_strPrefixPath + "callingtime", data.m_strCallingTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "answeringtime", data.m_strAnsweringTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "seatno", data.m_strSeatNo);
                    p_pJson->SetNodeValue(l_strPrefixPath + "contact", data.m_strContact);
                    p_pJson->SetNodeValue(l_strPrefixPath + "contactno", data.m_strContactNo);
                    p_pJson->SetNodeValue(l_strPrefixPath + "contactsex", data.m_strContactSex);
                    p_pJson->SetNodeValue(l_strPrefixPath + "casedestrict", data.m_strCaseDestrict);
                    p_pJson->SetNodeValue(l_strPrefixPath + "areaorg", data.m_strAreaOrg);
                    p_pJson->SetNodeValue(l_strPrefixPath + "caseaddress", data.m_strCaseAddress);
                    p_pJson->SetNodeValue(l_strPrefixPath + "longitude", data.m_strLongitude);
                    p_pJson->SetNodeValue(l_strPrefixPath + "latitude", data.m_strLatitude);
                    p_pJson->SetNodeValue(l_strPrefixPath + "casetime", data.m_strCaseTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "casetype", data.m_strCaseType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "casesubtype", data.m_strCaseSubType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "casethreetype", data.m_strCaseThreeType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "casesource", data.m_strCaseSource);
                    p_pJson->SetNodeValue(l_strPrefixPath + "caselevel", data.m_strCaseLevel);
                    p_pJson->SetNodeValue(l_strPrefixPath + "casedesc", data.m_strCaseDesc);
                    p_pJson->SetNodeValue(l_strPrefixPath + "createuser", data.m_strCreateUser);
                    p_pJson->SetNodeValue(l_strPrefixPath + "createtime", data.m_strCreateTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "updateuser", data.m_strUpdateUser);
                    p_pJson->SetNodeValue(l_strPrefixPath + "updatetime", data.m_strUpdateTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "delflag", data.m_strDelFlag);
                    p_pJson->SetNodeValue(l_strPrefixPath + "status", data.m_strStatus);

					p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", data.m_strMsgSource);*/

					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", data.m_strReceiptDeptDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", data.m_strReceiptDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "id", data.m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", data.m_strMergeID);
					p_pJson->SetNodeValue(l_strPrefixPath + "merge_type", data.m_strMergeType);
					p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", data.m_strCalledNoType);
					p_pJson->SetNodeValue(l_strPrefixPath + "source_type", data.m_strSourceType);

					p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", data.m_strHandleType);
					p_pJson->SetNodeValue(l_strPrefixPath + "source_id", data.m_strSourceID);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", data.m_strReceiptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", data.m_strReceiptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "receiving_time", data.m_strTime);  //time����ֶ���
					p_pJson->SetNodeValue(l_strPrefixPath + "received_time", data.m_strReceivedTime);  //����
					p_pJson->SetNodeValue(l_strPrefixPath + "first_submit_time", data.m_strFirstSubmitTime); //first_submit_time  �����Լ��ǵ�	
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", data.m_strCallerNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_user_name", data.m_strCallerUserName); //����
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", data.m_strCallerAddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", data.m_strCallerName);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", data.m_strCallerGender);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", data.m_strContactNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", data.m_strCallerIDType);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", data.m_strCallerID);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_addr", data.m_strAlarmAddr);  //����  ������ַ
					p_pJson->SetNodeValue(l_strPrefixPath + "addr", data.m_strAddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "content", data.m_strContent);
					p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", data.m_strAdminDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "first_type", data.m_strFirstType);
					p_pJson->SetNodeValue(l_strPrefixPath + "second_type", data.m_strSecondType);
					p_pJson->SetNodeValue(l_strPrefixPath + "third_type", data.m_strThirdType);
					p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", data.m_strFourthType);
					p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", data.m_strSymbolCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "erpetrators_number", data.m_strErpetratorsNumber); //����  
					p_pJson->SetNodeValue(l_strPrefixPath + "is_armed", data.m_strIsArmed);  //����  
					p_pJson->SetNodeValue(l_strPrefixPath + "is_hazardous_substances", data.m_strIsHazardousSubstances);  //����  
					p_pJson->SetNodeValue(l_strPrefixPath + "is_explosion_or_leakage", data.m_strIsExplosionOrLeakage); //����  
					p_pJson->SetNodeValue(l_strPrefixPath + "desc_of_trapped", data.m_strDescOfTrapped); //����  
					p_pJson->SetNodeValue(l_strPrefixPath + "desc_of_injured", data.m_strDescOfInjured); //����  
					p_pJson->SetNodeValue(l_strPrefixPath + "desc_of_dead", data.m_strDescOfDead); //����  
					p_pJson->SetNodeValue(l_strPrefixPath + "is_foreign_language", data.m_strIsForeignLanguage); //����  
					p_pJson->SetNodeValue(l_strPrefixPath + "longitude", data.m_strLongitude);
					p_pJson->SetNodeValue(l_strPrefixPath + "latitude", data.m_strLatitude);
					p_pJson->SetNodeValue(l_strPrefixPath + "manual_longitude", data.m_strManualLongitude); //����  
					p_pJson->SetNodeValue(l_strPrefixPath + "manual_latitude", data.m_strManualLatitude);//����  
					p_pJson->SetNodeValue(l_strPrefixPath + "remark", data.m_strRemark);
					p_pJson->SetNodeValue(l_strPrefixPath + "level", data.m_strLevel);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", data.m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "emergency_rescue_level", data.m_strEmergencyRescueLevel); //����  
					p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", data.m_strVehicleType);
					p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", data.m_strVehicleNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_hazardous_vehicle", data.m_strIsHazardousVehicle); //����
					p_pJson->SetNodeValue(l_strPrefixPath + "create_time", data.m_strCreateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_time", data.m_strUpdateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "label", data.m_strTitle);  //title����ֶ���
					p_pJson->SetNodeValue(l_strPrefixPath + "is_privacy", data.m_strPrivacy);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_srv_name", data.m_strReceiptSrvName);  //����
					p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", data.m_strAdminDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_org_code", data.m_strAdminDeptOrgCode);   //����
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", data.m_strReceiptDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_org_code", data.m_strReceiptDeptOrgCode);   //����
					p_pJson->SetNodeValue(l_strPrefixPath + "is_delete", data.m_strDeleteFlag);//is_delete����Э���д���
					p_pJson->SetNodeValue(l_strPrefixPath + "is_invalid", data.m_strIsInvalid);   //����
					p_pJson->SetNodeValue(l_strPrefixPath + "business_status", data.m_strBusinessState);  //����
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_seatno", data.m_strSeatNo);   //seatno����
					p_pJson->SetNodeValue(l_strPrefixPath + "is_merge", data.m_strIsMerge);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_user", data.m_strCreateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_user", data.m_strUpdateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_visitor", data.m_strIsVisitor);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_feedback", data.m_strIsFeedBack);
                    ++l_uiIndex;
                }

				return p_pJson->ToString();
			}

		public:
    //        class CData
    //        {
    //        public:
				//std::string m_strID;
				//std::string m_strMerge_ID;					//����ϲ�ID
    //            std::string m_strCallerNo;					//����id�����˺���
    //            std::string m_strCalledNo;		    		//����id���������ֵ�����
    //            std::string m_strCallingTime;		    	//����ʱ��
    //            std::string m_strAnsweringTime;		    	//Ӧ��ʱ��
    //            std::string m_strSeatNo;		    		//�Ӿ���ϯ��
    //            std::string m_strContact;				    //��ϵ��
    //            std::string m_strContactNo;					//��ϵ�绰
    //            std::string m_strContactSex;				//��ϵ���Ա�
    //            std::string m_strCaseDestrict;		        //������������
    //            std::string m_strAreaOrg;		            //Ͻ����λ
    //            std::string m_strCaseAddress;		        //������ַ
    //            std::string m_strLongitude;					//����
    //            std::string m_strLatitude;					//ά��
    //            std::string m_strCaseTime;					//����ʱ��
    //            std::string m_strCaseType;					//��������
    //            std::string m_strCaseSubType;				//��������
    //            std::string m_strCaseThreeType;				//����С��
    //            std::string m_strCaseSource;				//������Դ
    //            std::string m_strCaseLevel;					//��������
    //            std::string m_strCaseDesc;					//��������
    //            std::string m_strCreateUser;				//������
    //            std::string m_strCreateTime;				//����ʱ��
    //            std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
    //            std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ
    //            std::string m_strDelFlag;				    //0��Ч��1ɾ��
    //            std::string m_strStatus;				    //����״̬
    //        };
            class CBody
            {
            public:
                std::string m_strCount;
                std::vector<CAlarmInfo> m_vecData;
            };
            CHeader m_oHeader;
            CBody	m_oBody;
		};
	}
}
