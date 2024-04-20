#pragma once
#include <Json/IJsonFactory.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CDeviceInfo
		{
		public:
			std::string ToJson(JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/acd", m_strACD);
				p_pJson->SetNodeValue("/device", m_strDevice);
				p_pJson->SetNodeValue("/device_type", m_strDeviceType);
				p_pJson->SetNodeValue("/call_direction", m_strCallDirection);
				p_pJson->SetNodeValue("/callref_id", m_strCallRefId);
				p_pJson->SetNodeValue("/csta_callref_id", m_strCSTACallRefId);
				p_pJson->SetNodeValue("/caller_id", m_strCallerId);
				p_pJson->SetNodeValue("/called_id", m_strCalledId);
				p_pJson->SetNodeValue("/original_caller_id", m_strOriginalCallerId);
				p_pJson->SetNodeValue("/original_called_id", m_strOriginalCalledId);
				p_pJson->SetNodeValue("/state", m_strState);
				p_pJson->SetNodeValue("/time", m_strTime);
				p_pJson->SetNodeValue("/dept_code", m_strDeptCode);
				p_pJson->SetNodeValue("/dept_name", m_strDeptName);
				return p_pJson->ToString();
			}

			bool Parse(std::string l_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(l_strJson))
				{
					return false;
				}

				m_strDevice = p_pJson->GetNodeValue("/device", "");
				m_strACD = p_pJson->GetNodeValue("/acd", "");
				m_strDeviceType = p_pJson->GetNodeValue("/device_type", "");
				m_strCallDirection = p_pJson->GetNodeValue("/call_direction", "");
				m_strCallRefId = p_pJson->GetNodeValue("/callref_id", "");
				m_strCSTACallRefId = p_pJson->GetNodeValue("/csta_callref_id", "");
				m_strCallerId = p_pJson->GetNodeValue("/caller_id", "");
				m_strCalledId = p_pJson->GetNodeValue("/called_id", "");
				m_strOriginalCallerId = p_pJson->GetNodeValue("/original_caller_id", "");
				m_strOriginalCalledId = p_pJson->GetNodeValue("/original_called_id", "");
				m_strState = p_pJson->GetNodeValue("/state", "");
				m_strTime = p_pJson->GetNodeValue("/time", "");
				m_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
				m_strDeptName = p_pJson->GetNodeValue("/dept_name", "");

				return true;
			}

		public:
			std::string m_strDevice;
			std::string m_strDeviceType;
			std::string m_strCallDirection;
			std::string m_strCallRefId;
			std::string m_strCSTACallRefId;
			std::string m_strCallerId;
			std::string m_strCalledId;
			std::string m_strOriginalCallerId;
			std::string m_strOriginalCalledId;
			std::string m_strState;
			std::string m_strTime;
			std::string m_strDeptCode;
			std::string m_strDeptName;
			std::string m_strACD;
		};
	}
};