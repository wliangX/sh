#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAdminRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				unsigned int l_uiIndex = 0;
				for (CAdmin data : m_oBody.m_vecData)
				{
					p_pJson->SetNodeValue("/body/guid", data.m_strGuid);
					p_pJson->SetNodeValue("/body/parent_guid", data.m_strParentGuid);
					p_pJson->SetNodeValue("/body/parent_guid", data.m_strBindDeptCode);
					p_pJson->SetNodeValue("/body/bind_dept_code", data.m_strBindParentDeptCode);
					p_pJson->SetNodeValue("/body/bind_parent_dept_code", data.m_strBelongDeptCode);
					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CAdmin
			{
			public:
				std::string m_strGuid;				//可管辖单位guid
				std::string m_strParentGuid;		//上级可管辖单位guid
				std::string m_strBindDeptCode;		//该节点所绑定的单位code
				std::string m_strBindParentDeptCode;//该节点上级所绑定的单位code
				std::string m_strBelongDeptCode;	//该节点所归属的单位code
			};
			class CBody
			{
			public:
				std::string m_strCount;
				std::vector<CAdmin> m_vecData;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}