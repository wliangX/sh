#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTASetStateRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strGuid;				 //��������
				std::string m_strState;				 //״̬��0δ����1�Ѷ���2�Ѵ���
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};		
	}
}
