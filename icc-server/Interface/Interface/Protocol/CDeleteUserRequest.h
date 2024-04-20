#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CAuthDeleteRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strCode = p_pJson->GetNodeValue("/body/coid", "");
				m_oBody.m_strName = p_pJson->GetNodeValue("/body/name", "");
				m_oBody.m_strPwd = p_pJson->GetNodeValue("/body/pwd", "");
				m_oBody.m_strRemark = p_pJson->GetNodeValue("/body/remark", "");
				return true;
			}
            virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
				if (nullptr == p_pJson)
				{
					return "";
				}

                m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/coid", m_oBody.m_strCode);
				p_pJson->SetNodeValue("/body/name", m_oBody.m_strName);
				p_pJson->SetNodeValue("/body/pwd", m_oBody.m_strPwd);
				p_pJson->SetNodeValue("/body/remark", m_oBody.m_strRemark);
                return p_pJson->ToString();
            }
		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strGuid;
				std::string m_strCode;
				std::string m_strName;
				std::string m_strPwd;
				std::string m_strRemark;
			};
			CBody m_oBody;
		};
	}
}
