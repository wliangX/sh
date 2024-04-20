#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CDeleteBindRequest :
			public IRequest,IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
                m_oBody.m_strType = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
                m_oBody.m_strFromGuid = p_pJson->GetNodeValue("/body/from_guid", "");
                m_oBody.m_strToGuid = p_pJson->GetNodeValue("/body/to_guid", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
               	p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
                p_pJson->SetNodeValue("/body/from_guid",  m_oBody.m_strFromGuid);
              	p_pJson->SetNodeValue("/body/to_guid",  m_oBody.m_strToGuid);
				return p_pJson->ToString();;
			}

		public:			
			class CBody
			{
			public:
				std::string m_strType;		//绑定类型
				std::string m_strGuid;
				std::string m_strFromGuid;
				std::string m_strToGuid;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}