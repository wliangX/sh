#pragma once 

namespace ICC
{
	namespace PROTOCOL
	{
		class CCallInfo :
			public IRequest,
			public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}

				m_strCallRefId = p_pJson->GetNodeValue("/callref_id", "");
				m_strCallerId = p_pJson->GetNodeValue("/caller_id", "");
				m_strCalledId = p_pJson->GetNodeValue("/called_id", "");
				m_strState = p_pJson->GetNodeValue("/state", "");
				m_strTime = p_pJson->GetNodeValue("/time", "");
				m_strCstCallrefId = p_pJson->GetNodeValue("/cst_callref_id", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/callref_id", m_strCallRefId);
				p_pJson->SetNodeValue("/caller_id", m_strCallerId);
				p_pJson->SetNodeValue("/called_id", m_strCalledId);
				p_pJson->SetNodeValue("/state", m_strState);
				p_pJson->SetNodeValue("/time", m_strTime);
				p_pJson->SetNodeValue("/cst_callref_id", m_strCstCallrefId);
				return p_pJson->ToString();
			}

		public:
			std::string m_strCallRefId;				// 话务ID
			std::string	m_strCallerId;				// 主叫号码
			std::string	m_strCalledId;				// 被叫分机号(7001,8001)
			std::string m_strState;					// 当前话务状态
			std::string m_strTime;					// 话务状态时间
			std::string m_strCstCallrefId;			// 交换机产生的话务 ID
		};
	}
}
