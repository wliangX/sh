#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
   
		class CAdminDeptRespond :
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
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strResult;
			};
			CBody m_oBody;
		};

		class CAdminDeptRequset : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCaseID = p_pJson->GetNodeValue("/body/caseid", "");
				m_oBody.m_strCallerno = p_pJson->GetNodeValue("/body/callerno", "");
				m_oBody.m_strCalledno = p_pJson->GetNodeValue("/body/calledno", "");
				m_oBody.m_strCallingtime = p_pJson->GetNodeValue("/body/callingtime", "");
				m_oBody.m_strAnsweringtime = p_pJson->GetNodeValue("/body/answeringtime", "");

				m_oBody.m_strStaffCode = p_pJson->GetNodeValue("/body/staff_code", "");
				m_oBody.m_strAlarmPersonName = p_pJson->GetNodeValue("/body/alarm_person_name", "");
				m_oBody.m_strAlarmPersonSex = p_pJson->GetNodeValue("/body/alarm_person_sex", "");
				m_oBody.m_strAlarmPersonID = p_pJson->GetNodeValue("/body/alarm_person_sex", "");
				m_oBody.m_strAlarmPersonTag = p_pJson->GetNodeValue("/body/alarm_person_tag", "");

				m_oBody.m_strAddress = p_pJson->GetNodeValue("/body/address", "");
				m_oBody.m_strLon = p_pJson->GetNodeValue("/body/lon", "");
				m_oBody.m_strLat = p_pJson->GetNodeValue("/body/lat", "");

				m_oBody.m_strCallRefId = p_pJson->GetNodeValue("/body/callref_id", "");

				return true;
			}

		public:

			class CBody
			{
			public:
				std::string m_strCaseID;
				std::string m_strCallerno;
				std::string m_strCalledno;
				std::string m_strCallingtime;
				std::string m_strAnsweringtime;

				std::string m_strStaffCode;
				std::string m_strAlarmPersonName;
				std::string m_strAlarmPersonSex;
				std::string m_strAlarmPersonID;
				std::string m_strAlarmPersonTag;
				std::string m_strAddress;
				std::string m_strLon;
				std::string m_strLat;

				std::string m_strCallRefId;
			};

			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}
