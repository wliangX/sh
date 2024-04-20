#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CAlarmInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CMakeCallRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCallerId = p_pJson->GetNodeValue("/body/caller_id", "");
				m_oBody.m_strCalledId = p_pJson->GetNodeValue("/body/called_id", "");
				m_oBody.m_strTargetPhoneType = p_pJson->GetNodeValue("/body/target_phone_type", "");
				m_oBody.m_strCaseId = p_pJson->GetNodeValue("/body/case_id", "");
				m_oBody.m_strRelateCallRefId = p_pJson->GetNodeValue("/body/relate_callref_id", "");
				m_oBody.m_strReceiptCode = p_pJson->GetNodeValue("/body/receipt_code", "");
				m_oBody.m_strReceiptName = p_pJson->GetNodeValue("/body/receipt_name", "");
				m_oBody.m_strCallType = p_pJson->GetNodeValue("/body/call_type", "");
				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCallerId;				//	����
				std::string m_strCalledId;				//	����
				std::string m_strTargetPhoneType;		//	��������
				std::string m_strCaseId;				//	���� ID
				std::string m_strRelateCallRefId;		//	��������ID
				std::string m_strReceiptCode;			//	�Ӿ�Ա���
				std::string m_strReceiptName;			//	�Ӿ�Ա����
				std::string m_strCallType;				//	�绰����,�����ж��Ƿ�Ϊ����ط�

				CAlarmInfo m_oAlarm;
			};
			CBody m_oBody;
		};
	}
}
