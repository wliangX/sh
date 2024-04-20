#pragma once 
#include <vector>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class GetLicenseInfo : public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strGenDate = p_pJson->GetNodeValue("/body/gen_date", "");
				m_oBody.m_strValidDayse = p_pJson->GetNodeValue("/body/valid_days", "");
				m_oBody.m_strExpirationDate = p_pJson->GetNodeValue("/body/expiration_date", "");
				m_oBody.m_strClientNum = p_pJson->GetNodeValue("/body/client_num", "");
				m_oBody.m_strLicenseState = p_pJson->GetNodeValue("/body/status", "");
				return true;
			}
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/gen_date", m_oBody.m_strGenDate);
				p_pJson->SetNodeValue("/body/valid_days", m_oBody.m_strValidDayse);
				p_pJson->SetNodeValue("/body/expiration_date", m_oBody.m_strExpirationDate);
				p_pJson->SetNodeValue("/body/client_num", m_oBody.m_strClientNum);
				p_pJson->SetNodeValue("/body/status", m_oBody.m_strLicenseState);				
				return p_pJson->ToString();
			}

		public:
			
			class CBody
			{				
			public:
				std::string m_strGenDate;			//License����ʱ��
				std::string m_strValidDayse;		//ʣ����Ч����
				std::string m_strExpirationDate;	//��ֹʱ��
				std::string m_strClientNum;			//�ͻ�������
				std::string m_strLicenseState;		//license״̬  0��Ч��1 ��Ч
			};

			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
