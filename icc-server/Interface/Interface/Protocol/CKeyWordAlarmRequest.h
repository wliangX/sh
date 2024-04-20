/*
敏感警情同步消息
*/
#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>


namespace ICC
{
	namespace PROTOCOL
	{
		class CKeyWordAlarmQryRequest :
			public IRequest
		{
		public:
			
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strguid = p_pJson->GetNodeValue("/body/guid", "");

				return true;
			}


		public:
			class CBody
			{
			public:
				std::string m_strPageSize;  //每页数量
				std::string  m_strPageIndex;  //第几页
			};

			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
