#pragma once 
#include <vector>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTransGetDeviceListRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				unsigned int l_nCount = p_pJson->GetCount("/body/data");
				for (unsigned int i = 0; i < l_nCount;i++)
				{
					CBody::CData l_oData;

					l_oData.m_strDevice = p_pJson->GetNodeValue("/body/data/"+std::to_string(i)+ "/device", "");
					l_oData.m_strDeviceType = p_pJson->GetNodeValue("/body/data/" + std::to_string(i) + "/device_type", "");
					l_oData.m_strState = p_pJson->GetNodeValue("/body/data/" + std::to_string(i) + "/state", "");
					l_oData.m_strTime = p_pJson->GetNodeValue("/body/data/" + std::to_string(i) + "/time", "");
					l_oData.m_strCallDirection = p_pJson->GetNodeValue("/body/data/" + std::to_string(i) + "/call_direction", "");
					l_oData.m_strCallRefId = p_pJson->GetNodeValue("/body/data/" + std::to_string(i) + "/callref_id", "");
					l_oData.m_strCallerId = p_pJson->GetNodeValue("/body/data/" + std::to_string(i) + "/caller_id", "");
					l_oData.m_strCalledId = p_pJson->GetNodeValue("/body/data/" + std::to_string(i) + "/called_id", "");

					if (l_oData.m_strState == "freestate")
					{
						m_vFreeState.push_back(l_oData.m_strDevice);
					}
					m_oBody.m_vecData.push_back(l_oData);
				}

				return true;
			}
		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
				std::string m_strCount;

				class CData
				{
				public:
					std::string m_strDevice;
					std::string m_strDeviceType;
					std::string m_strState;
					std::string m_strTime;
					std::string m_strCallDirection;
					std::string m_strCallRefId;
					std::string m_strCallerId;
					std::string m_strCalledId;
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
			std::vector<std::string>m_vFreeState;
		};
	}
}
