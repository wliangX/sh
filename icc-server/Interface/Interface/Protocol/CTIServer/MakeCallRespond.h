#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CMakeCallRespond :
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

				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallRefId);
				p_pJson->SetNodeValue("/body/caller_id", m_oBody.m_strCallerId);
				p_pJson->SetNodeValue("/body/called_id", m_oBody.m_strCalledId);
				p_pJson->SetNodeValue("/body/case_id", m_oBody.m_strCaseId);
				//p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

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
				std::string m_strCaseId;
				//std::string m_strResult;
			};
			CBody m_oBody;
		};
	}
}
