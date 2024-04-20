#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetSignRespond :
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
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				unsigned int l_uiIndex = 0;
				for (auto alarm : m_oBody.m_vecAlarm)
				{
					//std::string l_strAlarmPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					std::string l_strAlarmPrefixPath("/body/data/" + std::to_string(l_uiIndex));
					alarm.ComJson(l_strAlarmPrefixPath, p_pJson);

					/*p_pJson->SetNodeValue(l_strAlarmPrefixPath + "id", alarm.m_strID);
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

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "create_user", alarm.m_strCreateUser);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "create_time", alarm.m_strCreateTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "update_user", alarm.m_strUpdateUser);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "update_time", alarm.m_strUpdateTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "is_privacy", alarm.m_strPrivacy);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "remark", alarm.m_strRemark);*/

					l_uiIndex++;
				}

				return p_pJson->ToString();
			}

		public:
			//class CAlarm
			//{
			//public:
			//	std::string	m_strID;						//����id
			//	std::string	m_strMergeID;					//����ϲ�id
			//	std::string m_strTitle;						//�������
			//	std::string m_strContent;					//��������
			//	std::string m_strTime;						//����id����ʱ��			
			//	std::string m_strActualOccurTime;			//����idʵ�ʷ���ʱ��
			//	std::string m_strAddr;						//����id��ϸ������ַ
			//	std::string m_strLongitude;					//����id����
			//	std::string m_strLatitude;					//����idά��
			//	std::string m_strState;						//����id״̬
			//	std::string m_strLevel;						//����id����
			//	std::string m_strSourceType;				//����id������Դ����
			//	std::string m_strSourceID;					//����id������Դid
			//	std::string m_strHandleType;				//����id��������
			//	std::string m_strFirstType;					//����idһ������
			//	std::string m_strSecondType;				//����id��������
			//	std::string m_strThirdType;					//����id��������
			//	std::string m_strFourthType;				//����id�ļ�����
			//	std::string m_strVehicleNo;					//����id��ͨ���ͱ������ƺ�
			//	std::string m_strVehicleType;				//����id��ͨ���ͱ���������
			//	std::string m_strSymbolCode;				//����id������ַ������ʾ����
			//	std::string m_strSymbolAddr;				//����id������ַ������ʾ���ַ
			//	std::string m_strFireBuildingType;			//����id������ȼ�ս�������

			//	std::string m_strEventType;					//�¼����ͣ����Ÿ���

			//	std::string m_strCalledNoType;				//����id���������ֵ�����
			//	std::string m_strActualCalledNoType;		//����idʵ�ʱ��������ֵ�����

			//	std::string m_strCallerNo;					//����id�����˺���
			//	std::string m_strCallerName;				//����id����������
			//	std::string m_strCallerAddr;				//����id�����˵�ַ
			//	std::string m_strCallerID;					//����id���������֤
			//	std::string m_strCallerIDType;				//����id���������֤����
			//	std::string m_strCallerGender;				//����id�������Ա�
			//	std::string m_strCallerAge;					//����id����������
			//	std::string m_strCallerBirthday;			//����id�����˳���������

			//	std::string m_strContactNo;					//����id��ϵ�˺���
			//	std::string m_strContactName;				//����id��ϵ������
			//	std::string m_strContactAddr;				//����id��ϵ�˵�ַ
			//	std::string m_strContactID;					//����id��ϵ�����֤
			//	std::string m_strContactIDType;				//����id��ϵ�����֤����
			//	std::string m_strContactGender;				//����id��ϵ���Ա�
			//	std::string m_strContactAge;				//����id��ϵ������
			//	std::string m_strContactBirthday;			//����id��ϵ�˳���������

			//	std::string m_strAdminDeptDistrictCode;		//����id��Ͻ��λ��������
			//	std::string m_strAdminDeptCode;				//����id��Ͻ��λ����
			//	std::string m_strAdminDeptName;				//����id��Ͻ��λ����

			//	std::string m_strReceiptDeptDistrictCode;	//����id�Ӿ���λ��������
			//	std::string m_strReceiptDeptCode;			//����id�Ӿ���λ����
			//	std::string m_strReceiptDeptName;			//����id�Ӿ���λ����
			//	std::string m_strLeaderCode;				//����idֵ���쵼����
			//	std::string m_strLeaderName;				//����idֵ���쵼����
			//	std::string m_strReceiptCode;				//����id�Ӿ��˾���
			//	std::string m_strReceiptName;				//����id�Ӿ�������


			//	std::string m_strDispatchSuggestion;		//�������

			//	std::string m_strCreateUser;				//������
			//	std::string m_strCreateTime;				//����ʱ��
			//	std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
			//	std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ
			//	std::string m_strPrivacy;				    //��˽���� true or false
			//	std::string m_strRemark;				    //������Ϣ
			//};
			class CBody
			{
			public:
				std::string m_strCount;
				std::vector<CAlarmInfo> m_vecAlarm;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}