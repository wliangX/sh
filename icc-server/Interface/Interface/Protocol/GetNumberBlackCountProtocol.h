/*
������Ϣ����Ͻ��λ/���鼶������
*/
#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>


namespace ICC
{
	namespace PROTOCOL
	{
		class CGetNumberBlackCountRequest :
			public IRequest
		{
		public:
			
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strNumber = p_pJson->GetNodeValue("/body/number", "");                
				return true;
			}


		public:
			class CBody
			{
			public:
				std::string m_strNumber;  //��ʼʱ��				
			};

			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};

		class CGetNumberBlackCountRespond :
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

				p_pJson->SetNodeValue("/body/number", m_oBody.m_strNumber);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);				

				return p_pJson->ToString();
			}

		public:			
			class CBody
			{
			public:
				std::string m_strCount;									
				std::string m_strNumber;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};	
		
	}
}
