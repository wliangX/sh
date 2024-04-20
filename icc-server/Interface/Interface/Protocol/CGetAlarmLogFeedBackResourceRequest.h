#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
#include <vector>
namespace ICC
{
	namespace PROTOCOL
	{
        class CGetAlarmLogFeedBackResourceRequest :
            public IRequest
		{
        public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }
                m_oBody.m_strResourceId = p_pJson->GetNodeValue("/body/resource_id", "");

                return true;
            }
		public:
			CHeader m_oHeader;
			class CBody
			{
			public:               
                std::string m_strResourceId;                   
			};
			CBody m_oBody;
		};
	}
}
