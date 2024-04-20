#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTodayStatisticsByReceiveTypeRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");			
				m_oBody.m_strReceiveTypes = p_pJson->GetNodeValue("/body/receive_types", "");
				m_oBody.m_strIsRecursive = p_pJson->GetNodeValue("/body/is_recursive", "");
				return true;
			}

		public:

			class CBody
			{
			public:				
				std::string	m_strBeginTime;					//��ѯ��ʼʱ�䣨����Ϊ�գ�
				std::string	m_strEndTime;					//��ѯ����ʱ�䣨����Ϊ�գ�
				std::string	m_strDeptCode;				    //��ѯ���ű���		
				std::string m_strReceiveTypes;              //��ѯ�Ӿ�����
				std::string m_strIsRecursive;        		//0���������¼����ţ�1�������¼�����
			};

			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}