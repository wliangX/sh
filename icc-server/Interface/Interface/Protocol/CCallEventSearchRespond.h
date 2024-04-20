#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CCallEventSearchRespond :
			public IRespond
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
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
				p_pJson->SetNodeValue("/body/page_index", m_oBody.m_strPageIndex);

				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strValueNum = std::to_string(i);
					CData& data = m_oBody.m_vecData[i];
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/callref_id", data.m_strCallrefID);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/rel_alarm_id", data.m_strRelAlarmID);

					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/acd", data.m_strAcd);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/caller_id", data.m_strCallerID);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/called_id", data.m_strCalledID);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/call_direction", data.m_strCallDirection);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/call_time", data.m_strCallTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/talk_time", data.m_strTalkTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/talk_time_len", data.m_strTalkTimeLen);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/ring_time_len", data.m_strRingTimeLen);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/hangup_type", data.m_strHangupType);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/switch_type", data.m_strSwitchType);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/case_id", data.m_strCaseID);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/receipt_code", data.m_strReceiptCode);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/receipt_name", data.m_strReceiptName);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/is_callback", data.m_strIsCallBack);	
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/name", data.m_strName);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/address", data.m_strAddress);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/province", data.m_strProvince);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/city", data.m_strCity);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/operator", data.m_strOperator);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/record_id", data.m_strRecordFileId);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/release_type", data.m_strReleaseType);
				}

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				std::string m_strCallrefID;
				std::string m_strRelAlarmID;  //��������ID
				std::string m_strAcd;
				std::string m_strCallerID;
				std::string m_strCalledID;
				std::string m_strCallDirection;
				std::string m_strCallTime;
				std::string m_strTalkTime;		//ͨ��ʱ��
				std::string m_strTalkTimeLen;	//ͨ��ʱ������λ���룩
				std::string m_strRingTimeLen;	//����ʱ������λ���룩
				std::string m_strHangupType;	//�Ҷ�����
				std::string m_strSwitchType;	//�Ҷ�����
				std::string m_strReleaseType;	// ��������
				std::string m_strCaseID;		//��������ID
				std::string m_strReceiptCode;
				std::string m_strReceiptName;
				std::string m_strIsCallBack;	//���ͻ����Ƿ����Ƴ���0�����Ƴ���1��δ�Ƴ���
				std::string m_strName;			//װ��������
				std::string m_strAddress;		//װ���˵�ַ
				std::string m_strProvince;		//������ʡ��
				std::string m_strCity;			//�����س���
				std::string m_strOperator;		//��Ӫ��
				std::string m_strRecordFileId;	//¼���ļ�ID
				std::string m_strRecordFileName;//¼���ļ���
			};
			class CBody
			{
			public:
				std::string m_strCount;
				std::string m_strAllCount;
				std::string m_strPageIndex;
				std::vector<CData> m_vecData;
			};

			CHeaderEx m_oHeader;			
			CBody m_oBody;
		};
	}
}
