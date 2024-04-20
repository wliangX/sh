#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CReviveAlarmRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strHandleType = p_pJson->GetNodeValue("/body/handle_type", "");
				m_oBody.m_strOperateCode = p_pJson->GetNodeValue("/body/operate_code", "");
				m_oBody.m_strOperateName = p_pJson->GetNodeValue("/body/operate_name", "");
				m_oBody.m_strOperateDeptOrgCode = p_pJson->GetNodeValue("/body/operate_dept_org_code", "");
				m_oBody.m_strOperateDeptOrgName = p_pJson->GetNodeValue("/body/operate_dept_org_name", "");
				m_oBody.m_strOperateTime = p_pJson->GetNodeValue("/body/operate_time", "");
				return true;
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strID; //id	����id
                std::string m_strHandleType;  //handle_type	��������
                std::string m_strOperateCode; //operate_code	�����û��������û�����
				std::string m_strOperateName;
				std::string m_strOperateDeptOrgCode; //operate_dept_org_code	�����û��������û�����
				std::string m_strOperateDeptOrgName;
                std::string m_strOperateTime;	//operate_time����ʱ��

			};
			CBody m_oBody;
		};
	}
}
