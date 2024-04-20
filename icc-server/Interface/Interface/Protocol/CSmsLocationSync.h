#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmsLocationSync :
			public ISync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/telnum", m_oBody.m_strTelnum);
				p_pJson->SetNodeValue("/body/long", m_oBody.m_strLong);
				p_pJson->SetNodeValue("/body/lat", m_oBody.m_strLat);
				p_pJson->SetNodeValue("/body/venderError", m_oBody.m_strVenderError);
				p_pJson->SetNodeValue("/body/description", m_oBody.m_strDescription);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strResult;
				std::string m_strTelnum;
				std::string m_strLong;
				std::string m_strLat;
				std::string m_strVenderError;
				std::string m_strDescription;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
