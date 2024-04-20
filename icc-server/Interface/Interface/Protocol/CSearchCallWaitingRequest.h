#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchCallWaitingRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");	
				m_oBody.m_strCallerId = p_pJson->GetNodeValue("/body/caller_id", "");
				m_oBody.m_strCalledId = p_pJson->GetNodeValue("/body/called_id", "");
				m_oBody.m_strHangupType = p_pJson->GetNodeValue("/body/hangup_type", "");
				m_oBody.m_strWaitlen = p_pJson->GetNodeValue("/body/wait_len", "");

				return true;
			}

		public:

			class CBody
			{
			public:
				std::string	m_strPageSize;					//ÿҳ����������Ϊ�գ�
				std::string	m_strPageIndex;					//ҳ�룬1��ʾ��һҳ������Ϊ�գ�
				std::string	m_strBeginTime;					//��ѯ��ʼʱ�䣨����Ϊ�գ�
				std::string	m_strEndTime;					//��ѯ����ʱ�䣨����Ϊ�գ�			
				std::string m_strCallerId;					//���к�
				std::string m_strCalledId;					// ���к�
				std::string m_strHangupType;				// �Ҷ�����
				std::string m_strWaitlen;					// �Ŷ�ʱ��

			};


			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}