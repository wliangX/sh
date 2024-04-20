#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CAlarmAttentionJudgeResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/result",m_oBody.m_result);

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;


			class CBody
			{
			public:
				std::string m_result;
			};
			CBody m_oBody;
		};
	}
}
