#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGPSGetBJRDHGpsRespond :
			public ISend
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
                for (unsigned int i = 0; i < m_oBody.m_vecData.size(); i++)
                {
                    std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
                    p_pJson->SetNodeValue(l_strPrefixPath + "call_time", m_oBody.m_vecData.at(i).m_strCallTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", m_oBody.m_vecData.at(i).m_strCallerNo);
                    p_pJson->SetNodeValue(l_strPrefixPath + "called_no", m_oBody.m_vecData.at(i).m_strCalledNo);
                    p_pJson->SetNodeValue(l_strPrefixPath + "area", m_oBody.m_vecData.at(i).m_strArea);
                    p_pJson->SetNodeValue(l_strPrefixPath + "lac", m_oBody.m_vecData.at(i).m_strLac);
                    p_pJson->SetNodeValue(l_strPrefixPath + "ci", m_oBody.m_vecData.at(i).m_strCi);
                    p_pJson->SetNodeValue(l_strPrefixPath + "longitude", m_oBody.m_vecData.at(i).m_strLongitude);
                    p_pJson->SetNodeValue(l_strPrefixPath + "latitude", m_oBody.m_vecData.at(i).m_strLatitude);
                    p_pJson->SetNodeValue(l_strPrefixPath + "address", m_oBody.m_vecData.at(i).m_strAddress);
                }
                return p_pJson->ToString();
            }

		public:
            class CBJRDH
            {
            public:
                std::string	m_strCallTime;				    //拨打时间
                std::string	m_strCallerNo;					//主叫号码
                std::string m_strCalledNo;				    //被叫号码
                std::string m_strArea;					    //主叫归属区号
                std::string m_strLac;						//位置区码			
                std::string m_strCi;			            //小区识别
                std::string m_strLongitude;					//经度
                std::string m_strLatitude;					//维度
                std::string m_strAddress;				    //基础位置
            };

			class CBody
			{
			public:
                std::string m_strCount;
                std::vector<CBJRDH> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};


        class CPostespond :
            public ISend
        {
        public:
            virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
                if (nullptr == p_pJson)
                {
                    return "";
                }

                m_oHeader.SaveTo(p_pJson);
                p_pJson->SetNodeValue("/body/code", m_oBody.m_strcode);
                p_pJson->SetNodeValue("/body/message", m_oBody.m_strmessage);

                return p_pJson->ToString();
            }

        public:
            class CBody
            {
            public:
                std::string m_strcode;
                std::string m_strmessage;
            };
            CHeaderEx m_oHeader;
            CBody	m_oBody;
        };

	}
}