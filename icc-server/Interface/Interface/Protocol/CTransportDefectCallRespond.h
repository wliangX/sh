#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTransPortDeflectCallRespond :
			public IRequest
		{
		public:
			bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/new_callref_id", "");
				m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/sponsor", "");
				m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/target", "");
				m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/result", "");
				return true;
			}

		public:

			class CBody
			{
			public:
				std::string m_strCallrefID;    //����ID
				std::string m_strNewCallrefID; //�»���ID
				std::string m_strSponsor;      //ƫת����ֻ���
				std::string m_strTarget;       //ƫתĿ��ֻ���
				std::string m_strResult;	   //0���ɹ� 1��ʧ��
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}

