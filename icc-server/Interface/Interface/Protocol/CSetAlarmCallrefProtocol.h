#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetAlarmCallrefRequest:
			public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/callref_id", "");

				m_oBody.m_strUpdateUser = p_pJson->GetNodeValue("/body/update_user", "");
				m_oBody.m_strUpdateUserName = p_pJson->GetNodeValue("/body/update_user_name", "");
				m_oBody.m_strUpdateUserDeptCode = p_pJson->GetNodeValue("/body/update_user_dept_code", "");
				m_oBody.m_strUpdateUserDeptName = p_pJson->GetNodeValue("/body/update_user_dept_name", "");
				m_oBody.m_strNeedLog = p_pJson->GetNodeValue("/body/need_log", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/id", m_oBody.m_strAlarmID);
				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallrefID);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strAlarmID;                   // 警情id
                std::string m_strCallrefID;                 // 话务id
				std::string m_strUpdateUser;				// 警员号
				std::string m_strUpdateUserName;            // 警员名称     
				std::string m_strUpdateUserDeptCode;       
				std::string m_strUpdateUserDeptName;     
				std::string m_strNeedLog;					// 是否需要些日志

			};
			CBody m_oBody;
		};

		typedef CSetAlarmCallrefRequest CSetAlarmCallrefSync;
	}
}
