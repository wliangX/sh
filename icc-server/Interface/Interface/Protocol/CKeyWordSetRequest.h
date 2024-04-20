#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CKeyWordInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CKeyWordSetRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_oKeyWord.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_oKeyWord.m_strKeyWord = p_pJson->GetNodeValue("/body/key_word", "");
				m_oBody.m_oKeyWord.m_strContent = p_pJson->GetNodeValue("/body/content", "");

				if (m_oBody.m_oKeyWord.m_strGuid.empty())
				{
					m_oBody.m_strCreateUser = m_oHeader.m_strSeatNo;
				}
				else
				{
					m_oBody.m_strUpdateUser = m_oHeader.m_strSeatNo;
				}
				
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				CKeyWordInfo m_oKeyWord;
				std::string m_strCreateUser;
				std::string m_strUpdateUser;
			};
			CBody m_oBody;
		};
	}
}
