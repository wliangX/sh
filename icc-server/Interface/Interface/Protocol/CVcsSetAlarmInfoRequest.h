#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CVcsSetAlarmInfoRequest :
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
				m_oBody.m_strCaseAddress = p_pJson->GetNodeValue("/body/caseAddress", "");
				m_oBody.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/areaorg", "");
                m_oBody.m_strLongitude = p_pJson->GetNodeValue("/body/longitude", "");
                m_oBody.m_strLatitude = p_pJson->GetNodeValue("/body/latitude", "");  
				m_oBody.m_strOperatorCode = p_pJson->GetNodeValue("/body/usercode", "");
				m_oBody.m_strOperatorName = p_pJson->GetNodeValue("/body/username", "");
                return true;
            }			

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strId;
				std::string m_strCaseAddress;
				std::string m_strAdminDeptCode;
                std::string m_strLongitude;  
                std::string m_strLatitude;  
				std::string m_strOperatorCode;
				std::string m_strOperatorName;
			};
			CBody m_oBody;
		};
	}
}
