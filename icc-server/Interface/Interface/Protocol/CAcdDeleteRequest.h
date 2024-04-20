#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
#include "Protocol/CAcdDeleteRequest.h"

namespace ICC
{
	namespace PROTOCOL
	{
		class CAcdDeleteRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_acd = p_pJson->GetNodeValue("/body/acd", "");
				m_oBody.m_dept_code = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_dept_name = p_pJson->GetNodeValue("/body/dept_name", "");
				m_oBody.m_dept_code_path = p_pJson->GetNodeValue("/body/dept_code_path", "");
				m_oBody.m_dept_name_path = p_pJson->GetNodeValue("/body/dept_name_path", "");

				
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_acd;
				std::string m_dept_code;
				std::string m_dept_name;
				std::string m_remark;
				std::string m_dept_code_path;
				std::string m_dept_name_path;
			};
			CBody m_oBody;
		};
	}
}

