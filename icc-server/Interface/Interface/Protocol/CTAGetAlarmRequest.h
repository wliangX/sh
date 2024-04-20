#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAGetAlarmRequest :
			public IReceive
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
				m_oBody.m_strCurrDeptCode = p_pJson->GetNodeValue("/body/curr_dept_code", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_strKeyword = p_pJson->GetNodeValue("/body/keyword", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strPageSize;			//ÿҳ����������Ϊ�գ�
				std::string m_strPageIndex;			//ҳ�룬1��ʾ��һҳ������Ϊ�գ�
				std::string m_strBeginTime;			//��ʼʱ��
				std::string m_strEndTime;			//����ʱ��
				std::string m_strCurrDeptCode;		//���ű��루��ѯ�����Ҫ�����¼����ţ�
				std::string m_strState;				//��ֵȡȫ��
				std::string m_strKeyword;			//�ؼ�������
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};		
	}
}
