/*
��ϯĬ�ϵ�ַ��Ϣ
*/
#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSeatAddressInfo
		{
		public:
			std::string m_strGuid; //guid
			std::string m_strSeatNo;  //��ϯ��
			std::string m_strAddress;  //Ĭ�ϵ�ַ


		public:

			//redis��ȡ������ʹ��
			std::string ToString(JsonParser::IJsonPtr p_pJson)
			{

				if (nullptr == p_pJson)
				{
					return "";
				}

			
				p_pJson->SetNodeValue("/guid", m_strGuid);

				p_pJson->SetNodeValue("/seat_no", m_strSeatNo);
				
				p_pJson->SetNodeValue("/address", m_strAddress);
				
				return p_pJson->ToString();
			}

			bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}

				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}

				m_strGuid = p_pJson->GetNodeValue("/guid", "");
				m_strSeatNo = p_pJson->GetNodeValue("/seat_no", "");
				m_strAddress = p_pJson->GetNodeValue("/address", "");
				return true;
			}
		};	
	}
}
