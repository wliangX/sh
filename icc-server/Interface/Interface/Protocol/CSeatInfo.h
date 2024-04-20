#pragma once 
#include <Protocol/ISync.h>
namespace ICC
{
	namespace PROTOCOL
	{
        class CSeatInfo :
			public ISync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				//m_oHeader.SaveTo(p_pJson);

                p_pJson->SetNodeValue("/no", m_oBody.m_strNo);
				p_pJson->SetNodeValue("/name", m_oBody.m_strName);
                p_pJson->SetNodeValue("/dept_code", m_oBody.m_strDeptCode);
                p_pJson->SetNodeValue("/dept_name", m_oBody.m_strDeptName);
				return p_pJson->ToString();
			}
			virtual bool Parse(std::string p_strStaffInfo, JsonParser::IJsonPtr p_pJson)
			{
				if (p_pJson && p_pJson->LoadJson(p_strStaffInfo))
				{
					m_oBody.m_strNo = p_pJson->GetNodeValue("/no", "");
					m_oBody.m_strName = p_pJson->GetNodeValue("/name", "");
					m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
					m_oBody.m_strDeptName = p_pJson->GetNodeValue("/dept_name", "");
					return true;
				}
				return false;
			}

		public:
			//PROTOCOL::CHeader m_oHeader;
			class CBody
			{
			public:
                std::string m_strNo;
				std::string m_strName;
                std::string m_strDeptCode;
                std::string m_strDeptName;
			};
			CBody m_oBody;
		};
	}
}
