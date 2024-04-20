/*
获取坐席默认地址响应
*/
#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CSeatAddressInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSeatAddressGetRespond :
			public IRespond
		{
		public:
			

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				//如果没有地址，不返回body内容
				if (!m_oBody.m_strAddress.empty())
				{
					std::string l_strPrefixPath("/body/");

					p_pJson->SetNodeValue(l_strPrefixPath + "guid", m_oBody.m_strGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "address", m_oBody.m_strAddress);
				}
				
				return p_pJson->ToString();
			}

		public:
			
			CHeaderEx m_oHeader;			
			CSeatAddressInfo m_oBody;
		};	
	}
}
