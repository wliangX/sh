#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmsServerStatusSync :
			public ISync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				p_pJson->SetNodeValue("/body/status", m_oBody.m_strStatus);
				p_pJson->SetNodeValue("/body/description", m_oBody.m_strDescription);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strType;
				std::string m_strStatus;
				std::string m_strDescription;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
