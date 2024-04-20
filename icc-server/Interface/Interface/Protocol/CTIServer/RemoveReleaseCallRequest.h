#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CRemoveReleaseCallRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				int l_nCount = p_pJson->GetCount("/body/data");;
				m_oBody.m_strCount = std::to_string(l_nCount);

				CBody::CData l_oData;
				for (int i = 0; i < l_nCount; ++i)
				{
					std::string l_strIndex = std::to_string(i);
					l_oData.m_strCallRefId = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/callref_id", "");

					m_oBody.m_vecData.push_back(l_oData);
				}

				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCount;

				class CData
				{
				public:
					std::string m_strCallRefId;
				};
				std::vector<CData> m_vecData;

			};
			CBody m_oBody;
		};
	}
}
