#pragma once
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetBookPhoneRequest :
			public IRequest,public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");
				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strOrgGuid = p_pJson->GetNodeValue("/body/org_guid", "");
				m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
				m_oBody.m_strName = p_pJson->GetNodeValue("/body/name", "");
				m_oBody.m_strSex = p_pJson->GetNodeValue("/body/sex", "");
				m_oBody.m_strMobile = p_pJson->GetNodeValue("/body/mobile", "");
				m_oBody.m_strPhone = p_pJson->GetNodeValue("/body/phone", "");
				m_oBody.m_strRemark = p_pJson->GetNodeValue("/body/remark", "");
				m_oBody.m_strSort = p_pJson->GetNodeValue("/body/sort", "");
				
				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/org_guid", m_oBody.m_strOrgGuid);
				p_pJson->SetNodeValue("/body/code", m_oBody.m_strCode);
				p_pJson->SetNodeValue("/body/name", m_oBody.m_strName);
				p_pJson->SetNodeValue("/body/sex", m_oBody.m_strSex);
				p_pJson->SetNodeValue("/body/mobile", m_oBody.m_strMobile);
				p_pJson->SetNodeValue("/body/phone", m_oBody.m_strPhone);
				p_pJson->SetNodeValue("/body/remark", m_oBody.m_strRemark);
				p_pJson->SetNodeValue("/body/sort", m_oBody.m_strSort);
				return p_pJson->ToString();
			}
		public:
			class CBody
			{
			public:
				std::string m_strSyncType;	//1����ʾ��ӣ�2����ʾ����, 3:��ʾɾ��
				std::string m_strGuid;			//��Աguid
				std::string m_strOrgGuid;		//��Ա���ڵ�λguid
				std::string m_strCode;			//��Ա����
				std::string m_strName;			//��Ա����
				std::string m_strSex;			//��Ա�Ա�
				std::string m_strMobile;		//��Ա�ֻ�
				std::string m_strPhone;			//��Ա����     
				std::string m_strSort;			//��Ա��ţ�һ���1��ʼ����
				std::string m_strRemark;
			};
			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};
		
		typedef CSetBookPhoneRequest CBookPhoneInfo;
	}
}
