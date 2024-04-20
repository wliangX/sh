#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CSmpSynDataDef.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmpSynDataFuncNofify : public IReceive
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
				m_dataFunc.m_strGuid = p_pJson->GetNodeValue(l_strPrefixPath + "guid", "");
				m_dataFunc.m_strFunctionCode = p_pJson->GetNodeValue(l_strPrefixPath + "functionCode", "");
				m_dataFunc.m_strSystemCode = p_pJson->GetNodeValue(l_strPrefixPath + "systemCode", "");
				m_dataFunc.m_strFunctionParentGuid = p_pJson->GetNodeValue(l_strPrefixPath + "functionParentGuid", "");
				m_dataFunc.m_strFunctionName = p_pJson->GetNodeValue(l_strPrefixPath + "functionName", "");
				m_dataFunc.m_strFunctionMenuType = p_pJson->GetNodeValue(l_strPrefixPath + "functionMenuType", "");
				m_dataFunc.m_strFunctionSourceUri = p_pJson->GetNodeValue(l_strPrefixPath + "functionSourceUri", "");
				m_dataFunc.m_strFunctionSourceUriMethod = p_pJson->GetNodeValue(l_strPrefixPath + "functionSourceUriMethod", "");
				m_dataFunc.m_strIcon = p_pJson->GetNodeValue(l_strPrefixPath + "icon", "");
				m_dataFunc.m_strStatus = p_pJson->GetNodeValue(l_strPrefixPath + "status", "");
				m_dataFunc.m_strRemark = p_pJson->GetNodeValue(l_strPrefixPath + "remark", "");
				m_dataFunc.m_strSort = p_pJson->GetNodeValue(l_strPrefixPath + "sort", "");
				m_dataFunc.m_strEnableFlag = p_pJson->GetNodeValue(l_strPrefixPath + "enableFlag", "");
				m_dataFunc.m_strVersion = p_pJson->GetNodeValue(l_strPrefixPath + "version", "");
				m_dataFunc.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "createUser", "");
				m_dataFunc.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "updateUser", "");
				m_dataFunc.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "createTime", "");
				m_dataFunc.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "updateTime", "");
				m_dataFunc.m_strSynVersion = p_pJson->GetNodeValue(l_strPrefixPath + "syncVersion", "");
									
				
				return true;
			}

		public:			
			SmpDataFunc m_dataFunc;
			std::string m_strCode;
			std::string m_strMessage;
		};
	}
}
