#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>
#include <Protocol/CAlarmInfo.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CProcessDept
		{
		public:
			std::string	m_strDeptCode;					//������λ����
			std::string	m_strDeptName;					//������λ����
			std::string m_strParentDeptCode;			//����������λ����
		};

		class CWebAlarmSync :
			public IReceive,public ISend
		{
		public:	
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				std::string l_strPrefixPath("/body/");
				m_oBody.m_strSyncType = p_pJson->GetNodeValue(l_strPrefixPath + "sync_type", "");
				m_oBody.m_oAlarm.ParseString("/body", p_pJson);
				/*m_oBody.m_oAlarm.m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "id", "");
				m_oBody.m_oAlarm.m_strMergeID = p_pJson->GetNodeValue(l_strPrefixPath + "merge_id", "");
				m_oBody.m_oAlarm.m_strTitle = p_pJson->GetNodeValue(l_strPrefixPath + "title", "");
				m_oBody.m_oAlarm.m_strContent = p_pJson->GetNodeValue(l_strPrefixPath + "content", "");
				m_oBody.m_oAlarm.m_strTime = p_pJson->GetNodeValue(l_strPrefixPath + "time", "");
				m_oBody.m_oAlarm.m_strActualOccurTime = p_pJson->GetNodeValue(l_strPrefixPath + "actual_occur_time", "");
				m_oBody.m_oAlarm.m_strAddr = p_pJson->GetNodeValue(l_strPrefixPath + "addr", "");
				m_oBody.m_oAlarm.m_strLongitude = p_pJson->GetNodeValue(l_strPrefixPath + "longitude", "");
				m_oBody.m_oAlarm.m_strLatitude = p_pJson->GetNodeValue(l_strPrefixPath + "latitude", "");
				m_oBody.m_oAlarm.m_strState = p_pJson->GetNodeValue(l_strPrefixPath + "state", "");
				m_oBody.m_oAlarm.m_strLevel = p_pJson->GetNodeValue(l_strPrefixPath + "level", "");
				m_oBody.m_oAlarm.m_strSourceType = p_pJson->GetNodeValue(l_strPrefixPath + "source_type", "");
				m_oBody.m_oAlarm.m_strSourceID = p_pJson->GetNodeValue(l_strPrefixPath + "source_id", "");
				m_oBody.m_oAlarm.m_strHandleType = p_pJson->GetNodeValue(l_strPrefixPath + "handle_type", "");
				m_oBody.m_oAlarm.m_strFirstType = p_pJson->GetNodeValue(l_strPrefixPath + "first_type", "");
				m_oBody.m_oAlarm.m_strSecondType = p_pJson->GetNodeValue(l_strPrefixPath + "second_type", "");
				m_oBody.m_oAlarm.m_strThirdType = p_pJson->GetNodeValue(l_strPrefixPath + "third_type", "");
				m_oBody.m_oAlarm.m_strFourthType = p_pJson->GetNodeValue(l_strPrefixPath + "fourth_type", "");
				m_oBody.m_oAlarm.m_strVehicleNo = p_pJson->GetNodeValue(l_strPrefixPath + "vehicle_no", "");
				m_oBody.m_oAlarm.m_strVehicleType = p_pJson->GetNodeValue(l_strPrefixPath + "vehicle_type", "");
				m_oBody.m_oAlarm.m_strSymbolCode = p_pJson->GetNodeValue(l_strPrefixPath + "symbol_code", "");
				m_oBody.m_oAlarm.m_strSymbolAddr = p_pJson->GetNodeValue(l_strPrefixPath + "symbol_addr", "");
				m_oBody.m_oAlarm.m_strFireBuildingType = p_pJson->GetNodeValue(l_strPrefixPath + "fire_building_type", "");

				m_oBody.m_oAlarm.m_strEventType = p_pJson->GetNodeValue(l_strPrefixPath + "event_type", "");

				m_oBody.m_oAlarm.m_strCalledNoType = p_pJson->GetNodeValue(l_strPrefixPath + "called_no_type", "");
				m_oBody.m_oAlarm.m_strActualCalledNoType = p_pJson->GetNodeValue(l_strPrefixPath + "actual_called_no_type", "");

				m_oBody.m_oAlarm.m_strCallerNo = p_pJson->GetNodeValue(l_strPrefixPath + "caller_no", "");
				m_oBody.m_oAlarm.m_strCallerName = p_pJson->GetNodeValue(l_strPrefixPath + "caller_name", "");
				m_oBody.m_oAlarm.m_strCallerAddr = p_pJson->GetNodeValue(l_strPrefixPath + "caller_addr", "");
				m_oBody.m_oAlarm.m_strCallerID = p_pJson->GetNodeValue(l_strPrefixPath + "caller_id", "");
				m_oBody.m_oAlarm.m_strCallerIDType = p_pJson->GetNodeValue(l_strPrefixPath + "caller_id_type", "");
				m_oBody.m_oAlarm.m_strCallerGender = p_pJson->GetNodeValue(l_strPrefixPath + "caller_gender", "");
				m_oBody.m_oAlarm.m_strCallerAge = p_pJson->GetNodeValue(l_strPrefixPath + "caller_age", "");
				m_oBody.m_oAlarm.m_strCallerBirthday = p_pJson->GetNodeValue(l_strPrefixPath + "caller_birthday", "");

				m_oBody.m_oAlarm.m_strContactNo = p_pJson->GetNodeValue(l_strPrefixPath + "contact_no", "");
				m_oBody.m_oAlarm.m_strContactName = p_pJson->GetNodeValue(l_strPrefixPath + "contact_name", "");
				m_oBody.m_oAlarm.m_strContactAddr = p_pJson->GetNodeValue(l_strPrefixPath + "contact_addr", "");
				m_oBody.m_oAlarm.m_strContactID = p_pJson->GetNodeValue(l_strPrefixPath + "contact_id", "");
				m_oBody.m_oAlarm.m_strContactIDType = p_pJson->GetNodeValue(l_strPrefixPath + "contact_id_type", "");
				m_oBody.m_oAlarm.m_strContactGender = p_pJson->GetNodeValue(l_strPrefixPath + "contact_gender", "");
				m_oBody.m_oAlarm.m_strContactAge = p_pJson->GetNodeValue(l_strPrefixPath + "contact_age", "");
				m_oBody.m_oAlarm.m_strContactBirthday = p_pJson->GetNodeValue(l_strPrefixPath + "contact_birthday", "");

				m_oBody.m_oAlarm.m_strAdminDeptDistrictCode = p_pJson->GetNodeValue(l_strPrefixPath + "admin_dept_district_code", "");
				m_oBody.m_oAlarm.m_strAdminDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "admin_dept_code", "");
				m_oBody.m_oAlarm.m_strAdminDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "admin_dept_name", "");

				m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue(l_strPrefixPath + "receipt_dept_district_code", "");
				m_oBody.m_oAlarm.m_strReceiptDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "receipt_dept_code", "");
				m_oBody.m_oAlarm.m_strReceiptDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "receipt_dept_name", "");
				m_oBody.m_oAlarm.m_strLeaderCode = p_pJson->GetNodeValue(l_strPrefixPath + "leader_code", "");
				m_oBody.m_oAlarm.m_strLeaderName = p_pJson->GetNodeValue(l_strPrefixPath + "leader_name", "");
				m_oBody.m_oAlarm.m_strReceiptCode = p_pJson->GetNodeValue(l_strPrefixPath + "receipt_code", "");
				m_oBody.m_oAlarm.m_strReceiptName = p_pJson->GetNodeValue(l_strPrefixPath + "receipt_name", "");

				m_oBody.m_oAlarm.m_strDispatchSuggestion = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_suggestion", "");*/

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				std::string l_strPrefixPath("/body/alarm");

				p_pJson->SetNodeValue(l_strPrefixPath + "/sync_type", m_oBody.m_strSyncType);
				m_oBody.m_oAlarm.ComJson(l_strPrefixPath, p_pJson);
				/*p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_oAlarm.m_strID);
				p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", m_oBody.m_oAlarm.m_strMergeID);
				p_pJson->SetNodeValue(l_strPrefixPath + "title", m_oBody.m_oAlarm.m_strTitle);
				p_pJson->SetNodeValue(l_strPrefixPath + "content", m_oBody.m_oAlarm.m_strContent);
				p_pJson->SetNodeValue(l_strPrefixPath + "time", m_oBody.m_oAlarm.m_strTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_time", m_oBody.m_oAlarm.m_strActualOccurTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "addr", m_oBody.m_oAlarm.m_strAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "longitude", m_oBody.m_oAlarm.m_strLongitude);
				p_pJson->SetNodeValue(l_strPrefixPath + "latitude", m_oBody.m_oAlarm.m_strLatitude);
				p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.m_oAlarm.m_strState);
				p_pJson->SetNodeValue(l_strPrefixPath + "level", m_oBody.m_oAlarm.m_strLevel);
				p_pJson->SetNodeValue(l_strPrefixPath + "source_type", m_oBody.m_oAlarm.m_strSourceType);
				p_pJson->SetNodeValue(l_strPrefixPath + "source_id", m_oBody.m_oAlarm.m_strSourceID);
				p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", m_oBody.m_oAlarm.m_strHandleType);
				p_pJson->SetNodeValue(l_strPrefixPath + "first_type", m_oBody.m_oAlarm.m_strFirstType);
				p_pJson->SetNodeValue(l_strPrefixPath + "second_type", m_oBody.m_oAlarm.m_strSecondType);
				p_pJson->SetNodeValue(l_strPrefixPath + "third_type", m_oBody.m_oAlarm.m_strThirdType);
				p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", m_oBody.m_oAlarm.m_strFourthType);
				p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", m_oBody.m_oAlarm.m_strVehicleNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", m_oBody.m_oAlarm.m_strVehicleType);
				p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", m_oBody.m_oAlarm.m_strSymbolCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "symbol_addr", m_oBody.m_oAlarm.m_strSymbolAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_type", m_oBody.m_oAlarm.m_strFireBuildingType);

				p_pJson->SetNodeValue(l_strPrefixPath + "event_type", m_oBody.m_oAlarm.m_strEventType);

				p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", m_oBody.m_oAlarm.m_strCalledNoType);
				p_pJson->SetNodeValue(l_strPrefixPath + "actual_called_no_type", m_oBody.m_oAlarm.m_strActualCalledNoType);

				p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", m_oBody.m_oAlarm.m_strCallerNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", m_oBody.m_oAlarm.m_strCallerName);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", m_oBody.m_oAlarm.m_strCallerAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", m_oBody.m_oAlarm.m_strCallerID);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", m_oBody.m_oAlarm.m_strCallerIDType);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", m_oBody.m_oAlarm.m_strCallerGender);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_age", m_oBody.m_oAlarm.m_strCallerAge);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_birthday", m_oBody.m_oAlarm.m_strCallerBirthday);

				p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", m_oBody.m_oAlarm.m_strContactNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_name", m_oBody.m_oAlarm.m_strContactName);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_addr", m_oBody.m_oAlarm.m_strContactAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_id", m_oBody.m_oAlarm.m_strContactID);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_id_type", m_oBody.m_oAlarm.m_strContactIDType);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_gender", m_oBody.m_oAlarm.m_strContactGender);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_age", m_oBody.m_oAlarm.m_strContactAge);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_birthday", m_oBody.m_oAlarm.m_strContactBirthday);

				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_district_code", m_oBody.m_oAlarm.m_strAdminDeptDistrictCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", m_oBody.m_oAlarm.m_strAdminDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", m_oBody.m_oAlarm.m_strAdminDeptName);

				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", m_oBody.m_oAlarm.m_strReceiptDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", m_oBody.m_oAlarm.m_strReceiptDeptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "leader_code", m_oBody.m_oAlarm.m_strLeaderCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "leader_name", m_oBody.m_oAlarm.m_strLeaderName);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", m_oBody.m_oAlarm.m_strReceiptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", m_oBody.m_oAlarm.m_strReceiptName);

				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", m_oBody.m_oAlarm.m_strDispatchSuggestion);*/

				unsigned int l_uiIndex = 0;
				for (auto processdept : m_oBody.m_vecProcessDept)
				{
					std::string l_strProcessDeptPrefixPath("/body/process_dept/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strProcessDeptPrefixPath + "dept_code", processdept.m_strDeptCode);
					p_pJson->SetNodeValue(l_strProcessDeptPrefixPath + "dept_name", processdept.m_strDeptName);
					p_pJson->SetNodeValue(l_strProcessDeptPrefixPath + "parent_dept_code", processdept.m_strParentDeptCode);
					l_uiIndex++;
				}

				return p_pJson->ToString();
			}

		public:
			//class CAlarm
			//{
			//public:
			//	std::string m_strSyncType;					//1����ʾ��ӣ�2����ʾ����
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
			//};

			class CBody
			{
			public:
				std::string m_strSyncType;					//1����ʾ��ӣ�2����ʾ����
				CAlarmInfo m_oAlarm;
				std::vector<CProcessDept> m_vecProcessDept;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}