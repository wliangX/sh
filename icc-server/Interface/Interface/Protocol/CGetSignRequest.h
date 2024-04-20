#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetSignRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				
				m_oBody.m_strProcessDeptCode = p_pJson->GetNodeValue("/body/process_dept_code", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strProcessDeptCode;						//������λ����
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}
