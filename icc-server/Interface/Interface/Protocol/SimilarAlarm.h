#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CAlarmInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSimilarAlarm :
			public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strStartTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_oBody.m_strPhoneNo = p_pJson->GetNodeValue("/body/caller_no", "");
				m_oBody.m_strContactNo = p_pJson->GetNodeValue("/body/contact_no", "");
				m_oBody.m_strAddr = p_pJson->GetNodeValue("/body/addr", "");
				m_oBody.m_strSourceType = p_pJson->GetNodeValue("/body/source_type", "");

				return true;
			}

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
					//std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex));
					data.ComJson(l_strPrefixPath, p_pJson);
//					p_pJson->SetNodeValue(l_strPrefixPath + "id", data.m_strID);
//					p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", data.m_strMergeID);
//					p_pJson->SetNodeValue(l_strPrefixPath + "title", data.m_strTitle);
//					p_pJson->SetNodeValue(l_strPrefixPath + "content", data.m_strContent);
//					p_pJson->SetNodeValue(l_strPrefixPath + "time", data.m_strTime);
//					p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_time", data.m_strActualOccurTime);
//					p_pJson->SetNodeValue(l_strPrefixPath + "addr", data.m_strAddr);
//					p_pJson->SetNodeValue(l_strPrefixPath + "longitude", data.m_strLongitude);
//					p_pJson->SetNodeValue(l_strPrefixPath + "latitude", data.m_strLatitude);
//					p_pJson->SetNodeValue(l_strPrefixPath + "state", data.m_strState);
//					p_pJson->SetNodeValue(l_strPrefixPath + "level", data.m_strLevel);
//					p_pJson->SetNodeValue(l_strPrefixPath + "source_type", data.m_strSourceType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "source_id", data.m_strSourceID);
//					p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", data.m_strHandleType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "first_type", data.m_strFirstType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "second_type", data.m_strSecondType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "third_type", data.m_strThirdType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", data.m_strFourthType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", data.m_strVehicleNo);
//					p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", data.m_strVehicleType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", data.m_strSymbolCode);
//					p_pJson->SetNodeValue(l_strPrefixPath + "symbol_addr", data.m_strSymbolAddr);
//					p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_type", data.m_strFireBuildingType);
//
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_foreign", data.m_strIsInvolveForeign);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_police", data.m_strIsInvolvePolice);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_gun", data.m_strIsInvolveGun);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_knife", data.m_strIsInvolveKnife);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_terror", data.m_strIsInvolveTerror);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_pornography", data.m_strIsInvolvePornography);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_gamble", data.m_strIsInvolveGamble);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_poison", data.m_strIsInvolvePoison);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext1", data.m_strIsInvolveExt1);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext2", data.m_strIsInvolveExt2);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext3", data.m_strIsInvolveExt3);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "event_type", data.m_strEventType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", data.m_strDispatchSuggestion);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", data.m_strCalledNoType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "actual_called_no_type", data.m_strActualCalledNoType);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", data.m_strCallerNo);
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", data.m_strCallerName);
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", data.m_strCallerAddr);
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", data.m_strCallerID);
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", data.m_strCallerIDType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", data.m_strCallerGender);
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_age", data.m_strCallerAge);
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_birthday", data.m_strCallerBirthday);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", data.m_strContactNo);
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_name", data.m_strContactName);
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_addr", data.m_strContactAddr);
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_id", data.m_strContactID);
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_id_type", data.m_strContactIDType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_gender", data.m_strContactGender);
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_age", data.m_strContactAge);
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_birthday", data.m_strContactBirthday);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_district_code", data.m_strAdminDeptDistrictCode);
//					p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", data.m_strAdminDeptCode);
//					p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", data.m_strAdminDeptName);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", data.m_strReceiptDeptDistrictCode);
//					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", data.m_strReceiptDeptCode);
//					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", data.m_strReceiptDeptName);
//					p_pJson->SetNodeValue(l_strPrefixPath + "leader_code", data.m_strLeaderCode);
//					p_pJson->SetNodeValue(l_strPrefixPath + "leader_name", data.m_strLeaderName);
//					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", data.m_strReceiptCode);
//					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", data.m_strReceiptName);
//					p_pJson->SetNodeValue(l_strPrefixPath + "seatno", data.m_strReceiptSeatno);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "create_user", data.m_strCreateUser);
//					p_pJson->SetNodeValue(l_strPrefixPath + "create_time", data.m_strCreateTime);
//					p_pJson->SetNodeValue(l_strPrefixPath + "update_user", data.m_strUpdateUser);
//					p_pJson->SetNodeValue(l_strPrefixPath + "update_time", data.m_strUpdateTime);
//					p_pJson->SetNodeValue(l_strPrefixPath + "is_merge", data.m_strIsMerge);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "is_feedback", data.m_strIsFeedBack);
//					p_pJson->SetNodeValue(l_strPrefixPath + "is_visitor", data.m_strIsVisitor);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "is_privacy", data.m_strPrivacy);
//					p_pJson->SetNodeValue(l_strPrefixPath + "remark", data.m_strRemark);

					l_uiIndex++;
				}

				return p_pJson->ToString();
			}

		public:
//			class CAlarm
//			{
//			public:
//				std::string	m_strID;						//����id
//				std::string	m_strMergeID;					//����ϲ�id
//				std::string m_strTitle;						//�������
//				std::string m_strContent;					//��������
//				std::string m_strTime;						//����id����ʱ��			
//				std::string m_strActualOccurTime;			//����idʵ�ʷ���ʱ��
//				std::string m_strAddr;						//����id��ϸ������ַ
//				std::string m_strLongitude;					//����id����
//				std::string m_strLatitude;					//����idά��
//				std::string m_strState;						//����id״̬
//				std::string m_strLevel;						//����id����
//				std::string m_strSourceType;				//����id������Դ����
//				std::string m_strSourceID;					//����id������Դid
//				std::string m_strHandleType;				//����id��������
//				std::string m_strFirstType;					//����idһ������
//				std::string m_strSecondType;				//����id��������
//				std::string m_strThirdType;					//����id��������
//				std::string m_strFourthType;				//����id�ļ�����
//				std::string m_strVehicleNo;					//����id��ͨ���ͱ������ƺ�
//				std::string m_strVehicleType;				//����id��ͨ���ͱ���������
//				std::string m_strSymbolCode;				//����id������ַ������ʾ����
//				std::string m_strSymbolAddr;				//����id������ַ������ʾ���ַ
//				std::string m_strFireBuildingType;			//����id������ȼ�ս�������
//
//// 				std::string m_strIsInvolveForeign;			//�Ƿ�����
//// 				std::string m_strIsInvolvePolice;			//�Ƿ��澯
//// 				std::string m_strIsInvolveGun;				//�Ƿ���ǹ
//// 				std::string m_strIsInvolveKnife;			//�Ƿ��浶
//// 				std::string m_strIsInvolveTerror;			//�Ƿ����
//// 				std::string m_strIsInvolvePornography;		//�Ƿ����
//// 				std::string m_strIsInvolveGamble;			//�Ƿ����
//// 				std::string m_strIsInvolvePoison;			//�Ƿ��涾
//// 				std::string m_strIsInvolveExt1;				//��չ�ֶ�1
//// 				std::string m_strIsInvolveExt2;				//��չ�ֶ�2
//// 				std::string m_strIsInvolveExt3;				//��չ�ֶ�3
//				std::string m_strEventType;					//�¼����ͣ����Ÿ���
//
//				std::string m_strCalledNoType;				//����id���������ֵ�����
//				std::string m_strActualCalledNoType;		//����idʵ�ʱ��������ֵ�����
//
//				std::string m_strCallerNo;					//����id�����˺���
//				std::string m_strCallerName;				//����id����������
//				std::string m_strCallerAddr;				//����id�����˵�ַ
//				std::string m_strCallerID;					//����id���������֤
//				std::string m_strCallerIDType;				//����id���������֤����
//				std::string m_strCallerGender;				//����id�������Ա�
//				std::string m_strCallerAge;					//����id����������
//				std::string m_strCallerBirthday;			//����id�����˳���������
//
//				std::string m_strContactNo;					//����id��ϵ�˺���
//				std::string m_strContactName;				//����id��ϵ������
//				std::string m_strContactAddr;				//����id��ϵ�˵�ַ
//				std::string m_strContactID;					//����id��ϵ�����֤
//				std::string m_strContactIDType;				//����id��ϵ�����֤����
//				std::string m_strContactGender;				//����id��ϵ���Ա�
//				std::string m_strContactAge;				//����id��ϵ������
//				std::string m_strContactBirthday;			//����id��ϵ�˳���������
//
//				std::string m_strAdminDeptDistrictCode;		//����id��Ͻ��λ��������
//				std::string m_strAdminDeptCode;				//����id��Ͻ��λ����
//				std::string m_strAdminDeptName;				//����id��Ͻ��λ����
//
//				std::string m_strReceiptDeptDistrictCode;	//����id�Ӿ���λ��������
//				std::string m_strReceiptDeptCode;			//����id�Ӿ���λ����
//				std::string m_strReceiptDeptName;			//����id�Ӿ���λ����
//				std::string m_strLeaderCode;				//����idֵ���쵼����
//				std::string m_strLeaderName;				//����idֵ���쵼����
//				std::string m_strReceiptCode;				//����id�Ӿ��˾���
//				std::string m_strReceiptName;				//����id�Ӿ�������
//				std::string m_strReceiptSeatno;				//�Ӿ���ϯ��
//
//				std::string m_strDispatchSuggestion;		//�������
//
//				std::string m_strCreateUser;				//������
//				std::string m_strCreateTime;				//����ʱ��
//				std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
//				std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ			
//				std::string m_strIsMerge;					//�ϲ���־
//
//				std::string m_strIsFeedBack;				//�Ƿ��ѷ���0��δ������1���ѷ���				
//				std::string m_strIsVisitor;					//�Ƿ��ѻط�0��δ�طã�1���ѻط�	
//				std::string m_strPrivacy;					//��˽����  true or false	
//				std::string m_strRemark;				    //������Ϣ
//			};

			class CBody
			{
			public:
				std::string m_strStartTime;		//������ʼʱ������ǰʱ�䣬ǰ�˸�ֵ
				std::string m_strPhoneNo;		//�����绰����
				std::string m_strContactNo;     //��ϵ�绰����
				std::string m_strAddr;			//��ַ
				std::string m_strSourceType;	//������Դ��ʽ

				std::string m_strCount;
				std::vector<CAlarmInfo> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
			
		};
	}
}
