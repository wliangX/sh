#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAllBlackListRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strValueNum = std::to_string(i);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/limit_num", m_oBody.m_vecData.at(i).m_strLimit_Num);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/limit_minute", m_oBody.m_vecData.at(i).m_strLimit_Minute);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/limit_reason", m_oBody.m_vecData.at(i).m_strLimit_Reason);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/staff_code", m_oBody.m_vecData.at(i).m_strStaff_Code);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/staff_name", m_oBody.m_vecData.at(i).m_strStaff_Name);
				}
				return p_pJson->ToString();
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
					std::string m_strLimit_Num;
					std::string m_strLimit_Minute;
					std::string m_strLimit_Reason;
					std::string m_strStaff_Code;
					std::string m_strStaff_Name;
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
