#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchStatisticsReceiveAlarmRequest :
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
				m_oBody.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");				;
				m_oBody.m_strStaffDeptCode = p_pJson->GetNodeValue("/body/staff_dept_code", "");
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
				std::string	m_strReceiptDeptCode;			//����Ӿ���λ���루�ݹ��ѯ��
				std::string m_strStaffDeptCode;             //�Ӿ��˲��ű��룬�ڽӾ���λΪ��ʱʹ�ã��ݹ��ѯ
			};


			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}