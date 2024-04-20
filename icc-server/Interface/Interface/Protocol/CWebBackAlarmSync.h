#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebBackAlarmSync :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				
				p_pJson->SetNodeValue("/body/data/alarm_id", m_oBody.m_oData.m_strAlarmID);
				p_pJson->SetNodeValue("/body/data/process_id", m_oBody.m_oData.m_strProcessID);
				p_pJson->SetNodeValue("/body/data/dispatch_id", m_oBody.m_oData.m_strDispatchID);
				p_pJson->SetNodeValue("/body/data/dispatch_name", m_oBody.m_oData.m_strDispatchName);
				p_pJson->SetNodeValue("/body/data/back_user_id", m_oBody.m_oData.m_strBackUserID);
				p_pJson->SetNodeValue("/body/data/back_user_name", m_oBody.m_oData.m_strBackUserName);
				p_pJson->SetNodeValue("/body/data/back_dept_id", m_oBody.m_oData.m_strBackDeptID);
				p_pJson->SetNodeValue("/body/data/back_dept_name", m_oBody.m_oData.m_strBackDeptName);
				p_pJson->SetNodeValue("/body/data/reason", m_oBody.m_oData.m_strReason);

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				std::string m_strAlarmID;						//����id
				std::string m_strProcessID;						//������id
				std::string m_strDispatchID;					//������id
				std::string m_strDispatchName;					//����������
				std::string m_strBackUserID;					//�˾���id
				std::string m_strBackUserName;					//�˾�������
				std::string m_strBackDeptID;					//�˾���λid
				std::string m_strBackDeptName;					//�˾���λ����
				std::string m_strReason;						//ԭ��
			};
			class CBody
			{
			public:
				CData m_oData;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}