#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CVcsBillStatusSync :
            public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }
				m_oBody.m_strId = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strProcessID = p_pJson->GetNodeValue("/body/process_id", "");
                m_oBody.m_strOwner = p_pJson->GetNodeValue("/body/owner", "");  
				m_oBody.m_strOwnerType = p_pJson->GetNodeValue("/body/owner_type", "");
				m_oBody.m_strStatus = p_pJson->GetNodeValue("/body/status", "");
				m_oBody.m_strUpdateTime = p_pJson->GetNodeValue("/body/update_time", "");
				m_oBody.m_strCreateTime = p_pJson->GetNodeValue("/body/create_time", "");
                return true;
            }			

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strId;
				std::string m_strAlarmID;
				std::string m_strProcessID;
                std::string m_strOwner;  
                std::string m_strOwnerType;  
				std::string m_strStatus;
				std::string m_strUpdateTime;
				std::string m_strCreateTime;
			};
			CBody m_oBody;
		};
	}
}
