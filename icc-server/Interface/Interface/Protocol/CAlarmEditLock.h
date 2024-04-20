#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAlarmEditLock : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strId = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strCode = p_pJson->GetNodeValue("/body/user_code", "");
				m_oBody.m_strLockTime = p_pJson->GetNodeValue("/body/lock_time", "");

				return true;
			}

			//Ӧ��
			std::string ToStringResponse(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/result", m_strResult);
				return p_pJson->ToString();
			}

			std::string ToGet(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_strId);
				p_pJson->SetNodeValue("/body/user_code", m_oBody.m_strCode);
				p_pJson->SetNodeValue("/body/lock_status", m_oBody.m_strLock_Status);
				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strId;		        //�Ӿ�����ID�򴦾���ID����ɵ�ID
				std::string m_strCode;			    //�û���
				std::string m_strLockTime;			//����ʱ��
				std::string m_strLock_Status;       //����״̬ 0 ������1 ����

			};
			
			std::string m_strResult;
			
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}
