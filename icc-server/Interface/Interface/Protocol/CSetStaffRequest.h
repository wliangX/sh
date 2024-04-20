#pragma once
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CBaseDataEntities.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetStaffRequest :
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
				m_oBody.m_strDeptGuid = p_pJson->GetNodeValue("/body/dept_guid", "");
				m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
				m_oBody.m_strName = p_pJson->GetNodeValue("/body/name", "");
				m_oBody.m_strSex = p_pJson->GetNodeValue("/body/sex", "");
				m_oBody.m_strType = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_strPosition = p_pJson->GetNodeValue("/body/position", "");
				m_oBody.m_strMobile = p_pJson->GetNodeValue("/body/mobile", "");
				m_oBody.m_strPhone = p_pJson->GetNodeValue("/body/phone", "");
				m_oBody.m_strShortcut = p_pJson->GetNodeValue("/body/shortcut", "");
				m_oBody.m_strSort = p_pJson->GetNodeValue("/body/sort", "");
				m_oBody.m_strIsLeader = p_pJson->GetNodeValue("/body/isleader", "");
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
				p_pJson->SetNodeValue("/body/dept_guid", m_oBody.m_strDeptGuid);
				p_pJson->SetNodeValue("/body/code", m_oBody.m_strCode);
				p_pJson->SetNodeValue("/body/name", m_oBody.m_strName);
				p_pJson->SetNodeValue("/body/sex", m_oBody.m_strSex);
				p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				p_pJson->SetNodeValue("/body/position", m_oBody.m_strPosition);
				p_pJson->SetNodeValue("/body/mobile", m_oBody.m_strMobile);
				p_pJson->SetNodeValue("/body/phone", m_oBody.m_strPhone);
				p_pJson->SetNodeValue("/body/shortcut", m_oBody.m_strShortcut);
				p_pJson->SetNodeValue("/body/sort", m_oBody.m_strSort);
				p_pJson->SetNodeValue("/body/isleader", m_oBody.m_strIsLeader);
				return p_pJson->ToString();
			}
		public:
			class CBody
			{
			public:
				std::string m_strSyncType;	//1����ʾ��ӣ�2����ʾ����, 3:��ʾɾ��
				std::string m_strGuid;			//��Աguid
				std::string m_strDeptGuid;		//��Ա���ڵ�λguid
				std::string m_strCode;			//��Ա����
				std::string m_strName;			//��Ա����
				std::string m_strSex;			//��Ա�Ա�
				std::string m_strType;			//��Ա����
				std::string m_strPosition;		//��Աְ��
				std::string m_strMobile;		//��Ա�ֻ�
				std::string m_strPhone;			//��Ա����     
				std::string m_strShortcut;		//��Ա��ݲ�ѯ��д
				std::string m_strSort;			//��Ա��ţ�һ���1��ʼ����
				std::string m_strIsLeader;		//�Ƿ�Ϊ�쵼,�ǣ�1����0
				//std::string m_strRemark;
			};
			CHeader m_oHeader;			
			CBody	m_oBody;
		};
		
		typedef CSetStaffRequest CStaffSync;
	}
}
