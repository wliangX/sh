#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetBindInfo :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strType = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_strFromGuid = p_pJson->GetNodeValue("/body/from_guid", "");
				m_oBody.m_strToGuid = p_pJson->GetNodeValue("/body/to_guid", "");
				m_oBody.m_strSort = p_pJson->GetNodeValue("/body/sort", "");

				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				p_pJson->SetNodeValue("/body/from_guid", m_oBody.m_strFromGuid);
				p_pJson->SetNodeValue("/body/to_guid", m_oBody.m_strToGuid);
				p_pJson->SetNodeValue("/body/sort", m_oBody.m_strSort);
                p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);

				return p_pJson->ToString();
			}

			virtual std::string ToStringResponse(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				return p_pJson->ToString();
			}
		public:
			CHeader m_oHeader;
			class CBindInfo
			{
			public:
				std::string m_strGuid;
				std::string m_strType;
				std::string m_strFromGuid;
				std::string m_strToGuid;
				std::string m_strFlag;
				std::string m_strLevel;
				std::string m_strSort;
				std::string m_strShortcut;
				std::string m_strCreateUser;
				std::string m_strCreateTime;
				std::string m_strUpdateUser;
				std::string m_strUpdatetime;
				std::string m_strResult;
                std::string m_strSyncType;
			};
			CBindInfo m_oBody;
		};
	}

}
