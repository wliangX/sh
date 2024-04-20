#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CDeviceStateEvent :
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
				p_pJson->SetNodeValue("/body/acd", m_oBody.m_strACD);
				p_pJson->SetNodeValue("/body/device", m_oBody.m_strDevice);
				p_pJson->SetNodeValue("/body/device_type", m_oBody.m_strDeviceType);
				p_pJson->SetNodeValue("/body/call_direction", m_oBody.m_strCallDirection);
				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallRefId);
				p_pJson->SetNodeValue("/body/csta_callref_id", m_oBody.m_strCSTACallRefId);
				p_pJson->SetNodeValue("/body/caller_id", m_oBody.m_strCallerId);
				p_pJson->SetNodeValue("/body/called_id", m_oBody.m_strCalledId);
				p_pJson->SetNodeValue("/body/original_caller_id", m_oBody.m_strOriginalCallerId);
				p_pJson->SetNodeValue("/body/original_called_id", m_oBody.m_strOriginalCalledId);
				p_pJson->SetNodeValue("/body/state", m_oBody.m_strState);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);
				p_pJson->SetNodeValue("/body/dept_code", m_oBody.m_strDeptCode);
				p_pJson->SetNodeValue("/body/dept_name", m_oBody.m_strDeptName);
				if (!m_oBody.m_strTransferType.empty())
				{
					p_pJson->SetNodeValue("/body/transfer_type", m_oBody.m_strTransferType);
				}
				
				if (!m_oBody.m_strConferenceId.empty())
				{
					p_pJson->SetNodeValue("/body/conference_id", m_oBody.m_strConferenceId);
					p_pJson->SetNodeValue("/body/conference_compere", m_oBody.m_strConferenceCompere);
					if (!m_oBody.m_strConfBargeinSponsor.empty())
					{
						p_pJson->SetNodeValue("/body/conference_bargeinsponsor", m_oBody.m_strConfBargeinSponsor);
					}
				}
				

				p_pJson->SetNodeValue("/body/begin_talk_time", m_oBody.m_strBeginTalkTime);
			//	p_pJson->SetNodeValue("/body/is_transfer_call", m_oBody.m_strIsTransferCall);
				/*p_pJson->SetNodeValue("/body/hangup_type", m_oBody.m_strHangupType);
				p_pJson->SetNodeValue("/body/hangup_device", m_oBody.m_strHangupDevice);*/
				if (!m_oBody.m_strSwitchAddress.empty())
				{
					p_pJson->SetNodeValue("/body/switch_address", m_oBody.m_strSwitchAddress);
				}
				if (!m_oBody.m_strRecordFilePath.empty())
				{
					p_pJson->SetNodeValue("/body/record_file", m_oBody.m_strRecordFilePath);
				}

				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strDevice = p_pJson->GetNodeValue("/body/device", "");
				m_oBody.m_strACD = p_pJson->GetNodeValue("/body/acd", "");
				m_oBody.m_strDeviceType = p_pJson->GetNodeValue("/body/device_type", "");
				m_oBody.m_strCallDirection = p_pJson->GetNodeValue("/body/call_direction", "");
				m_oBody.m_strCallRefId = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strCSTACallRefId = p_pJson->GetNodeValue("/body/csta_callref_id", "");
				m_oBody.m_strCallerId = p_pJson->GetNodeValue("/body/caller_id", "");
				m_oBody.m_strCalledId = p_pJson->GetNodeValue("/body/called_id", "");
				m_oBody.m_strOriginalCallerId = p_pJson->GetNodeValue("/body/original_caller_id", "");
				m_oBody.m_strOriginalCalledId = p_pJson->GetNodeValue("/body/original_called_id", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");

				m_oBody.m_strBeginTalkTime = p_pJson->GetNodeValue("/body/begin_talk_time", "");
				m_oBody.m_strSwitchAddress = p_pJson->GetNodeValue("/body/switch_address", "");
				m_oBody.m_strRecordFilePath = p_pJson->GetNodeValue("/body/record_file", "");

				m_oBody.m_strTransferType = p_pJson->GetNodeValue("/body/transfer_type", "");

				return true;
			}


		public:
			CHeaderEx m_oHeader;

			class CBody
			{
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
			//	std::string m_strIsTransferCall;		//	是否为转移话务
				/*std::string m_strHangupType;
				std::string m_strHangupDevice;*/
				std::string m_strACD;
				std::string m_strBeginTalkTime;
				std::string m_strSwitchAddress;
				std::string m_strRecordFilePath;

				std::string m_strConferenceId;
				std::string m_strConferenceCompere;
				std::string m_strConfBargeinSponsor;

				std::string m_strTransferType;
			};
			CBody m_oBody;
		};
	}
}
