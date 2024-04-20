#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAGetChangeInfoRespond :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				unsigned int l_uiIndex = 0;
				for (auto changeinfo : m_oBody.m_vecChangeInfo)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "guid", changeinfo.m_strGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "third_alarm_guid", changeinfo.m_strThirdAlarmGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "orientation", changeinfo.m_strOrientation);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", changeinfo.m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "phone", changeinfo.m_strPhone);
					p_pJson->SetNodeValue(l_strPrefixPath + "open_id", changeinfo.m_strOpenID);
					p_pJson->SetNodeValue(l_strPrefixPath + "change_type", changeinfo.m_strChangeType);
					p_pJson->SetNodeValue(l_strPrefixPath + "change_content", changeinfo.m_strChangeContent);
					p_pJson->SetNodeValue(l_strPrefixPath + "change_date", changeinfo.m_strChangeDate);
					p_pJson->SetNodeValue(l_strPrefixPath + "long", changeinfo.m_strLong);
					p_pJson->SetNodeValue(l_strPrefixPath + "lat", changeinfo.m_strLat);
					p_pJson->SetNodeValue(l_strPrefixPath + "remark1", changeinfo.m_strRemark1);
					p_pJson->SetNodeValue(l_strPrefixPath + "remark2", changeinfo.m_strRemark2);

					l_uiIndex++;
				}

				return p_pJson->ToString();
			}

		public:
			class CChangeInfo
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

			class CBody
			{
			public:
				std::string m_strCount;
				std::vector<CChangeInfo> m_vecChangeInfo;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}