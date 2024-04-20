#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CBJRDHPositionRequest : public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{		
				if (nullptr == p_pJson)
				{
					return "";
				}
				
				p_pJson->SetNodeValue("/enduser/xm", m_strXm);
				p_pJson->SetNodeValue("/enduser/sfzh", m_strSfzh);
				p_pJson->SetNodeValue("/enduser/jgdm", m_strJgdm);
				p_pJson->SetNodeValue("/condition/callingnumber", m_strCallingNumber);

				return p_pJson->ToString();
			}

		public:
			std::string m_strXm;
			std::string m_strSfzh;
			std::string m_strJgdm;
			std::string m_strCallingNumber;
		};

		class CBJRDHPositionRequestJZ : public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/appid", m_strAppid);
				p_pJson->SetNodeValue("/phone", m_strPhone);
				p_pJson->SetNodeValue("/station", m_strStation);
				p_pJson->SetNodeValue("/time", m_strTime);
				p_pJson->SetNodeValue("/sign", m_strSign);
				p_pJson->SetNodeValue("/maxWaitSecondTime", m_strMaxWaitSecondTime);

				return p_pJson->ToString();
			}

		public:
			std::string m_strAppid;
			std::string m_strPhone;
			std::string m_strStation;
			std::string m_strTime;
			std::string m_strSign;
			std::string m_strMaxWaitSecondTime;
		};
	}
}
