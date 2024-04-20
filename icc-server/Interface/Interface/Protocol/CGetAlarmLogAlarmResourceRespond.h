#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/CAlarmInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAlarmLogAlarmResRespond :
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
				int i = 0;
				for (auto iter = m_oBody.m_vData.begin(); iter != m_oBody.m_vData.end(); iter++)
				{

					std::string l_strPrefixPath("/body/data/" + std::to_string(i));

					p_pJson->SetNodeValue(l_strPrefixPath + "/guid", iter->m_strGuID);
					p_pJson->SetNodeValue(l_strPrefixPath + "/flag", iter->m_strFlag);
					iter->m_oAlarm.ComJson(l_strPrefixPath, p_pJson);
					/*p_pJson->SetNodeValue(l_strPrefixPath + "id", iter->m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", iter->m_strMergeID);
					p_pJson->SetNodeValue(l_strPrefixPath + "title", iter->m_strTitle);
					p_pJson->SetNodeValue(l_strPrefixPath + "content", iter->m_strContent);
					p_pJson->SetNodeValue(l_strPrefixPath + "time", iter->m_strTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_time", iter->m_strActualOccurTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "addr", iter->m_strAddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "longitude", iter->m_strLongitude);
					p_pJson->SetNodeValue(l_strPrefixPath + "latitude", iter->m_strLatitude);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", iter->m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "level", iter->m_strLevel);
					p_pJson->SetNodeValue(l_strPrefixPath + "source_type", iter->m_strSourceType);
					p_pJson->SetNodeValue(l_strPrefixPath + "source_id", iter->m_strSourceID);
					p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", iter->m_strHandleType);
					p_pJson->SetNodeValue(l_strPrefixPath + "first_type", iter->m_strFirstType);
					p_pJson->SetNodeValue(l_strPrefixPath + "second_type", iter->m_strSecondType);
					p_pJson->SetNodeValue(l_strPrefixPath + "third_type", iter->m_strThirdType);
					p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", iter->m_strFourthType);
					p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", iter->m_strVehicleNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", iter->m_strVehicleType);
					p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", iter->m_strSymbolCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "symbol_addr", iter->m_strSymbolAddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_type", iter->m_strFireBuildingType);

					p_pJson->SetNodeValue(l_strPrefixPath + "event_type", iter->m_strEventType);

					p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", iter->m_strCalledNoType);
					p_pJson->SetNodeValue(l_strPrefixPath + "actual_called_no_type", iter->m_strActualCalledNoType);

					p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", iter->m_strCallerNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", iter->m_strCallerName);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", iter->m_strCallerAddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", iter->m_strCallerID);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", iter->m_strCallerIDType);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", iter->m_strCallerGender);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_age", iter->m_strCallerAge);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_birthday", iter->m_strCallerBirthday);

					p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", iter->m_strContactNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_name", iter->m_strContactName);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_addr", iter->m_strContactAddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_id", iter->m_strContactID);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_id_type", iter->m_strContactIDType);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_gender", iter->m_strContactGender);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_age", iter->m_strContactAge);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_birthday", iter->m_strContactBirthday);

					p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_district_code", iter->m_strAdminDeptDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", iter->m_strAdminDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", iter->m_strAdminDeptName);

					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", iter->m_strReceiptDeptDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", iter->m_strReceiptDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", iter->m_strReceiptDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "leader_code", iter->m_strLeaderCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "leader_name", iter->m_strLeaderName);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", iter->m_strReceiptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", iter->m_strReceiptName);

					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", iter->m_strDispatchSuggestion);
					p_pJson->SetNodeValue(l_strPrefixPath + "flag", iter->m_strFlag);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_user", iter->m_strCreateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_time", iter->m_strCreateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_user", iter->m_strUpdateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_time", iter->m_strUpdateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_privacy", iter->m_strPrivacy);
					p_pJson->SetNodeValue(l_strPrefixPath + "remark", iter->m_strRemark);*/
					i++;
				}
				return p_pJson->ToString();
			}
		public:
			class CData
			{
			public:
				std::string m_strGuID;	
				CAlarmInfo  m_oAlarm;
				//std::string	m_strID;						//����id
				//std::string	m_strMergeID;					//����ϲ�id
				//std::string m_strTitle;						//�������
				//std::string m_strContent;					//��������
				//std::string m_strTime;						//����id����ʱ��			
				//std::string m_strActualOccurTime;			//����idʵ�ʷ���ʱ��
				//std::string m_strAddr;						//����id��ϸ������ַ
				//std::string m_strLongitude;					//����id����
				//std::string m_strLatitude;					//����idά��
				//std::string m_strState;						//����id״̬
				//std::string m_strLevel;						//����id����
				//std::string m_strSourceType;				//����id������Դ����
				//std::string m_strSourceID;					//����id������Դid
				//std::string m_strHandleType;				//����id��������
				//std::string m_strFirstType;					//����idһ������
				//std::string m_strSecondType;				//����id��������
				//std::string m_strThirdType;					//����id��������
				//std::string m_strFourthType;				//����id�ļ�����
				//std::string m_strVehicleNo;					//����id��ͨ���ͱ������ƺ�
				//std::string m_strVehicleType;				//����id��ͨ���ͱ���������
				//std::string m_strSymbolCode;				//����id������ַ������ʾ����
				//std::string m_strSymbolAddr;				//����id������ַ������ʾ���ַ
				//std::string m_strFireBuildingType;			//����id������ȼ�ս�������

				//std::string m_strEventType;					//�¼����ͣ����Ÿ���

				//std::string m_strCalledNoType;				//����id���������ֵ�����
				//std::string m_strActualCalledNoType;		//����idʵ�ʱ��������ֵ�����

				//std::string m_strCallerNo;					//����id�����˺���
				//std::string m_strCallerName;				//����id����������
				//std::string m_strCallerAddr;				//����id�����˵�ַ
				//std::string m_strCallerID;					//����id���������֤
				//std::string m_strCallerIDType;				//����id���������֤����
				//std::string m_strCallerGender;				//����id�������Ա�
				//std::string m_strCallerAge;					//����id����������
				//std::string m_strCallerBirthday;			//����id�����˳���������

				//std::string m_strContactNo;					//����id��ϵ�˺���
				//std::string m_strContactName;				//����id��ϵ������
				//std::string m_strContactAddr;				//����id��ϵ�˵�ַ
				//std::string m_strContactID;					//����id��ϵ�����֤
				//std::string m_strContactIDType;				//����id��ϵ�����֤����
				//std::string m_strContactGender;				//����id��ϵ���Ա�
				//std::string m_strContactAge;				//����id��ϵ������
				//std::string m_strContactBirthday;			//����id��ϵ�˳���������

				//std::string m_strAdminDeptDistrictCode;		//����id��Ͻ��λ��������
				//std::string m_strAdminDeptCode;				//����id��Ͻ��λ����
				//std::string m_strAdminDeptName;				//����id��Ͻ��λ����

				//std::string m_strReceiptDeptDistrictCode;	//����id�Ӿ���λ��������
				//std::string m_strReceiptDeptCode;			//����id�Ӿ���λ����
				//std::string m_strReceiptDeptName;			//����id�Ӿ���λ����
				//std::string m_strLeaderCode;				//����idֵ���쵼����
				//std::string m_strLeaderName;				//����idֵ���쵼����
				//std::string m_strReceiptCode;				//����id�Ӿ��˾���
				//std::string m_strReceiptName;				//����id�Ӿ�������

				//std::string m_strDispatchSuggestion;		//�������

				std::string m_strFlag;		

				//std::string m_strCreateUser;				//������
				//std::string m_strCreateTime;				//����ʱ��
				//std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
				//std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ	

				//std::string m_strPrivacy;				    //��˽���� true or false
				//std::string m_strRemark;				    //������Ϣ
			};
			class CBody
			{
			public:
				std::vector<CData> m_vData;
			};
			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
