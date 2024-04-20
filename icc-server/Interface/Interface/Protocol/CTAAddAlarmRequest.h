#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAAddAlarmRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strType = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_strAddress = p_pJson->GetNodeValue("/body/address", "");
				m_oBody.m_strCrcontent = p_pJson->GetNodeValue("/body/content", "");
				m_oBody.m_strIsPhone = p_pJson->GetNodeValue("/body/is_phone", "");
				m_oBody.m_strCreateDate = p_pJson->GetNodeValue("/body/create_date", "");
				m_oBody.m_strUserCode = p_pJson->GetNodeValue("/body/user_code", "");
				m_oBody.m_strUserName = p_pJson->GetNodeValue("/body/user_name", "");
				m_oBody.m_strUserPhone = p_pJson->GetNodeValue("/body/user_phone", "");
				m_oBody.m_strUserAddress = p_pJson->GetNodeValue("/body/user_address", "");
				m_oBody.m_strUnitContactor = p_pJson->GetNodeValue("/body/unit_contactor", "");
				m_oBody.m_strHandPhone = p_pJson->GetNodeValue("/body/hand_phone", "");
				m_oBody.m_strToOrgID = p_pJson->GetNodeValue("/body/to_org_id", "");
				m_oBody.m_strToOrgName = p_pJson->GetNodeValue("/body/to_org_name", "");
				m_oBody.m_strToUserID = p_pJson->GetNodeValue("/body/to_user_id", "");
				m_oBody.m_strToUserName = p_pJson->GetNodeValue("/body/to_user_name", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strComNo = p_pJson->GetNodeValue("/body/com_no", "");
				m_oBody.m_strOpenID = p_pJson->GetNodeValue("/body/open_id", "");
				m_oBody.m_strLong = p_pJson->GetNodeValue("/body/long", "");
				m_oBody.m_strLat = p_pJson->GetNodeValue("/body/lat", "");
				m_oBody.m_strRemark1 = p_pJson->GetNodeValue("/body/remark1", "");
				m_oBody.m_strRemark2 = p_pJson->GetNodeValue("/body/remark2", "");
				//2022-02-14�޸Ķ��ű������ֵ�����
				if (m_oBody.m_strType != "06")//DIC001003
				{
					int l_uiCount = p_pJson->GetCount("/body/material_list");
					for (int i = 0; i < l_uiCount; ++i)
					{
						std::string l_strfixPath = "/body/material_list/" + std::to_string(i) + "/";
						CMaterial l_oMaterial;
						l_oMaterial.m_strFGuid = p_pJson->GetNodeValue(l_strfixPath + "f_guid", "");
						l_oMaterial.m_strFName = p_pJson->GetNodeValue(l_strfixPath + "f_name", "");
						l_oMaterial.m_strAttachType = p_pJson->GetNodeValue(l_strfixPath + "attach_type", "");
						l_oMaterial.m_strAttachPath = p_pJson->GetNodeValue(l_strfixPath + "attach_path", "");

						m_oBody.m_vecMaterial.push_back(l_oMaterial);
					}
				}				

				return true;
			}

		public:
			class CMaterial
			{
			public:
				std::string		m_strFGuid;					//����ID
				std::string		m_strFName;					//��������
				std::string		m_strAttachType;			//�������� 0:ͼƬ��1��Ƶ��2����
				std::string		m_strAttachPath;			//����·��
			};

			class CBody
			{
			public:
				std::string		m_strGuid;					//��������
				std::string		m_strType;					//0δ֪��1�о�΢�ţ�2ʡ��΢�ţ�3����
				std::string		m_strAddress;				//�·���ַ
				std::string		m_strCrcontent;				//��������
				std::string   	m_strIsPhone;				//�Ƿ񷽱�����绰
				std::string		m_strCreateDate;			//����ʱ��
				std::string		m_strUserCode;				//���������֤����
				std::string		m_strUserName;				//����������
				std::string		m_strUserPhone;				//�������ֻ�
				std::string		m_strUserAddress;			//��������ϵ��ַ
				std::string		m_strUnitContactor;			//��ϵ��
				std::string		m_strHandPhone;				//��ϵ�绰
				std::string		m_strToOrgID;				//���ܲ���ID
				std::string		m_strToOrgName;				//���ܲ�������
				std::string		m_strToUserID;				//�����˾���
				std::string		m_strToUserName;			//����������
				std::string		m_strState;					//״̬��0δ����1�Ѷ���2�Ѵ���
				std::string		m_strComNo;					//�������
				std::string		m_strOpenID;				//�������ڵı��(΢�Ž���������)
				std::string		m_strLong;					//ά��
				std::string		m_strLat;					//����
				std::string		m_strNotReadChangeCount;	//δ��������Ϣ����
				std::string		m_strRemark1;				//����1
				std::string		m_strRemark2;				//����2
				
				std::vector<CMaterial>		m_vecMaterial;

			};

			CHeaderEx m_oHeader;
			CBody m_oBody;
		};		
	}
}
