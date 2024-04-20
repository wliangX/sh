/*
坐席默认地址增，删，改请求消息
*/
#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CKeyWordInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSeatAddressSetRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strSetType = p_pJson->GetNodeValue("/body/set_type", "");
				m_oBody.m_strAddress = p_pJson->GetNodeValue("/body/address", "");
				
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strSetType;  //1：添加，2：更新，3：删除
				std::string m_strAddress;  //默认地址
			};
			CBody m_oBody;
		};
	}
}
