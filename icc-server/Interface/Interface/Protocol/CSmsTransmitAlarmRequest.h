#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmsTransmitAlarmRequest :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				p_pJson->SetNodeValue("/body/address", m_oBody.m_strAddress);
				p_pJson->SetNodeValue("/body/content", m_oBody.m_strCrcontent);
				p_pJson->SetNodeValue("/body/is_phone", m_oBody.m_strIsPhone);
				p_pJson->SetNodeValue("/body/create_date", m_oBody.m_strCreateDate);
				p_pJson->SetNodeValue("/body/user_code", m_oBody.m_strUserCode);
				p_pJson->SetNodeValue("/body/user_name", m_oBody.m_strUserName);
				p_pJson->SetNodeValue("/body/user_phone", m_oBody.m_strUserPhone);
				p_pJson->SetNodeValue("/body/user_address", m_oBody.m_strUserAddress);
				p_pJson->SetNodeValue("/body/unit_contactor", m_oBody.m_strUnitContactor);
				p_pJson->SetNodeValue("/body/hand_phone", m_oBody.m_strHandPhone);
				p_pJson->SetNodeValue("/body/to_org_id", m_oBody.m_strToOrgID);
				p_pJson->SetNodeValue("/body/to_org_name", m_oBody.m_strToOrgName);
				p_pJson->SetNodeValue("/body/to_user_id", m_oBody.m_strToUserID);
				p_pJson->SetNodeValue("/body/to_user_name", m_oBody.m_strToUserName);
				p_pJson->SetNodeValue("/body/state", m_oBody.m_strState);
				p_pJson->SetNodeValue("/body/com_no", m_oBody.m_strComNo);
				p_pJson->SetNodeValue("/body/open_id", m_oBody.m_strOpenID);
				p_pJson->SetNodeValue("/body/long", m_oBody.m_strLong);
				p_pJson->SetNodeValue("/body/lat", m_oBody.m_strLat);
				p_pJson->SetNodeValue("/body/remark1", m_oBody.m_strRemark1);
				p_pJson->SetNodeValue("/body/remark2", m_oBody.m_strRemark2);

				return p_pJson->ToString();
			}

		public:
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
				std::string		m_strRemark1;				//����1
				std::string		m_strRemark2;				//����2
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};		
	}
}
