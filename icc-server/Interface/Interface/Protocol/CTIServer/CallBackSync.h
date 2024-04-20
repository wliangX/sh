#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CCallBackSync :
			public IRequest, public IRespond
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
				//m_oBody.m_strTargetPhoneType = p_pJson->GetNodeValue("/body/target_phone_type", "");
				m_oBody.m_strCallRefId = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strCaseId = p_pJson->GetNodeValue("/body/case_id", "");
				m_oBody.m_strRelateCallRefId = p_pJson->GetNodeValue("/body/relate_callref_id", "");
				m_oBody.m_strReceiptCode = p_pJson->GetNodeValue("/body/receipt_code", "");
				m_oBody.m_strReceiptName = p_pJson->GetNodeValue("/body/receipt_name", "");
				//m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/caller_id", m_oBody.m_strCallerId);
				p_pJson->SetNodeValue("/body/called_id", m_oBody.m_strCalledId);
				//p_pJson->SetNodeValue("/body/target_phone_type", m_oBody.m_strTargetPhoneType);
				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallRefId);
				p_pJson->SetNodeValue("/body/case_id", m_oBody.m_strCaseId);
				p_pJson->SetNodeValue("/body/relate_callref_id", m_oBody.m_strRelateCallRefId);
				p_pJson->SetNodeValue("/body/receipt_code", m_oBody.m_strReceiptCode);
				p_pJson->SetNodeValue("/body/receipt_name", m_oBody.m_strReceiptName);
				//p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCallRefId;				//	话务ID
				std::string m_strCallerId;				//	主叫
				std::string m_strCalledId;				//	被叫
				//std::string m_strTargetPhoneType;		//	被叫类型
				std::string m_strCaseId;				//	警情 ID
				std::string m_strRelateCallRefId;		//	关联话务ID
				std::string m_strReceiptCode;			//	接警员编号
				std::string m_strReceiptName;			//	接警员姓名
				//std::string m_strTime;					//	时间
			};
			CBody m_oBody;
		};
	}
}
