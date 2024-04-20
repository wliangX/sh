#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebLoginRespond :
			public IReceive, public ISend
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strResult = p_pJson->GetNodeValue("/body/result", "");
				m_oBody.m_strToken = p_pJson->GetNodeValue("/body/token", "");

				m_oBody.m_oUser.m_strCode = p_pJson->GetNodeValue("/body/user/code", "");
				m_oBody.m_oUser.m_strName = p_pJson->GetNodeValue("/body/user/name", "");

				m_oBody.m_oStaff.m_strCode = p_pJson->GetNodeValue("/body/staff/code", "");
				m_oBody.m_oStaff.m_strName = p_pJson->GetNodeValue("/body/staff/name", "");
				m_oBody.m_oStaff.m_strSex = p_pJson->GetNodeValue("/body/staff/sex", "");
				m_oBody.m_oStaff.m_strType = p_pJson->GetNodeValue("/body/staff/type", "");
				m_oBody.m_oStaff.m_strPosition = p_pJson->GetNodeValue("/body/staff/position", "");
				m_oBody.m_oStaff.m_strMobile = p_pJson->GetNodeValue("/body/staff/mobile", "");
				m_oBody.m_oStaff.m_strPhone = p_pJson->GetNodeValue("/body/staff/phone", "");

				m_oBody.m_oDept.m_strCode = p_pJson->GetNodeValue("/body/dept/code", "");
				m_oBody.m_oDept.m_strName = p_pJson->GetNodeValue("/body/dept/name", "");
				m_oBody.m_oDept.m_strDistrictCode = p_pJson->GetNodeValue("/body/dept/district_code", "");
				m_oBody.m_oDept.m_strType = p_pJson->GetNodeValue("/body/dept/type", "");
				m_oBody.m_oDept.m_strPhone = p_pJson->GetNodeValue("/body/dept/phone", "");
				m_oBody.m_strServerGuid = p_pJson->GetNodeValue("/body/server_guid", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/token", m_oBody.m_strToken);

				p_pJson->SetNodeValue("/body/user/code", m_oBody.m_oUser.m_strCode);
				p_pJson->SetNodeValue("/body/user/name", m_oBody.m_oUser.m_strName);

				p_pJson->SetNodeValue("/body/staff/code", m_oBody.m_oStaff.m_strCode);
				p_pJson->SetNodeValue("/body/staff/name", m_oBody.m_oStaff.m_strName);
				p_pJson->SetNodeValue("/body/staff/sex", m_oBody.m_oStaff.m_strSex);
				p_pJson->SetNodeValue("/body/staff/type", m_oBody.m_oStaff.m_strType);
				p_pJson->SetNodeValue("/body/staff/position", m_oBody.m_oStaff.m_strPosition);
				p_pJson->SetNodeValue("/body/staff/mobile", m_oBody.m_oStaff.m_strMobile);
				p_pJson->SetNodeValue("/body/staff/phone", m_oBody.m_oStaff.m_strPhone);

				p_pJson->SetNodeValue("/body/dept/code", m_oBody.m_oDept.m_strCode);
				p_pJson->SetNodeValue("/body/dept/name", m_oBody.m_oDept.m_strName);
				p_pJson->SetNodeValue("/body/dept/district_code", m_oBody.m_oDept.m_strDistrictCode);
				p_pJson->SetNodeValue("/body/dept/type", m_oBody.m_oDept.m_strType);
				p_pJson->SetNodeValue("/body/dept/phone", m_oBody.m_oDept.m_strPhone);
				p_pJson->SetNodeValue("/body/server_guid", m_oBody.m_strServerGuid);
				
				return p_pJson->ToString();
			}

		public:
			class CUser
			{
			public:
				std::string m_strCode;				//�û�����
				std::string m_strName;				//�û�����
			};
			class CStaff
			{
			public:
				std::string m_strCode;				//��Ա����
				std::string m_strName;				//��Ա����
				std::string m_strSex;				//��Ա�Ա�
				std::string m_strType;				//��Ա����
				std::string m_strPosition;			//��Աְ��
				std::string m_strMobile;			//��Ա�ֻ�
				std::string m_strPhone;				//��Ա����
			};
			class CDept
			{
			public:
				std::string m_strCode;				//��λ����
				std::string m_strName;				//��λ����
				std::string m_strDistrictCode;		//��λ������������
				std::string m_strType;				//��λ����
				std::string m_strPhone;				//��λ����
			};
			class CBody
			{
			public:
				std::string m_strResult;			//0����ʾ�ɹ���1����ʾʧ��
				std::string m_strToken;				//��Ȩ�룬��ʽΪicc_web_user_yyyymmddhhmissmmmmmm
				std::string m_strServerGuid;        //web��̨����guid
				CUser m_oUser;
				CStaff m_oStaff;
				CDept m_oDept;

			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}