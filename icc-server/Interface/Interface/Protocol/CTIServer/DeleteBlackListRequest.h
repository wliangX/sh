#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CDeleteBlackListRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strLimitType = p_pJson->GetNodeValue("/body/limit_type", "");
				m_oBody.m_strLimitNum = p_pJson->GetNodeValue("/body/limit_num", "");

				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strLimitType;
				std::string m_strLimitNum;		// ÊÜÏŞºÅÂë
			};
			CBody m_oBody;
		};
	}
}
