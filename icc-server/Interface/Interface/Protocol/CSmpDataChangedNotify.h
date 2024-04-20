#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		struct SmpChangedData
		{		
			std::string m_strCallback;
			std::string m_strAction;
			std::string m_strCmd;
			std::string m_strDataGuid;
		};

		class CSmpDataChangedNotify : public IReceive
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
					SmpChangedData data;
					std::string l_strPrefixPath("/body/dataList/" + std::to_string(i) + "/");
					data.m_strCallback = p_pJson->GetNodeValue(l_strPrefixPath + "callback", "");
					data.m_strAction = p_pJson->GetNodeValue(l_strPrefixPath + "action", "");
					data.m_strCmd = p_pJson->GetNodeValue(l_strPrefixPath + "cmd", "");
					data.m_strDataGuid = p_pJson->GetNodeValue(l_strPrefixPath + "dataGuid", "");
					m_vecChangedDatas.push_back(data);
				}
				
				
				return true;
			}

		public:
			std::vector<SmpChangedData> m_vecChangedDatas;
		};
	}
}
