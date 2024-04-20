#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAuthCheckUserRequest :
			public IRequest, public ISend
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strUser_Code = p_pJson->GetNodeValue("/body/code", "");
				m_oBody.m_strUser_Pwd = p_pJson->GetNodeValue("/body/pwd", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/code", m_oBody.m_strUser_Code);
				p_pJson->SetNodeValue("/body/pwd", m_oBody.m_strUser_Pwd);
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strUser_Code;
				std::string m_strUser_Pwd;
			};
			CBody m_oBody;
		};
	}
}
