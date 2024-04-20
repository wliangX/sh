#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CVcsSetRecordInfoRequest :
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
				m_oBody.m_strCaller = p_pJson->GetNodeValue("/body/caller", "");
				m_oBody.m_strCalled = p_pJson->GetNodeValue("/body/called", "");
                m_oBody.m_strRecordFile = p_pJson->GetNodeValue("/body/recordfile", "");
                m_oBody.m_strCallTime = p_pJson->GetNodeValue("/body/calltime", "");
                m_oBody.m_strRecordTime = p_pJson->GetNodeValue("/body/recordtime", "");
                return true;
            }			

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strCaseID;
				std::string m_strCaller;
				std::string m_strCalled;
				std::string m_strRecordFile;
				std::string m_strCallTime;
                std::string m_strRecordTime;
			};
			CBody m_oBody;
		};
	}
}
