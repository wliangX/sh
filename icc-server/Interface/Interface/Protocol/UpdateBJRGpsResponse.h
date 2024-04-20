#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CUpdateBJRGpsResponse :
            public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
                m_oHeader.SaveTo(p_pJson);               
                return p_pJson->ToString();
            }
			

		public:
			CHeaderEx m_oHeader;			
		};
	}
}
