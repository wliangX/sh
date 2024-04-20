#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>


namespace ICC
{
	namespace PROTOCOL
	{
        class CGetAlarmRemarkRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
                m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				return true;
			}
			
		public:
            CHeaderEx m_oHeader;
            class CBody
            {
            public:
                std::string m_strGuid;
            };
            CBody m_oBody;
		};	
	}
}
