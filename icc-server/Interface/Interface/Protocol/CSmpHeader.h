#pragma once 
#include <Json/IJson.h>

namespace ICC
{
    namespace PROTOCOL
    {
//#define ACTION_ADD           "add"
//#define ACTION_UPDATE        "update"
//#define ACTION_DELETE        "delete"
//实时更新消息 begin
#define ACTION_ADD           "i"  
#define ACTION_UPDATE        "u"
#define ACTION_DELETE        "d"
//实时更新消息 end

#define ACTION_BIND			 "bind"
#define ACTION_BIND_STAFF    "bind"
#define ACTION_UNBIND		 "unbind"
#define ACTION_UNBIND_STAFF  "unbind"
#define ACTION_BIND_ROLE     "bind_role"
#define ACTION_UNBIND_ROLE   "unbind_role"
#define ACTION_BIND_ORG      "bind_org"
#define ACTION_UNBIND_ORG    "unbind_org"

#define SYNC_TYPE_ADD		"1"
#define SYNC_TYPE_UPDATE	"2"
#define SYNC_TYPE_DELETE	"3"

        class CSmpHeader
        {
        public:
            virtual bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
            {
                if (nullptr == p_pJson)
                {
                    return false;
                }
                if (!p_pJson->LoadJson(p_strJson))
                {
                    return false;
                }
                m_strCmd = p_pJson->GetNodeValue("/header/cmd", "");
                m_strMsgID = p_pJson->GetNodeValue("/header/msgid", "");
                m_strResponse = p_pJson->GetNodeValue("/header/reponse", "");
                m_strResponseType = p_pJson->GetNodeValue("/header/reponse_type", "");
                m_strRequestType = p_pJson->GetNodeValue("/header/reques_type", "");
                m_strRequest = p_pJson->GetNodeValue("/header/request", "");
                m_strSendTime = p_pJson->GetNodeValue("/header/send_time", "");
                m_strSubsystemID = p_pJson->GetNodeValue("/header/subsystem_id", "");
                m_strSystemID = p_pJson->GetNodeValue("/header/system_id", "");
                m_strRelatedID = p_pJson->GetNodeValue("/header/related_id", "");
                m_strAction = p_pJson->GetNodeValue("/header/action", "");
                return true;
            };

            virtual void SaveTo(JsonParser::IJsonPtr p_pJson)
            {
                p_pJson->SetNodeValue("/header/cmd", m_strCmd);
                p_pJson->SetNodeValue("/header/msgid", m_strMsgID);
                p_pJson->SetNodeValue("/header/reponse", m_strResponse);
                p_pJson->SetNodeValue("/header/reponse_type", m_strResponseType);
                p_pJson->SetNodeValue("/header/reques_type", m_strRequestType);
                p_pJson->SetNodeValue("/header/request", m_strRequest);
                p_pJson->SetNodeValue("/header/send_time", m_strSendTime);
                p_pJson->SetNodeValue("/header/subsystem_id", m_strSubsystemID);
                p_pJson->SetNodeValue("/header/system_id", m_strSystemID);
                p_pJson->SetNodeValue("/header/related_id", m_strRelatedID);
                p_pJson->SetNodeValue("/header/action", m_strAction);
            }

        public:
            std::string m_strCmd;
            std::string m_strMsgID;
            std::string m_strResponse;
            std::string m_strResponseType;
            std::string m_strRequestType;
            std::string m_strRequest;
            std::string m_strSendTime;
            std::string m_strSubsystemID;
            std::string m_strSystemID;
            std::string m_strRelatedID;
            std::string m_strAction;
        };
    }
}
