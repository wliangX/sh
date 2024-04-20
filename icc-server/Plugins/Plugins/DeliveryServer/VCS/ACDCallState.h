#pragma once
#include <map>
#include <string>
#include <Json/IJsonFactory.h>

namespace ICC
{
    class ACDCallState
	{	
	public:
        virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
        {
            if (nullptr == p_pJson)
            {
                return "";
            }
            p_pJson->SetNodeValue("/body/agent", m_strAgent);
            p_pJson->SetNodeValue("/body/acd", m_strACD);
            p_pJson->SetNodeValue("/body/callref_id", m_strCallRefId);
            p_pJson->SetNodeValue("/body/caller_num", m_strCallerNum);
            p_pJson->SetNodeValue("/body/called_num", m_strCalledNum);
            p_pJson->SetNodeValue("/body/state", m_strState);
            p_pJson->SetNodeValue("/body/time", m_strTime);

            p_pJson->SetNodeValue("/body/answeringtime", m_strAnsweringtTime);
            p_pJson->SetNodeValue("/body/seatno", m_strSeatNo);

            p_pJson->SetNodeValue("/body/alarm_id", m_strAlarmID);

            p_pJson->SetNodeValue("/body/addr", m_strAddr);
            p_pJson->SetNodeValue("/body/person_name", m_strPersonName);
            p_pJson->SetNodeValue("/body/person_sex", m_strPersonSex);
            p_pJson->SetNodeValue("/body/person_id", m_strPersonID);
            return p_pJson->ToString();
        }

        virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
        {
            if (!p_pJson || !p_pJson->LoadJson(p_strReq))
            {
                return false;
            }

            m_strAgent = p_pJson->GetNodeValue("/body/agent", "");
            m_strACD = p_pJson->GetNodeValue("/body/acd", "");
            m_strCallRefId = p_pJson->GetNodeValue("/body/callref_id", "");
            m_strCallerNum = p_pJson->GetNodeValue("/body/caller_num", "");
            m_strCalledNum = p_pJson->GetNodeValue("/body/called_num", "");
            m_strState = p_pJson->GetNodeValue("/body/state", "");
            m_strTime = p_pJson->GetNodeValue("/body/time", "");

            m_strAnsweringtTime = p_pJson->GetNodeValue("/body/answeringtime", "");
            m_strSeatNo = p_pJson->GetNodeValue("/body/seatno", "");

            m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");

            m_strAddr = p_pJson->GetNodeValue("/body/addr", "");
            m_strPersonName = p_pJson->GetNodeValue("/body/person_name", "");
            m_strPersonSex = p_pJson->GetNodeValue("/body/person_sex", "");
            m_strPersonID = p_pJson->GetNodeValue("/body/person_id", "");
            return true;
        }


	public:
        std::string m_strAgent;
        std::string m_strACD;
        std::string m_strCallRefId;
        std::string m_strCallerNum;
        std::string m_strCalledNum;
        std::string m_strState;
        std::string m_strTime;
        std::string m_strAnsweringtTime;
        std::string m_strSeatNo;        
        std::string m_strAlarmID;

        std::string m_strAddr;
        std::string m_strPersonName;
        std::string m_strPersonSex;
        std::string m_strPersonID;
	};
}