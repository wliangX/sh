#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetTipsRequest :
			public IReceive, public ISend
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strTipsId = p_pJson->GetNodeValue("/body/tips_id", "");
				m_oBody.m_strCalledNoType = p_pJson->GetNodeValue("/body/called_no_type", "");
				m_oBody.m_strLevel = p_pJson->GetNodeValue("/body/level", "");
				m_oBody.m_strFirstType = p_pJson->GetNodeValue("/body/first_type", "");
				m_oBody.m_strContent = p_pJson->GetNodeValue("/body/tips_content", "");
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (p_pJson == nullptr)
				{
					return "";
				}
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/tips_id", m_oBody.m_strTipsId);
				p_pJson->SetNodeValue("/body/called_no_type", m_oBody.m_strCalledNoType);
				p_pJson->SetNodeValue("/body/level", m_oBody.m_strLevel);
				p_pJson->SetNodeValue("/body/first_type", m_oBody.m_strFirstType);
				p_pJson->SetNodeValue("/body/tips_content", m_oBody.m_strContent);
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strTipsId;			//��ʾID
				std::string m_strCalledNoType;		//������������
				std::string m_strLevel;				//���鼶��
				std::string m_strFirstType;			//����һ������
				std::string m_strContent;			//��ʾ����
				std::string m_strSyncType;			//ͬ�����ͣ�1����ӣ�2�����£�3��ɾ��
			};

			CHeaderEx m_oHeader;
			CBody m_oBody;
		};

		class CSetTipsSync :
			public IReceive, public ISend
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strTipsId = p_pJson->GetNodeValue("/body/tips_id", "");
				m_oBody.m_strCalledNoType = p_pJson->GetNodeValue("/body/called_no_type", "");
				m_oBody.m_strLevel = p_pJson->GetNodeValue("/body/level", "");
				m_oBody.m_strFirstType = p_pJson->GetNodeValue("/body/first_type", "");
				m_oBody.m_strContent = p_pJson->GetNodeValue("/body/tips_content", "");
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (p_pJson == nullptr)
				{
					return "";
				}
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/tips_id", m_oBody.m_strTipsId);
				p_pJson->SetNodeValue("/body/called_no_type", m_oBody.m_strCalledNoType);
				p_pJson->SetNodeValue("/body/level", m_oBody.m_strLevel);
				p_pJson->SetNodeValue("/body/first_type", m_oBody.m_strFirstType);
				p_pJson->SetNodeValue("/body/tips_content", m_oBody.m_strContent);
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strTipsId;			//��ʾID
				std::string m_strCalledNoType;		//������������
				std::string m_strLevel;				//���鼶��
				std::string m_strFirstType;			//����һ������
				std::string m_strContent;			//��ʾ����
				std::string m_strSyncType;			//ͬ�����ͣ�1����ӣ�2�����£�3��ɾ��
			};

			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
