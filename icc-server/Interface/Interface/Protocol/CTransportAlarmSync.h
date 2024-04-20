#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTransPortAlarmSync :
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

				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_strAlarmID);
				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallRefID);
				p_pJson->SetNodeValue("/body/user_code", m_oBody.m_strUserCode);
				p_pJson->SetNodeValue("/body/user_name", m_oBody.m_strUserName);
				p_pJson->SetNodeValue("/body/dept_code", m_oBody.m_strDeptCode);
				p_pJson->SetNodeValue("/body/dept_name", m_oBody.m_strDeptName);
				p_pJson->SetNodeValue("/body/seat_no", m_oBody.m_strSeatNo);
				p_pJson->SetNodeValue("/body/target_seat_no", m_oBody.m_strTargetSeatNo);
				p_pJson->SetNodeValue("/body/target_dept_code", m_oBody.m_strTargetDeptCode);
				p_pJson->SetNodeValue("/body/target_dept_name", m_oBody.m_strTargetDeptName);
				p_pJson->SetNodeValue("/body/transport_reason", m_oBody.m_strTransportReason);

				p_pJson->SetNodeValue("/body/transport_type", m_oBody.m_strTransportType);
				return p_pJson->ToString();
			}

		public:

			class CBody
			{
			public:
				std::string m_strAlarmID;
				std::string m_strCallRefID;
				std::string m_strUserCode;
				std::string m_strUserName;
				std::string m_strDeptCode;
				std::string m_strDeptName;
				std::string m_strSeatNo;
				std::string m_strTargetSeatNo;
				std::string m_strTargetDeptCode;
				std::string m_strTargetDeptName;
				std::string m_strTransportReason;
				std::string m_strTransportType;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};


		

		class CAlarmTransportCacheRespond :
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

				//p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

				for (unsigned int i = 0; i < m_oBody.m_vData.size();i++)
				{

					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/source_alarm_id", m_oBody.m_vData[i].m_strSourceAlarmID);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/source_user_code", m_oBody.m_vData[i].m_strSourceUserCode);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/source_seat_no", m_oBody.m_vData[i].m_strSourceSeatNo);

					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/target_user_code", m_oBody.m_vData[i].m_strTargetUserCode);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/target_seat_no", m_oBody.m_vData[i].m_strTargetSeatNo);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/target_dept_code", m_oBody.m_vData[i].m_strTargetDeptCode);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/target_alarm_id", m_oBody.m_vData[i].m_strTargetAlarmID);
				}
				return p_pJson->ToString();
			}

		public:

			class CData
			{
			public:
				std::string m_strSourceAlarmID;
				std::string m_strSourceUserCode;
				std::string m_strSourceSeatNo;

				std::string m_strTargetUserCode;
				std::string m_strTargetSeatNo;
				std::string m_strTargetDeptCode;
				std::string m_strTargetAlarmID;
			};
			class CBody
			{
			public:
				std::string m_strResult;
				std::vector<CData>m_vData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}

