#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/CAddAlarm.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAddAlarm :
			public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_alarm.ParseString("/body", p_pJson);
				/*m_oBody.m_alarm.m_strID = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_alarm.m_strMergeID = p_pJson->GetNodeValue("/body/merge_id", "");
				m_oBody.m_alarm.m_strTitle = p_pJson->GetNodeValue("/body/alarm/title", "");
				m_oBody.m_alarm.m_strContent = p_pJson->GetNodeValue("/body/content", "");
				m_oBody.m_alarm.m_strTime = p_pJson->GetNodeValue("/body/time", "");
				m_oBody.m_alarm.m_strActualOccurTime = p_pJson->GetNodeValue("/body/actual_occur_time", "");
				m_oBody.m_alarm.m_strAddr = p_pJson->GetNodeValue("/body/addr", "");
				m_oBody.m_alarm.m_strLongitude = p_pJson->GetNodeValue("/body/longitude", "");
				m_oBody.m_alarm.m_strLatitude = p_pJson->GetNodeValue("/body/latitude", "");
				m_oBody.m_alarm.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_alarm.m_strLevel = p_pJson->GetNodeValue("/body/level", "");
				m_oBody.m_alarm.m_strSourceType = p_pJson->GetNodeValue("/body/source_type", "");
				m_oBody.m_alarm.m_strSourceID = p_pJson->GetNodeValue("/body/source_id", "");
				m_oBody.m_alarm.m_strHandleType = p_pJson->GetNodeValue("/body/handle_type", "");
				m_oBody.m_alarm.m_strFirstType = p_pJson->GetNodeValue("/body/first_type", "");
				m_oBody.m_alarm.m_strSecondType = p_pJson->GetNodeValue("/body/second_type", "");
				m_oBody.m_alarm.m_strThirdType = p_pJson->GetNodeValue("/body/third_type", "");
				m_oBody.m_alarm.m_strFourthType = p_pJson->GetNodeValue("/body/fourth_type", "");
				m_oBody.m_alarm.m_strVehicleNo = p_pJson->GetNodeValue("/body/vehicle_no", "");
				m_oBody.m_alarm.m_strVehicleType = p_pJson->GetNodeValue("/body/vehicle_type", "");
				m_oBody.m_alarm.m_strSymbolCode = p_pJson->GetNodeValue("/body/symbol_code", "");
				m_oBody.m_alarm.m_strSymbolAddr = p_pJson->GetNodeValue("/body/symbol_addr", "");
				m_oBody.m_alarm.m_strFireBuildingType = p_pJson->GetNodeValue("/body/fire_building_type", "");
				m_oBody.m_alarm.m_strDispatchSuggestion = p_pJson->GetNodeValue("/body/dispatch_suggestion", "");
				m_oBody.m_alarm.m_strEventType = p_pJson->GetNodeValue("/body/event_type", "");
				m_oBody.m_alarm.m_strCalledNoType = p_pJson->GetNodeValue("/body/called_no_type", "");
				m_oBody.m_alarm.m_strActualCalledNoType = p_pJson->GetNodeValue("/body/actual_called_no_type", "");
				m_oBody.m_alarm.m_strCallerNo = p_pJson->GetNodeValue("/body/caller_no", "");
				m_oBody.m_alarm.m_strCallerName = p_pJson->GetNodeValue("/body/caller_name", "");
				m_oBody.m_alarm.m_strCallerAddr = p_pJson->GetNodeValue("/body/caller_addr", "");
				m_oBody.m_alarm.m_strCallerID = p_pJson->GetNodeValue("/body/caller_id", "");
				m_oBody.m_alarm.m_strCallerIDType = p_pJson->GetNodeValue("/body/caller_id_type", "");
				m_oBody.m_alarm.m_strCallerGender = p_pJson->GetNodeValue("/body/caller_gender", "");
				m_oBody.m_alarm.m_strCallerAge = p_pJson->GetNodeValue("/body/caller_age", "");
				m_oBody.m_alarm.m_strCallerBirthday = p_pJson->GetNodeValue("/body/caller_birthday", "");
				m_oBody.m_alarm.m_strContactNo = p_pJson->GetNodeValue("/body/contact_no", "");
				m_oBody.m_alarm.m_strContactName = p_pJson->GetNodeValue("/body/contact_name", "");
				m_oBody.m_alarm.m_strContactAddr = p_pJson->GetNodeValue("/body/contact_addr", "");
				m_oBody.m_alarm.m_strContactID = p_pJson->GetNodeValue("/body/contact_id", "");
				m_oBody.m_alarm.m_strContactIDType = p_pJson->GetNodeValue("/body/contact_id_type", "");
				m_oBody.m_alarm.m_strContactAge = p_pJson->GetNodeValue("/body/contact_age", "");
				m_oBody.m_alarm.m_strContactBirthday = p_pJson->GetNodeValue("/body/contact_birthday", "");
				m_oBody.m_alarm.m_strAdminDeptDistrictCode = p_pJson->GetNodeValue("/body/admin_dept_ district_code", "");
				m_oBody.m_alarm.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/admin_dept_code", "");
				m_oBody.m_alarm.m_strAdminDeptName = p_pJson->GetNodeValue("/body/admin_dept_name", "");
				m_oBody.m_alarm.m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue("/body/receipt_dept_district_code", "");
				m_oBody.m_alarm.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/receipt_dept_code", "");
				m_oBody.m_alarm.m_strReceiptDeptName = p_pJson->GetNodeValue("/body/receipt_dept_name", "");
				m_oBody.m_alarm.m_strLeaderCode = p_pJson->GetNodeValue("/body/leader_code", "");
				m_oBody.m_alarm.m_strLeaderName = p_pJson->GetNodeValue("/body/leader_name", "");
				m_oBody.m_alarm.m_strReceiptCode = p_pJson->GetNodeValue("/body/receipt_code", "");
				m_oBody.m_alarm.m_strReceiptName = p_pJson->GetNodeValue("/body/receipt_name", "");
				m_oBody.m_alarm.m_strSeatNo = p_pJson->GetNodeValue("/body/seatno", "");
				m_oBody.m_alarm.m_strIsMerge = p_pJson->GetNodeValue("/body/is_merge", "");
				m_oBody.m_alarm.m_strCityCode = p_pJson->GetNodeValue("/body/city_code", "");
				m_oBody.m_alarm.m_strCreateUser = p_pJson->GetNodeValue("/body/create_user", "");
				m_oBody.m_alarm.m_strCreateTime = p_pJson->GetNodeValue("/body/create_time", "");
				m_oBody.m_alarm.m_strUpdateUser = p_pJson->GetNodeValue("/body/update_user", "");
				m_oBody.m_alarm.m_strUpdateTime = p_pJson->GetNodeValue("/body/update_time", "");*/

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/id", m_oBody.m_alarm.m_strID);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_alarm.m_strTime);
				
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
			//	std::string m_strSeatNo;					//�Ӿ���ϯλ��
			//	std::string m_strIsMerge;					//�Ƿ�ϲ�
			//	std::string m_strDispatchSuggestion;		//�������
			//	std::string m_strCityCode;
			//	std::string m_strCreateUser;				//������
			//	std::string m_strCreateTime;				//����ʱ��
			//	std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
			//	std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ			
			//};

			class CBody
			{
			public:
				CAlarmInfo m_alarm;
			};

			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}
