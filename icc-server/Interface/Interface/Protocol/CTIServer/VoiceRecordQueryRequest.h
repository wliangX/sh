#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CVoiceRecordQueryRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCallref_Id = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strBegin_Time = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_strEnd_Time = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_strCaller_Id = p_pJson->GetNodeValue("/body/caller_id", "");
				m_oBody.m_strCalled_Id = p_pJson->GetNodeValue("/body/called_id", "");

				return true;
			}

		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
				std::string m_strCallref_Id;
				std::string m_strBegin_Time;
				std::string m_strEnd_Time;
				std::string m_strCaller_Id;
				std::string m_strCalled_Id;
			};
			CBody m_oBody;
		};
	}
}
