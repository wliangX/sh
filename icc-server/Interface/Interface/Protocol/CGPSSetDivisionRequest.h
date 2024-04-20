#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CGPSSetDivisionRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strPhonePre = p_pJson->GetNodeValue("/body/phone_pre", "");
				m_oBody.m_strProvince = p_pJson->GetNodeValue("/body/province", "");
				m_oBody.m_strCity = p_pJson->GetNodeValue("/body/city", "");
				m_oBody.m_strOperator = p_pJson->GetNodeValue("/body/operator", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strPhonePre;				//�绰����
				std::string m_strProvince;				//ʡ��
				std::string m_strCity;					//����
				std::string m_strOperator;				//��Ӫ��
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
