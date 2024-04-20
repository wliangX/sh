#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CGetSeatRespond :
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
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/no", m_oBody.m_vecData.at(i).m_strNo);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/name", m_oBody.m_vecData.at(i).m_strName);
                    p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/dept_code", m_oBody.m_vecData.at(i).m_strDeptCode);
                    p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/dept_name", m_oBody.m_vecData.at(i).m_strDeptName);
                }
                return p_pJson->ToString();
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
                    std::string m_strNo;
					std::string m_strName;
                    std::string m_strDeptCode;
                    std::string m_strDeptName;

                };
                std::vector<CData>m_vecData;
            };
			CBody m_oBody;
		};
	}
}
