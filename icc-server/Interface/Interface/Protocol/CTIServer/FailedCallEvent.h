#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CFailedCallEvent :
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

				p_pJson->SetNodeValue("/body/device_num", m_oBody.m_strFailedDeviceNum);
				p_pJson->SetNodeValue("/body/error_code", m_oBody.m_strErrorCode);
				p_pJson->SetNodeValue("/body/error_msg", m_oBody.m_strErrorMsg);

				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallRefId);
				p_pJson->SetNodeValue("/body/called_id", m_oBody.m_strCalledId);
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCallRefId;
				std::string m_strCallerId;
				std::string m_strCalledId;
				std::string	m_strErrorCode;	//Ê§°ÜÔ­Òò´íÎóÂë	
				std::string	m_strErrorMsg;	//´íÎóÂëÃèÊö	
				std::string m_strFailedDeviceNum;
			};
			CBody m_oBody;
		};
	}
}
