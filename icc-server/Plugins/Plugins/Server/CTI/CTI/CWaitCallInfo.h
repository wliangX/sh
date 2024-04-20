#pragma once
#include <map>
#include <string>
#include <Json/IJsonFactory.h>

namespace ICC
{
    class CWaitCallInfo
	{	
	public:
        virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
        {
            if (nullptr == p_pJson)
            {
                return "";
            }
            p_pJson->SetNodeValue("/acd", m_strACD);
            p_pJson->SetNodeValue("/original_acd", m_strOriginalACD);
           //p_pJson->SetNodeValue("/callref_id", m_strCallRefId);
			
			p_pJson->SetNodeValue("/caller_id", m_strCallerId);
			p_pJson->SetNodeValue("/called_id", m_strCalledId);
				
            p_pJson->SetNodeValue("/acd_dept", m_strACDDept);
            p_pJson->SetNodeValue("/start_time", m_strTime);
            p_pJson->SetNodeValue("/original_acd_dept", m_strOriginalACDDept);
            
            return p_pJson->ToString();
        }

        virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
        {
            if (!p_pJson || !p_pJson->LoadJson(p_strReq))
            {
                return false;
            }

            m_strACD = p_pJson->GetNodeValue("/acd", "");
            m_strOriginalACD = p_pJson->GetNodeValue("/original_acd", "");
            //m_strCallRefId = p_pJson->GetNodeValue("/callref_id", "");
			m_strCallerId = p_pJson->GetNodeValue("/caller_id", "");
            m_strCalledId = p_pJson->GetNodeValue("/called_id", "");
			
            m_strACDDept = p_pJson->GetNodeValue("/acd_dept", "");
            m_strTime = p_pJson->GetNodeValue("/start_time", "");
            return true;
        }


	public:
        std::string m_strACD;           //
        std::string m_strOriginalACD;  //
        std::string m_strCallRefId;      //
		std::string m_strCallerId;
		std::string m_strCalledId;
		
        std::string m_strACDDept;       //
        std::string m_strTime;
        std::string m_strOriginalACDDept;
	};
}