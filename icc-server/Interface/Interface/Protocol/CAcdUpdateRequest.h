#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
#include "Protocol/CAcdAddRequest.h"

namespace ICC
{
	namespace PROTOCOL
	{
		class CAcdUpdateRequest :
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
				m_oBody.m_remark = p_pJson->GetNodeValue("/body/remark", "");
				m_oBody.m_dept_code_path = p_pJson->GetNodeValue("/body/dept_code_path", "");
				m_oBody.m_dept_name_path = p_pJson->GetNodeValue("/body/dept_name_path", "");
				return true;
			}

			virtual bool ParseString_NoHead(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				m_oBody.m_acd = p_pJson->GetNodeValue("/acd", "");
				m_oBody.m_dept_code = p_pJson->GetNodeValue("/dept_code", "");
				m_oBody.m_dept_name = p_pJson->GetNodeValue("/dept_name", "");
				m_oBody.m_remark = p_pJson->GetNodeValue("/remark", "");
				m_oBody.m_dept_code_path = p_pJson->GetNodeValue("/dept_code_path", "");
				m_oBody.m_dept_name_path = p_pJson->GetNodeValue("/dept_name_path", "");
				return true;
			}

			virtual std::string ToString_NoHead(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";

				}
				p_pJson->SetNodeValue("/acd", m_oBody.m_acd);
				p_pJson->SetNodeValue("/dept_code", m_oBody.m_dept_code);
				p_pJson->SetNodeValue("/dept_name", m_oBody.m_dept_name);
				p_pJson->SetNodeValue("/remark", m_oBody.m_remark);
				p_pJson->SetNodeValue("/dept_code_path", m_oBody.m_dept_code_path);
				p_pJson->SetNodeValue("/dept_name_path", m_oBody.m_dept_name_path);
				p_pJson->SetNodeValue("/update_time", m_oBody.m_update_time);


				return p_pJson->ToString();
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
				std::string m_update_time;
			};
			CBody m_oBody;
		};
	}
}

