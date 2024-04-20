#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAlarmLogSync :
			public IRespond
		{
		public:	
			
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				std::string l_strPrefixPath("/body/");
				m_oBody.m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "id", "");
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue(l_strPrefixPath + "alarm_id", "");
				m_oBody.m_strProcessID = p_pJson->GetNodeValue(l_strPrefixPath + "process_id", "");
				m_oBody.m_strFeedbackID = p_pJson->GetNodeValue(l_strPrefixPath + "feedback_id", "");
				m_oBody.m_strOperate = p_pJson->GetNodeValue(l_strPrefixPath + "operate", "");
				m_oBody.m_strOperateContent = p_pJson->GetNodeValue(l_strPrefixPath + "operate_content", "");
				m_oBody.m_strFromType = p_pJson->GetNodeValue(l_strPrefixPath + "from_type", "");
				m_oBody.m_strFromObject = p_pJson->GetNodeValue(l_strPrefixPath + "from_object", "");
				m_oBody.m_strFromObjectName = p_pJson->GetNodeValue(l_strPrefixPath + "from_object_name", "");
				m_oBody.m_strFromObjectOrgName = p_pJson->GetNodeValue(l_strPrefixPath + "from_object_org_name", "");
				m_oBody.m_strFromObjectOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "from_object_org_code", "");
				m_oBody.m_strToType = p_pJson->GetNodeValue(l_strPrefixPath + "to_type", "");
				m_oBody.m_strToObject = p_pJson->GetNodeValue(l_strPrefixPath + "to_object", "");
				m_oBody.m_strToObjectName = p_pJson->GetNodeValue(l_strPrefixPath + "to_object_name", "");
				m_oBody.m_strToObjectOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "to_object_org_code", "");
				m_oBody.m_strToObjectOrgName = p_pJson->GetNodeValue(l_strPrefixPath + "to_object_org_name", "");
				m_oBody.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "create_time", "");
				m_oBody.m_strSourceName = p_pJson->GetNodeValue(l_strPrefixPath + "source_name", "");
				m_oBody.m_strOperateAttachDesc = p_pJson->GetNodeValue(l_strPrefixPath + "operate_attach_desc", "");
				m_oBody.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "create_user", "");

				m_oBody.m_strFromOrgIdentifier = p_pJson->GetNodeValue(l_strPrefixPath + "from_object_org_identifier", "");//new
				m_oBody.m_strToObjectOrgIdentifier = p_pJson->GetNodeValue(l_strPrefixPath + "to_object_org_identifier", "");//new
				m_oBody.m_strDescription = p_pJson->GetNodeValue(l_strPrefixPath + "description", "");  //new
				m_oBody.m_strCreateOrg = p_pJson->GetNodeValue(l_strPrefixPath + "create_org", "");		//new
				
				m_oBody.m_strDeptOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "dept_org_code", ""); //delete
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue(l_strPrefixPath + "seat_no", "");	//delete
				m_oBody.m_strReceivedTime = p_pJson->GetNodeValue(l_strPrefixPath + "received_time", "");	//delete

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson){ return ""; }
			std::string ToString(JsonParser::IJsonPtr p_pJson, JsonParser::IJsonPtr p_pJsonParam)
			{
				if (nullptr == p_pJson || !p_pJsonParam->LoadJson(m_oBody.m_strOperateContent))
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				std::string l_strPrefixPath("/body/");
				
 				p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_strID);
 				p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", m_oBody.m_strAlarmID);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_id", m_oBody.m_strProcessID);
				p_pJson->SetNodeValue(l_strPrefixPath + "feedback_id", m_oBody.m_strFeedbackID);
				p_pJson->SetNodeValue(l_strPrefixPath + "seat_no", m_oBody.m_strSeatNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "operate", m_oBody.m_strOperate);

				// ��Content�Ĳ����б�ƴ��Json��
				int l_iParamNum = p_pJsonParam->GetCount("/param");
				for (int i = 0; i < l_iParamNum; i++)
				{
					p_pJson->SetNodeValue(l_strPrefixPath + "operate_content/" + std::to_string(i)
						, p_pJsonParam->GetNodeValue("/param/" + std::to_string(i), ""));
				}
				
				p_pJson->SetNodeValue(l_strPrefixPath + "from_type", m_oBody.m_strFromType);
				p_pJson->SetNodeValue(l_strPrefixPath + "from_object", m_oBody.m_strFromObject);
				p_pJson->SetNodeValue(l_strPrefixPath + "from_object_name", m_oBody.m_strFromObjectName);
				p_pJson->SetNodeValue(l_strPrefixPath + "from_object_org_name", m_oBody.m_strFromObjectOrgName);
				p_pJson->SetNodeValue(l_strPrefixPath + "from_object_org_code", m_oBody.m_strFromObjectOrgCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "to_type", m_oBody.m_strToType);
				p_pJson->SetNodeValue(l_strPrefixPath + "to_object", m_oBody.m_strToObject);
				p_pJson->SetNodeValue(l_strPrefixPath + "to_object_name", m_oBody.m_strToObjectName);
				p_pJson->SetNodeValue(l_strPrefixPath + "to_object_org_name", m_oBody.m_strToObjectOrgName);
				p_pJson->SetNodeValue(l_strPrefixPath + "to_object_org_code", m_oBody.m_strToObjectOrgCode);
 				p_pJson->SetNodeValue(l_strPrefixPath + "create_user", m_oBody.m_strCreateUser);
 				p_pJson->SetNodeValue(l_strPrefixPath + "create_time", m_oBody.m_strCreateTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "dept_org_code", m_oBody.m_strDeptOrgCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "source_name", m_oBody.m_strSourceName);
				p_pJson->SetNodeValue(l_strPrefixPath + "operate_attach_desc", m_oBody.m_strOperateAttachDesc);

				p_pJson->SetNodeValue(l_strPrefixPath + "from_object_org_identifier", m_oBody.m_strFromOrgIdentifier);
				p_pJson->SetNodeValue(l_strPrefixPath + "to_object_org_identifier", m_oBody.m_strToObjectOrgIdentifier);
				p_pJson->SetNodeValue(l_strPrefixPath + "description", m_oBody.m_strDescription);
				p_pJson->SetNodeValue(l_strPrefixPath + "create_org", m_oBody.m_strCreateOrg);
				p_pJson->SetNodeValue(l_strPrefixPath + "received_time", m_oBody.m_strReceivedTime);
				return p_pJson->ToString();
			}

		public:		
			class CBody
			{
			public:				
				std::string	m_strID;						//������ˮid
				std::string	m_strAlarmID;					//�Ӿ������
				std::string	m_strProcessID;					//�ɾ������
				std::string	m_strFeedbackID;			    //���������
				std::string	m_strOperate;				    //��������
				std::string	m_strOperateContent;			//��������
				std::string	m_strFromType;				    //����������
				std::string	m_strFromObject;			    //�����ߴ���
				std::string	m_strFromObjectName;			//����������
				std::string	m_strFromObjectOrgName;			//��������֯����
				std::string	m_strFromObjectOrgCode;			//�����ߵ�λ����
				std::string	m_strToType;				    //����������
				std::string	m_strToObject;				    //�����ߴ���
				std::string	m_strToObjectName;			    //����������
				std::string	m_strToObjectOrgName;			//��������֯����
				std::string	m_strToObjectOrgCode;			//�����ߵ�λ����
				std::string m_strCreateUser;				//������
				std::string m_strCreateTime;				//����ʱ��
				std::string m_strSourceName;				//��־����Դ
				std::string m_strOperateAttachDesc;			//�����б�

				std::string	m_strSeatNo;					//������ϯ
				std::string m_strDeptOrgCode;				//�ɼ��ĵ�λ����

				std::string m_strFromOrgIdentifier;         //�����ߵ�λ����  ����  ͨ��m_strFromObjectOrgCode��
				std::string m_strToObjectOrgIdentifier;     //�����ߵ�λ����
				std::string m_strDescription;				//����
				std::string m_strCreateOrg;					//������λ ����  = m_strFromObjectOrgCode

				std::string m_strReceivedTime;              //�Ӿ�ʱ�� received_time  2022/4/2

			};
			CHeader m_oHeader;			
			CBody	m_oBody;
		public:
			static bool SetLogInsertSql(const CBody &p_AlarmLogInfo, DataBase::SQLRequest &p_tSQLReqInsertAlarm)
			{
				p_tSQLReqInsertAlarm.sql_id = "insert_icc_t_alarm_log";

				p_tSQLReqInsertAlarm.param["id"] = p_AlarmLogInfo.m_strID;
				p_tSQLReqInsertAlarm.param["jjdbh"] = p_AlarmLogInfo.m_strAlarmID;
				p_tSQLReqInsertAlarm.param["pjdbh"] = p_AlarmLogInfo.m_strProcessID;
				p_tSQLReqInsertAlarm.param["fkdbh"] = p_AlarmLogInfo.m_strFeedbackID;
				p_tSQLReqInsertAlarm.param["operate"] = p_AlarmLogInfo.m_strOperate;
				p_tSQLReqInsertAlarm.param["operate_content"] = p_AlarmLogInfo.m_strOperateContent;
				p_tSQLReqInsertAlarm.param["from_type"] = p_AlarmLogInfo.m_strFromType;
				p_tSQLReqInsertAlarm.param["from_object"] = p_AlarmLogInfo.m_strFromObject;
				p_tSQLReqInsertAlarm.param["from_object_name"] = p_AlarmLogInfo.m_strFromObjectName;
				p_tSQLReqInsertAlarm.param["from_object_org_name"] = p_AlarmLogInfo.m_strFromObjectOrgName;
				p_tSQLReqInsertAlarm.param["from_object_org_code"] = p_AlarmLogInfo.m_strFromObjectOrgCode;
				p_tSQLReqInsertAlarm.param["from_object_org_identifier"] = p_AlarmLogInfo.m_strFromOrgIdentifier;
				p_tSQLReqInsertAlarm.param["to_type"] = p_AlarmLogInfo.m_strToType;
				p_tSQLReqInsertAlarm.param["to_object"] = p_AlarmLogInfo.m_strToObject;
				p_tSQLReqInsertAlarm.param["to_object_name"] = p_AlarmLogInfo.m_strToObjectName;
				p_tSQLReqInsertAlarm.param["to_object_org_name"] = p_AlarmLogInfo.m_strToObjectOrgName;
				p_tSQLReqInsertAlarm.param["to_object_org_code"] = p_AlarmLogInfo.m_strToObjectOrgCode;
				p_tSQLReqInsertAlarm.param["to_object_org_identifier"] = p_AlarmLogInfo.m_strToObjectOrgIdentifier;
				p_tSQLReqInsertAlarm.param["create_time"] = p_AlarmLogInfo.m_strCreateTime;
				p_tSQLReqInsertAlarm.param["source_name"] = p_AlarmLogInfo.m_strSourceName;
				p_tSQLReqInsertAlarm.param["operate_attach_desc"] = p_AlarmLogInfo.m_strOperateAttachDesc;
				p_tSQLReqInsertAlarm.param["description"] = p_AlarmLogInfo.m_strDescription;
				p_tSQLReqInsertAlarm.param["create_user"] = p_AlarmLogInfo.m_strCreateUser;
				p_tSQLReqInsertAlarm.param["create_org"] = p_AlarmLogInfo.m_strCreateOrg;

				p_tSQLReqInsertAlarm.param["seat_no"] = p_AlarmLogInfo.m_strSeatNo;
				p_tSQLReqInsertAlarm.param["dept_org_code"] = p_AlarmLogInfo.m_strDeptOrgCode;
				p_tSQLReqInsertAlarm.param["jjsj"] = p_AlarmLogInfo.m_strReceivedTime;
				return true;
			}

		};		
	}
}