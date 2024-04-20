#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CBlackCallQueryRespond :
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
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strValueNum = std::to_string(i);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/limit_num", m_oBody.m_vecData.at(i).m_strLimitNum);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/incoming_time", m_oBody.m_vecData.at(i).m_strIncomingTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/limit_reason", m_oBody.m_vecData.at(i).m_strLimitReason);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/staff_code", m_oBody.m_vecData.at(i).m_strStaffCode);
				}

				return p_pJson->ToString();
			}


		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
				std::string m_strCount = "0";

				class CData
				{
				public:
					std::string m_strLimitNum;			//受限号码
					std::string m_strIncomingTime;		//呼入时间
					std::string m_strLimitReason;		//受限原因
					std::string m_strStaffCode;			//警员编号
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
