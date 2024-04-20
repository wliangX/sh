#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CCanAdminDept : public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_data.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_data.m_strParentGuid = p_pJson->GetNodeValue("/body/parent_guid", "");
				m_oBody.m_data.m_strBindDeptCode = p_pJson->GetNodeValue("/body/bind_dept_code", "");
				m_oBody.m_data.m_strBindParentDeptCode = p_pJson->GetNodeValue("/body/bind_parent_dept_code", "");
				m_oBody.m_data.m_BelongDeptCode = p_pJson->GetNodeValue("/body/belong_dept_code", "");
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");

				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				unsigned int l_uiIndex = 0;
				for (CAdmin data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "guid", data.m_strGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "parent_guid", data.m_strParentGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "bind_dept_code", data.m_strBindDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "bind_parent_dept_code", data.m_strBindParentDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "belong_dept_code", data.m_BelongDeptCode);
					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

			//应答
			std::string ToStringResponse(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				return p_pJson->ToString();
			}

			//同步
			std::string Sync(JsonParser::IJsonPtr p_pJson, std::string strType)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/sync_type", strType);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_data.m_strGuid);
				p_pJson->SetNodeValue("/body/parent_guid", m_oBody.m_data.m_strParentGuid);
				p_pJson->SetNodeValue("/body/bind_dept_code", m_oBody.m_data.m_strBindDeptCode);
				p_pJson->SetNodeValue("/body/bind_parent_dept_code", m_oBody.m_data.m_strBindParentDeptCode);
				p_pJson->SetNodeValue("/body/belong_dept_code", m_oBody.m_data.m_BelongDeptCode);

				return p_pJson->ToString();
			}
		public:
			class CAdmin
			{
			public:
				std::string m_strGuid;					//可调派单位guid
				std::string m_strParentGuid;			//上级可调派单位guid
				std::string m_strBindDeptCode;			//该节点所绑定的单位guid
				std::string m_strBindParentDeptCode;	//该节点上级所绑定的单位guid
				std::string m_BelongDeptCode;			//该节点所归属的单位guid
			};
			class CBody
			{
			public:
				std::string m_strCount;
				//获取所有单位下的所有可调派单位信息时有效
				std::vector<CAdmin> m_vecData;
				//设置某一单位下某一个可调派单位信息时有效
				CAdmin m_data;
				//应答结果
				std::string m_strResult;
				//同步信息 1：表示添加，2：表示更新，3：表示删除
				std::string m_strSyncType;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}
