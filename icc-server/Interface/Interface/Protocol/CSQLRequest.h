#pragma once 
#include <map>
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSQLRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				return true;
			}

		public:
			CHeader m_oHeader;
			struct tSQLRequest
			{
				std::string m_strSQLID;
				std::map<std::string, std::string> m_mapParam;
			};
			tSQLRequest m_oBody;
		};
	}
}
