#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CSmpSynDataDef.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmpSynDataFunc : public IReceive
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
				
				m_strCurrentPackageCount = p_pJson->GetNodeValue("/currentPackage", "");
				m_strPackageSize = p_pJson->GetNodeValue("/step", "");

				int iCount = p_pJson->GetCount("/data");
				for (int i = 0; i < iCount; i++)
				{
					SmpDataFunc func;
					std::string l_strPrefixPath("/data/" + std::to_string(i) + "/");
					func.m_strGuid = p_pJson->GetNodeValue(l_strPrefixPath + "guid", "");
					func.m_strFunctionCode = p_pJson->GetNodeValue(l_strPrefixPath + "functionCode", "");
					func.m_strSystemCode = p_pJson->GetNodeValue(l_strPrefixPath + "systemCode", "");
					func.m_strFunctionParentGuid = p_pJson->GetNodeValue(l_strPrefixPath + "functionParentGuid", "");
					func.m_strFunctionName = p_pJson->GetNodeValue(l_strPrefixPath + "functionName", "");
					func.m_strFunctionMenuType = p_pJson->GetNodeValue(l_strPrefixPath + "functionMenuType", "");
					func.m_strFunctionSourceUri = p_pJson->GetNodeValue(l_strPrefixPath + "functionSourceUri", "");
					func.m_strFunctionSourceUriMethod = p_pJson->GetNodeValue(l_strPrefixPath + "functionSourceUriMethod", "");
					func.m_strIcon = p_pJson->GetNodeValue(l_strPrefixPath + "icon", "");
					func.m_strStatus = p_pJson->GetNodeValue(l_strPrefixPath + "status", "");
					func.m_strRemark = p_pJson->GetNodeValue(l_strPrefixPath + "remark", "");
					func.m_strSort = p_pJson->GetNodeValue(l_strPrefixPath + "sort", "");
					func.m_strEnableFlag = p_pJson->GetNodeValue(l_strPrefixPath + "enableFlag", "");
					func.m_strVersion = p_pJson->GetNodeValue(l_strPrefixPath + "version", "");
					func.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "createUser", "");
					func.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "updateUser", "");
					func.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "createTime", "");
					func.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "updateTime", "");
					func.m_strSynVersion = p_pJson->GetNodeValue(l_strPrefixPath + "syncVersion", "");
									
					m_vecFuncs.push_back(func);
				}
				return true;
			}

		public:			
			std::vector<SmpDataFunc> m_vecFuncs;
			std::string m_strCurrentPackageCount;
			std::string m_strPackageSize;
		};
	}
}
