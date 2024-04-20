#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmsSetReadSync :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strID = p_pJson->GetNodeValue("/body/guid", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strID;				//ÏûÏ¢id£º¶ÌÐÅSequence_Id
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};		
	}
}
