/*
����ͳ��������Ϣ
*/
#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>


namespace ICC
{
	namespace PROTOCOL
	{
		class CGetPhoneMarkStatisticsRequest :
			public IRequest
		{
		public:
			
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
			
				m_oBody.m_strPhone = p_pJson->GetNodeValue("/body/phone", "");
				return true;
			}


		public:
			class CBody
			{
			public:
				std::string m_strPhone;  //��ָ������ģ�������Ϊ��
			};

			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
