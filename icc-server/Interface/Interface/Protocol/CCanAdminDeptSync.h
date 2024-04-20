#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CCanAdminDeptSync :
			public ISync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

                p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
                p_pJson->SetNodeValue("/body/parent_guid", m_oBody.m_strParentGuid);
                p_pJson->SetNodeValue("/body/parent_guid", m_oBody.m_strBindDeptCode);
                p_pJson->SetNodeValue("/body/bind_dept_code", m_oBody.m_strBindParentDeptCode);
				p_pJson->SetNodeValue("/body/bind_parent_dept_code", m_oBody.m_strBelongDeptCode);
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
                std::string m_strGuid;				//可管辖单位guid
				std::string m_strParentGuid;		//上级可管辖单位guid
				std::string m_strBindDeptCode;		//该节点所绑定的单位code
				std::string m_strBindParentDeptCode;//该节点上级所绑定的单位code
				std::string m_strBelongDeptCode;	//该节点所归属的单位code
				std::string m_strSyncType;          //同步类型
			};
			CBody m_oBody;
		};
	}
}
