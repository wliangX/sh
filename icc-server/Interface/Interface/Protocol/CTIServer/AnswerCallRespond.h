#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAnswerCallRespond :
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

				/*p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallRefId);
				p_pJson->SetNodeValue("/body/device", m_oBody.m_strDevice);*/
				//p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCallRefId;
				std::string m_strDevice;
				//std::string m_strResult;
			};
			CBody m_oBody;
		};
	}
}
