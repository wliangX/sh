#pragma once
#include <Json/IJsonFactory.h>

#include <Data/IBaseData.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CACDDeptInfo
		{
		public:
			std::string ToJson(JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/acd", m_strACD);
				p_pJson->SetNodeValue("/dept_code", m_strDeptCode);
				p_pJson->SetNodeValue("/dept_name", m_strDeptName);
				p_pJson->SetNodeValue("/remark", m_strRemark);
				p_pJson->SetNodeValue("/dept_code_path", m_strDeptCodePath);
				p_pJson->SetNodeValue("/dept_name_path", m_strDeptNamePath);
				return p_pJson->ToString();
			}

			bool Parse(std::string l_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(l_strJson))
				{
					return false;
				}

				m_strACD = p_pJson->GetNodeValue("/acd", "");
				m_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
				m_strDeptName = p_pJson->GetNodeValue("/dept_name", "");
				m_strRemark = p_pJson->GetNodeValue("/remark", "");
				m_strDeptCodePath = p_pJson->GetNodeValue("/dept_code_path", "");
				m_strDeptNamePath = p_pJson->GetNodeValue("/dept_name_path", "");

				return true;
			}

		public:
			std::string m_strACD;		
			std::string m_strDeptCode;	
			std::string m_strDeptName;	
			std::string m_strRemark;	
			std::string m_strDeptCodePath;
			std::string m_strDeptNamePath;
		};
	}

};