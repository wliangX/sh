#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAESStateRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{


				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/state", m_oBody.m_strState);
			//	p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
				std::string m_strState;
				std::string m_strResult;
			};
			CBody m_oBody;
		};
	}
}
