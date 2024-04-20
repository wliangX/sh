#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAllParamRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strName = p_pJson->GetNodeValue("/body/name", "");
				m_oBody.m_strValue = p_pJson->GetNodeValue("/body/value", "");
				m_oBody.m_strIsVisibility = p_pJson->GetNodeValue("/body/is_visibility", "");
				m_oBody.m_strIsReadonly = p_pJson->GetNodeValue("/body/is_readonly", "");
				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strName;
				std::string m_strValue;
				std::string m_strIsVisibility;
				std::string m_strIsReadonly;
			};
			CBody m_oBody;
		};
	}
}