#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CVcsUpdateLogRequset : public IRequest
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
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue(l_strPrefixPath + "seat_no", "");
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
				m_oBody.m_strToObjectOrgName = p_pJson->GetNodeValue(l_strPrefixPath + "to_object_org_name", "");
				m_oBody.m_strToObjectOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "to_object_org_code", "");
				m_oBody.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "create_user", "");
				m_oBody.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "create_time", "");
				m_oBody.m_strDeptOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "dept_org_code", "");
				m_oBody.m_strSourceName = p_pJson->GetNodeValue(l_strPrefixPath + "source_name", "");
				m_oBody.m_strOperateAttachDesc = p_pJson->GetNodeValue(l_strPrefixPath + "operate_attach_desc", "");

				m_oBody.m_strFromOrgIdentifier = p_pJson->GetNodeValue(l_strPrefixPath + "from_object_org_identifier", "");//new
				m_oBody.m_strToObjectOrgIdentifier = p_pJson->GetNodeValue(l_strPrefixPath + "to_object_org_identifier", "");//new
				m_oBody.m_strDescription = p_pJson->GetNodeValue(l_strPrefixPath + "description", "");  //new
				m_oBody.m_strCreateOrg = p_pJson->GetNodeValue(l_strPrefixPath + "create_org", "");		//new
				m_oBody.m_strReceivedTime = p_pJson->GetNodeValue(l_strPrefixPath + "received_time", "");		//new
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
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
				p_pJson->SetNodeValue(l_strPrefixPath + "operate_content", m_oBody.m_strOperateContent);
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
				p_pJson->SetNodeValue(l_strPrefixPath + "received_time", m_oBody.m_strReceivedTime);

				return p_pJson->ToString();
			}
		public:
			
			class CBody
			{
			public:
				std::string	m_strID;						//警情流水id
				std::string	m_strAlarmID;					//接警单编号
				std::string	m_strProcessID;					//派警单编号
				std::string	m_strFeedbackID;			    //反馈单编号
				std::string	m_strSeatNo;					//处置坐席
				std::string	m_strOperate;				    //操作类型
				std::string	m_strOperateContent;			//操作内容
				std::string	m_strFromType;				    //发起者类型
				std::string	m_strFromObject;			    //发起者代码
				std::string	m_strFromObjectName;			//发起者名称
				std::string	m_strFromObjectOrgName;			//发起者组织名称
				std::string	m_strFromObjectOrgCode;			//发起者单位短码
				std::string	m_strToType;				    //接收者类型
				std::string	m_strToObject;				    //接收者代码
				std::string	m_strToObjectName;			    //接收者名称
				std::string	m_strToObjectOrgName;			//接收者组织名称
				std::string	m_strToObjectOrgCode;			//接收者单位短码
				std::string m_strCreateUser;				//创建人
				std::string m_strCreateTime;				//创建时间
				std::string m_strDeptOrgCode;				//可见的单位短码
				std::string m_strSourceName;				//日志的来源
				std::string m_strOperateAttachDesc;			//附件列表

				std::string m_strFromOrgIdentifier;         //发起者单位短码
				std::string m_strToObjectOrgIdentifier;     //接收者单位短码
				std::string m_strDescription;				//描述
				std::string m_strCreateOrg;					//创建单位
				std::string m_strReceivedTime;              //接警时间 received_time 
			};

			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}
