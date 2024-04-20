#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
    namespace PROTOCOL
    {
		class CSearchAlarmVisitStatisticRequest :
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
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");

                return true;
            }

        public:
            class CBody
            {
            public:
				std::string m_strPageSize;			//ÿҳ����
				std::string m_strPageIndex;			//ҳ�룬1��ʾ��һҳ
				std::string m_strBeginTime;			//��ʼʱ��
				std::string m_strEndTime;			//����ʱ��
				std::string m_strDeptCode;			//���������λ����
            };
            CHeaderEx m_oHeader;
            CBody m_oBody;
        };
    }
}