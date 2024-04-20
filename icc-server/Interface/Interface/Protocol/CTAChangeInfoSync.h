#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAChangeInfoSync
		{
		public:
			std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				p_pJson->SetNodeValue("/body/third_type", m_oBody.m_strThirdType);

				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/third_alarm_guid", m_oBody.m_strThirdAlarmGuid);
				p_pJson->SetNodeValue("/body/orientation", m_oBody.m_strOrientation);
				p_pJson->SetNodeValue("/body/state", m_oBody.m_strState);
				p_pJson->SetNodeValue("/body/phone", m_oBody.m_strPhone);
				p_pJson->SetNodeValue("/body/open_id", m_oBody.m_strOpenID);
				p_pJson->SetNodeValue("/body/change_type", m_oBody.m_strChangeType);
				p_pJson->SetNodeValue("/body/change_content", m_oBody.m_strChangeContent);
				p_pJson->SetNodeValue("/body/change_date", m_oBody.m_strChangeDate);
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
				std::string		m_strSyncType;				//1����ʾ��ӣ�2����ʾ���£�3����ʾɾ��
				std::string		m_strThirdType;					//�������ֵ�ֵ
				std::string		m_strGuid;					//����
				std::string		m_strThirdAlarmGuid;		//��������
				std::string		m_strOrientation;			//0�����˸���Ա���ͣ�1��Ա�������˷���
				std::string		m_strState;					//״̬��0δ����1�Ѷ�
				std::string   	m_strPhone;					//�ֻ�����
				std::string		m_strOpenID;				//�������ڵı��(΢�Ž���������)
				std::string		m_strChangeType;			//�������ͣ�1�����֣�2��ͼƬ��3����Ƶ��4��������
				std::string		m_strChangeContent;			//�������ݣ���������Ϊ1ʱΪ�������ݣ�2-4��Ϊ��ý���ļ�ȫ·����
				std::string		m_strChangeDate;			//����ʱ��  yyyy-mm-dd  hh24:mi:ss
				std::string		m_strLong;					//���ȣ�wgs84����ϵ��
				std::string		m_strLat;					//γ�ȣ�wgs84����ϵ��
				std::string		m_strRemark1;				//�����ֶ�1
				std::string		m_strRemark2;				//�����ֶ�2
			};
					
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
