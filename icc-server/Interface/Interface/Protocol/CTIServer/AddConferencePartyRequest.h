#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAddConferencePartyRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCallRefId = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strCompere = p_pJson->GetNodeValue("/body/compere", "");
				int l_nCount = p_pJson->GetCount("/body/party");;
				m_oBody.m_strCount = std::to_string(l_nCount);

				CBody::CData l_oData;
				for (int i = 0; i < l_nCount; ++i)
				{
					std::string l_strIndex = std::to_string(i);
					l_oData.m_strTarget = p_pJson->GetNodeValue("/body/party/" + l_strIndex + "/target", "");
					l_oData.m_strTargetDeviceType = p_pJson->GetNodeValue("/body/party/" + l_strIndex + "/target_device_type", "");

					m_oBody.m_vecData.push_back(l_oData);
				}

				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCompere;
				std::string m_strCount;
				std::string m_strCallRefId;

				class CData
				{
				public:
					std::string m_strTarget;
					std::string m_strTargetDeviceType;
				};
				std::vector<CData> m_vecData;

			};
			CBody m_oBody;
		};
	}
}
