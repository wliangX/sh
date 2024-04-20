#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTodayCallStatisticsRespond :
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
				
			    if (m_oBody.m_strCount.empty())
			    {
					m_oBody.m_strCount = "0";
			    }
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				int iCount = m_oBody.m_vecDatas.size();
				for (size_t i = 0; i < iCount; ++i)
				{
					std::string l_strIndex = std::to_string(i);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/start_time", m_oBody.m_vecDatas[i].m_strStartTime);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/end_time", m_oBody.m_vecDatas[i].m_strEndTime);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/call_count", m_oBody.m_vecDatas[i].m_strCallCount);
				}

				return p_pJson->ToString();
			}

		public:		
			class CData
			{
			public:
				std::string m_strStartTime;  //开始时间
				std::string m_strEndTime;    //结束时间
				std::string m_strCallCount;  //呼叫数量
			};
			class CBody
			{
			public:		
				std::string m_strCount;
				std::vector<CData> m_vecDatas;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}