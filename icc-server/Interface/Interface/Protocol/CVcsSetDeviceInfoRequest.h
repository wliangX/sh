#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CVcsSetDeviceInfoRequest :
            public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }
                m_oBody.m_strCaseID = p_pJson->GetNodeValue("/body/caseid", "");
                m_oBody.m_strDispatchName = p_pJson->GetNodeValue("/body/dispatch_name", "");
                m_oBody.m_strDispatchCode = p_pJson->GetNodeValue("/body/dispatch_code", "");
                m_oBody.m_strDispatchDept = p_pJson->GetNodeValue("/body/dispatch_dept", "");
                m_oBody.m_strInterPhoneID = p_pJson->GetNodeValue("/body/interphoneid", "");
                m_oBody.m_strStatus = p_pJson->GetNodeValue("/body/status", "");
                return true;
            }
		

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strCaseID;
				std::string m_strDispatchName;
                std::string m_strDispatchCode;
                std::string m_strDispatchDept;
                std::string m_strInterPhoneID;
                std::string m_strStatus;
			};
			CBody m_oBody;
		};
	}
}
