#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTimeOutEvent :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_strAlarmId);
				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallRefId);
				p_pJson->SetNodeValue("/body/receipt_dept_code", m_oBody.m_strReceiptDeptCode);
				p_pJson->SetNodeValue("/body/receipt_dept_name", m_oBody.m_strReceiptDeptName);
				p_pJson->SetNodeValue("/body/receipt_code", m_oBody.m_strReceiptCode);
				p_pJson->SetNodeValue("/body/receipt_name", m_oBody.m_strReceiptName);
				p_pJson->SetNodeValue("/body/receipt_seatno", m_oBody.m_strReceiptSeatNo);
				p_pJson->SetNodeValue("/body/start_time", m_oBody.m_strStartTime);
				p_pJson->SetNodeValue("/body/end_time", m_oBody.m_strEndTime);
				p_pJson->SetNodeValue("/body/alarm_type", m_oBody.m_strAlarmType);
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);

				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strAlarmId = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strCallRefId = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/receipt_dept_code", "");
				m_oBody.m_strReceiptDeptName = p_pJson->GetNodeValue("/body/receipt_dept_name", "");
				m_oBody.m_strReceiptCode = p_pJson->GetNodeValue("/body/receipt_code", "");
				m_oBody.m_strReceiptName = p_pJson->GetNodeValue("/body/receipt_name", "");
				m_oBody.m_strReceiptSeatNo = p_pJson->GetNodeValue("/body/receipt_seatno", "");
				m_oBody.m_strStartTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_strAlarmType = p_pJson->GetNodeValue("/body/alarm_type", "");
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");

				return true;
			}

		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
				std::string m_strGuid;
				std::string m_strAlarmId;
				std::string m_strCallRefId;
				std::string m_strReceiptDeptCode;
				std::string m_strReceiptDeptName;
				std::string m_strReceiptCode;
				std::string m_strReceiptName;
				std::string m_strReceiptSeatNo;
				std::string m_strStartTime;
				std::string m_strEndTime;
				std::string m_strAlarmType;
				std::string m_strSyncType;
			};
			CBody m_oBody;
		};
	}
}
