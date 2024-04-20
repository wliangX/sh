#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		struct SmpNotifyData
		{
			std::string strAction;
			std::string strCallBack;
			std::string strGuid;
			std::string strCmd;
		};

		class CSmpSynNotifyDatas : public IReceive
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

				int iCount = p_pJson->GetCount("/body/dataList");
				for (int i = 0; i < iCount; i++)
				{
					SmpNotifyData notifyData;
					std::string l_strPrefixPath("/body/dataList/" + std::to_string(i) + "/");
					notifyData.strAction = p_pJson->GetNodeValue(l_strPrefixPath + "action", "");
					notifyData.strCallBack = p_pJson->GetNodeValue(l_strPrefixPath + "callback", "");
					notifyData.strGuid = p_pJson->GetNodeValue(l_strPrefixPath + "dataGuid", "");
					notifyData.strCmd = p_pJson->GetNodeValue(l_strPrefixPath + "cmd", "");
					m_vecDatas.push_back(notifyData);
				}

				return true;
			}

		public:			
			std::vector<SmpNotifyData> m_vecDatas;			
		};
	}
}
