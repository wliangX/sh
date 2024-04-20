/*
获取报警人身份证号请求
*/
#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CAuthInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetBJRSFXXRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_oAuthInfo.m_strXm = p_pJson->GetNodeValue("/body/xm", "");
				m_oBody.m_oAuthInfo.m_stSFZH = p_pJson->GetNodeValue("/body/sfzh", "");
				m_oBody.m_oAuthInfo.m_strDWDM = p_pJson->GetNodeValue("/body/dwdm", "");
				m_oBody.m_oAuthInfo.m_strDWMC = p_pJson->GetNodeValue("/body/dwmc", "");
				m_oBody.m_strBJRSFZH = p_pJson->GetNodeValue("/body/bjrsfzh", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				CAuthInfo m_oAuthInfo;
				std::string m_strBJRSFZH;  //报警人身份证号
			};
			CBody m_oBody;
		};
	}
}
