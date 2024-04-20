#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAuthSetRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strUser_Code = p_pJson->GetNodeValue("/body/code", "");
				m_oBody.m_strUser_Pwd = p_pJson->GetNodeValue("/body/pwd", "");
				m_oBody.m_strUser_Guid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strUser_Name = p_pJson->GetNodeValue("/body/name", "");
                m_oBody.m_strRemark = p_pJson->GetNodeValue("/body/remark", "");
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");
				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strUser_Code;
				std::string m_strUser_Pwd;
				std::string m_strUser_Guid;
				std::string m_strUser_Name;
                std::string m_strRemark;
				std::string m_strSyncType;
			};
			CBody m_oBody;
		};
	}
}
