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
				std::string m_strCallerId;				//	主叫
				std::string m_strCalledId;				//	被叫
				std::string m_strTargetPhoneType;		//	被叫类型
				std::string m_strCaseId;				//	警情 ID
				std::string m_strRelateCallRefId;		//	关联话务ID
				std::string m_strReceiptCode;			//	接警员编号
				std::string m_strReceiptName;			//	接警员姓名
				std::string m_strCallType;				//	电话类型,根据判定是否为话务回访

				CAlarmInfo m_oAlarm;
			};
			CBody m_oBody;
		};
	}
}
