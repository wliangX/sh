#pragma once
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{		
		class CGetBookOrgRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				unsigned int l_uiIndex = 0;
				for (COrg l_oDeptData : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex)+"/");
					p_pJson->SetNodeValue(l_strPrefixPath + "guid", l_oDeptData.m_strGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "parent_guid", l_oDeptData.m_strParentGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "code", l_oDeptData.m_strCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "name", l_oDeptData.m_strName);
					p_pJson->SetNodeValue(l_strPrefixPath + "phone", l_oDeptData.m_strPhone);
					p_pJson->SetNodeValue(l_strPrefixPath + "remark", l_oDeptData.m_strRemark);
					p_pJson->SetNodeValue(l_strPrefixPath + "sort", l_oDeptData.m_strSort);
					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class COrg
			{
			public:
				std::string m_strGuid;			//��λguid
				std::string m_strParentGuid;	//�ϼ���λguid
				std::string m_strCode;			//��λ����
				std::string m_strName;			//��λ����
				std::string m_strPhone;			//��λ����
				std::string m_strRemark;		//
				std::string m_strSort;			//��λ��ţ�һ���1��ʼ����	
			};
			class CBody
			{
			public:
				std::string m_strResult;
				std::string m_strCount;
				std::vector<COrg> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}