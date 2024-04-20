#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CDeleteClientRegisterRequest :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/client_id", m_oBody.m_strClientID);
				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strClientID;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
