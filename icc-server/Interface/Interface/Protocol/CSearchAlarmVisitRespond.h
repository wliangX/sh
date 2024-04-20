#pragma once
#include <Protocol/IRespond.h>


namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchAlarmVisitRespond :
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

                for (unsigned int i = 0; i < m_oBody.m_vecData.size(); i++)
                {
                    std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_vecData.at(i).m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", m_oBody.m_vecData.at(i).m_strAlarmID);
					p_pJson->SetNodeValue(l_strPrefixPath + "callref_id", m_oBody.m_vecData.at(i).m_strCallrefID);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_code", m_oBody.m_vecData.at(i).m_strDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_name", m_oBody.m_vecData.at(i).m_strDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", m_oBody.m_vecData.at(i).m_strCallerNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", m_oBody.m_vecData.at(i).m_strCallerName);
					p_pJson->SetNodeValue(l_strPrefixPath + "time", m_oBody.m_vecData.at(i).m_strTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "visitor_code", m_oBody.m_vecData.at(i).m_strVisitorCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "visitor_name", m_oBody.m_vecData.at(i).m_strVisitorName);
					p_pJson->SetNodeValue(l_strPrefixPath + "satisfaction", m_oBody.m_vecData.at(i).m_strSatisfaction);
					p_pJson->SetNodeValue(l_strPrefixPath + "suggest", m_oBody.m_vecData.at(i).m_strSuggest);
					p_pJson->SetNodeValue(l_strPrefixPath + "result_content", m_oBody.m_vecData.at(i).m_strResultContent);
					p_pJson->SetNodeValue(l_strPrefixPath + "call_type", m_oBody.m_vecData.at(i).m_strCallType);
					p_pJson->SetNodeValue(l_strPrefixPath + "reason", m_oBody.m_vecData.at(i).m_strReason);
                }
				return p_pJson->ToString();
			}

		public:
            class CData
            {
            public:
				std::string m_strID;								//回访单ID
				std::string m_strAlarmID;							//警情ID
				std::string m_strCallrefID;							//话务ID（回访电话的话务id）
				std::string m_strDeptCode;							//警情归属单位编码
				std::string m_strDeptName;							//警情归属单位名称
				std::string m_strCallerNo;							//报警人号码
				std::string m_strCallerName;						//报警人姓名
				std::string m_strTime;								//回访时间
				std::string m_strVisitorCode;						//回访人编码
				std::string m_strVisitorName;						//回访人姓名
				std::string m_strSatisfaction;						//满意度 0：满意，1：不满意，默认0
				std::string m_strSuggest;							//建议
				std::string m_strResultContent;						//处理结果内容
				std::string m_strCallType;							//话务类型
				std::string m_strReason;							//原因
            };

			class CBody
			{
			public:
				std::string m_strCount;								//分包总数
                std::vector<CData> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}
