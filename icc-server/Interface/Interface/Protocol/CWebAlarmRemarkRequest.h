#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebLoginRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_oUser.m_strCode = p_pJson->GetNodeValue("/body/user/code", "");
				m_oBody.m_oUser.m_strPwd = p_pJson->GetNodeValue("/body/user/pwd", "");
				m_oBody.m_strLangCode = p_pJson->GetNodeValue("/body/lang_code", "");
				return true;
			}

		public:
			class CUser
			{
			public:
				std::string m_strCode;				//�û�����
				std::string m_strPwd;				//�û�����
			};
			class CBody
			{
			public:
				CUser m_oUser;
				std::string m_strLangCode;			//���Ա���
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
