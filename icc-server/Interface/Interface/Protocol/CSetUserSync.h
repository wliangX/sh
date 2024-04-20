#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetUserSync :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strUserGuid);
				p_pJson->SetNodeValue("/body/code", m_oBody.m_strUserCode);
				p_pJson->SetNodeValue("/body/name", m_oBody.m_strUserName);
				p_pJson->SetNodeValue("/body/pwd", m_oBody.m_strUserPwd);
				p_pJson->SetNodeValue("/body/remark", m_oBody.m_strRemark);
				p_pJson->SetNodeValue("/body/delete", m_oBody.m_strDelete);
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strUserGuid;
				std::string m_strUserCode;
				std::string m_strUserName;
				std::string m_strUserPwd;
                std::string m_strRemark;
				std::string m_strDelete;
				std::string m_strSyncType;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
