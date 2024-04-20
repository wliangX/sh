#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CUrgesRequest :
			public IReceive, public ISend
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strAlarmId = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strProcessId = p_pJson->GetNodeValue("/body/process_id", "");
				m_oBody.m_strDispatchDeptCode = p_pJson->GetNodeValue("/body/dispatch_dept_code", "");
				m_oBody.m_strDispatchDeptName = p_pJson->GetNodeValue("/body/dispatch_dept_name", "");
				m_oBody.m_strProcessDeptCode = p_pJson->GetNodeValue("/body/process_dept_code", "");
				m_oBody.m_strProcessDeptName = p_pJson->GetNodeValue("/body/process_dept_name", "");
				m_oBody.m_strContent = p_pJson->GetNodeValue("/body/content", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (p_pJson == nullptr)
				{
					return "";
				}
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_strAlarmId);
				p_pJson->SetNodeValue("/body/process_id", m_oBody.m_strProcessId);
				p_pJson->SetNodeValue("/body/dispatch_dept_code", m_oBody.m_strDispatchDeptCode);
				p_pJson->SetNodeValue("/body/dispatch_dept_name", m_oBody.m_strDispatchDeptName);
				p_pJson->SetNodeValue("/body/process_dept_code", m_oBody.m_strProcessDeptCode);
				p_pJson->SetNodeValue("/body/process_dept_name", m_oBody.m_strProcessDeptName);
				p_pJson->SetNodeValue("/body/content", m_oBody.m_strContent);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strAlarmId;				//����ID
				std::string m_strProcessId;				//����ID
				std::string m_strDispatchDeptCode;		//���ɲ��� ID
				std::string m_strDispatchDeptName;		//���ɲ�������
				std::string m_strProcessDeptCode;		//��������ID
				std::string m_strProcessDeptName;		//������������
				std::string m_strContent;				//�ߴ�����
			};

			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
