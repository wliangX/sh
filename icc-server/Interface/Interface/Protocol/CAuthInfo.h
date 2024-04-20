/*
认证信息
*/
#pragma once 
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAuthInfo
		{
		public:
			std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/xm", m_strXm);
				p_pJson->SetNodeValue("/sfz", m_stSFZH);
				p_pJson->SetNodeValue("/dwdm", m_strDWDM);
				p_pJson->SetNodeValue("/dwmc", m_strDWMC);
				
				return p_pJson->ToString();

			}
		public:
				std::string m_strXm;  //发起操作人姓名，必填 
				std::string m_stSFZH;  //发起操作人身份证号码，必填
				std::string m_strDWDM;  //发起操作人单位代码，必填
				std::string m_strDWMC;  //发起操作人的单位名称，必填

		
		};
	}
}
