#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CLanguageSync :
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

                p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
                p_pJson->SetNodeValue("/body/code", m_oBody.m_strCode);
                p_pJson->SetNodeValue("/body/name", m_oBody.m_strName);
                p_pJson->SetNodeValue("/body/sort", m_oBody.m_strSort);
                p_pJson->SetNodeValue("/body/sync_typ", m_oBody.m_strSyncTyp);
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
                std::string m_strGuid;
                std::string m_strCode;
                std::string m_strName;
                std::string m_strSort;
                std::string m_strSyncTyp;
			};
			CBody m_oBody;
		};
	}
}
