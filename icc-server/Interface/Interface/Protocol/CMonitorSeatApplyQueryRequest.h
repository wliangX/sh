#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CMonitorSeatApplyQueryRequest :
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
				
				m_oBody.m_strApplyType = p_pJson->GetNodeValue("/body/apply_type", "");
				
				m_oBody.m_strApproveResult = p_pJson->GetNodeValue("/body/approve_result", "");
				
				if (m_oBody.m_bSingelRequest)
				{
					m_oBody.m_strApplyTargetID = p_pJson->GetNodeValue("/body/apply_target_id", "");
				}
				else
				{
				    m_oBody.m_strStartTime = p_pJson->GetNodeValue("/body/start_time", "");
				    m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				    m_oBody.m_strApplyOrgCode = p_pJson->GetNodeValue("/body/apply_org_code", "");
				}
				return true;
			}


		public:
			class CBody
			{
			public:
			    //��������ʹ��
			    bool        m_bSingelRequest;                //�Ƿ񵥸�����
				
				std::string m_strApplyTargetID;             //Ŀ��id  ������ȷƥ��ʹ��
			    
				std::string	m_strPageSize;					//ÿҳ����    ����
				std::string	m_strPageIndex;						//�ڼ�ҳ   ����
				std::string	m_strStartTime;					//��ʼʱ��
				std::string m_strEndTime;					//����ʱ��
				std::string m_strApplyType;						//��������
				std::string m_strApplyOrgCode;			    //�������������ű���
				std::string m_strApproveResult;						//��Ϊȫ��,0δ������1ͨ����2��ͨ��   ����
				
			public:
			    CBody():m_bSingelRequest(false)
				{
					
				}
			};

		
			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
