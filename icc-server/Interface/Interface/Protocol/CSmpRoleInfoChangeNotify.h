#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CSmpHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CSMPRoleInfoChangeNotify :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_Header.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				if (m_Header.m_strAction == ACTION_ADD)
				{
					for (int i = 0; i < p_pJson->GetCount("/body/dataList"); i++)
					{
						std::string l_strPath = "/body/dataList/";
						std::string l_strNum = std::to_string(i);
						CBody::CData l_CData;
						l_CData.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/guid", "");
						l_CData.m_strCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/code", "");
						l_CData.m_strName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/name", "");
						l_CData.m_strRoleAttr = p_pJson->GetNodeValue(l_strPath + l_strNum + "/role_attr", "");
						l_CData.m_strEnableFlag = p_pJson->GetNodeValue(l_strPath + l_strNum + "/enable_flag", "");
						l_CData.m_IsValid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/isvalid", "");
						l_CData.m_strRemark = p_pJson->GetNodeValue(l_strPath + l_strNum + "/remark", "");
						l_CData.m_strCreateTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/create_time", "");
						l_CData.m_strCreateUser = p_pJson->GetNodeValue(l_strPath + l_strNum + "/create_user", "");
						m_Body.m_vecData.push_back(l_CData);
					}
					return true;
				}
				
				if (m_Header.m_strAction == ACTION_UPDATE)
				{
					for (int i = 0; i < p_pJson->GetCount("/body/dataList"); i++)
					{
						std::string l_strPath = "/body/dataList/";
						std::string l_strNum = std::to_string(i);
						CBody::CData l_CData;
						l_CData.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/guid", "");
						l_CData.m_strCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/code", "");
						l_CData.m_strName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/name", "");
						l_CData.m_strRoleAttr = p_pJson->GetNodeValue(l_strPath + l_strNum + "/role_attr", "");
						l_CData.m_strEnableFlag = p_pJson->GetNodeValue(l_strPath + l_strNum + "/enable_flag", "");						
						l_CData.m_strRemark = p_pJson->GetNodeValue(l_strPath + l_strNum + "/remark", "");
						l_CData.m_strUpdateTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/update_time", "");
						l_CData.m_strUpdateUser = p_pJson->GetNodeValue(l_strPath + l_strNum + "/update_user", "");
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
						l_CData.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/guid", "");
						m_Body.m_vecData.push_back(l_CData);
					}
					return true;
				}

				if (m_Header.m_strAction == ACTION_BIND)
				{
					for (int i = 0; i < p_pJson->GetCount("/body/dataList"); i++)
					{
						std::string l_strPath = "/body/dataList/";
						std::string l_strNum = std::to_string(i);
						CBody::CData l_CData;
						l_CData.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/role_guid", "");

						int l_iCount = p_pJson->GetCount(l_strPath + l_strNum + "/data_authority_array");
						while (--l_iCount>=0)
						{
							l_CData.m_vecDataAuthority.push_back(p_pJson->GetNodeValue(l_strPath + l_strNum + "/data_authority_array/" + std::to_string(l_iCount),""));
						}

						l_iCount = p_pJson->GetCount(l_strPath + l_strNum + "/system_guid_array");
						while (--l_iCount>=0)
						{
							l_CData.m_vecSystemGuid.push_back(p_pJson->GetNodeValue(l_strPath + l_strNum + "/system_guid_array/" + std::to_string(l_iCount), ""));
						}

						l_iCount = p_pJson->GetCount(l_strPath + l_strNum + "/function_guid_array");
						while (--l_iCount>=0)
						{
							l_CData.m_vecFuncGuid.push_back(p_pJson->GetNodeValue(l_strPath + l_strNum + "/function_guid_array/" + std::to_string(l_iCount), ""));
						}

						m_Body.m_vecData.push_back(l_CData);
					}
					return true;
				}
				return false;
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
						std::string m_strRoleAttr;
						std::string m_strEnableFlag;
						std::string m_IsValid;
						std::string m_strRemark;
						std::string m_strCreateTime;
						std::string m_strCreateUser;
						std::string m_strUpdateTime;
						std::string m_strUpdateUser;
						std::vector<std::string> m_vecDataAuthority;
						std::vector<std::string> m_vecSystemGuid;
						std::vector<std::string> m_vecFuncGuid;
					};
					std::vector<CData>m_vecData;
				};

				CSmpHeader m_Header;
				CBody m_Body;
			};
		}
	}
