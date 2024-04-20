#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>
#include <vector>
#include <map>

namespace ICC
{
	namespace PROTOCOL
	{
		class CVcsQueryCallInfoRespond : public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{				
				p_pJson->SetNodeValue("/code", m_strResultCode);
				p_pJson->SetNodeValue("/message", m_strResultMessage);

				size_t iCount = m_vecDatas.size();

				p_pJson->SetNodeValue("/data/all_count", m_strTotalCount);
				p_pJson->SetNodeValue("/data/count", iCount);

				for (size_t i = 0; i < iCount; i++)
				{
					std::string l_strValueNum = std::to_string(i);
					std::map<std::string, std::string>::const_iterator itr;
					for (itr = m_vecDatas[i].begin(); itr != m_vecDatas[i].end(); ++itr)
					{
						p_pJson->SetNodeValue("/data/list/" + l_strValueNum + "/" + itr->first, itr->second);
					}
				}
				
				return p_pJson->ToString();
			}


		public:
			std::string m_strResultCode;
			std::string m_strResultMessage;
			std::string m_strTotalCount;
			std::vector<std::map<std::string, std::string>> m_vecDatas;
		};

		class CSetCallInfoRespond : public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/code", m_strCode);
				p_pJson->SetNodeValue("/message", m_strMessage);
				p_pJson->SetNodeValue("/msg_id", m_strMesID);
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			std::string m_strCode;			// 200 ³É¹¦
			std::string m_strMessage;		// success
			std::string m_strMesID;			// 
		};

	}
}
