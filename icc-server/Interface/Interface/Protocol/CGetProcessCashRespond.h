#pragma once
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetProcessCashRespond : public IRespond
		{
		public:

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", std::to_string(m_oBody.m_ProcessCasheds.size()));
				unsigned int l_uiIndex = 0;
				for (ProcessCashed data : m_oBody.m_ProcessCasheds)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex++) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "seat_no", data.m_strSeatNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", data.m_strAlarmID);
					p_pJson->SetNodeValue(l_strPrefixPath + "content", data.m_strContent);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller", data.m_strCaller);
					p_pJson->SetNodeValue(l_strPrefixPath + "time", data.m_strTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", data.m_strHandleType);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", data.m_strState);
				}			
				return p_pJson->ToString();
			}

		public:
			
			class ProcessCashed
			{
			public:
				std::string m_strSeatNo;			//席位号
				std::string m_strAlarmID;			//警情id
				std::string m_strContent;			//警情内容
				std::string m_strCaller;			//报警人电话
				std::string m_strTime;				//报警时间
				std::string m_strHandleType;		//警情种类
				std::string m_strState;				//状态
			};

			class CBody
			{
			public:
				std::vector<ProcessCashed> m_ProcessCasheds;
			};

			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
