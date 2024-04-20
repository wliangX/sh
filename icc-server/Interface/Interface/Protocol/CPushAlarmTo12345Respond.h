#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
namespace ICC
{
	namespace PROTOCOL
	{
		//前端调用接口
		class CPushAlarmResponse :
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

				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/code", m_oBody.m_code);
				p_pJson->SetNodeValue("/body/data", m_oBody.m_data);
				p_pJson->SetNodeValue("/body/message", m_oBody.m_message);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strResult;
				std::string m_code;
				std::string m_data;
				std::string m_message;
			};
			CBody m_oBody;
		};

		class CRepulseAlarmResponse :
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

				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/code", m_oBody.m_code);
				p_pJson->SetNodeValue("/body/data", m_oBody.m_data);
				p_pJson->SetNodeValue("/body/message", m_oBody.m_message);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strResult;
				std::string m_code;
				std::string m_data;
				std::string m_message;
			};
			CBody m_oBody;
		};
		
		//调用12345接口部分
		class CPushAlarmTo12345Response :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_code = p_pJson->GetNodeValue("code", "");
				m_oBody.m_data = p_pJson->GetNodeValue("data", "");
				m_oBody.m_message = p_pJson->GetNodeValue("message", "");


				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_code;
				std::string m_data;
				std::string m_message;
			};
			CBody m_oBody;
		};

		class CRepulseAlarmTo12345Response :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				/*if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}*/

				m_oBody.m_code = p_pJson->GetNodeValue("code", "");
				m_oBody.m_data = p_pJson->GetNodeValue("data", "");
				m_oBody.m_message = p_pJson->GetNodeValue("message", "");


				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_code;
				std::string m_data;
				std::string m_message;
			};
			CBody m_oBody;
		};
	}
}