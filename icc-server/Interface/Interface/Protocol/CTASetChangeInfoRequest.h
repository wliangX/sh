#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTASetChangeInfoRequest :
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
				m_oBody.m_strThirdAlarmGuid = p_pJson->GetNodeValue("/body/third_alarm_guid", "");
				m_oBody.m_strOrientation = p_pJson->GetNodeValue("/body/orientation", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strPhone = p_pJson->GetNodeValue("/body/phone", "");
				m_oBody.m_strOpenID = p_pJson->GetNodeValue("/body/open_id", "");
				m_oBody.m_strChangeType = p_pJson->GetNodeValue("/body/change_type", "");
				m_oBody.m_strChangeContent = p_pJson->GetNodeValue("/body/change_content", "");
				m_oBody.m_strChangeDate = p_pJson->GetNodeValue("/body/change_date", "");
				m_oBody.m_strLong = p_pJson->GetNodeValue("/body/long", "");
				m_oBody.m_strLat = p_pJson->GetNodeValue("/body/lat", "");
				m_oBody.m_strRemark1 = p_pJson->GetNodeValue("/body/remark1", "");
				m_oBody.m_strRemark2 = p_pJson->GetNodeValue("/body/remark2", "");

				return true;
			}

		public:
			class CBody
			{
			public:
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
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};		
	}
}
