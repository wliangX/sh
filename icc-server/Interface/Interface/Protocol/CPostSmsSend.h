#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CPostSmsSendRequest :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				//m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/content", m_oBody.m_strContent);
				p_pJson->SetNodeValue("/elementName", m_oBody.m_strElementName);
				p_pJson->SetNodeValue("/extensionNo", m_oBody.m_strExtensionNo);
				p_pJson->SetNodeValue("/fromUserName", m_oBody.m_strFromUserName);
				p_pJson->SetNodeValue("/phone", m_oBody.m_strPhone);

				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strContent = p_pJson->GetNodeValue("/content", "");
				m_oBody.m_strElementName = p_pJson->GetNodeValue("/elementName", "");
				m_oBody.m_strExtensionNo = p_pJson->GetNodeValue("/extensionNo", "");
				m_oBody.m_strFromUserName = p_pJson->GetNodeValue("/fromUserName", "");
				m_oBody.m_strPhone = p_pJson->GetNodeValue("/phone", "");
				m_oBody.m_strAlarmId = p_pJson->GetNodeValue("/alarmid", "");
				m_oBody.m_strStaffCode = p_pJson->GetNodeValue("/staffCode", "");
				m_oBody.m_strStaffName = p_pJson->GetNodeValue("/staffName", "");
				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strContent;         // ��������
				std::string m_strElementName;     // ��Ԫ����
				std::string m_strExtensionNo;     // ��չ�ţ�Ϊ�������ͣ����Ȳ�������λ�����Żظ�ʱ��ΪΨһ���жϣ�
				std::string m_strFromUserName;    // ������������ʾ����������
				std::string m_strPhone;           // �绰���루�������ʹ��ͬһ������ʱ������,������

				std::string m_strAlarmId;		  // 
				std::string m_strStaffCode;		  // ���Ͷ��ž�Ա���
				std::string m_strStaffName;		  // ���Ͷ��ž�Ա����
			};
			CBody m_oBody;
		};


		class CPostSmsSendResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				//m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/code", m_oBody.m_strCode);
				p_pJson->SetNodeValue("/data/code", m_oBody.m_strDataCode);
				p_pJson->SetNodeValue("/data/errorCode", m_oBody.m_strDataErrorCode);
				p_pJson->SetNodeValue("/data/message", m_oBody.m_strDataMessage);
				p_pJson->SetNodeValue("/data/smId", m_oBody.m_strDataSmId);
				p_pJson->SetNodeValue("/message", m_oBody.m_strMessage);
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCode = p_pJson->GetNodeValue("/code", "");
				m_oBody.m_strDataCode = p_pJson->GetNodeValue("/data/code", "");
				m_oBody.m_strDataErrorCode = p_pJson->GetNodeValue("/data/errorCode", "");
				m_oBody.m_strDataMessage = p_pJson->GetNodeValue("/data/message", "");
				m_oBody.m_strDataSmId = p_pJson->GetNodeValue("/data/smId", "");
				m_oBody.m_strMessage = p_pJson->GetNodeValue("/message", "");
				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strCode;             // ������Ϣ��,Ĭ��:200
				std::string m_strDataCode;         // �ӿڵ��÷����루1���ɹ���0��ʧ�ܣ�
				std::string m_strDataErrorCode;    // �ӿڵ��ô�����Ӧ��
				std::string m_strDataMessage;      // �������ʾ��Ϣ
				std::string m_strDataSmId;         // �ɶ���ƽ̨���ɵ���ϢID���ں�����ѯ����״̬ʱ��Ҫ�����ֵ
				std::string m_strMessage;          // ������Ϣ���ݣ�Ĭ��:�����ɹ�
			};
			CBody m_oBody;
		};
	}
}
