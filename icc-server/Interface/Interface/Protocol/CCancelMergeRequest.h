#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CCancelMergeRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");

				m_oBody.m_strMergeID = p_pJson->GetNodeValue("/body/merge_id", "");
				m_oBody.m_strMergeType = p_pJson->GetNodeValue("/body/merge_type", "");
				m_oBody.m_strUpdateUser = p_pJson->GetNodeValue("/body/update_user", "");
				m_oBody.m_strUpdateUserName = p_pJson->GetNodeValue("/body/update_user_name", "");
				m_oBody.m_strUpdateUserDeptCode = p_pJson->GetNodeValue("/body/update_user_dept_code", "");
				m_oBody.m_strUpdateUserDeptName = p_pJson->GetNodeValue("/body/update_user_dept_name", "");

				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strAlarmID;						//接警id

				std::string m_strMergeID;						//合并id
				std::string m_strMergeType;					   //合并type
				std::string m_strUpdateUser;				//修改人,取最后一次修改值
				std::string m_strUpdateUserName;
				std::string m_strUpdateUserDeptCode;
				std::string m_strUpdateUserDeptName;
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}
