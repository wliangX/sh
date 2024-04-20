#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CUpdateParamSync :
			public ISync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

                p_pJson->SetNodeValue("/body/name", m_oBody.m_strName);
                p_pJson->SetNodeValue("/body/value", m_oBody.m_strValue);
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				p_pJson->SetNodeValue("/body/is_visibility", m_oBody.l_strIsVisibility);
				p_pJson->SetNodeValue("/body/is_readonly", m_oBody.l_strIsReadonly);
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strName;
                std::string m_strValue;
				std::string m_strSyncType;
				std::string l_strIsVisibility;
				std::string l_strIsReadonly;
			};
			CBody m_oBody;
		};
	}
}
