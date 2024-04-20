#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>
#include <Protocol/CKeyWordInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetBlacklistStatisticsRespond :
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
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
			
				
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			
			class CBody
			{
			public:				
				std::string m_strCount;

				CBody():m_strCount("0")
				{

				}
			};
			CBody m_oBody;
		};
	}
}
