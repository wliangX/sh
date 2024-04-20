#pragma once

namespace ICC
{
	namespace PROTOCOL
	{
		class CMaterial
		{
		public:
			std::string		m_strFGuid;					//����ID
			std::string		m_strFName;					//��������
			std::string		m_strAttachType;			//�������� 0:ͼƬ��1��Ƶ��2����
			std::string		m_strAttachPath;			//����·��
		};

		class CThirdAlarm
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

			std::vector<PROTOCOL::CMaterial> m_vecMaterial;
		};
	}
}
