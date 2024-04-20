#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSyncReloadConfigSuccess : public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);				
				p_pJson->SetNodeValue("/body/clientid", m_oBody.m_strClientID);
				return p_pJson->ToString();
			}
		
			class CBody
			{
			public:								
				std::string m_strClientID;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};


		class CHealthActivemq : public ISend
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strhealthActivemq = p_pJson->GetNodeValue("/body/healthActivemq", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/healthActivemq", m_oBody.m_strhealthActivemq);
				return p_pJson->ToString();
			}

			class CBody
			{
			public:
				std::string m_strhealthActivemq;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};


		class CHealthRequest : public ISend
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strHealthActivemq = p_pJson->GetNodeValue("/body/healthActivemq", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/status", m_oBody.m_strStatus);
				p_pJson->SetNodeValue("/components/redis/status", m_oBody.m_strRedisStatus);
				p_pJson->SetNodeValue("/components/activemq/status", m_oBody.m_strActivemqStatus);
				p_pJson->SetNodeValue("/components/postgres/status", m_oBody.m_strpostgreSqlStatus);
				return p_pJson->ToString();
			}

			class CBody
			{
			public:
				std::string m_strHealthActivemq;
				std::string m_strStatus;
				std::string m_strActivemqStatus;
				std::string m_strpostgreSqlStatus;
				std::string m_strRedisStatus;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};




	}
}
