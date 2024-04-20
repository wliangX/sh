#pragma once

#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetTimeOutEventRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				unsigned int l_uiIndex = 0;
				for (CData l_oEventData : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "guid", l_oEventData.m_strGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", l_oEventData.m_strAlarmId);
					p_pJson->SetNodeValue(l_strPrefixPath + "callref_id", l_oEventData.m_strCallRefId);

					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", l_oEventData.m_strReceiptDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", l_oEventData.m_strReceiptDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", l_oEventData.m_strReceiptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", l_oEventData.m_strReceiptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_seatno", l_oEventData.m_strReceiptSeatNo);

					p_pJson->SetNodeValue(l_strPrefixPath + "start_time", l_oEventData.m_strStartTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "end_time", l_oEventData.m_strEndTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_type", l_oEventData.m_strAlarmType);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CData
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
			};

			class CBody
			{
			public:
				std::string m_strCount;
				std::vector<CData> m_vecData;
			};

			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}