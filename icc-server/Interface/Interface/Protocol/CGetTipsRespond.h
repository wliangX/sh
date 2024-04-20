#pragma once

#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetTipsRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				unsigned int l_uiIndex = 0;
				for (CData l_oNoticeData : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "tips_id", l_oNoticeData.m_strTipsId);
					p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", l_oNoticeData.m_strCalledNoType);
					p_pJson->SetNodeValue(l_strPrefixPath + "level", l_oNoticeData.m_strLevel);
					p_pJson->SetNodeValue(l_strPrefixPath + "first_type", l_oNoticeData.m_strFirstType);
					p_pJson->SetNodeValue(l_strPrefixPath + "tips_content", l_oNoticeData.m_strContent);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				std::string m_strTipsId;			//提示ID
				std::string m_strCalledNoType;		//报警号码类型
				std::string m_strLevel;				//警情级别
				std::string m_strFirstType;			//警情一级类型
				std::string m_strContent;			//提示内容
			};

			class CBody
			{
			public:
				std::string m_strCount;
				std::vector<CData> m_vecData;
			};

			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}