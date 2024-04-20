#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{

		class CTransGetReadyAgentRespond :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strSponsor = p_pJson->GetNodeValue("/body/sponsor", "");
				m_oBody.m_strTarget = p_pJson->GetNodeValue("/body/agent", "");
				m_oBody.m_strResult = p_pJson->GetNodeValue("/body/result", "");

				return true;
			}

		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
				std::string m_strSponsor;			//转移发起分机号
				std::string m_strTarget;			//转移目标分机号，失败时为空
				std::string m_strResult;
			};
			CBody m_oBody;
		};

		
		class CAlarmTransferCallRespond:
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCallRefId = p_pJson->GetNodeValue("/body/held_callref_id", "");
				m_oBody.m_strNewCallRefId = p_pJson->GetNodeValue("/body/active_callref_id", "");
				m_oBody.m_strSponsor = p_pJson->GetNodeValue("/body/sponsor", "");
				m_oBody.m_strTarget = p_pJson->GetNodeValue("/body/target", "");
				m_oBody.m_strResult = p_pJson->GetNodeValue("/body/result", "");

				return true;
			}
			virtual std::string ToJson(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/body/held_callref_id", m_oBody.m_strCallRefId);
				p_pJson->SetNodeValue("/body/active_callref_id", m_oBody.m_strNewCallRefId);
				p_pJson->SetNodeValue("/body/sponsor", m_oBody.m_strSponsor);
				p_pJson->SetNodeValue("/body/target", m_oBody.m_strTarget);
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
				std::string m_strCallRefId;
				std::string m_strNewCallRefId;		//新话务ID（转移失败时为空）
				std::string m_strSponsor;			//转移发起分机号
				std::string m_strTarget;			//转移目标分机号，失败时为空
				std::string m_strResult;
			};
			CBody m_oBody;
		};
	}
}
