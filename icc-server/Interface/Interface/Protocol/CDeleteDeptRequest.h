#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CDeleteDeptRequest :
            public IRequest, IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
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
				p_pJson->SetNodeValue("/body/code", m_oBody.m_strCode);
                return p_pJson->ToString();
            }
		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strGuid;
				std::string m_strCode;
			};
			CBody m_oBody;
		};
	}
}
