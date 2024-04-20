#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CRoleSync :
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

				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/parent_guid", m_oBody.m_strParentGuid);
				p_pJson->SetNodeValue("/body/code", m_oBody.m_strCode);
				p_pJson->SetNodeValue("/body/name", m_oBody.m_strName);
				p_pJson->SetNodeValue("/body/sort", m_oBody.m_strSort);
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strGuid;
				std::string m_strParentGuid;
				std::string m_strCode;
				std::string m_strName;
				std::string m_strSort;
				std::string m_strSyncType;
			};
			CBody m_oBody;
		};
	}
}
