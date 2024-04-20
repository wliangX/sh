#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetCTIConnStateRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				//

				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				//
			};
			CBody m_oBody;
		};
	}
}
