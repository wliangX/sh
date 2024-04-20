#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebSignRespond :
			public ISend,public IReceive
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);				
				return p_pJson->ToString();
			}
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strResult = p_pJson->GetNodeValue("/body/result", "");				
				return true;
			}
		public:
			class CBody
			{
			public:
				std::string m_strResult;			//0：表示成功，1：表示失败
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}