#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CVcsDeleteDisposalRequset : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				//m_oBody.m_strID = p_pJson->GetNodeValue("/body/ID", "");


				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				CVcsDeleteDisposalRequset::CData l_pData;
				int l_iCount = p_pJson->GetCount("/body/data");
				for (int i = 0; i < l_iCount; i++)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");

					l_pData.m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "id", ""); 
					l_pData.m_strUserID = p_pJson->GetNodeValue(l_strPrefixPath + "RYID", "");
					l_pData.m_strUserName = p_pJson->GetNodeValue(l_strPrefixPath + "RYXM", "");
					l_pData.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "updatetime", "");

					l_pData.m_strCancelReason = p_pJson->GetNodeValue(l_strPrefixPath + "cancel_reason", "");
					m_oBody.m_vecData.push_back(l_pData);
				}
				return true;
			}
		public:
			
			class CData
			{
			public:
				std::string m_strID;						//´¦¾¯ID
				std::string m_strUserID;
				std::string m_strUpdateTime;
				std::string m_strUserName;

				std::string m_strCancelReason;
			};
			class CBody
			{
			public:
				std::vector<CData> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
			
		};
	}
}
