#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAlarmSync :
			public IRespond, public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (p_pJson == nullptr)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				std::string l_strPrefixPath("/body/");
				p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", m_oBody.m_strMsgSource);
				p_pJson->SetNodeValue(l_strPrefixPath + "sync_type", m_oBody.m_strSyncType);
				p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_strID);
				p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", m_oBody.m_strMergeID);
				p_pJson->SetNodeValue(l_strPrefixPath + "seatno", m_oBody.m_strSeatNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "title", m_oBody.m_strTitle);
				p_pJson->SetNodeValue(l_strPrefixPath + "content", m_oBody.m_strContent);
				p_pJson->SetNodeValue(l_strPrefixPath + "time", m_oBody.m_strTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_time", m_oBody.m_strActualOccurTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "addr", m_oBody.m_strAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "longitude", m_oBody.m_strLongitude);
				p_pJson->SetNodeValue(l_strPrefixPath + "latitude", m_oBody.m_strLatitude);
				p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.m_strState);
				p_pJson->SetNodeValue(l_strPrefixPath + "level", m_oBody.m_strLevel);
				p_pJson->SetNodeValue(l_strPrefixPath + "source_type", m_oBody.m_strSourceType);
				p_pJson->SetNodeValue(l_strPrefixPath + "source_id", m_oBody.m_strSourceID);
				p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", m_oBody.m_strHandleType);
				p_pJson->SetNodeValue(l_strPrefixPath + "first_type", m_oBody.m_strFirstType);
				p_pJson->SetNodeValue(l_strPrefixPath + "second_type", m_oBody.m_strSecondType);
				p_pJson->SetNodeValue(l_strPrefixPath + "third_type", m_oBody.m_strThirdType);
				p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", m_oBody.m_strFourthType);
				p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", m_oBody.m_strVehicleNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", m_oBody.m_strVehicleType);
				p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", m_oBody.m_strSymbolCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "symbol_addr", m_oBody.m_strSymbolAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_type", m_oBody.m_strFireBuildingType);

				p_pJson->SetNodeValue(l_strPrefixPath + "event_type", m_oBody.m_strEventType);

				p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", m_oBody.m_strCalledNoType);
				p_pJson->SetNodeValue(l_strPrefixPath + "actual_called_no_type", m_oBody.m_strActualCalledNoType);

				p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", m_oBody.m_strCallerNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", m_oBody.m_strCallerName);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", m_oBody.m_strCallerAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", m_oBody.m_strCallerID);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", m_oBody.m_strCallerIDType);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", m_oBody.m_strCallerGender);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_age", m_oBody.m_strCallerAge);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_birthday", m_oBody.m_strCallerBirthday);

				p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", m_oBody.m_strContactNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_name", m_oBody.m_strContactName);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_addr", m_oBody.m_strContactAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_id", m_oBody.m_strContactID);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_id_type", m_oBody.m_strContactIDType);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_gender", m_oBody.m_strContactGender);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_age", m_oBody.m_strContactAge);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_birthday", m_oBody.m_strContactBirthday);

				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_district_code", m_oBody.m_strAdminDeptDistrictCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", m_oBody.m_strAdminDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", m_oBody.m_strAdminDeptName);

				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", m_oBody.m_strReceiptDeptDistrictCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", m_oBody.m_strReceiptDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", m_oBody.m_strReceiptDeptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "leader_code", m_oBody.m_strLeaderCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "leader_name", m_oBody.m_strLeaderName);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", m_oBody.m_strReceiptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", m_oBody.m_strReceiptName);

				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", m_oBody.m_strDispatchSuggestion);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_merge", m_oBody.m_strIsMerge);

				p_pJson->SetNodeValue(l_strPrefixPath + "create_user", m_oBody.m_strCreateUser);
				p_pJson->SetNodeValue(l_strPrefixPath + "create_time", m_oBody.m_strCreateTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_user", m_oBody.m_strUpdateUser);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_time", m_oBody.m_strUpdateTime);

				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strMsgSource = p_pJson->GetNodeValue("/body/msg_source", "");
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");
				m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strMergeID = p_pJson->GetNodeValue("/body/merge_id", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seatno", "");
				m_oBody.m_strTitle = p_pJson->GetNodeValue("/body/title", "");
				m_oBody.m_strContent = p_pJson->GetNodeValue("/body/content", "");
				m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");
				m_oBody.m_strActualOccurTime = p_pJson->GetNodeValue("/body/actual_occur_time", "");
				m_oBody.m_strAddr = p_pJson->GetNodeValue("/body/addr", "");
				m_oBody.m_strLongitude = p_pJson->GetNodeValue("/body/longitude", "");
				m_oBody.m_strLatitude = p_pJson->GetNodeValue("/body/latitude", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strLevel = p_pJson->GetNodeValue("/body/level", "");
				m_oBody.m_strSourceType = p_pJson->GetNodeValue("/body/source_type", "");
				m_oBody.m_strSourceID = p_pJson->GetNodeValue("/body/source_id", "");
				m_oBody.m_strHandleType = p_pJson->GetNodeValue("/body/handle_type", "");
				m_oBody.m_strFirstType = p_pJson->GetNodeValue("/body/first_type", "");
				m_oBody.m_strSecondType = p_pJson->GetNodeValue("/body/second_type", "");
				m_oBody.m_strThirdType = p_pJson->GetNodeValue("/body/third_type", "");
				m_oBody.m_strFourthType = p_pJson->GetNodeValue("/body/fourth_type", "");
				m_oBody.m_strVehicleNo = p_pJson->GetNodeValue("/body/vehicle_no", "");
				m_oBody.m_strVehicleType = p_pJson->GetNodeValue("/body/vehicle_type", "");
				m_oBody.m_strSymbolCode = p_pJson->GetNodeValue("/body/symbol_code", "");
				m_oBody.m_strSymbolAddr = p_pJson->GetNodeValue("/body/symbol_addr", "");
				m_oBody.m_strFireBuildingType = p_pJson->GetNodeValue("/body/fire_building_type", "");

				m_oBody.m_strEventType = p_pJson->GetNodeValue("/body/event_type", "");

				m_oBody.m_strCalledNoType = p_pJson->GetNodeValue("/body/called_no_type", "");
				m_oBody.m_strActualCalledNoType = p_pJson->GetNodeValue("/body/actual_called_no_type", "");

				m_oBody.m_strCallerNo = p_pJson->GetNodeValue("/body/caller_no", "");
				m_oBody.m_strCallerName = p_pJson->GetNodeValue("/body/caller_name", "");
				m_oBody.m_strCallerAddr = p_pJson->GetNodeValue("/body/caller_addr", "");
				m_oBody.m_strCallerID = p_pJson->GetNodeValue("/body/caller_id", "");
				m_oBody.m_strCallerIDType = p_pJson->GetNodeValue("/body/caller_id_type", "");
				m_oBody.m_strCallerGender = p_pJson->GetNodeValue("/body/caller_gender", "");
				m_oBody.m_strCallerAge = p_pJson->GetNodeValue("/body/caller_age", "");
				m_oBody.m_strCallerBirthday = p_pJson->GetNodeValue("/body/caller_birthday", "");

				m_oBody.m_strContactNo = p_pJson->GetNodeValue("/body/contact_no", "");
				m_oBody.m_strContactName = p_pJson->GetNodeValue("/body/contact_name", "");
				m_oBody.m_strContactAddr = p_pJson->GetNodeValue("/body/contact_addr", "");
				m_oBody.m_strContactID = p_pJson->GetNodeValue("/body/contact_id", "");
				m_oBody.m_strContactIDType = p_pJson->GetNodeValue("/body/contact_id_type", "");
				m_oBody.m_strContactGender = p_pJson->GetNodeValue("/body/contact_gender", "");
				m_oBody.m_strContactAge = p_pJson->GetNodeValue("/body/contact_age", "");
				m_oBody.m_strContactBirthday = p_pJson->GetNodeValue("/body/contact_birthday", "");

				m_oBody.m_strAdminDeptDistrictCode = p_pJson->GetNodeValue("/body/admin_dept_district_code", "");
				m_oBody.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/admin_dept_code", "");
				m_oBody.m_strAdminDeptName = p_pJson->GetNodeValue("/body/admin_dept_name", "");

				m_oBody.m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue("/body/receipt_dept_district_code", "");
				m_oBody.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/receipt_dept_code", "");
				m_oBody.m_strReceiptDeptName = p_pJson->GetNodeValue("/body/receipt_dept_name", "");
				m_oBody.m_strLeaderCode = p_pJson->GetNodeValue("/body/leader_code", "");
				m_oBody.m_strLeaderName = p_pJson->GetNodeValue("/body/leader_name", "");
				m_oBody.m_strReceiptCode = p_pJson->GetNodeValue("/body/receipt_code", "");
				m_oBody.m_strReceiptName = p_pJson->GetNodeValue("/body/receipt_name", "");

				m_oBody.m_strDispatchSuggestion = p_pJson->GetNodeValue("/body/dispatch_suggestion", "");
				m_oBody.m_strIsMerge = p_pJson->GetNodeValue("/body/is_merge", "");

				m_oBody.m_strCreateUser = p_pJson->GetNodeValue("/body/create_user", "");
				m_oBody.m_strCreateTime = p_pJson->GetNodeValue("/body/create_time", "");
				m_oBody.m_strUpdateUser = p_pJson->GetNodeValue("/body/update_user", "");
				m_oBody.m_strUpdateTime = p_pJson->GetNodeValue("/body/update_time", "");

				return true;
			}
		public:
			class CBody
			{
			public:
				std::string	m_strMsgSource;					//��Ϣ��Դ
				std::string	m_strSyncType;					//����ͬ�����ͣ�1-��ӣ�2-����
				std::string	m_strID;						//����id
				std::string	m_strMergeID;					//����ϲ�id
				std::string	m_strSeatNo;					//�Ӿ���ϯ��
				std::string m_strTitle;						//�������
				std::string m_strContent;					//��������
				std::string m_strTime;						//����id����ʱ��			
				std::string m_strActualOccurTime;			//����idʵ�ʷ���ʱ��
				std::string m_strAddr;						//����id��ϸ������ַ
				std::string m_strLongitude;					//����id����
				std::string m_strLatitude;					//����idά��
				std::string m_strState;						//����id״̬
				std::string m_strLevel;						//����id����
				std::string m_strSourceType;				//����id������Դ����
				std::string m_strSourceID;					//����id������Դid
				std::string m_strHandleType;				//����id��������
				std::string m_strFirstType;					//����idһ������
				std::string m_strSecondType;				//����id��������
				std::string m_strThirdType;					//����id��������
				std::string m_strFourthType;				//����id�ļ�����
				std::string m_strVehicleNo;					//����id��ͨ���ͱ������ƺ�
				std::string m_strVehicleType;				//����id��ͨ���ͱ���������
				std::string m_strSymbolCode;				//����id������ַ������ʾ����
				std::string m_strSymbolAddr;				//����id������ַ������ʾ���ַ
				std::string m_strFireBuildingType;			//����id������ȼ�ս�������

				std::string m_strEventType;					//�¼����ͣ����Ÿ���

				std::string m_strCalledNoType;				//����id���������ֵ�����
				std::string m_strActualCalledNoType;		//����idʵ�ʱ��������ֵ�����

				std::string m_strCallerNo;					//����id�����˺���
				std::string m_strCallerName;				//����id����������
				std::string m_strCallerAddr;				//����id�����˵�ַ
				std::string m_strCallerID;					//����id���������֤
				std::string m_strCallerIDType;				//����id���������֤����
				std::string m_strCallerGender;				//����id�������Ա�
				std::string m_strCallerAge;					//����id����������
				std::string m_strCallerBirthday;			//����id�����˳���������

				std::string m_strContactNo;					//����id��ϵ�˺���
				std::string m_strContactName;				//����id��ϵ������
				std::string m_strContactAddr;				//����id��ϵ�˵�ַ
				std::string m_strContactID;					//����id��ϵ�����֤
				std::string m_strContactIDType;				//����id��ϵ�����֤����
				std::string m_strContactGender;				//����id��ϵ���Ա�
				std::string m_strContactAge;				//����id��ϵ������
				std::string m_strContactBirthday;			//����id��ϵ�˳���������

				std::string m_strAdminDeptDistrictCode;		//����id��Ͻ��λ��������
				std::string m_strAdminDeptCode;				//����id��Ͻ��λ����
				std::string m_strAdminDeptName;				//����id��Ͻ��λ����

				std::string m_strReceiptDeptDistrictCode;	//����id�Ӿ���λ��������
				std::string m_strReceiptDeptCode;			//����id�Ӿ���λ����
				std::string m_strReceiptDeptName;			//����id�Ӿ���λ����
				std::string m_strLeaderCode;				//����idֵ���쵼����
				std::string m_strLeaderName;				//����idֵ���쵼����
				std::string m_strReceiptCode;				//����id�Ӿ��˾���
				std::string m_strReceiptName;				//����id�Ӿ�������

				std::string m_strDispatchSuggestion;		//�������
				std::string m_strIsMerge;					//�Ƿ�ϲ�

				std::string m_strCreateUser;				//������
				std::string m_strCreateTime;				//����ʱ��
				std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
				std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ              
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
		class CMaterial
		{
		public:
			std::string		m_strfguid;		//����ID
			std::string		m_strfname;		//��������
			std::string		m_attachtype;	//�������� 0:ͼƬ��1��Ƶ��2����
			std::string		m_strattachpath;//����·��
		};
		class CWeChatAlarmInfo :
			public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_bIsSuccess = p_pJson->GetNodeValue("/body/ReturnInfo/IsSuccess", "");
				m_oBody.m_strMsg = p_pJson->GetNodeValue("/body/ReturnInfo/Msg", "");
				unsigned int l_iCount = p_pJson->GetCount("/body/ResultData");
				for (unsigned int i = 0; i < l_iCount; i++)
				{
					std::string l_strPrefixPath("/body/ResultData/" + std::to_string(i) + "/");
					COMPLAINTLIST l_oComplaintlist;

					l_oComplaintlist.m_strGUID = p_pJson->GetNodeValue(l_strPrefixPath + "GUID", "");
					l_oComplaintlist.m_strADDRESS = p_pJson->GetNodeValue(l_strPrefixPath + "ADDRESS", "");
					l_oComplaintlist.m_strCRCONTENT = p_pJson->GetNodeValue(l_strPrefixPath + "CRCONTENT", "");
					l_oComplaintlist.m_bISPHONE = p_pJson->GetNodeValue(l_strPrefixPath + "ISPHONE", "");
					l_oComplaintlist.m_strCREATEDATE = p_pJson->GetNodeValue(l_strPrefixPath + "CREATEDATE", "");
					l_oComplaintlist.m_strUSERCODE = p_pJson->GetNodeValue(l_strPrefixPath + "USERCODE", "");
					l_oComplaintlist.m_strUSERNAME = p_pJson->GetNodeValue(l_strPrefixPath + "USERNAME", "");
					l_oComplaintlist.m_strUSERPHONE = p_pJson->GetNodeValue(l_strPrefixPath + "USERPHONE", "");
					l_oComplaintlist.m_strUSERADDRESS = p_pJson->GetNodeValue(l_strPrefixPath + "USERADDRESS", "");
					l_oComplaintlist.m_strUNITCONTACTOR = p_pJson->GetNodeValue(l_strPrefixPath + "UNITCONTACTOR", "");
					l_oComplaintlist.m_strHANDPHONE = p_pJson->GetNodeValue(l_strPrefixPath + "HANDPHONE", "");
					l_oComplaintlist.m_strTOORGID = p_pJson->GetNodeValue(l_strPrefixPath + "TOORGID", "");
					l_oComplaintlist.m_strTOORGNAME = p_pJson->GetNodeValue(l_strPrefixPath + "TOORGNAME", "");
					l_oComplaintlist.m_strCOMNO = p_pJson->GetNodeValue(l_strPrefixPath + "COMNO", "");
					l_oComplaintlist.m_strOPENID = p_pJson->GetNodeValue(l_strPrefixPath + "OPENID", "");
					l_oComplaintlist.m_strLONG = p_pJson->GetNodeValue(l_strPrefixPath + "LONG", "");
					l_oComplaintlist.m_strLAT = p_pJson->GetNodeValue(l_strPrefixPath + "LAT", "");
					l_oComplaintlist.m_strREMARK1 = p_pJson->GetNodeValue(l_strPrefixPath + "REMARK1", "");
					l_oComplaintlist.m_strREMARK2 = p_pJson->GetNodeValue(l_strPrefixPath + "REMARK2", "");
			
					unsigned int l_iCountM = p_pJson->GetCount(l_strPrefixPath + "Material");
					for (unsigned int i = 0; i < l_iCountM; ++i)
					{
						std::string l_strfixPath = l_strPrefixPath + "Material/" + std::to_string(i) + "/";
						CMaterial l_oMaterial;
						l_oMaterial.m_strfguid = p_pJson->GetNodeValue(l_strfixPath + "fguid", "");
						l_oMaterial.m_strfname = p_pJson->GetNodeValue(l_strfixPath + "fname", "");
						l_oMaterial.m_attachtype = p_pJson->GetNodeValue(l_strfixPath + "attachtype", "");
						l_oMaterial.m_strattachpath = p_pJson->GetNodeValue(l_strfixPath + "attachpath", "");
						l_oComplaintlist.m_material.push_back(l_oMaterial);
					}

					m_oBody.m_Complaintlist.push_back(l_oComplaintlist);
				}

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				
				return p_pJson->ToString();
			}
		public:
			class COMPLAINTLIST
			{
			public:
				std::string		m_strGUID;		//΢�ű�������
				std::string		m_strADDRESS;    //�·���ַ
				std::string		m_strCRCONTENT;  //��������
				std::string   	m_bISPHONE;      //�Ƿ񷽱�����绰
				std::string		m_strCREATEDATE; //����ʱ��
				std::string		m_strUSERCODE;	//���������֤����
				std::string		m_strUSERNAME;	//����������
				std::string		m_strUSERPHONE;	//�������ֻ�
				std::string		m_strUSERADDRESS;//��������ϵ��ַ
				std::string		m_strUNITCONTACTOR;//��ϵ��
				std::string		m_strHANDPHONE;	  //��ϵ�绰
				std::string		m_strTOORGID;	     //���ܲ���ID
				std::string		m_strTOORGNAME;	  //���ܲ�������
				std::string		m_strCOMNO;		  //΢�ű������
				std::string		m_strOPENID;		  //�������ڵı��
				std::string		m_strLONG;		  //ά��
				std::string		m_strLAT;		  //����
				std::string		m_strREMARK1;	  //����1
				std::string		m_strREMARK2;	  //����2
				std::vector<CMaterial> m_material;
			};

			class CBody
			{
			public:
				std::string		m_bIsSuccess;	//�Ƿ�ɹ���ȡ����ǰʱ���΢�ű�����Ϣ
				std::string		m_strMsg;        //��ʾ ��Ϣ
				std::vector<COMPLAINTLIST> m_Complaintlist;
			};

			CHeader m_oHeader;
			CBody	m_oBody;
		};
		class CDSWeChatChangeInfo
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strThirdAlarmID = p_pJson->GetNodeValue("/body/paras/GUID", "");
				m_oBody.m_strPHONE = p_pJson->GetNodeValue("/body/paras/PHONE", "");
				m_oBody.m_strOPENID = p_pJson->GetNodeValue("/body/paras/OPENID", "");
				m_oBody.m_strCHANGETYPE = p_pJson->GetNodeValue("/body/paras/CHANGETYPE", "");
				m_oBody.m_strCHANGECONTENT = p_pJson->GetNodeValue("/body/paras/CHANGECONTENT", "");
				m_oBody.m_strCHANGEDATE = p_pJson->GetNodeValue("/body/paras/CHANGEDATE", "");
				m_oBody.m_strLONG = p_pJson->GetNodeValue("/body/paras/LONG", "");
				m_oBody.m_strLAT = p_pJson->GetNodeValue("/body/paras/LAT", "");
				m_oBody.m_strREMARK1 = p_pJson->GetNodeValue("/body/paras/REMARK1", "");
				m_oBody.m_strREMARK2 = p_pJson->GetNodeValue("/body/paras/REMARK2", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGUID);
				p_pJson->SetNodeValue("/body/third_alarm_guid", m_oBody.m_strThirdAlarmID);
				p_pJson->SetNodeValue("/body/orientation", m_oBody.m_strOrientation);
				p_pJson->SetNodeValue("/body/state", m_oBody.m_strState);
				p_pJson->SetNodeValue("/body/phone", m_oBody.m_strPHONE);
				p_pJson->SetNodeValue("/body/open_id", m_oBody.m_strOPENID);
				p_pJson->SetNodeValue("/body/change_type", m_oBody.m_strCHANGETYPE);
				p_pJson->SetNodeValue("/body/change_content", m_oBody.m_strCHANGECONTENT);
				p_pJson->SetNodeValue("/body/change_date", m_oBody.m_strCHANGEDATE);
				p_pJson->SetNodeValue("/body/long", m_oBody.m_strLONG);
				p_pJson->SetNodeValue("/body/lat", m_oBody.m_strLAT);
				p_pJson->SetNodeValue("/body/remark1", m_oBody.m_strREMARK1);
				p_pJson->SetNodeValue("/body/remark2", m_oBody.m_strREMARK2);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string		m_strGUID;			
				std::string		m_strPHONE;			//�ֻ�����
				std::string		m_strOPENID;         //�������ڵı���
				std::string		m_strCHANGETYPE;     //�������ͣ���1�����֣�2��ͼƬ��3����Ƶ��4��������
				std::string		m_strCHANGECONTENT;  //�������ݣ�����������Ϊ1ʱΪ�������ݣ�2-4��Ϊ��ý���ļ�ȫ·����
				std::string		m_strCHANGEDATE;     //����ʱ��
				std::string     m_strLONG;			//����
				std::string     m_strLAT;            //γ��
				std::string     m_strREMARK1;		//�����ֶ�1
				std::string     m_strREMARK2;		//�����ֶ�2
				std::string     m_strfguid;			//����guid
				std::string     m_strfname;			//��������
				std::string     m_strOrientation;	//0�������˸���Ա���ͣ�1����Ա�������˷��͡�
				std::string     m_strState;			//״̬��0δ����1�Ѷ�
				std::string		m_strThirdAlarmID;	//΢�ű�������
			};

			CHeader m_oHeader;
			CBody	m_oBody;
		};
		class CWeChatProcessResult
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (p_pJson == nullptr)
				{
					return "";
				}
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/paras/ALARMGUID", m_oBody.m_strALARMGUID);
				p_pJson->SetNodeValue("/body/paras/OPENID", m_oBody.m_strOPENID);
				p_pJson->SetNodeValue("/body/paras/CHECKTYPE", m_oBody.m_strCHECKTYPE);
				p_pJson->SetNodeValue("/body/paras/ACORGID", m_oBody.m_strACORGID);
				p_pJson->SetNodeValue("/body/paras/ACORGNAME", m_oBody.m_strACORGNAME);
				p_pJson->SetNodeValue("/body/paras/CHECKRESULT", m_oBody.m_strCHECKRESULT);
				p_pJson->SetNodeValue("/body/paras/CHECKDATE", m_oBody.m_strCHECKDATE);
				p_pJson->SetNodeValue("/body/paras/ALARMCODE", m_oBody.m_strALARMCODE);
				p_pJson->SetNodeValue("/body/paras/REMARK1", m_oBody.m_strREMARK1);
				p_pJson->SetNodeValue("/body/paras/REMARK2", m_oBody.m_strREMARK2);


				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string		m_strALARMGUID;			//΢�ű�������
				std::string		m_strOPENID;            //�������ڵı���
				std::string		m_strCHECKTYPE;			//����״̬����0��������1�����ڴ��ã�6����Ϊ��Ч�������ս᣻7����Ϊ�ر��������ս᣻8��������ϲ��սᣨ����״̬��ֵ����5��Ϊ΢�ű�������������9���������ޣ�
				std::string		m_strACORGID;			//������ID
				std::string		m_strACORGNAME;         //����������
				std::string     m_strCHECKRESULT;	    //������
				std::string     m_strCHECKDATE;         //����ʱ�䣨����ʱ��  yyyy-mm-dd  hh24:mi:ss��
				std::string     m_strALARMCODE;			//110ϵͳ������
				std::string     m_strREMARK1;			//�����ֶ�1
				std::string     m_strREMARK2;			//�����ֶ�2
			};

			CHeader m_oHeader;
			CBody	m_oBody;
		};
		class COSWeChatChangeInfo
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (p_pJson == nullptr)
				{
					return "";
				}
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/paras/GUID", m_oBody.m_strGUID);
				p_pJson->SetNodeValue("/body/paras/PHONE", m_oBody.m_strPHONE);
				p_pJson->SetNodeValue("/body/paras/OPENID", m_oBody.m_strOPENID);
				p_pJson->SetNodeValue("/body/paras/CHANGECONTENT", m_oBody.m_strCHANGECONTENT);
				p_pJson->SetNodeValue("/body/paras/CHANGETYPE", m_oBody.m_strCHANGETYPE);
				p_pJson->SetNodeValue("/body/paras/CHANGEDATE", m_oBody.m_strCHANGEDATE);
				p_pJson->SetNodeValue("/body/paras/REMARK1", m_oBody.m_strREMARK1);
				p_pJson->SetNodeValue("/body/paras/REMARK2", m_oBody.m_strREMARK2);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string		m_strGUID;
				std::string		m_strPHONE;			//�ֻ�����
				std::string		m_strOPENID;         //�������ڵı���
				std::string		m_strCHANGECONTENT;  //�������ݣ�����������Ϊ1ʱΪ�������ݣ�2-4��Ϊ��ý���ļ�ȫ·����
				std::string		m_strCHANGETYPE;     //��������
				std::string		m_strCHANGEDATE;     //����ʱ��
				std::string     m_strREMARK1;		//�����ֶ�1
				std::string     m_strREMARK2;		//�����ֶ�2
			};

			CHeader m_oHeader;
			CBody	m_oBody;
		};
		class CAddWeChatAlarm :
			public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				return true;
			}

			std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				p_pJson->SetNodeValue("/body/address", m_oBody.m_strAddress);
				p_pJson->SetNodeValue("/body/content", m_oBody.m_strCrcontent);
				p_pJson->SetNodeValue("/body/is_phone", m_oBody.m_strIsPhone);
				p_pJson->SetNodeValue("/body/create_date", m_oBody.m_strCreateDate);
				p_pJson->SetNodeValue("/body/user_code", m_oBody.m_strUserCode);
				p_pJson->SetNodeValue("/body/user_name", m_oBody.m_strUserName);
				p_pJson->SetNodeValue("/body/user_phone", m_oBody.m_strUserPhone);
				p_pJson->SetNodeValue("/body/user_address", m_oBody.m_strUserAddress);
				p_pJson->SetNodeValue("/body/unit_contactor", m_oBody.m_strUnitContactor);
				p_pJson->SetNodeValue("/body/hand_phone", m_oBody.m_strHandPhone);
				p_pJson->SetNodeValue("/body/to_org_id", m_oBody.m_strToOrgID);
				p_pJson->SetNodeValue("/body/to_org_name", m_oBody.m_strToOrgName);
				p_pJson->SetNodeValue("/body/to_user_id", m_oBody.m_strToUserID);
				p_pJson->SetNodeValue("/body/to_user_name", m_oBody.m_strToUserName);
				p_pJson->SetNodeValue("/body/state", m_oBody.m_strState);
				p_pJson->SetNodeValue("/body/com_no", m_oBody.m_strComNo);
				p_pJson->SetNodeValue("/body/open_id", m_oBody.m_strOpenID);
				p_pJson->SetNodeValue("/body/long", m_oBody.m_strLat);
				p_pJson->SetNodeValue("/body/lat", m_oBody.m_strLat);
				p_pJson->SetNodeValue("/body/remark1", m_oBody.m_strRemark1);
				p_pJson->SetNodeValue("/body/remark2", m_oBody.m_strRemark2);

				unsigned int l_uiIndex = 0;
				for (auto material : m_oBody.m_oMaterial)
				{
					std::string l_strPrefixPath("/body/material_list/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "f_guid", material.m_strfguid);
					p_pJson->SetNodeValue(l_strPrefixPath + "f_name", material.m_strfname);
					p_pJson->SetNodeValue(l_strPrefixPath + "attach_type", material.m_attachtype);
					p_pJson->SetNodeValue(l_strPrefixPath + "attach_path", material.m_strattachpath);
					l_uiIndex++;
				}
				return p_pJson->ToString();
			}
		public:
			class CBody
			{
			public:
				std::string		m_strGuid;					//��������
				std::string		m_strType;					//0δ֪��1�о�΢�ţ�2ʡ��΢�ţ�3����
				std::string		m_strAddress;				//�·���ַ
				std::string		m_strCrcontent;				//��������
				std::string   	m_strIsPhone;					//�Ƿ񷽱�����绰
				std::string		m_strCreateDate;			//����ʱ��
				std::string		m_strUserCode;				//���������֤����
				std::string		m_strUserName;				//����������
				std::string		m_strUserPhone;				//�������ֻ�
				std::string		m_strUserAddress;			//��������ϵ��ַ
				std::string		m_strUnitContactor;			//��ϵ��
				std::string		m_strHandPhone;				//��ϵ�绰
				std::string		m_strToOrgID;				//���ܲ���ID
				std::string		m_strToOrgName;				//���ܲ�������
				std::string		m_strToUserID;				//�����˾���
				std::string		m_strToUserName;			//����������
				std::string		m_strState;					//״̬��0δ����1�Ѷ���2�Ѵ���
				std::string		m_strComNo;					//�������
				std::string		m_strOpenID;				//�������ڵı��(΢�Ž���������)
				std::string		m_strLong;					//ά��
				std::string		m_strLat;					//����
				std::string		m_strNotReadChangeCount;	//δ��������Ϣ����
				std::string		m_strRemark1;				//����1
				std::string		m_strRemark2;				//����2

				std::vector<CMaterial> m_oMaterial;
			};

			CHeader m_oHeader;
			CBody	m_oBody;
		};
		class CRetureMsg
		{
		public:
			std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/EpointDataBody/ReturnInfo/Msg", m_oBody.m_strMsg);
				p_pJson->SetNodeValue("/body/EpointDataBody/ReturnInfo/IsSuccess", m_oBody.m_strIsSuccess);

				return p_pJson->ToString();
			}
		public:
			class CBody
			{
			public:
				std::string m_strMsg;
				std::string m_strIsSuccess;
			};
			CBody m_oBody;
			CHeader m_oHeader;
		};


		class CAcceptWeChatAlarm
		{
		public:
			std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/paras/GUID", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/paras/ACCEPTDATE", m_oBody.m_strAcceptData);
				p_pJson->SetNodeValue("/body/paras/ISACCEPT", m_oBody.m_strIsAccept);

				return p_pJson->ToString();
			}
		public:
			class CBody
			{
			public:
				std::string m_strGuid;
				std::string m_strAcceptData;
				std::string m_strIsAccept;
			};
			CBody m_oBody;
			CHeader m_oHeader;
		};


		class CTASyncChangeInfoRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strType = p_pJson->GetNodeValue("/body/third_type", "");
				m_oBody.m_strThirdAlarmGuid = p_pJson->GetNodeValue("/body/third_alarm_guid", "");
				m_oBody.m_strOrientation = p_pJson->GetNodeValue("/body/orientation", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strPhone = p_pJson->GetNodeValue("/body/phone", "");
				m_oBody.m_strOpenID = p_pJson->GetNodeValue("/body/open_id", "");
				m_oBody.m_strChangeType = p_pJson->GetNodeValue("/body/change_type", "");
				m_oBody.m_strChangeContent = p_pJson->GetNodeValue("/body/change_content", "");
				m_oBody.m_strChangeDate = p_pJson->GetNodeValue("/body/change_date", "");
				m_oBody.m_strLong = p_pJson->GetNodeValue("/body/long", "");
				m_oBody.m_strLat = p_pJson->GetNodeValue("/body/lat", "");
				m_oBody.m_strRemark1 = p_pJson->GetNodeValue("/body/remark1", "");
				m_oBody.m_strRemark2 = p_pJson->GetNodeValue("/body/remark2", "");

				return true;
			}

		public:
			class CBody
			{
			public:
				std::string		m_strSyncType;				//1����ʾ��ӣ�2����ʾ���£�3����ʾɾ��
				std::string		m_strGuid;					//����
				std::string		m_strType;					//����
				std::string		m_strThirdAlarmGuid;		//��������
				std::string		m_strOrientation;			//0�����˸���Ա���ͣ�1��Ա�������˷���
				std::string		m_strState;					//״̬��0δ����1�Ѷ�
				std::string   	m_strPhone;					//�ֻ�����
				std::string		m_strOpenID;				//�������ڵı��(΢�Ž���������)
				std::string		m_strChangeType;			//�������ͣ�1�����֣�2��ͼƬ��3����Ƶ��4��������
				std::string		m_strChangeContent;			//�������ݣ���������Ϊ1ʱΪ�������ݣ�2-4��Ϊ��ý���ļ�ȫ·����
				std::string		m_strChangeDate;			//����ʱ��  yyyy-mm-dd  hh24:mi:ss
				std::string		m_strLong;					//���ȣ�wgs84����ϵ��
				std::string		m_strLat;					//γ�ȣ�wgs84����ϵ��
				std::string		m_strRemark1;				//�����ֶ�1
				std::string		m_strRemark2;				//�����ֶ�2
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};

	}
}
