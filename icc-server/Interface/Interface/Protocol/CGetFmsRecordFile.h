#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetFmsRecordFile :
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
				m_oBody.m_strCode = p_pJson->GetNodeValue("/code", "");
				m_oBody.m_strMsg = p_pJson->GetNodeValue("/message", "");
				m_oBody.l_oCData.m_strHttpUrl = p_pJson->GetNodeValue("/data/httpUrl", "");
				m_oBody.l_oCData.m_strHttpsUrl = p_pJson->GetNodeValue("/data/httpsUrl", "");
				m_oBody.l_oCData.m_strFileName = p_pJson->GetNodeValue("/data/fileName", "");
				m_oBody.l_oCData.m_strLength = p_pJson->GetNodeValue("/data/length", "");

				return true;
			}

		public:
			CHeaderEx m_oHeader;
			class CData {
			public:
				std::string m_strHttpUrl;
				std::string m_strHttpsUrl;
				std::string m_strFileName;
				std::string m_strLength;
			};
			class CBody
			{
			public:
				std::string m_strCode;
				std::string m_strMsg;
				CData l_oCData;
			};
			CBody m_oBody;
		};
	}
}
