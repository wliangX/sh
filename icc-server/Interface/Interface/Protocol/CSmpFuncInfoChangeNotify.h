#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CSmpHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CSMPFuncInfoChangeNotify :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_Header.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				if (m_Header.m_strAction == ACTION_ADD || m_Header.m_strAction == ACTION_UPDATE)
				{
					for (int i = 0; i < p_pJson->GetCount("/body/dataList"); i++)
					{
						std::string l_strPath = "/body/dataList/";
						std::string l_strNum = std::to_string(i);

						std::string l_strSystemNo = p_pJson->GetNodeValue(l_strPath + l_strNum + "/system_info/systemNo", "");
						if (l_strSystemNo != "ICC")
						{
							continue;
						}

						CBody::CData l_CData;
						l_CData.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/guid", "");
						l_CData.m_strCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/code", "");
						l_CData.m_strName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/name", "");
						l_CData.m_strParentCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/parent_code", "");
						l_CData.m_strParentName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/parent_name", "");
						l_CData.m_strIsAvailabe = p_pJson->GetNodeValue(l_strPath + l_strNum + "/isavailable", "");
						l_CData.m_strRemark = p_pJson->GetNodeValue(l_strPath + l_strNum + "/remark", "");
						l_CData.m_strFuncType = p_pJson->GetNodeValue(l_strPath + l_strNum + "/func_type", "");
						l_CData.m_strFuncMenuType = p_pJson->GetNodeValue(l_strPath + l_strNum + "/func_menu_type", "");
						l_CData.m_strSort= p_pJson->GetNodeValue(l_strPath + l_strNum + "/sort", "");
						l_CData.m_strCreateTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/create_time", "");
						l_CData.m_strCreateUser = p_pJson->GetNodeValue(l_strPath + l_strNum + "/create_user", "");
						m_Body.m_vecData.push_back(l_CData);					
					}
					return true;
				}			

				if (m_Header.m_strAction == ACTION_DELETE)
				{
					for (int i = 0; i < p_pJson->GetCount("/body/dataList"); i++)
					{
						std::string l_strPath = "/body/dataList/";
						std::string l_strNum = std::to_string(i);
						CBody::CData l_CData;
						l_CData.m_strCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/code", "");
						m_Body.m_vecData.push_back(l_CData);
					}
					return true;
				}
				return true;
			}
			public:
				class CBody
				{
				public:
					class CData
					{
					public:
						std::string m_strGuid;
						std::string m_strCode;
						std::string m_strName;
						std::string m_strParentCode;
						std::string m_strParentName;						
						std::string m_strIsAvailabe;
						std::string m_strFuncType;
						std::string m_strFuncMenuType;
						std::string m_strRemark;
						std::string m_strSort;
						std::string m_strCreateTime;
						std::string m_strCreateUser;
						std::string m_strUpdateTime;
						std::string m_strUpdateUser;
					};
					std::vector<CData>m_vecData;
				};

				CSmpHeader m_Header;
				CBody m_Body;
			};
		}
	}
