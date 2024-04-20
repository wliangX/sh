#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CSetPhoneInfoRespond :
            public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
				if (nullptr == p_pJson)
				{
					return "";
				}

                m_oHeader.SaveTo(p_pJson);
              //  p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
                return p_pJson->ToString();
            }
		
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				//std::string m_strResult;
			};
			CBody m_oBody;
		};
	}
}
