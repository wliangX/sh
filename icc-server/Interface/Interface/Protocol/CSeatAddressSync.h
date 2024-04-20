/*
坐席默认地址同步消息
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
		class CSeatAddressSync :
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
				std::string l_strPrefixPath("/body/");
				p_pJson->SetNodeValue(l_strPrefixPath + "guid", m_oBody.m_oSeatAddress.m_strGuid);

				if (!m_oBody.m_oSeatAddress.m_strSeatNo.empty())
				{
					p_pJson->SetNodeValue(l_strPrefixPath + "seat_no", m_oBody.m_oSeatAddress.m_strSeatNo);
				}
			
				if (!m_oBody.m_oSeatAddress.m_strAddress.empty())
				{
					p_pJson->SetNodeValue(l_strPrefixPath + "address", m_oBody.m_oSeatAddress.m_strAddress);
				}
			
				p_pJson->SetNodeValue(l_strPrefixPath + "sync_type", m_oBody.m_strSyncType);
				
				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				CSeatAddressInfo m_oSeatAddress;
				std::string  m_strSyncType;
			};

			CHeader m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
