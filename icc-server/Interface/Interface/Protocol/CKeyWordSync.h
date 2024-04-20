#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>
#include <Protocol/CKeyWordInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CKeyWordSync :
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

				
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_oKeyWordInfo.m_strGuid);
				
				if (!m_oBody.m_oKeyWordInfo.m_strKeyWord.empty())
				{
					p_pJson->SetNodeValue("/body/key_word", m_oBody.m_oKeyWordInfo.m_strKeyWord);
				}

				if (!m_oBody.m_oKeyWordInfo.m_strContent.empty())
				{
					p_pJson->SetNodeValue("/body/content", m_oBody.m_oKeyWordInfo.m_strContent);
				}
				
				
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
				CKeyWordInfo m_oKeyWordInfo;
				std::string m_strSyncType;
			};
			CBody m_oBody;
		};
	}
}
