#ifndef __IfdsAgentListRequest_H__
#define __IfdsAgentListRequest_H__

#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CIfdsIfdsAgentListRequest : public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/acd", m_oBody.m_strAcd);
				return p_pJson->ToString();
			}

			class CBody
			{
			public:
				std::string m_strAcd;				
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};

	}
}

#endif