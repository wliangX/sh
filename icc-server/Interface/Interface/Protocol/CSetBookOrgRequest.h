#pragma once
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetBookOrgRequest :
			public IRequest,public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strParentGuid = p_pJson->GetNodeValue("/body/parent_guid", "");
				m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
				m_oBody.m_strName = p_pJson->GetNodeValue("/body/name", "");
				m_oBody.m_strPhone = p_pJson->GetNodeValue("/body/phone", "");
				m_oBody.m_strRemark = p_pJson->GetNodeValue("/body/remark", "");
				m_oBody.m_strSort = p_pJson->GetNodeValue("/body/sort", "");
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
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/parent_guid", m_oBody.m_strParentGuid);
				p_pJson->SetNodeValue("/body/code", m_oBody.m_strCode);
				p_pJson->SetNodeValue("/body/name", m_oBody.m_strName);
				p_pJson->SetNodeValue("/body/phone", m_oBody.m_strPhone);
				p_pJson->SetNodeValue("/body/remark", m_oBody.m_strRemark);
				p_pJson->SetNodeValue("/body/sort", m_oBody.m_strSort);
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strGuid;			//单位guid
				std::string m_strParentGuid;	//上级单位guid
				std::string m_strCode;			//单位编码
				std::string m_strName;			//单位名称
				std::string m_strPhone;			//单位座机
				std::string m_strRemark;		//
				std::string m_strSort;			//单位序号，一般从1开始排序	
				std::string m_strSyncType;		//1:add, 2:update, 3:delete
			};
			CHeaderEx	m_oHeader;
			CBody	m_oBody;
		};

		typedef CSetBookOrgRequest CBookOrgInfo;
	}
}