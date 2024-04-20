#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CRecordIDToDBRequest
		{
		public:
			virtual bool ParseString(std::string l_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(l_strJson))
				{
					return false;
				}
				int l_iCount = p_pJson->GetCount("/data");
				CData l_CData;
				for (int i = 0; i < l_iCount; i++)
				{
					std::string l_strDataNum = std::to_string(i);

					l_CData.m_strFileID = p_pJson->GetNodeValue("/data/" + l_strDataNum + "/record_id", "");
					l_CData.m_strStartTime = p_pJson->GetNodeValue("/data/" + l_strDataNum + "/start_time", "");
					m_oBody.m_vecData.push_back(l_CData);
				}
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson)
				{
					return "";
				}
				unsigned int l_uiIndex = 0;
				for (CData data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/data/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "record_id", data.m_strFileID);
					p_pJson->SetNodeValue(l_strPrefixPath + "start_time", data.m_strStartTime);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				std::string m_strFileID;		//文件ID
				std::string m_strStartTime;		//录音开始时间
				std::string m_strCallRefId;		//话务ID
				std::string m_strCaller;
				std::string m_strCalled;
				std::string m_strRecordType;	//录音类型,0代表Fps，1代表Fms
			};
			class CBody
			{
			public:
				std::vector<CData> m_vecData;
			};
			CBody	m_oBody;
		};
	}
}
