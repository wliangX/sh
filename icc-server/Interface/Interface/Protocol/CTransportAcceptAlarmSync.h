#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTransPortAcceptAlarmSync :
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

				p_pJson->SetNodeValue("/body/user_code", m_oBody.m_strUserCode);
				p_pJson->SetNodeValue("/body/user_name", m_oBody.m_strUserName);
				p_pJson->SetNodeValue("/body/dept_code", m_oBody.m_strDeptCode);
				p_pJson->SetNodeValue("/body/dept_name", m_oBody.m_strDeptName);
				p_pJson->SetNodeValue("/body/seat_no", m_oBody.m_strSeatNo);

				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallref);
				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_strSoureAlarmID);
				p_pJson->SetNodeValue("/body/new_alarm_id", m_oBody.m_strNewAlarmID);
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/refuse_reason", m_oBody.m_strRefuseReason);
				p_pJson->SetNodeValue("/body/transport_type", m_oBody.m_strTransportType);
				return p_pJson->ToString();
			}

		public:

			class CBody
			{
			public:
				std::string m_strUserCode;
				std::string m_strUserName;
				std::string m_strDeptCode;
				std::string m_strDeptName;
				std::string m_strSeatNo;
				std::string m_strResult;   //0,³É¹¦£¬1:Ê§°Ü£¬2£º³¬Ê±
				std::string m_strCallref;
				std::string m_strSoureAlarmID;
				std::string m_strNewAlarmID;
				std::string m_strRefuseReason;
				std::string m_strTransportType;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}

