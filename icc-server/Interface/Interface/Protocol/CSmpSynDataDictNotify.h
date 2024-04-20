#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CSmpSynDataDef.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmpSynDataDictNotify : public IReceive
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
				
				m_strCode = p_pJson->GetNodeValue("/code", "");
				m_strMessage = p_pJson->GetNodeValue("/message", "");
				
				std::string l_strPrefixPath("/data/");
				m_dataDict.m_strGuid = p_pJson->GetNodeValue(l_strPrefixPath + "guid", "");
				m_dataDict.m_strDictKey = p_pJson->GetNodeValue(l_strPrefixPath + "dictKey", "");
				m_dataDict.m_strParentDictKey = p_pJson->GetNodeValue(l_strPrefixPath + "parentDictKey", "");
				m_dataDict.m_strValue = p_pJson->GetNodeValue(l_strPrefixPath + "dictValue", "");
				m_dataDict.m_strLanguage = p_pJson->GetNodeValue(l_strPrefixPath + "languageCode", "");
				m_dataDict.m_strSystemCode = p_pJson->GetNodeValue(l_strPrefixPath + "systemCode", "");
				m_dataDict.m_strEnableFlag = p_pJson->GetNodeValue(l_strPrefixPath + "enableFlag", "");
				m_dataDict.m_strVersion = p_pJson->GetNodeValue(l_strPrefixPath + "version", "");
				m_dataDict.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "createUser", "");
				m_dataDict.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "updateUser", "");
				m_dataDict.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "createTime", "");
				m_dataDict.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "updateTime", "");
				m_dataDict.m_strDictCode = p_pJson->GetNodeValue(l_strPrefixPath + "dictCode", "");
				m_dataDict.m_strLevel = p_pJson->GetNodeValue(l_strPrefixPath + "level", "");
				m_dataDict.m_strSort = p_pJson->GetNodeValue(l_strPrefixPath + "sort", "");
				m_dataDict.m_strDictType = p_pJson->GetNodeValue(l_strPrefixPath + "dictType", "");
				m_dataDict.m_strDescribe = p_pJson->GetNodeValue(l_strPrefixPath + "describe", "");
				m_dataDict.m_strSynVersion = p_pJson->GetNodeValue(l_strPrefixPath + "syncVersion", "");
				
				return true;
			}

		public:			
			SmpDataDict m_dataDict;
			std::string m_strCode;
			std::string m_strMessage;
		};
	}
}
