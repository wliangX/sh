#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>
#include <Protocol/CAlarmInfo.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchAlarmRespond :
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
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strTotalAlarmCount);
				
				unsigned int l_uiIndex = 0;
				for (CData &data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex));
					data.m_oAlarm.ComJson(l_strPrefixPath, p_pJson);
					//p_pJson->SetNodeValue(l_strPrefixPath + "id", data.m_strID);
					//p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", data.m_strMergeID);
					//p_pJson->SetNodeValue(l_strPrefixPath + "title", data.m_strTitle);
					//p_pJson->SetNodeValue(l_strPrefixPath + "content", data.m_strContent);
					//p_pJson->SetNodeValue(l_strPrefixPath + "time", data.m_strTime);
					//p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_time", data.m_strActualOccurTime);
					//p_pJson->SetNodeValue(l_strPrefixPath + "addr", data.m_strAddr);
					//p_pJson->SetNodeValue(l_strPrefixPath + "longitude", data.m_strLongitude);
					//p_pJson->SetNodeValue(l_strPrefixPath + "latitude", data.m_strLatitude);
					//p_pJson->SetNodeValue(l_strPrefixPath + "state", data.m_strState);
					//p_pJson->SetNodeValue(l_strPrefixPath + "level", data.m_strLevel);
					//p_pJson->SetNodeValue(l_strPrefixPath + "source_type", data.m_strSourceType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "source_id", data.m_strSourceID);
					//p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", data.m_strHandleType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "first_type", data.m_strFirstType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "second_type", data.m_strSecondType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "third_type", data.m_strThirdType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", data.m_strFourthType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", data.m_strVehicleNo);
					//p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", data.m_strVehicleType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", data.m_strSymbolCode);
					//p_pJson->SetNodeValue(l_strPrefixPath + "symbol_addr", data.m_strSymbolAddr);
					//p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_type", data.m_strFireBuildingType);

					///*p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_foreign", data.m_strIsInvolveForeign);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_police", data.m_strIsInvolvePolice);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_gun", data.m_strIsInvolveGun);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_knife", data.m_strIsInvolveKnife);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_terror", data.m_strIsInvolveTerror);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_pornography", data.m_strIsInvolvePornography);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_gamble", data.m_strIsInvolveGamble);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_poison", data.m_strIsInvolvePoison);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext1", data.m_strIsInvolveExt1);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext2", data.m_strIsInvolveExt2);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext3", data.m_strIsInvolveExt3);*/
					//p_pJson->SetNodeValue(l_strPrefixPath + "event_type", data.m_strEventType);

					//p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", data.m_strCalledNoType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "actual_called_no_type", data.m_strActualCalledNoType);

					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", data.m_strCallerNo);
					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", data.m_strCallerName);
					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", data.m_strCallerAddr);
					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", data.m_strCallerID);
					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", data.m_strCallerIDType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", data.m_strCallerGender);
					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_age", data.m_strCallerAge);
					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_birthday", data.m_strCallerBirthday);

					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", data.m_strContactNo);
					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_name", data.m_strContactName);
					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_addr", data.m_strContactAddr);
					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_id", data.m_strContactID);
					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_id_type", data.m_strContactIDType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_gender", data.m_strContactGender);
					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_age", data.m_strContactAge);
					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_birthday", data.m_strContactBirthday);

					//p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_district_code", data.m_strAdminDeptDistrictCode);
					//p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", data.m_strAdminDeptCode);
					//p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", data.m_strAdminDeptName);

					//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", data.m_strReceiptDeptDistrictCode);
					//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", data.m_strReceiptDeptCode);
					//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", data.m_strReceiptDeptName);
					//p_pJson->SetNodeValue(l_strPrefixPath + "leader_code", data.m_strLeaderCode);
					//p_pJson->SetNodeValue(l_strPrefixPath + "leader_name", data.m_strLeaderName);
					//p_pJson->SetNodeValue(l_strPrefixPath + "seatno", data.m_strSeatNo);
					//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", data.m_strReceiptCode);
					//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", data.m_strReceiptName);

					//p_pJson->SetNodeValue(l_strPrefixPath + "is_merge", data.m_strIsMerge);
					//p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", data.m_strDispatchSuggestion);
					//
					//p_pJson->SetNodeValue(l_strPrefixPath + "create_user", data.m_strCreateUser);
					//p_pJson->SetNodeValue(l_strPrefixPath + "create_time", data.m_strCreateTime);
					//p_pJson->SetNodeValue(l_strPrefixPath + "update_user", data.m_strUpdateUser);
					//p_pJson->SetNodeValue(l_strPrefixPath + "update_time", data.m_strUpdateTime);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_privacy", data.m_strPrivacy);
					//p_pJson->SetNodeValue(l_strPrefixPath + "remark", data.m_strRemark);

					p_pJson->SetNodeValue(l_strPrefixPath + "/acd", data.m_strAcd);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				//std::string	m_strID;						//����id
				//std::string	m_strMergeID;					//����ϲ�id
				//std::string m_strTitle;						//�������
				//std::string m_strContent;					//��������
				//std::string m_strTime;						//���鱨��ʱ��			
				//std::string m_strActualOccurTime;			//����ʵ�ʷ���ʱ��
				//std::string m_strAddr;						//������ϸ������ַ
				//std::string m_strLongitude;					//���龭��
				//std::string m_strLatitude;					//����ά��
				//std::string m_strState;						//����״̬
				//std::string m_strLevel;						//���鼶��
				//std::string m_strSourceType;				//���鱨����Դ����
				//std::string m_strSourceID;					//���鱨����Դid
				//std::string m_strHandleType;				//���鴦������
				//std::string m_strFirstType;					//����һ������
				//std::string m_strSecondType;				//�����������
				//std::string m_strThirdType;					//������������
				//std::string m_strFourthType;				//�����ļ�����

				//std::string m_strVehicleNo;					//���齻ͨ���ͱ������ƺ�
				//std::string m_strVehicleType;				//���齻ͨ���ͱ���������

				//std::string m_strSymbolCode;				//���鷢����ַ������ʾ����
				//std::string m_strSymbolAddr;				//���鷢����ַ������ʾ���ַ

				//std::string m_strFireBuildingType;			//���������ȼ�ս�������
				//std::string m_strEventType;					//�¼�����

				//std::string m_strCalledNoType;				//���鱨�������ֵ�����
				//std::string m_strActualCalledNoType;		//����ʵ�ʱ��������ֵ�����

				//std::string m_strCallerNo;					//���鱨���˺���
				//std::string m_strCallerName;				//���鱨��������
				//std::string m_strCallerAddr;				//���鱨���˵�ַ
				//std::string m_strCallerID;					//���鱨�������֤
				//std::string m_strCallerIDType;				//���鱨�������֤����
				//std::string m_strCallerGender;				//���鱨�����Ա�
				//std::string m_strCallerAge;					//���鱨��������
				//std::string m_strCallerBirthday;			//���鱨���˳���������

				//std::string m_strContactNo;					//������ϵ�˺���
				//std::string m_strContactName;				//������ϵ������
				//std::string m_strContactAddr;				//������ϵ�˵�ַ
				//std::string m_strContactID;					//������ϵ�����֤
				//std::string m_strContactIDType;				//������ϵ�����֤����
				//std::string m_strContactGender;				//������ϵ���Ա�
				//std::string m_strContactAge;				//������ϵ������
				//std::string m_strContactBirthday;			//������ϵ�˳���������

				//std::string m_strAdminDeptDistrictCode;		//�����Ͻ��λ��������
				//std::string m_strAdminDeptCode;				//�����Ͻ��λ����
				//std::string m_strAdminDeptName;				//�����Ͻ��λ����

				//std::string m_strReceiptDeptDistrictCode;	//����Ӿ���λ��������
				//std::string m_strReceiptDeptCode;			//����Ӿ���λ����
				//std::string m_strReceiptDeptName;			//����Ӿ���λ����

				//std::string m_strLeaderCode;				//����ֵ���쵼����
				//std::string m_strLeaderName;				//����ֵ���쵼����

				//std::string m_strSeatNo;					//�Ӿ�����ϯ��
				//std::string m_strReceiptCode;				//����Ӿ��˾���
				//std::string m_strReceiptName;				//����Ӿ�������
				//
				//std::string m_strIsMerge;					//�Ƿ�ϲ������־
				//std::string m_strDispatchSuggestion;		//�������

				//std::string m_strCreateUser;				//������
				//std::string m_strCreateTime;				//����ʱ��
				//std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
				//std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ
				CAlarmInfo m_oAlarm;

				std::string m_strAcd;

				//std::string m_strCityCode;
				//std::string m_strPrivacy;					//��˽����  true or false	
				//std::string m_strRemark;				    //������Ϣ
			};
			class CBody
			{
			public:
				std::string m_strCount;					//�ְ�����
				std::string m_strTotalAlarmCount;		//��������
				std::vector<CData> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}