#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>
#include <Protocol/CKeyWordInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAlarmStatisticsRespond :
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
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
			
				for (size_t i = 0; i < m_oBody.m_vecLevelCount.size(); ++i)
				{
					std::string l_strIndex = std::to_string(i);
					p_pJson->SetNodeValue("/body/level_count/" + l_strIndex + "/level", m_oBody.m_vecLevelCount[i].m_strLevel);
					p_pJson->SetNodeValue("/body/level_count/" + l_strIndex + "/count", m_oBody.m_vecLevelCount[i].m_strCount);
										
				}
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CLevelCount
			{
			public:
			
			    std::string m_strLevel;  //警情级别
				std::string m_strCount;  //警情数量
			};
			
			class CBody
			{
			public:				
				std::string m_strAllCount;
				std::vector<CLevelCount> m_vecLevelCount;

			public:
				CBody() :m_strAllCount("0")
				{

				}
			};
			CBody m_oBody;
		};
	}
}
