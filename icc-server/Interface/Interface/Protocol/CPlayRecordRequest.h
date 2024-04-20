#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CReceivePlayRecordRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strRadioBackUp = p_pJson->GetNodeValue("/radio_backup","");
				int iCount = p_pJson->GetCount("/trafficIdList");
				for (int i = 0; i < iCount; i++)
				{
					std::string strTrafficId= p_pJson->GetNodeValue("/trafficIdList/" + std::to_string(i),"");
					m_oBody.m_vecTrafficId.push_back(strTrafficId);
				}

				return true;
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strRadioBackUp;
				std::vector<std::string> m_vecTrafficId;
			};
			CBody m_oBody;
		};
		class CDownloadRecordRequest :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				int iCount = m_oBody.m_vecTrafficId.size();
				for (int i = 0; i < iCount; ++i)
				{
					p_pJson->SetNodeValue("/trafficIdList/"+ std::to_string(i), m_oBody.m_vecTrafficId.at(i));
				}
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::vector<std::string> m_vecTrafficId;
			};
			CBody m_oBody;
		};
	}
}
