#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CSmpSynDataDef.h>

#define DICT_VALUE_JSON R"(,"dictValueJson)"

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmpSynDataDict : public IReceive
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
					SmpDataDict dict;
					std::string l_strPrefixPath("/data/" + std::to_string(i) + "/");
					dict.m_strGuid = p_pJson->GetNodeValue(l_strPrefixPath + "guid", "");
					dict.m_strDictKey = p_pJson->GetNodeValue(l_strPrefixPath + "dictKey", "");
					dict.m_strParentDictKey = p_pJson->GetNodeValue(l_strPrefixPath + "parentDictKey", "");
					dict.m_strValue = p_pJson->GetNodeValue(l_strPrefixPath + "dictValue", "");
					dict.m_strLanguage = p_pJson->GetNodeValue(l_strPrefixPath + "languageCode", "");
					dict.m_strSystemCode = p_pJson->GetNodeValue(l_strPrefixPath + "systemCode", "");
					dict.m_strEnableFlag = p_pJson->GetNodeValue(l_strPrefixPath + "enableFlag", ""); 
					dict.m_strVersion = p_pJson->GetNodeValue(l_strPrefixPath + "version", "");
					dict.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "createUser", "");
					dict.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "updateUser", "");
					dict.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "createTime", "");
					dict.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "updateTime", "");
					dict.m_strDictCode = p_pJson->GetNodeValue(l_strPrefixPath + "dictCode", "");
					dict.m_strLevel = p_pJson->GetNodeValue(l_strPrefixPath + "level", "");
					dict.m_strSort = p_pJson->GetNodeValue(l_strPrefixPath + "sort", "");
					dict.m_strDictType = p_pJson->GetNodeValue(l_strPrefixPath + "dictType", "");
					dict.m_strDescribe = p_pJson->GetNodeValue(l_strPrefixPath + "describe", "");
					dict.m_strSynVersion = p_pJson->GetNodeValue(l_strPrefixPath + "syncVersion", "");

									
					m_vecDicts.push_back(dict);
				}
				return true;
			}
			bool ParseStringEx(std::string p_strReq, JsonParser::IJsonPtr p_pJson, JsonParser::IJsonPtr p_pJsonEx)
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
					std::string l_strCurDataIndex = p_pJson->GetNodeValue("/data/" + std::to_string(i), "");

					int l_curCount = p_pJson->GetCount("/data/" + std::to_string(i));

					if (l_curCount >= 2)
					{
						for (int j = 0; j < l_curCount; j++)
						{
							SmpDataDict dict;
							std::string l_strCurDataIndexEx = p_pJson->GetNodeValue("/data/" + std::to_string(i) + "/" + std::to_string(j), "");

							std::string l_strCurDataEx = StandardString(l_strCurDataIndexEx, dict.m_strDictValueJson);

							if (!p_pJsonEx->LoadJson(l_strCurDataEx))
							{
								return false;
							}

							dict.m_strGuid = p_pJsonEx->GetNodeValue("/guid", "");
							dict.m_strDictKey = p_pJsonEx->GetNodeValue("/dictKey", "");
							dict.m_strParentDictKey = p_pJsonEx->GetNodeValue("/parentDictKey", "");
							dict.m_strValue = p_pJsonEx->GetNodeValue("/dictValue", "");
							dict.m_strLanguage = p_pJsonEx->GetNodeValue("/languageCode", "");
							dict.m_strSystemCode = p_pJsonEx->GetNodeValue("/systemCode", "");
							dict.m_strEnableFlag = p_pJsonEx->GetNodeValue("/enableFlag", "");
							dict.m_strVersion = p_pJsonEx->GetNodeValue("/version", "");
							dict.m_strCreateUser = p_pJsonEx->GetNodeValue("/createUser", "");
							dict.m_strUpdateUser = p_pJsonEx->GetNodeValue("/updateUser", "");
							dict.m_strCreateTime = p_pJsonEx->GetNodeValue("/createTime", "");
							dict.m_strUpdateTime = p_pJsonEx->GetNodeValue("/updateTime", "");
							dict.m_strDictCode = p_pJsonEx->GetNodeValue("/dictCode", "");
							dict.m_strLevel = p_pJsonEx->GetNodeValue("/level", "");
							dict.m_strSort = p_pJsonEx->GetNodeValue("/sort", "");
							dict.m_strDictType = p_pJsonEx->GetNodeValue("/dictType", "");
							dict.m_strDescribe = p_pJsonEx->GetNodeValue("/describe", "");
							dict.m_strSynVersion = p_pJsonEx->GetNodeValue("/syncVersion", "");
							dict.m_strUnionKey = p_pJsonEx->GetNodeValue("/unionKey", "");
							dict.m_strParentUnionKey = p_pJsonEx->GetNodeValue("/parentUnionKey", "");

							m_vecDicts.push_back(dict);
						}
					}
					else
					{
						SmpDataDict dict;
						std::string l_strCurData = StandardString(l_strCurDataIndex, dict.m_strDictValueJson);

						if (!p_pJsonEx->LoadJson(l_strCurData))
						{
							//m_vecDicts.push_back(dict);
							//continue;

							return false;
						}

						dict.m_strGuid = p_pJsonEx->GetNodeValue("/guid", "");
						dict.m_strDictKey = p_pJsonEx->GetNodeValue("/dictKey", "");
						dict.m_strParentDictKey = p_pJsonEx->GetNodeValue("/parentDictKey", "");
						dict.m_strValue = p_pJsonEx->GetNodeValue("/dictValue", "");
						dict.m_strLanguage = p_pJsonEx->GetNodeValue("/languageCode", "");
						dict.m_strSystemCode = p_pJsonEx->GetNodeValue("/systemCode", "");
						dict.m_strEnableFlag = p_pJsonEx->GetNodeValue("/enableFlag", "");
						dict.m_strVersion = p_pJsonEx->GetNodeValue("/version", "");
						dict.m_strCreateUser = p_pJsonEx->GetNodeValue("/createUser", "");
						dict.m_strUpdateUser = p_pJsonEx->GetNodeValue("/updateUser", "");
						dict.m_strCreateTime = p_pJsonEx->GetNodeValue("/createTime", "");
						dict.m_strUpdateTime = p_pJsonEx->GetNodeValue("/updateTime", "");
						dict.m_strDictCode = p_pJsonEx->GetNodeValue("/dictCode", "");
						dict.m_strLevel = p_pJsonEx->GetNodeValue("/level", "");
						dict.m_strSort = p_pJsonEx->GetNodeValue("/sort", "");
						dict.m_strDictType = p_pJsonEx->GetNodeValue("/dictType", "");
						dict.m_strDescribe = p_pJsonEx->GetNodeValue("/describe", "");
						dict.m_strSynVersion = p_pJsonEx->GetNodeValue("/syncVersion", "");
						dict.m_strUnionKey = p_pJsonEx->GetNodeValue("/unionKey", "");
						dict.m_strParentUnionKey = p_pJsonEx->GetNodeValue("/parentUnionKey", "");

						m_vecDicts.push_back(dict);
					}
				}
				return true;
			}

		private:
			std::string StandardString(std::string l_strData, std::string& l_strDictValueJson)
			{
				int l_findStratIndex = l_strData.find(DICT_VALUE_JSON);
				int l_findLeftIndex = l_strData.find("{", l_findStratIndex);
				int l_findRightIndex = l_strData.find("}", l_findStratIndex);

				if (l_findStratIndex >= 0 && l_findStratIndex <= l_strData.size() && l_findLeftIndex >= 0 && l_findLeftIndex <= l_strData.size() && l_findRightIndex >= 0 && l_findRightIndex <= l_strData.size())
				{
					//获取dictValueJson部分字符串
					l_strDictValueJson = l_strData.substr(l_findLeftIndex, l_findRightIndex - l_findLeftIndex + 1);
					//删除dictValueJson部分并返回

					l_strData.erase(l_findStratIndex, l_findRightIndex - l_findStratIndex + 1);

					int l_findLeftEnd = l_strData.find("[");
					if (l_findLeftEnd >= 0 && l_findLeftEnd <= l_strData.size())
					{
						l_strData.erase(l_findLeftEnd, 1);
					}
					int l_findRightEnd = l_strData.find("]");

					if (l_findRightEnd >= 0 && l_findRightEnd <= l_strData.size())
					{
						l_strData.erase(l_findRightEnd, 1);
					}
					
					return l_strData;
				}
				else
				{
					l_strDictValueJson = "";
					return "";
				}
			}

		public:			
			std::vector<SmpDataDict> m_vecDicts;
			std::string m_strCurrentPackageCount;
			std::string m_strPackageSize;
		};
	}
}
