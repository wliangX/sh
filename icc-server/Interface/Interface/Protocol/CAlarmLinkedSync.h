#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include <Protocol/CAddOrUpdateProcessRequest.h>
#include <Protocol/CAddOrUpdateLinkedRequest.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CAlarmLinkedSync :
            public IRespond, public IRequest
		{
		public:			
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				std::string l_strLinkedPath("/body/");
				// p_pJson->SetNodeValue(l_strLinkedPath + "msg_source", m_oBody.m_strMsgSource);

				p_pJson->SetNodeValue(l_strLinkedPath + "id", m_oBody.m_LinkedData.m_strID);
				p_pJson->SetNodeValue(l_strLinkedPath + "alarm_id", m_oBody.m_LinkedData.m_strAlarmID);
				p_pJson->SetNodeValue(l_strLinkedPath + "state", m_oBody.m_LinkedData.m_strState);
				p_pJson->SetNodeValue(l_strLinkedPath + "linked_org_code", m_oBody.m_LinkedData.m_strLinkedOrgCode);
				p_pJson->SetNodeValue(l_strLinkedPath + "linked_org_name", m_oBody.m_LinkedData.m_strLinkedOrgName);
				p_pJson->SetNodeValue(l_strLinkedPath + "linked_org_type", m_oBody.m_LinkedData.m_strLinkedOrgType);
				p_pJson->SetNodeValue(l_strLinkedPath + "dispatch_code", m_oBody.m_LinkedData.m_strDispatchCode);
				p_pJson->SetNodeValue(l_strLinkedPath + "dispatch_name", m_oBody.m_LinkedData.m_strDispatchName);
				p_pJson->SetNodeValue(l_strLinkedPath + "create_user", m_oBody.m_LinkedData.m_strCreateUser);
				p_pJson->SetNodeValue(l_strLinkedPath + "create_time", m_oBody.m_LinkedData.m_strCreateTime);
				p_pJson->SetNodeValue(l_strLinkedPath + "update_user", m_oBody.m_LinkedData.m_strUpdateUser);
				p_pJson->SetNodeValue(l_strLinkedPath + "update_time", m_oBody.m_LinkedData.m_strUpdateTime);
				p_pJson->SetNodeValue(l_strLinkedPath + "result", m_oBody.m_LinkedData.m_strResult);

				return p_pJson->ToString();
			}

            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				std::string l_strLinkedPath("/body/");
				// m_oBody.m_strMsgSource = p_pJson->GetNodeValue(l_strPrefixPath + "msg_source", "");

				m_oBody.m_LinkedData.m_strID = p_pJson->GetNodeValue(l_strLinkedPath + "id", "");
				m_oBody.m_LinkedData.m_strAlarmID = p_pJson->GetNodeValue(l_strLinkedPath + "alarm_id", "");
				m_oBody.m_LinkedData.m_strState = p_pJson->GetNodeValue(l_strLinkedPath + "state", "");
				m_oBody.m_LinkedData.m_strLinkedOrgCode = p_pJson->GetNodeValue(l_strLinkedPath + "linked_org_code", "");
				m_oBody.m_LinkedData.m_strLinkedOrgName = p_pJson->GetNodeValue(l_strLinkedPath + "linked_org_name", "");
				m_oBody.m_LinkedData.m_strLinkedOrgType = p_pJson->GetNodeValue(l_strLinkedPath + "linked_org_type", "");
				m_oBody.m_LinkedData.m_strDispatchCode = p_pJson->GetNodeValue(l_strLinkedPath + "dispatch_code", "");
				m_oBody.m_LinkedData.m_strDispatchName = p_pJson->GetNodeValue(l_strLinkedPath + "dispatch_name", "");
				m_oBody.m_LinkedData.m_strCreateUser = p_pJson->GetNodeValue(l_strLinkedPath + "create_user", "");
				m_oBody.m_LinkedData.m_strCreateTime = p_pJson->GetNodeValue(l_strLinkedPath + "create_time", "");
				m_oBody.m_LinkedData.m_strUpdateUser = p_pJson->GetNodeValue(l_strLinkedPath + "update_user", "");
				m_oBody.m_LinkedData.m_strUpdateTime = p_pJson->GetNodeValue(l_strLinkedPath + "update_time", "");
				m_oBody.m_LinkedData.m_strResult = p_pJson->GetNodeValue(l_strLinkedPath + "result", "");
                return true;
            }
		public:		
			class CBody
			{
			public:
				std::string m_strMsgSource;					//消息来源
				std::string m_strSyncType;					//同步选项，1添加，2更新
				
				CAddOrUpdateLinkedRequest::CLinkedData m_LinkedData;
			};
			CHeader m_oHeader;			
			CBody	m_oBody;
		};
	}
}
