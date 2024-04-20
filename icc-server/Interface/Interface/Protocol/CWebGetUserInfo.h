#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebGetUserInfo :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strGuid = p_pJson->GetNodeValue("/guid", "");
				m_oBody.m_strCode = p_pJson->GetNodeValue("/code", "");
				m_oBody.m_strName = p_pJson->GetNodeValue("/name", "");
				m_oBody.m_strPwd = p_pJson->GetNodeValue("/pwd", "");
				m_oBody.m_strRemark = p_pJson->GetNodeValue("/remark", "");
				m_oBody.m_strIsDelete = p_pJson->GetNodeValue("/is_delete", "");
				m_oBody.m_strLevel = p_pJson->GetNodeValue("/level", "");
				m_oBody.m_strSort = p_pJson->GetNodeValue("/sort", "");
				m_oBody.m_strShortCut = p_pJson->GetNodeValue("/shortcut", "");
				m_oBody.m_strCreateUser = p_pJson->GetNodeValue("/create_user", "");
				m_oBody.m_strCreateTime = p_pJson->GetNodeValue("/create_time", "");
				m_oBody.m_strUpdateUser = p_pJson->GetNodeValue("/update_user", "");
				m_oBody.m_strUpdateTime = p_pJson->GetNodeValue("/update_time", "");
				
				return true;
			}

			class CBody
			{
			public:
				std::string m_strGuid;
				std::string m_strCode;
				std::string m_strName;
				std::string m_strPwd;
				std::string m_strIsDelete;
				std::string m_strRemark;
				std::string m_strLevel;
				std::string m_strSort;
				std::string m_strShortCut;
				std::string m_strCreateUser;
				std::string m_strCreateTime;
				std::string m_strUpdateUser;
				std::string m_strUpdateTime;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};	
	}
}
