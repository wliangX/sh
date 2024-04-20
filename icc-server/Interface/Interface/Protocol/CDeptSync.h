#pragma once
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>
#include <Protocol/CBaseDataEntities.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CDeptSync :
			public IRespond
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
				m_oBody.m_strLevel = p_pJson->GetNodeValue("/body/level", "");
				m_oBody.m_strShortcut = p_pJson->GetNodeValue("/body/shortcut", "");
				m_oBody.m_strSort = p_pJson->GetNodeValue("/body/sort", "");
				m_oBody.m_strRemark = p_pJson->GetNodeValue("/body/remark", "");
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
				p_pJson->SetNodeValue("/body/district_code", m_oBody.m_strDistrictCode);
				p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				p_pJson->SetNodeValue("/body/name", m_oBody.m_strName);
				p_pJson->SetNodeValue("/body/phone", m_oBody.m_strPhone);
				p_pJson->SetNodeValue("/body/level", m_oBody.m_strLevel);
				p_pJson->SetNodeValue("/body/shortcut", m_oBody.m_strShortcut);
				p_pJson->SetNodeValue("/body/sort", m_oBody.m_strSort);
				p_pJson->SetNodeValue("/body/remark", m_oBody.m_strRemark);
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				return p_pJson->ToString();
			}

		public:		
			class CBody
			{
			public:
				std::string m_strGuid;			//��λguid
				std::string m_strParentGuid;	//�ϼ���λguid
				std::string m_strCode;			//��λ����
				std::string m_strDistrictCode;	//��λ������������
				std::string m_strType;			//��λ����
				std::string m_strName;			//��λ����
				std::string m_strPhone;			//��λ����
				std::string m_strShortcut;		//��λ��ݲ�ѯ��д
				std::string m_strSort;			//��λ��ţ�һ���1��ʼ����
				std::string m_strLevel;
				std::string m_strRemark;
				std::string m_strSyncType;
			};
			CHeader m_oHeader;			
			CBody	m_oBody;
		};
	}
}
