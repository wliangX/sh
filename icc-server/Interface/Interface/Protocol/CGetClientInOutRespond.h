#pragma once
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetInOutInfoRespond :
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

				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				for (unsigned int i = 0; i < m_oBody.m_vData.size();i++)
				{
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/client_id", m_oBody.m_vData[i].m_strClientID);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/client_name", m_oBody.m_vData[i].m_strClientName);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/in_time", m_oBody.m_vData[i].m_strInTime);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/out_time", m_oBody.m_vData[i].m_strOutTime);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/duration", m_oBody.m_vData[i].m_strDuration);
				}

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				class CData
				{
				public:
					std::string m_strClientID;
					std::string m_strClientName;
					std::string m_strInTime;
					std::string m_strOutTime;
					std::string m_strDuration;
				};
			public:
				std::string m_strCount;
				std::vector<CData>m_vData;
			};
			CBody m_oBody;
		};
	}
}
