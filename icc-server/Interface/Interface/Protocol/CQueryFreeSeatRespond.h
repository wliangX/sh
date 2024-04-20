#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CQueryFreeSeatResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				for (unsigned int i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "seat_no", m_oBody.m_vecData[i].m_seat_no);

				}

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;

			class CData
			{
			public:
				std::string m_seat_no;
			};

			class CBody
			{
			public:
				std::vector<CData> m_vecData;
			};
			CBody m_oBody;
		};
	}
}
