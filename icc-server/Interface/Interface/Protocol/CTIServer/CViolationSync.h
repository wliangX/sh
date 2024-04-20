#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CViolationSync :
			public IReceive, public ISend
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}


				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGUID);
				p_pJson->SetNodeValue("/body/create_time", m_oBody.m_strCreateTime);
				p_pJson->SetNodeValue("/body/violation_time", m_oBody.m_strViolationTime);
				p_pJson->SetNodeValue("/body/timeout_value", m_oBody.m_strTimeOutValue);
				p_pJson->SetNodeValue("/body/violation_type", m_oBody.m_strVoilationType);
				p_pJson->SetNodeValue("/body/violation_target", m_oBody.m_strViolationTarget);
				p_pJson->SetNodeValue("/body/violation_user", m_oBody.m_strViolationUser);
				p_pJson->SetNodeValue("/body/dept_code", m_oBody.m_strDeptCode);
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strGUID;
				std::string m_strCreateTime;
				std::string m_strTimeOutValue;
				std::string m_strVoilationType;
				std::string m_strViolationTarget;
				std::string m_strViolationTime;
				std::string m_strViolationID;
				std::string m_strViolationUser;
				std::string m_strSyncType;
				std::string m_strDeptCode;
			};

			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}