#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetDispatchRequest :
			public IRequest, public IRespond
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
				m_oBody.m_strDistrictCode = p_pJson->GetNodeValue("/body/district_code", "");
				m_oBody.m_strType = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_strName = p_pJson->GetNodeValue("/body/name", "");
				m_oBody.m_strPhone = p_pJson->GetNodeValue("/body/phone", "");
				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				std::string l_strPrefixPath("/body/");
				p_pJson->SetNodeValue(l_strPrefixPath + "guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue(l_strPrefixPath + "parent_guid", m_oBody.m_strParentGuid);
				p_pJson->SetNodeValue(l_strPrefixPath + "code", m_oBody.m_strCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "district_code", m_oBody.m_strDistrictCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "type", m_oBody.m_strType);
				p_pJson->SetNodeValue(l_strPrefixPath + "name", m_oBody.m_strName);
				p_pJson->SetNodeValue(l_strPrefixPath + "phone", m_oBody.m_strPhone);
				return p_pJson->ToString();
			}
		public:
			class CBody
			{
			public:
				std::string m_strGuid;			//�ɵ��ɵ�λguid
				std::string m_strParentGuid;	//�ϼ��ɵ��ɵ�λguid
				std::string m_strCode;			//ԭ��λ����
				std::string m_strDistrictCode;	//ԭ��λ������������
				std::string m_strType;			//ԭ��λ����
				std::string m_strName;			//ԭ��λ����
				std::string m_strPhone;			//ԭ��λ����
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	
		typedef CSetDispatchRequest CDispatchSync;
	}
}
